cmake_minimum_required (VERSION 3.8)
set(PROJECT machjs)
project (${PROJECT})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_FLAGS -stdlib=libc++)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake_modules)

include(ExternalProject)
include(Resources)

MESSAGE("Building: ${CMAKE_BUILD_TYPE}")

execute_process(
  COMMAND git describe --tags --always --dirty
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE MACHJS_VERSION
)

execute_process(
  COMMAND git log -1 --format=%ci
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE MACHJS_COMMIT_DATE
)

execute_process(
  COMMAND date +%Y-%m-%d\ %H:%M:%S\ %z
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE MACHJS_BUILD_DATE
)

MESSAGE("MACHJS_VERSION: ${MACHJS_VERSION}")
MESSAGE("MACHJS_COMMIT_DATE: ${MACHJS_COMMIT_DATE}")
MESSAGE("MACHJS_BUILD_DATE: ${MACHJS_BUILD_DATE}")

configure_file (
  ${PROJECT_SOURCE_DIR}/src/version.h.in
  ${PROJECT_BINARY_DIR}/include-gen/version.h
)

set(MACH_INCLUDE_GEN_FILES ${PROJECT_BINARY_DIR}/include-gen)



# v8
set(V8_INCLUDE_FILES ${PROJECT_SOURCE_DIR}/deps/v8/include)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  set (V8_OBJ_DIR deps/v8/out/x64.release/obj)

  find_library(V8_LIBRARY_V8_LIBBASE NAMES v8_libbase HINTS ${V8_OBJ_DIR})
  find_library(V8_LIBRARY_V8_LIBPLATFORM NAMES v8_libplatform HINTS ${V8_OBJ_DIR})
  find_library(V8_LIBRARY_ICUUC NAMES icuuc HINTS ${V8_OBJ_DIR}/third_party/icu)
  find_library(V8_LIBRARY_ICUI18N NAMES icui18n HINTS ${V8_OBJ_DIR}/third_party/icu)


  set (V8_LIB_FILES
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

  set (V8_LIB_FILES
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
  
  ExternalProject_Add(spdlog
    GIT_REPOSITORY    https://github.com/gabime/spdlog.git
    GIT_TAG           v1.1.0
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/deps/spdlog
  )

  add_dependencies(${PROJECT_NAME} spdlog)
endif()

set(SPDLOG_INCLUDE_FILES ${CMAKE_SOURCE_DIR}/deps/spdlog/include)

# Threads
find_package(Threads)

# libuv
include_directories(${PROJECT_SOURCE_DIR}/deps/libuv/include)
find_library(LIBUV_LIB_FILES NAMES uv HINTS deps/libuv/out/Release)

# lib
file(GLOB LIB_JS_SOURCES lib/*.js lib/**/*.js)
add_resources(LIB_JS_RESOURCES ${LIB_JS_SOURCES})

# copy snapshots

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.release/natives_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.release/snapshot_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
else()
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.debug/natives_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
  file(COPY ${PROJECT_SOURCE_DIR}/deps/v8/out/x64.debug/snapshot_blob.bin DESTINATION ${PROJECT_BINARY_DIR})
endif()



# INCLUDE
include_directories(${PROJECT}
  ${MACH_INCLUDE_GEN_FILES}
  ${SPDLOG_INCLUDE_FILES}
  ${V8_INCLUDE_FILES}
)

# SOURCE
file(GLOB SOURCE_FILES src/*.cc src/**/*.cc)
add_executable(${PROJECT}
  ${SOURCE_FILES}
  ${LIB_JS_RESOURCES}
)

# LINK
target_link_libraries(${PROJECT}
  ${V8_LIB_FILES}
  ${LIBUV_LIB_FILES}
  ${CMAKE_THREAD_LIBS_INIT}
)