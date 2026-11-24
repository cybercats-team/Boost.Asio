//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2026 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <functional>
#include <iostream>
#include <boost/asio.hpp>
#include "util/singletone.h"

class printer final {  
public:
  printer(boost::asio::io_context& io, std::ostream& out, int interval = 1, int end = 5)
    : m_timer(io, boost::asio::chrono::seconds(interval)), m_out(out), m_interval(interval), m_end(end)
  {
    await();
  }

  ~printer() {
    m_out << "Final count is " << m_count << std::endl;
  }
  
protected:
  void print() {
    if (m_end <= m_count) {
      return;
    }

    m_out << (m_count++) << std::endl;
      
    m_timer.expires_at(m_timer.expiry() + boost::asio::chrono::seconds(m_interval));
    await();
  }

  void await() {
    m_timer.async_wait(std::bind(&printer::print, this));
  }  

private:
  boost::asio::steady_timer m_timer;
  std::ostream& m_out;

  int m_interval = 0;
  int m_count = 0;
  int m_end = 0;
};

int main() {
  auto& io = Singletone<boost::asio::io_context>::get();
  printer p(io, std::cout);

  io.run();
  return 0;
}