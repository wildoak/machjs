#include "mach_api.h"
#include "mach_api_use.h"
#include "mach_api_import.h"
#include "mach_api_fs.h"

#define REG_FN(js, fn) \
  current->Set(v8::String::NewFromUtf8(isolate, js), v8::Function::New(isolate, fn));


namespace mach {

  void InitializeMachGlobal(v8::Isolate *isolate, v8::Context *context) {
    v8::HandleScope scope(isolate);

    isolate->SetHostImportModuleDynamicallyCallback(api_import);

    v8::Local<v8::Object> current;

    // global
    current = context->Global();

    REG_FN("use", api_use);

    // fs
    current = v8::Object::New(isolate);
    context->Global()->Set(v8::String::NewFromUtf8(isolate, "fs"), current);

    REG_FN("open", api_fs_open);
  }
}