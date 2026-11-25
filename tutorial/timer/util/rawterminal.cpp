#include "rawterminal.h"

EnableRaw::EnableRaw(int fd) : m_fd(fd) {
  tcgetattr(m_fd, &m_term);

  auto raw = m_term;
  // Disable canonical mode (line buffering) and local echo
  raw.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(m_fd, TCSAFLUSH, &raw);
}
  
EnableRaw::~EnableRaw() {
  tcsetattr(m_fd, TCSAFLUSH, &m_term);
}