#pragma once

#include <v8.h>

#define LIB_V8_STRING(x)                                                      \
  ([](v8::Isolate *isolate) -> auto {                                         \
    extern const char _binary_##x##_start;                                    \
                      _binary_##x##_end;                                      \
    return v8::String::NewFromUtf8(isolate,                                   \
                                   &_binary_##x##_start,                      \
                                   v8::NewStringType::kNormal,                \
                                   &_binary_##x##_end - &_binary_##x##_start  \
                                   )                                          \
  })
  