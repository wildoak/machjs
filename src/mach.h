#pragma once

#include <iostream>

#define CHECK(expr)                               \
  do {                                            \
    if (!(expr)) {                                \
      throw mach::Error("CHECK failed: " #expr);  \
    }                                             \
  } while (0)

namespace mach {

  class Error : public std::exception {
  public:
    explicit Error(std::string message);
    virtual const char* what() const noexcept;

  private:
    std::string message_;
  };

  template<class T>
  void nothrow(T &&fn) {
    try {
      fn();
    } catch(std::exception &e) {
      std::cerr << e.what() << std::endl;
    } catch(...) {
      std::cerr << "unknown error occurred" << std::endl;
    }
  }
}