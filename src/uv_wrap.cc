#include "uv_wrap.h"

namespace uvwrap {
  
  // Error class

  Error::Error(std::string message) : message_(std::move(message)) {}

  const char *Error::what() const noexcept {
    return message_.c_str();
  }

  // error_msg
  std::string error_msg(int err, const char *expr_str) {
    std::string msg;
    msg += uv_err_name(err);
    msg += " ";
    msg += uv_strerror(err);

    if (expr_str) {
      msg += ": ";
      msg += expr_str;
    }

    return std::move(msg);
  }

  // Loop class

  Loop::shared Loop::New() {
    return std::make_shared<Loop>();
  }

  Loop::Loop() {
    UVWRAP_DO_OR_DIE(uv_loop_init(&loop_));
  }

  Loop::~Loop() {
    mach::nothrow([this]() {
      UVWRAP_DO_OR_DIE(uv_loop_close(&loop_));
    });
  }

  Loop::operator uv_loop_t *() {
    return &loop_;
  }

  void Loop::Run(uv_run_mode mode) {
    UVWRAP_DO_OR_DIE(uv_run(&loop_, mode));
  }


  // class Timer
  
  Timer::shared Timer::New(Loop::shared loop) {
    CHECK(loop);

    return NewUvClosed(std::make_unique<Timer>(loop));
  }
  
  Timer::Timer(std::shared_ptr<Loop> loop) {
    CHECK(loop);

    loop_ = std::move(loop);
    UVWRAP_DO_OR_DIE(uv_timer_init(*loop_, &handle_));
  }

  void Timer::Start(uint64_t timeout, uint64_t repeat) {
    UVWRAP_DO_OR_DIE(uv_timer_start(&handle_, Timer::TimerCallback, timeout, repeat));
  }

  void Timer::Start(uint64_t timeout) {
    Start(timeout, 0);
  }

  void Timer::Stop() {
    UVWRAP_DO_OR_DIE(uv_timer_stop(&handle_));
  }

  void Timer::SetCallback(std::function<void()> callback) {
    callback_ = std::move(callback);
  }

  void Timer::TimerCallback(uv_timer_t *timer) {
    CHECK(timer);

    Timer *t = static_cast<Timer *>(timer->data);
    CHECK(t);

    if (t->callback_) {
      t->callback_();
    }
  }
}