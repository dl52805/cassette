#include <stdio.h>

#include "arena.hpp"
#include "string.hpp"

int main(int argc, char *argv[])
{
  Arena arena(Arena::linked);

  String8 file_str;
  String8::read_from_file(&file_str, "kitchen_sink.cassette", &arena);
  printf("%s\n", file_str.c_str());
}

