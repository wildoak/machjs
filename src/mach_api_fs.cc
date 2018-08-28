#include "mach_api_fs.h"
#include <fstream>
#include "mach_v8.h"
#include "mach_environment.h"
#include <fcntl.h>

namespace mach {
  v8::Local<v8::Value> api_fs_readfile_sync(const FunctionCall<v8::Value> &info) {
    v8::EscapableHandleScope scope_v8(info.GetIsolate());

    if (info.Length() == 0) {
      logger->debug("api_fs_readfile_sync zero length");
      return CALLBACK_UNDEFINED();
    }

    v8::String::Utf8Value utf8_value(info.GetIsolate(), info[0]->ToString());
    logger->debug("api_fs_readfile_sync '{}'", *utf8_value);

    std::ifstream in;
    in.open(*utf8_value, std::ios::in | std::ios::binary);
    if (!in) {
      CALLBACK_THROW_ERROR(std::string("could not find: ") + *utf8_value);
      return CALLBACK_UNDEFINED();
    }

    in.seekg(0, std::ios::end);
    size_t bytes = in.tellg();
    in.seekg(0, std::ios::beg);

    std::unique_ptr<char[]> content(new char[bytes + 1]);
    in.read(content.get(), bytes);
    content[bytes] = '\0';

    logger->debug("read content: bytes: {}, '{}'", bytes, content.get());
    auto &&content_v8 = v8::String::NewFromOneByte(info.GetIsolate(), (uint8_t *)content.get(), v8::NewStringType::kNormal);
    return scope_v8.Escape(content_v8.ToLocalChecked());
  }

  v8::Local<v8::Value> api_fs_open(const FunctionCall<v8::Value> &info) {
    auto &&env = Environment::Get(info.GetIsolate());
    v8::EscapableHandleScope scope(info.GetIsolate());

    auto &&resolver = v8::Promise::Resolver::New(info.GetIsolate());

    auto &&req_wrapper = MakeReq<uv_fs_t>(info.GetIsolate(), [](auto &&req_wrapper, auto &&resolver) {
      auto &&result = uv_fs_get_result(*req_wrapper);
      if (result == -1) {
        resolver->Reject(v8::Undefined(req_wrapper->GetIsolate()));
      } else {
        resolver->Resolve(v8::Integer::New(req_wrapper->GetIsolate(), result));
      }

      uv_fs_req_cleanup(*req_wrapper);
    }, resolver);
    
    auto &&path = info[0]->ToString();
    v8::String::Utf8Value path_value(info.GetIsolate(), path);
    UVWRAP_DO_OR_DIE(uv_fs_open(env->loop, *req_wrapper.release(), *path_value, O_WRONLY | O_CREAT, S_IRUSR,
      req_wrapper->GetCallback()));

    return scope.Escape(resolver->GetPromise());
  }
}