#include <time.h>

template <typename ClockType>
void AsDateTime(typename ClockType::time_point& tp, std::tm& out) {
  auto ts = ClockType::to_time_t(tp);

  localtime_r(&ts, &out);
}

