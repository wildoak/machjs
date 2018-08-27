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

  template<class T>
  class ResolvingReq : public uvwrap::ReqWrapper<ResolvingReq<T>, T> {
  public:
    ResolvingReq(v8::Isolate *isolate, v8::Local<v8::Promise::Resolver> resolver) : isolate_(isolate)
    {
      context_ = v8::Global<v8::Context>(isolate, isolate->GetCurrentContext());
      resolver_ = v8::Global<v8::Promise::Resolver>(isolate, resolver);
    }

    v8::Local<v8::Promise::Resolver> GetResolver() const {
      return resolver_.Get(isolate_);
    }

    v8::Isolate *GetIsolate() const {
      return isolate_;
    }

    v8::Maybe<bool> Resolve(v8::Local<v8::Value> value) {
      return resolver_.Get(isolate_)->Resolve(context_.Get(isolate_), value);
    }

    v8::Maybe<bool> Reject(v8::Local<v8::Value> value) {
      return resolver_.Get(isolate_)->Reject(context_.Get(isolate_), value);
    }

  private:
    v8::Isolate *isolate_;
    v8::Global<v8::Context> context_;
    v8::Global<v8::Promise::Resolver> resolver_;
  };
}