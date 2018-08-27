#pragma once

#include <v8.h>

int main(int argc, char **argv);
void Init(int argc, char **argv);
void RunMach(v8::Isolate *isolate, const std::string &file_path);