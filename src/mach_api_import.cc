#include "mach_api_import.h"
#include "mach_environment.h"

namespace mach {

  v8::MaybeLocal<v8::Promise> api_import(v8::Local<v8::Context> context,
                                         v8::Local<v8::ScriptOrModule> referrer,
                                         v8::Local<v8::String> specifier) {
    v8::EscapableHandleScope scope(context->GetIsolate());
    auto &&resolver = v8::Promise::Resolver::New(context).ToLocalChecked();
    
    v8::Persistent<v8::Promise::Resolver> persistent_resolver(context->GetIsolate(), resolver);
    
    auto &&env = Environment::Get(context->GetIsolate());
    

    // v8::ScriptCompiler::Source()
    // v8::ScriptCompiler::CompileModule(context->GetIsolate(),  )
    return scope.Escape(resolver->GetPromise());
  }
}