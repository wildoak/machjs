#pragma once

#include <uv.h>
#include <exception>
#include <string>
#include <functional>
#include <memory>

#include "mach.h"

namespace mach {
  namespace uv {

    #define UV_DO_OR_DIE(expr) _do_or_die(expr, #expr)

    class Error : public std::exception {
    public:
      explicit Error(std::string message);
      virtual const char* what() const noexcept;

    private:
      std::string message_;
    };

    inline int _do_or_die(int err, const char *expr_str) {
      CHECK(expr_str);

      if (err < 0) {
        throw mach::uv::Error(std::string()
          + uv_err_name(err) + " " + uv_strerror(err) + ": " + expr_str);
      }

      return err;
    }

    template<class T, class H>
    class HandleAware {
    public:
      using shared = std::shared_ptr<T>;

    public:
      static void Deleter(T *this_pointer) {
        CHECK(this_pointer);

        uv_close(this_pointer->Handle(), HandleAware::CloseCallback);
      }

      static void CloseCallback(uv_handle_t *handle) {
        CHECK(handle);

        T *this_pointer = static_cast<T *>(handle->data);
        CHECK(this_pointer);

        delete this_pointer;
      }

    protected:
      static shared NewUvClosed(std::unique_ptr<T> ptr) {
        CHECK(ptr);

        return shared(ptr.release(), HandleAware::Deleter);
      }

    public:
      HandleAware() {
        handle_.data = this;
      }

      HandleAware(const HandleAware&) = delete;
      HandleAware &operator=(const HandleAware &loop) = delete;

      operator H *() {
        return &handle_;
      }

    protected:
      uv_handle_t *Handle() {
        return (uv_handle_t *)&handle_;
      }

    protected:
      H handle_;
    };

    class Loop {
    public:
      using shared = std::shared_ptr<Loop>;

    public:
      static shared New();

    public:
      Loop();
      ~Loop();

      Loop(const Loop&) = delete;
      Loop &operator=(const Loop &loop) = delete;

      operator uv_loop_t *();

      void Run();

    private:
      uv_loop_t loop_;
    };

    class Timer : public HandleAware<Timer, uv_timer_t> {
    public:
      static shared New(Loop::shared loop);
    
      Timer(std::shared_ptr<Loop> loop);

    public:
      void Start(uint64_t timeout, uint64_t repeat);
      void Start(uint64_t timeout);
      void Stop();

      void SetCallback(std::function<void()> callback);

    private:
      static void TimerCallback(uv_timer_t *timer);

    private:
      std::shared_ptr<Loop> loop_;
      std::function<void()> callback_;
    };
    
  }
}