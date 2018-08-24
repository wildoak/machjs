#include "uv_wrap.h"

namespace mach {
  namespace uv {
    
    
    // Error class

    Error::Error(std::string message) : message_(std::move(message)) {}

    const char *Error::what() const noexcept {
      return message_.c_str();
    }


    // Loop class

    Loop::shared Loop::New() {
      return std::make_shared<Loop>();
    }

    Loop::Loop() {
      UV_DO_OR_DIE(uv_loop_init(&loop_));
    }

    Loop::~Loop() {
      nothrow([this]() {
        UV_DO_OR_DIE(uv_loop_close(&loop_));
      });
    }

    Loop::operator uv_loop_t *() {
      return &loop_;
    }

    void Loop::Run() {
      UV_DO_OR_DIE(uv_run(&loop_, UV_RUN_DEFAULT));
    }


    // class Timer
    
    Timer::shared Timer::New(Loop::shared loop) {
      CHECK(loop);

      return NewUvClosed(std::make_unique<Timer>(loop));
    }
    
    Timer::Timer(std::shared_ptr<Loop> loop) {
      CHECK(loop);

      loop_ = std::move(loop);
      UV_DO_OR_DIE(uv_timer_init(*loop_, &handle_));
    }
  
    void Timer::Start(uint64_t timeout, uint64_t repeat) {
      UV_DO_OR_DIE(uv_timer_start(&handle_, Timer::TimerCallback, timeout, repeat));
    }

    void Timer::Start(uint64_t timeout) {
      Start(timeout, 0);
    }

    void Timer::Stop() {
      UV_DO_OR_DIE(uv_timer_stop(&handle_));
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
}