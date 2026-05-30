#include <stdio.h>

#include "array.hpp"
#include "arena.hpp"
#include "string.hpp"
#include "hash_table.hpp"
#include "lexer.hpp"
#include "token.hpp"
#include "token_meta.hpp"

void print_token(Token token, String8_View source)
{
  switch (token.type)
  {
    default:
      printf("| %-*s",
             token_type_max_field_len() + 5,
             str_from_token_type(token.type));
      printf("%.*s\n", token.length, &source.buffer[token.position]);
      break;
  }
}

int main(int argc, char *argv[])
{
  Arena arena(Arena::linked);

  String8 file_str;
  String8::read_from_file(&file_str, "kitchen_sink.cassette", &arena);
  printf("%s\n", file_str.c_str());

  Hash_Table<String8_View, Token_Type> ht(&arena);
  ht.insert(String8_View(String8("fn")), Token_Type::function);
  ht.insert(String8_View(String8("true")), Token_Type::true_keyword);
  ht.insert(String8_View(String8("false")), Token_Type::false_keyword);
  ht.insert(String8_View(String8("if")), Token_Type::if_keyword);
  ht.insert(String8_View(String8("else")), Token_Type::else_keyword);
  ht.insert(String8_View(String8("return")), Token_Type::return_keyword);
  ht.insert(String8_View(String8("import")), Token_Type::import_keyword);

  String8_View source_sv = String8_View(file_str);
  Lexer lexer = Lexer(source_sv, ht);

  Array<Token> token_arr(&arena);

  while (true)
  {
    Token scanned = lexer.scan_token();
    token_arr.append(scanned);
    if (scanned.type == Token_Type::end_of_file) break;
  }

  for (int i = 0; i < token_arr.length; i++)
  {
    print_token(token_arr[i], lexer.source);
  }
}

