#pragma once

#include <v8.h>
#include "mach_v8.h"
#include "uv_wrap.h"

namespace mach {
  class Environment : public IsolateData<Environment, kMachEnvironment> {
  public:
    Environment(v8::Isolate *isolate) : IsolateData(isolate) {
      
    }

  public:
    std::string argv0;
    uvwrap::Loop loop;
  };
}