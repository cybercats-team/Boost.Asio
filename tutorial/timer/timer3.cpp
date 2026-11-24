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

namespace {
  using namespace boost::system;
  using namespace boost::asio;

  void print(const error_code& /*e*/, steady_timer& t, int& count);
  void SetInterval(steady_timer& t,  int& count);
}

int main() {
  using namespace boost::asio;
  using namespace boost::asio::chrono;

  io_context io;

  int count = 0;
  steady_timer t(io, seconds(1));

  SetInterval(t, count);
  io.run();

  std::cout << "Final count is " << count << std::endl;
  return 0;
}

namespace {
  void print(const error_code& /*e*/, steady_timer& t, int& count) {
    if (5 <= count) {
      return;
    }

    std::cout << (count++) << std::endl;

    t.expires_at(t.expiry() + boost::asio::chrono::seconds(1));
    SetInterval(t, count);
  }

  void SetInterval(steady_timer& t,  int& count) {
    t.async_wait(std::bind(
      &print, 
      placeholders::error, 
      std::ref(t), 
      std::ref(count))
    );
  }
}
