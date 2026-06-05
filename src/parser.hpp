#pragma once

#include "array.hpp"
#include "string.hpp"
#include "token.hpp"

#include "ast.hpp"

void print_token(Token token, String8_View source);

struct Program
{
  Array<Ast_Stmt *> stmts;

  Program(Array<Ast_Stmt *> stmts)
  {
    this->stmts = stmts;
  }
};

struct Parser
{
  String8_View source;
  Array<Token> token_stream;

  u32 curr_token;
  u32 peek_token;

  Allocator *alloc;

  enum struct Precedence
  {
    lowest      = 1,
    equals      = 2, // =
    lessgreater = 3, // <, >
    sum         = 4, // +, -
    product     = 5, // *, /
    prefix      = 6, // -x
    call        = 7, // f(a)
  };

  constexpr Precedence get_prec(Token_Type operator_type)
  {
    switch (operator_type)
    {
      case Token_Type::eq:
      case Token_Type::neq:
        return Precedence::equals;
      case Token_Type::lt:
      case Token_Type::gt:
        return Precedence::lessgreater;
      case Token_Type::plus:
      case Token_Type::minus:
        return Precedence::sum;
      case Token_Type::asterisk:
      case Token_Type::slash:
        return Precedence::product;
      default:
        return Precedence::lowest;
    }
  }

  Program parse_program()
  {
    Array<Ast_Stmt *> stmt_arr(alloc);
    while (curr().type != Token_Type::end_of_file)
    {
      Ast_Stmt *stmt = parse_stmt();
      if (stmt != nullptr) stmt_arr.append(stmt);
    }

    Program program(stmt_arr);
    return program;
  }

  Token curr()
  {
    if (curr_token >= token_stream.length)
    {
      Token err;
      err.length = -1;
      return err;
    }
    return token_stream[curr_token];
  }

  Token peek()
  {
    if (peek_token >= token_stream.length)
    {
      Token err;
      err.length = -1;
      return err;
    }
    return token_stream[peek_token];
  }

  Parser(String8_View sv, Array<Token> tokens,
         Allocator *allocator = new Libc_Alloc())
  {
    source = sv;
    token_stream = tokens;
    alloc = allocator;
    curr_token = 0;
    peek_token = 1;
  }

  void next()
  {
    curr_token = peek_token;
    peek_token += 1;
  }

  bool expect(Token_Type type)
  {
    if (curr().type == type)
    {
      next();
      return true;
    }
    return false;
  }

  bool expect_peek(Token_Type type)
  {
    if (peek().type == type)
    {
      next();
      return true;
    }
    return false;
  }

  void match_or_err(Token token, Token_Type type, char *err_msg)
  {
    if (token.type != type)
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  void expect_or_err(Token token, Token_Type type, char *err_msg)
  {
    if (!expect(type))
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  void expect_peek_or_err(Token token, Token_Type type, char *err_msg)
  {
    if (!expect_peek(type))
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  Ast_Stmt *parse_stmt()
  {
    Token curr_token = curr();
    Token peek_token = peek();

    return nullptr;
  }
};

