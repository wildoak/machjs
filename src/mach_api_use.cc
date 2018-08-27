#include "mach_api_use.h"
#include "mach_api_fs.h"
#include "mach_environment.h"

#include <fstream>

namespace mach {

  v8::Local<v8::Value> api_use(const mach::FunctionCall<v8::Value>& info) {
    v8::EscapableHandleScope scope_v8(info.GetIsolate());
    auto &&env = mach::Environment::Get(info.GetIsolate());

    auto &&file_v8 = info[0]->ToString();

    // FIXME
    // std::string file_path = env->argv0 + "/" + *v8::String::Utf8Value(info.GetIsolate(), file_v8);
    std::string file_path = *v8::String::Utf8Value(info.GetIsolate(), file_v8);
    logger->debug("use '{}'", file_path);
    auto &&file_path_v8 = v8::String::NewFromUtf8(info.GetIsolate(), file_path.c_str());

    // maybe we can change this? e.g. mach_call(fs_read_file_sync).Return(&content)({file_path_v8}) ?
    auto &&content = api_fs_readfile_sync(FunctionCallMach<v8::Value>(info.GetIsolate())({file_path_v8}));
    logger->debug("compile '{}'", *v8::String::Utf8Value(content));

    auto &&script_v8 = v8::Script::Compile(content->ToString(), file_path_v8);
    // script_v8->Run()
    return scope_v8.Escape(script_v8->Run());
  }

}