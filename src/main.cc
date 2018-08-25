#include "version.h"
#include <v8.h>
#include <libplatform/libplatform.h>
#include "mach.h"
#include "main.h"

#include "uv_wrap.h"

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
  v8::V8::InitializePlatform(platform.get());
  v8::V8::Initialize();

  mach::logger->info("build  {}", MACHJS_BUILD_DATE);
  mach::logger->info("       ---");
  mach::logger->info("MachJS {} {}", MACHJS_VERSION, MACHJS_COMMIT_DATE);
  mach::logger->info("V8     {}", v8::V8::GetVersion());
  mach::logger->info("       ---");

  InitMach(argc, argv);
  
  v8::V8::Dispose();
  v8::V8::ShutdownPlatform();
  // delete create_params.array_buffer_allocator;
}

void InitMach(int argc, char **argv) {

  using namespace uvwrap;
  auto &&loop = Loop::New();
  {
    auto &&timer = Timer::New(loop);
    timer->SetCallback([]() {
      mach::logger->info("timer1 executed");
    });

    auto &&timer2 = Timer::New(loop);
    int i = 0;
    timer2->SetCallback([&i, &timer2]() {
      mach::logger->info("timer2 executed");

      if (i++ < 3) {
        timer2->Start(1000);
      }
    });

    timer->Start(3000);
    timer2->Start(4000);

    loop->Run();
  }

  // close handles
  loop->Run();
}

// v8::Isolate::CreateParams create_params;
  // create_params.array_buffer_allocator =
  //     v8::ArrayBuffer::Allocator::NewDefaultAllocator();
  // v8::Isolate* isolate = v8::Isolate::New(create_params);
  // {
  //   v8::Isolate::Scope isolate_scope(isolate);
  //   // Create a stack-allocated handle scope.
  //   v8::HandleScope handle_scope(isolate);
  //   // Create a new context.
  //   v8::Local<v8::Context> context = v8::Context::New(isolate);
  //   // Enter the context for compiling and running the hello world script.
  //   v8::Context::Scope context_scope(context);
  //   // Create a string containing the JavaScript source code.
  //   v8::Local<v8::String> source =
  //       v8::String::NewFromUtf8(isolate, "'Hello' + ', World!'",
  //                               v8::NewStringType::kNormal)
  //           .ToLocalChecked();
  //   // Compile the source code.
  //   v8::Local<v8::Script> script =
  //       v8::Script::Compile(context, source).ToLocalChecked();
  //   // Run the script to get the result.
  //   v8::Local<v8::Value> result = script->Run(context).ToLocalChecked();
  //   // Convert the result to an UTF8 string and print it.
  //   v8::String::Utf8Value utf8(isolate, result);
  //   printf("%s\n", *utf8);
  // }
  // // Dispose the isolate and tear down V8.
  // isolate->Dispose();

  // 2018-08-25 11:56:21 +0200