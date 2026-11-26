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

struct State {
  int count = 0;
  const int end;

  explicit State(int count_to) : end(count_to) {}
};

class printer final {  
public:
  printer(ICtxMT& ctx, std::shared_ptr<State> state) : 
    m_timer(ctx.GetCtx(), boost::asio::chrono::seconds(1)), 
    m_strand(ctx.GetStrand()),
    m_state(state)
  {
    await();
  }

  ~printer() {
    using namespace boost::asio;
    
    bind_executor(m_strand, [&state = m_state]() {
      std::cout << "Final count is " << state->count << std::endl;
    })();
  }
  
protected:
  void print() {
    using namespace boost::asio::chrono;
    auto& state = *m_state;

    if (state.end <= state.count) {
      return;
    }

    std::cout << (state.count++) << std::endl;
      
    m_timer.expires_at(m_timer.expiry() + seconds(1));
    await();
  }

  void await() {
    using namespace boost::asio;

    m_timer.async_wait(bind_executor(m_strand, std::bind(&printer::print, this)));
  }  

private:
  boost::asio::steady_timer m_timer;
  ICtxMT::strand_t& m_strand;
  
  std::shared_ptr<State> m_state = nullptr;
};

int main() {
  auto& ctx = Singletone<CtxMT>::get();
  auto state_ptr = std::make_shared<State>(40);
  
  std::vector<std::unique_ptr<printer>> printers{};
  for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i) {
    printers.emplace_back(std::make_unique<printer>(ctx, state_ptr));
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