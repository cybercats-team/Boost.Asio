#include <type_traits>

template <typename T, typename = std::enable_if_t<std::is_default_constructible<T>::value>>
class Singletone {
public:  
  Singletone() = delete;

  static T& get() {
    static T instance{};

    return instance;
  }
};