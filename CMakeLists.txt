cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS -stdlib=libc++)
set(PROJECT machjs)

MESSAGE("Building: " ${CMAKE_BUILD_TYPE})

project (${PROJECT})

# configuration
set (MACHJS_VERSION_MAJOR 1)
set (MACHJS_VERSION_MINOR 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/version.h.in"
  "${PROJECT_BINARY_DIR}/include-gen/version.h"
)

include_directories("${PROJECT_BINARY_DIR}/include-gen")

# project sources
file(GLOB SOURCE_FILES src/*.cc src/**/*.cc)
add_executable(${PROJECT} ${SOURCE_FILES})

# v8
include_directories("${PROJECT_SOURCE_DIR}/deps/v8/include")

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
  set (V8_OBJ_DIR deps/v8/out/x64.release/obj)

  find_library(V8_LIBRARY_V8_LIBBASE NAMES v8_libbase HINTS ${V8_OBJ_DIR})
  find_library(V8_LIBRARY_V8_LIBPLATFORM NAMES v8_libplatform HINTS ${V8_OBJ_DIR})
  find_library(V8_LIBRARY_ICUUC NAMES icuuc HINTS "${V8_OBJ_DIR}/third_party/icu")
  find_library(V8_LIBRARY_ICUI18N NAMES icui18n HINTS "${V8_OBJ_DIR}/third_party/icu")


  set (V8_LIBRARIES
    ${V8_LIBRARY_V8_LIBBASE}  
    ${V8_LIBRARY_V8_LIBPLATFORM}
    ${V8_LIBRARY_ICUI18N}
    ${V8_LIBRARY_ICUUC}
  )
else()
  set (V8_TARGET_DIR deps/v8/out/x64.debug)

  find_library(V8_LIBRARY_V8 NAMES v8 HINTS ${V8_TARGET_DIR})
  find_library(V8_LIBRARY_V8_LIBBASE NAMES v8_libbase HINTS ${V8_TARGET_DIR})
  find_library(V8_LIBRARY_V8_LIBPLATFORM NAMES v8_libplatform HINTS ${V8_TARGET_DIR})
  find_library(V8_LIBRARY_ICUUC NAMES icuuc HINTS ${V8_TARGET_DIR})
  find_library(V8_LIBRARY_ICUI18N NAMES icui18n HINTS ${V8_TARGET_DIR})
  find_library(V8_LIBRARY_CPP NAMES c++ HINTS ${V8_TARGET_DIR})

  set (V8_LIBRARIES
    ${V8_LIBRARY_V8_LIBBASE}
    ${V8_LIBRARY_V8}
    ${V8_LIBRARY_V8_LIBPLATFORM}
    ${V8_LIBRARY_ICUI18N}
    ${V8_LIBRARY_ICUUC}
    ${V8_LIBRARY_CPP}
  )
endif()

# SPDLOG
if (NOT EXISTS ${CMAKE_SOURCE_DIR}/deps/spdlog)
  include(ExternalProject)
  ExternalProject_Add(spdlog
    GIT_REPOSITORY    https://github.com/gabime/spdlog.git
    GIT_TAG           v1.1.0
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/deps/spdlog
  )

  add_dependencies(${PROJECT_NAME} spdlog)
endif()

include_directories(${CMAKE_SOURCE_DIR}/deps/spdlog/include)

# Threads
find_package(Threads)

# libuv
include_directories("${PROJECT_SOURCE_DIR}/deps/libuv/include")
find_library(LIBUV_LIBRARY NAMES uv HINTS deps/libuv/out/Release)

target_link_libraries(${PROJECT}
  ${V8_LIBRARIES}
  ${LIBUV_LIBRARY}
  ${CMAKE_THREAD_LIBS_INIT}
)

# copy snapshots

if (${CMAKE_BUILD_TYPE} MATCHES "Release")
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.release/natives_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.release/snapshot_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
else()
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.debug/natives_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.debug/snapshot_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
endif()