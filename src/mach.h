#pragma once

#include <iostream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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

  extern std::shared_ptr<spdlog::logger> logger;

  void InitializeLogger();

  template<class T>
  bool nothrow(T &&fn) {
    try {
      fn();
      return true;
    } catch(std::exception &e) {
      mach::logger->critical("error in nothrow: {}", e.what());
    } catch(...) {
      mach::logger->critical("unknown error occurred");
    }

    return false;
  }
}