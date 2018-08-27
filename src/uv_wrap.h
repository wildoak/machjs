#pragma once

#include <uv.h>
#include <exception>
#include <string>
#include <functional>
#include <memory>

#include "mach.h"

#define UVWRAP_DO_OR_DIE(expr) uvwrap::_do_or_die(expr, #expr)
#define UVWRAP_DO(expr) uvwrap::_do(expr, #expr)

namespace uvwrap {

  class Error : public std::exception {
  public:
    explicit Error(std::string message);
    virtual const char* what() const noexcept;

  private:
    std::string message_;
  };

  std::string error_msg(int err, const char *expr_str);

  inline int _do_or_die(int err, const char *expr_str) {
    if (err < 0) {
      throw Error(error_msg(err, expr_str));
    }

    return err;
  }

  inline int _do(int err, const char *expr_str) {
    if (err < 0) {
      mach::logger->error(error_msg(err, expr_str));
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
      mach::logger->debug("close {}", (void *)this_pointer);
    }

    static void CloseCallback(uv_handle_t *handle) {
      CHECK(handle);

      T *this_pointer = static_cast<T *>(handle->data);
      CHECK(this_pointer);

      mach::logger->debug("delete {}", (void *)this_pointer);
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

    virtual ~HandleAware() {
      mach::logger->debug("destruct {}", (void *)this);
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

    void Run(uv_run_mode mode = UV_RUN_DEFAULT);

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


  template<class R, class T>
  class ReqWrapper {
  public:

    template<class ...Args>
    static std::unique_ptr<R> New(Args &&...args) {
      return std::make_unique<R>(std::forward<Args>(args)...);
    }

    static std::unique_ptr<R> From(T *req) {
      return std::unique_ptr<R>(static_cast<R *>(req->data));
    }

  protected:
    ReqWrapper() {
      req.data = this;
    }
    
  public:
    virtual ~ReqWrapper() {
      uv_fs_req_cleanup(&req);
    }

  public:
    operator T *() {
      return &req;
    }

  private:
    T req;
  };
  
}