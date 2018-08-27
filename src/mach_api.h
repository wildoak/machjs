#pragma once

#include <v8.h>

namespace mach {

  void InitializeMachGlobal(v8::Isolate *isolate, v8::Context *context);

}