//
// timer.cpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2026 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <boost/asio.hpp>
#include "util/stopwatch.h"

namespace {
  StopWatch sw{};

  void print(const boost::system::error_code& /*e*/);
}

int main() {
  using namespace boost::asio;
  using namespace boost::asio::chrono;

  io_context io;
  steady_timer t(io, seconds(5));

  sw.Start();
  t.async_wait(&print);
  io.run();

  return 0;
}

namespace {
  void print(const boost::system::error_code& /*e*/) {
    std::cout << "Timer after " << sw.Stop() << "sec" << std::endl;
  }
}