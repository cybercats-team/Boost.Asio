#include <termios.h>

class EnableRaw {
public:
  explicit EnableRaw(int fd);
  ~EnableRaw();
private:
  termios m_term;
  int m_fd = 0;
};