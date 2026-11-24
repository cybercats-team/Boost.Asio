#include <ctime>

class StopWatch final {
  public:
    void Start();
    std::time_t Stop();
  private:
    std::time_t m_time = 0;
};