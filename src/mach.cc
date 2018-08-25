#include "mach.h"

namespace mach {
    
  Error::Error(std::string message) : message_(std::move(message)) {}

  const char *Error::what() const noexcept {
    return message_.c_str();
  }

  std::shared_ptr<spdlog::logger> logger;

  void InitializeLogger() {
    logger = spdlog::stdout_color_mt("logger");
    logger->set_level(spdlog::level::trace);
    logger->set_pattern("%Y-%m-%d %T%z %L | %^%v%$");
  }
}