//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2026 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <thread>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <boost/asio.hpp>
#include "util/singletone.h"

struct ICtxMT {
  using ctx_t = boost::asio::io_context;
  using strand_t = boost::asio::strand<ctx_t::executor_type>;

  virtual ctx_t& GetCtx() = 0;
  virtual strand_t& GetStrand() = 0;
};

class CtxMT : public ICtxMT {
public:
  ctx_t& GetCtx() override { return m_ctx; };
  strand_t& GetStrand() override { return m_strand; };
private:
  ctx_t m_ctx{};
  strand_t m_strand = boost::asio::make_strand(m_ctx);  
};

class printer final {  
public:
  printer(ICtxMT& ctx, std::shared_ptr<int> count, int interval = 1, int end = 5) : 
    m_timer(ctx.GetCtx(), boost::asio::chrono::seconds(interval)), 
    m_strand(ctx.GetStrand()),
    m_count(count),
    m_interval(interval), 
    m_end(end)
  {
    await();
  }

  ~printer() {
    bind_executor(m_strand, [&count = m_count]() {
      std::cout << "Final count is " << *count << std::endl;
    })();
  }
  
protected:
  void print() {
    using namespace boost::asio::chrono;
    auto& count = *m_count;

    if (m_end <= count) {
      return;
    }

    std::cout << (count++) << std::endl;
      
    m_timer.expires_at(m_timer.expiry() + seconds(m_interval));
    await();
  }

  void await() {
    using namespace boost::asio;

    m_timer.async_wait(bind_executor(m_strand, std::bind(&printer::print, this)));
  }  

private:
  boost::asio::steady_timer m_timer;
  ICtxMT::strand_t& m_strand;
  
  std::shared_ptr<int> m_count = nullptr;
  int m_interval = 0;
  int m_end = 0;
};

int main() {
  auto& ctx = Singletone<CtxMT>::get();
  auto count_ptr = std::make_shared<int>(0);
  
  std::vector<std::unique_ptr<printer>> printers{};
  for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i) {
    printers.emplace_back(std::make_unique<printer>(ctx, count_ptr, 1, 40));
  }

  auto& io = ctx.GetCtx();

  std::vector<std::thread> threads{};
  for (unsigned int i = 1; i < std::thread::hardware_concurrency(); ++i) {
    threads.emplace_back([&io]() { io.run(); });
  }
  
  io.run();
  for (auto& thread : threads) {
    thread.join();
  }

  return 0;
}