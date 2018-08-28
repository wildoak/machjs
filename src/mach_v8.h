#pragma once

#include <v8.h>
#include "mach.h"
#include "uv_wrap.h"

#define CALLBACK_THROW_ERROR(msg) \
  mach::callback_throw_error(info.GetIsolate(), msg)

#define CALLBACK_UNDEFINED() v8::Undefined(info.GetIsolate())
  

namespace mach {

  inline void callback_throw_error(v8::Isolate *isolate, const char *msg) {
    isolate->ThrowException(
      v8::Exception::Error(v8::String::NewFromUtf8(isolate, msg)));
  }

  inline void callback_throw_error(v8::Isolate *isolate, const std::string &msg) {
    callback_throw_error(isolate, msg.c_str());
  }


  enum v8_isolate_slots {
    kMachEnvironment
  };

  template<class T, uint32_t slot>
  class IsolateData {
  public:
    static T *Get(v8::Isolate *isolate) {
      CHECK(isolate);
      return static_cast<T *>(isolate->GetData(slot));
    }
  public:
    IsolateData(v8::Isolate *isolate) : isolate_(isolate) {
      CHECK(isolate);
      isolate->SetData(slot, this);
    }

    virtual ~IsolateData() {
      isolate_->SetData(slot, nullptr);
    }
  
  private:
    v8::Isolate *isolate_;
  };

  template<class T, class ...V>
  class UvReqWrapper {
  public:
    using My = UvReqWrapper<T, V...>;
    using MyCallback = std::function<void(std::unique_ptr<My> &&, v8::Local<V>...)>;
  public:
    static std::unique_ptr<My> New(v8::Isolate *isolate, MyCallback callback, v8::Local<V> ...values) {
      return std::make_unique<My>(isolate, std::move(callback), std::move(values)...);
    }

    static std::unique_ptr<My> From(T *req) {
      return std::unique_ptr<My>(static_cast<My *>(req->data));
    }

    static void Callback(T* req) {
      auto &&me = From(req);
      auto &&isolate = me->GetIsolate();
      v8::Isolate::Scope isolate_scope(isolate);
      v8::HandleScope scope(isolate);
      v8::Context::Scope context_scope(me->context_.Get(isolate));

      // fetch values from from `me`, before moving
      auto &&values = std::make_tuple<v8::Local<V>...>(std::get<v8::Global<std::remove_reference_t<V>>>(me->values_).Get(isolate)...);
      me->callback_(std::move(me), std::get<v8::Local<V>>(values)...);
    }
  public:
    UvReqWrapper(v8::Isolate *isolate, MyCallback callback, v8::Local<V> &&...values) {
      isolate_ = isolate;
      context_ = v8::Global<v8::Context>(isolate, isolate->GetCurrentContext());
      callback_ = std::move(callback);
      values_ = std::make_tuple(v8::Global<V>(isolate, std::forward<v8::Local<V>>(values))...);

      req_.data = this;
    }

    v8::Isolate *GetIsolate() {
      return isolate_;
    }

    T *Req() {
      return &req_;
    }

    operator T* () {
      return &req_;
    }

    void (*GetCallback())(T* req) {
      return &Callback;
    }
    
  private:
    T req_;
    v8::Isolate *isolate_;
    v8::Global<v8::Context> context_;
    MyCallback callback_;
    std::tuple<v8::Global<std::remove_reference_t<V>>...> values_;
  };

  template<class T, class ...V>
  std::unique_ptr<UvReqWrapper<T, V...>> MakeReq(v8::Isolate *isolate,
                                                 typename UvReqWrapper<T, V...>::MyCallback callback,
                                                 v8::Local<V> ...values) {
    return UvReqWrapper<T, V...>::New(isolate, std::move(callback), std::move(values)...);
  }
}