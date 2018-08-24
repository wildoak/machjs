#include "mach.h"

namespace mach {
    
  Error::Error(std::string message) : message_(std::move(message)) {}

  const char *Error::what() const noexcept {
    return message_.c_str();
  }
}