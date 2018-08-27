#include "mach.h"
#include "mach_api.h"
#include "mach_api_use.h"
#include "mach_environment.h"
#include "mach_version.h"
#include "main.h"
#include <memory>
#include <libplatform/libplatform.h>
#include <v8.h>


int main(int argc, char **argv) {
  mach::InitializeLogger();

  if (mach::nothrow([&]() { Init(argc, argv); })) {
    return 0;
  }

  return 1;
}

void Init(int argc, char **argv) {
  v8::V8::InitializeICUDefaultLocation(argv[0]);
  v8::V8::InitializeExternalStartupData(argv[0]);
  std::unique_ptr<v8::Platform> platform = v8::platform::NewDefaultPlatform();
  v8::V8::InitializePlatform(&*platform);
  v8::V8::Initialize();

  mach::logger->info("build  {}", MACHJS_BUILD_DATE);
  mach::logger->info("       ---");
  mach::logger->info("MachJS {} {}", MACHJS_VERSION, MACHJS_COMMIT_DATE);
  mach::logger->info("V8     {}", v8::V8::GetVersion());
  mach::logger->info("       ---");

  v8::Isolate::CreateParams create_params;
  std::unique_ptr<v8::ArrayBuffer::Allocator> array_buffer_allocator(
    v8::ArrayBuffer::Allocator::NewDefaultAllocator());
  
  create_params.array_buffer_allocator = &*array_buffer_allocator;
  v8::Isolate *isolate = v8::Isolate::New(create_params);

  {
    mach::Environment env(isolate);
    env.argv0 = argv[0];
  
    if (argc >= 2) {
      RunMach(isolate, argv[1]);
      env.loop.Run();
      env.loop.Run();
    } else {
      mach::logger->warn("no file given");
    }
  }

  isolate->Dispose();
  
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
}


void RunMach(v8::Isolate *isolate, const std::string &file_path) {
  v8::Isolate::Scope isolate_scope(isolate);
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = v8::Context::New(isolate);
  v8::Context::Scope context_scope(context);

  mach::InitializeMachGlobal(isolate, *context);
  
  auto &&file_path_v8 = v8::String::NewFromUtf8(isolate, file_path.c_str());
  auto &&result = mach::api_use(mach::FunctionCallMach<v8::Value>(isolate)({file_path_v8}));
  mach::logger->info("returned: '{}'", *v8::String::Utf8Value(result->ToString()));
}
