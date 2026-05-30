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
    Token_Type type = Token_Type::illegal;

    skip_whitespace();
    start = current;

    if (is_at_end())
    {
      return Token(Token_Type::end_of_file, start, current - start);
    }

    char c = advance();

    switch (c)
    {
      case '=':
        {
          if (peek() == '=')
          {
            advance();
            type = Token_Type::eq;
          } else type = Token_Type::assign;
          break;
        }
      case ';':
        {
          type = Token_Type::semicolon;
          break;
        }
      case '(':
        {
          type = Token_Type::lparen;
          break;
        }
      case ')':
        {
          type = Token_Type::rparen;
          break;
        }
      case '{':
        {
          type = Token_Type::lbrace;
          break;
        }
      case '}':
        {
          type = Token_Type::rbrace;
          break;
        }
      case '[':
        {
          if (peek() == ']')
          {
            advance();
            type = Token_Type::arr_brackets;
          } else type = Token_Type::lbracket;
          break;
        }
      case ']':
        {
          type = Token_Type::rbracket;
          break;
        }
      case ',':
        {
          type = Token_Type::comma;
          break;
        }
      case '!':
        {
          if (peek() == '=')
          {
            advance();
            type = Token_Type::neq;
          } else type = Token_Type::bang;
          break;
        }
      case '+':
        {
          type = Token_Type::plus;
          break;
        }
      case '-':
        {
          if (peek() == '>')
          {
            advance();
            type = Token_Type::arrow;
          } else type = Token_Type::minus;
          break;
        }
      case '/':
        {
          type = Token_Type::slash;
          break;
        }
      case '*':
        {
          type = Token_Type::asterisk;
          break;
        }
      case '<':
        {
          type = Token_Type::lt;
          break;
        }
      case '>':
        {
          type = Token_Type::gt;
          break;
        }
      case '.':
        {
          type = Token_Type::dot;
          break;
        }
      case ':':
        {
          char next = peek();
          if (next == '>')
          {
            advance();
            type = Token_Type::arrow;
          }
          else if (next == ':')
          {
            advance();
            type = Token_Type::const_assign;
          } else type = Token_Type::colon;
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

