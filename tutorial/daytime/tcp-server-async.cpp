//
// server.cpp
// ~~~~~~~~~~
//
// Copyright (c) 2003-2026 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <boost/asio.hpp>
#include "util/datetime.h"

class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
public:
  using tcp = boost::asio::ip::tcp;
  using pointer = std::shared_ptr<tcp_connection>;

  static pointer create(boost::asio::io_context& io_context) {
    return std::make_shared<tcp_connection>(io_context);
  }

  explicit tcp_connection(boost::asio::io_context& io_context) : m_socket(io_context) {}

  tcp::socket& socket() {
    return m_socket;
  }

  void start() {
    using boost::asio::placeholders::error;
    using boost::asio::placeholders::bytes_transferred;

    m_message = make_daytime_string();

    boost::asio::async_write(
      m_socket, 
      boost::asio::buffer(m_message),
      std::bind(
        &tcp_connection::handle_write, 
        shared_from_this(),
        error,
        bytes_transferred
    ));
  }

private:  
  void handle_write(
      const boost::system::error_code& /*error*/,
      size_t /*bytes_transferred*/)
  {}

  tcp::socket m_socket;
  std::string m_message;
};

class tcp_server {
public:
  using tcp = boost::asio::ip::tcp;

  tcp_server(boost::asio::io_context& io_context)
    : m_io_context(io_context),
      m_acceptor(io_context, tcp::endpoint(tcp::v4(), 13))
  {
    start_accept();
  }

private:
  void start_accept() {
    auto new_connection = tcp_connection::create(m_io_context);

    m_acceptor.async_accept(new_connection->socket(), std::bind(
      &tcp_server::handle_accept, 
      this, 
      new_connection,
        boost::asio::placeholders::error
    ));
  }

  void handle_accept(
      tcp_connection::pointer new_connection,
      const boost::system::error_code& error
  ) {
    if (!error) {
      new_connection->start();
    }

    start_accept();
  }

  boost::asio::io_context& m_io_context;
  tcp::acceptor m_acceptor;
};

int main() {
  try {
    boost::asio::io_context io_context;
    tcp_server server(io_context);

    io_context.run();
  } catch (std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  return 0;
}