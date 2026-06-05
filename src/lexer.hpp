#pragma once

#include <ctype.h>

#include "token.hpp"
#include "string.hpp"
#include "hash_table.hpp"

struct Lexer
{
  Hash_Table<String8_View, Token_Type> ht;
  String8_View source;
  u32 line;
  u32 current;
  u32 start;

  Lexer(String8_View source, Hash_Table<String8_View, Token_Type> ht)
  {
    this->source = source;
    this->line = 1;
    this->current = 0;
    this->start = 0;
    this->ht = ht;
  }

  bool is_at_end()
  {
    return current >= source.length;
  }

  char peek()
  {
    if (is_at_end()) return '\0';
    return source.buffer[current];
  }

  char peek_next()
  {
    char curr_char = source.buffer[current];
    current += 1;
    return curr_char;
  }

  char advance()
  {
    char curr_char = source.buffer[current];
    current += 1;
    return curr_char;
  }

  void skip_whitespace()
  {
    while (true)
    {
      if (is_at_end()) break;

      char c = peek();
      switch(c)
      {
        case ' ':
        case '\r':
        case '\t':
          advance();
          break;
        case '\n':
          line += 1;
          advance();
          break;
        default:
          return;
      }
    }
  }

  Token scan_integer()
  {
    while (isdigit(peek())) advance();
    return Token(Token_Type::int_num, start, current - start);
  }

  Token_Type scan_identifier()
  {
    while (isalnum(peek()) || (peek() == '_')) advance();

    u32 ident_length = current - start;
    String8_View ident_name((char *) &source.buffer[start], ident_length);

    Token_Type *keyword_type = ht.find(ident_name);
    if (keyword_type != nullptr) return *keyword_type;
    return Token_Type::ident;
  }

  Token scan_string()
  {
    while (peek() != '"') advance();
    advance();
    return Token(Token_Type::string, start + 1, current - start - 2);
  }

  Token scan_token()
  {
    using enum Token_Type;

    Token_Type type = illegal;

    skip_whitespace();
    start = current;

    if (is_at_end())
    {
      return Token(end_of_file, start, current - start);
    }

    char c = advance();

    switch (c)
    {
      case '=':
        {
          if (peek() == '=')
          {
            advance();
            type = eq;
          } else type = assign;
          break;
        }
      case ';':
        {
          type = semicolon;
          break;
        }
      case '(':
        {
          type = lparen;
          break;
        }
      case ')':
        {
          type = rparen;
          break;
        }
      case '{':
        {
          type = lbrace;
          break;
        }
      case '}':
        {
          type = rbrace;
          break;
        }
      case '[':
        {
          if (peek() == ']')
          {
            advance();
            type = arr_brackets;
          } else type = lbracket;
          break;
        }
      case ']':
        {
          type = rbracket;
          break;
        }
      case ',':
        {
          type = comma;
          break;
        }
      case '!':
        {
          if (peek() == '=')
          {
            advance();
            type = neq;
          } else type = bang;
          break;
        }
      case '+':
        {
          type = plus;
          break;
        }
      case '-':
        {
          if (peek() == '>')
          {
            advance();
            type = arrow;
          } else type = minus;
          break;
        }
      case '/':
        {
          type = slash;
          break;
        }
      case '*':
        {
          type = asterisk;
          break;
        }
      case '<':
        {
          type = lt;
          break;
        }
      case '>':
        {
          type = gt;
          break;
        }
      case '.':
        {
          type = dot;
          break;
        }
      case ':':
        {
          char next = peek();
          if (next == '>')
          {
            advance();
            type = arrow;
          }
          else if (next == ':')
          {
            advance();
            type = const_assign;
          } else type = colon;
          break;
        }
      case '"':
        return scan_string();
      default:
        break;
    }

    if (isdigit(c)) return scan_integer();

    if (isalnum(c) || (c == '_')) type = scan_identifier();

    return Token(type, start, current - start);
  }
};

