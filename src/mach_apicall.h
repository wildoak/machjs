#pragma once

#include <v8.h>
#include <vector>

#define DECLARE_API_FN(fn)                                          \
  v8::Local<v8::Value> fn(const FunctionCall<v8::Value> &info);     \
  inline void fn(const v8::FunctionCallbackInfo<v8::Value> &info) { \
    info.GetReturnValue().Set(fn(FunctionCallV8<v8::Value>(info))); \
  }

namespace mach {

  template<class T>
  class FunctionCall {
  public:
    virtual ~FunctionCall() {}
    virtual v8::Isolate *GetIsolate() const = 0;
    virtual int Length() const = 0;
    virtual v8::Local<v8::Value> operator[](int index) const = 0;
    virtual v8::Local<v8::Value> This() const = 0;
    virtual v8::Local<v8::Object> Holder() const = 0;
    virtual bool IsConstructCall() const = 0;
    virtual v8::Local<v8::Value> Data() const = 0;
    virtual v8::Local<v8::Value> NewTarget() const = 0;
  };

  template<class T>
  class FunctionCallV8 : public FunctionCall<T> {
  public:
    FunctionCallV8(const v8::FunctionCallbackInfo<T> &info) : info_(info) {}

    virtual v8::Isolate *GetIsolate() const override {
      return info_.GetIsolate();
    }

    virtual int Length() const override {
      return info_.Length();
    }

    virtual v8::Local<v8::Value> operator[](int index) const override {
      return info_[index];
    }

    virtual v8::Local<v8::Value> This() const override {
      return info_.This();
    }

    virtual v8::Local<v8::Object> Holder() const override {
      return info_.Holder();
    }

    virtual bool IsConstructCall() const override {
      return info_.IsConstructCall();
    }

    virtual v8::Local<v8::Value> Data() const override {
      return info_.Data();
    }

    virtual v8::Local<v8::Value> NewTarget() const override {
      return info_.NewTarget();
    }
  private:
    const v8::FunctionCallbackInfo<T> &info_;
  };

  template<class T>
  class FunctionCallMach : public FunctionCall<T> {
  public:
    FunctionCallMach(v8::Isolate *isolate) : isolate_(isolate) {}

  public:

    virtual v8::Isolate *GetIsolate() const override {
      return isolate_;
    }

    virtual int Length() const override {
      return params_.size();
    }

    virtual v8::Local<v8::Value> operator[](int index) const override {
      return params_[index];
    }

    FunctionCallMach &operator()(std::initializer_list<v8::Local<v8::Value>> params) {
      params_ = std::move(params);
      return *this;
    }

    virtual v8::Local<v8::Value> This() const override {
      return this_;
    }

    FunctionCallMach &This(v8::Local<v8::Value> thiz) {
      this_ = std::move(thiz);
      return *this;
    }

    virtual v8::Local<v8::Object> Holder() const override {
      return holder_;
    }

    FunctionCallMach &Holder(v8::Local<v8::Object> holder) {
      holder_ = std::move(holder);
      return *this;
    }

    virtual bool IsConstructCall() const override {
      return construct_call_;
    }

    FunctionCallMach &ConstructCall(bool construct_call = true) {
      construct_call_ = construct_call;
      return *this;
    }

    virtual v8::Local<v8::Value> Data() const override {
      return data_;
    }

    FunctionCallMach &Data(v8::Local<v8::Value> data) const {
      data_ = std::move(data);
      return *this;
    }

    virtual v8::Local<v8::Value> NewTarget() const override {
      return new_target_;
    }

    FunctionCallMach &NewTarget(v8::Local<v8::Value> new_target) {
      new_target_ = std::move(new_target);
      return *this;
    }

  private:
    v8::Isolate *isolate_ = nullptr;
    std::vector<v8::Local<v8::Value>> params_;
    v8::Local<v8::Value> this_;
    v8::Local<v8::Object> holder_;
    bool construct_call_ = false;
    v8::Local<v8::Value> data_;
    v8::Local<v8::Value> new_target_;
  };
}