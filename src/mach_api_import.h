#pragma once

#include "mach_apicall.h"

namespace mach {

  v8::MaybeLocal<v8::Promise> api_import(v8::Local<v8::Context> context,
                                         v8::Local<v8::ScriptOrModule> referrer,
                                         v8::Local<v8::String> specifier);
}