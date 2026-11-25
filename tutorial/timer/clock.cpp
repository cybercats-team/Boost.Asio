#include <atomic>
#include <cstdint>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <unistd.h>
#include <boost/asio.hpp>
#include <boost/type_traits.hpp>
#include "util/datetime.h"
#include "util/singletone.h"
#include "util/rawterminal.h"

class Timer {
public:
  using clock = std::chrono::_V2::steady_clock;
  using tp = typename clock::time_point;

  Timer(boost::asio::io_context& io, std::function<void(const tp&)> callback, uint32_t interval_ms = 1000);
  void Stop();
protected:
  void NextTick();  
  void OnTick();
private:
  boost::asio::steady_timer m_timer;
  std::function<void(const tp&)> m_callback;
  uint32_t m_interval_ms = 0;
  std::atomic_bool m_stopped{false};
};

class Clock {
public:
  Clock(boost::asio::io_context& io);
protected:
  void OnTimer(const Timer::tp&);
  void NextKey();
  void OnKeyPress(const boost::system::error_code& ec, size_t size);
private:
  Timer m_timer;
  boost::asio::posix::stream_descriptor m_console;  
  char m_buffer = 0;
};

int32_t main() {
  EnableRaw raii(::dup(STDIN_FILENO));
  
  auto& io = Singletone<boost::asio::io_context>::get();
  std::cout << "Q to exit" << std::endl << std::endl;
  
  Clock clock(io);
  io.run();

  std::cout << std::endl;
  return 0;
}

Timer::Timer(boost::asio::io_context& io, std::function<void(const tp&)> callback, uint32_t interval_ms)
  : m_timer(io, boost::asio::chrono::milliseconds(interval_ms)), 
    m_callback(callback), m_interval_ms(interval_ms) 
{
  NextTick();
}

void Timer::Stop() {
  m_stopped.store(true);
}

void Timer::NextTick() {
  m_timer.async_wait(std::bind(&Timer::OnTick, this));
}

void Timer::OnTick() {
  if (m_stopped.load()) {
    return;
  }

  auto tp = m_timer.expiry();

  if (nullptr != m_callback) {
    m_callback(tp);
  }
  
  m_timer.expires_at(tp + boost::asio::chrono::milliseconds(m_interval_ms));
  NextTick();
}

Clock::Clock(boost::asio::io_context& io) 
  : m_timer(io, std::bind(&Clock::OnTimer, this, std::placeholders::_1)), 
    m_console(io, ::dup(STDIN_FILENO)) 
{
  OnTimer(Timer::tp{});
  NextKey();
}

void Clock::OnTimer(const Timer::tp&) {
  using sys_clock = std::chrono::_V2::system_clock;

  std::tm tm_now{};
  auto now = sys_clock::now();
  AsDateTime<sys_clock>(now, tm_now);

  std::cout << "\033[2K\r" << std::put_time(&tm_now, "%H:%M:%S") << std::flush;
}

void Clock::NextKey() {
  using namespace boost::asio;  

  m_console.async_read_some(buffer(&m_buffer, 1), std::bind(
    &Clock::OnKeyPress, 
    this, 
    placeholders::error, 
    placeholders::bytes_transferred
  ));
}

void Clock::OnKeyPress(const boost::system::error_code& ec, size_t size) {
  if (ec || 1 != size) {
    return;
  }

  if ('q' == m_buffer || 'Q' == m_buffer) {
    m_timer.Stop();
    return;
  }

  NextKey();
}
