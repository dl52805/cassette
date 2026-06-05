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

  u32 curr_token_idx;
  u32 peek_token_idx;

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
    using enum Token_Type;

    switch (operator_type)
    {
      case eq:
      case neq:
        return Precedence::equals;
      case lt:
      case gt:
        return Precedence::lessgreater;
      case plus:
      case minus:
        return Precedence::sum;
      case asterisk:
      case slash:
        return Precedence::product;
      default:
        return Precedence::lowest;
    }
  }

  Program parse_program()
  {
    Array<Ast_Stmt *> stmt_arr(alloc);
    while (curr_token().type != Token_Type::end_of_file)
    {
      Ast_Stmt *stmt = parse_stmt();
      if (stmt != nullptr) stmt_arr.append(stmt);
    }

    Program program(stmt_arr);
    return program;
  }

  Token curr_token()
  {
    if (curr_token_idx >= token_stream.length)
    {
      Token err;
      err.length = -1;
      return err;
    }
    return token_stream[curr_token_idx];
  }

  Token peek_token()
  {
    if (peek_token_idx >= token_stream.length)
    {
      Token err;
      err.length = -1;
      return err;
    }
    return token_stream[peek_token_idx];
  }

  Parser(String8_View sv, Array<Token> tokens,
         Allocator *allocator = new Libc_Alloc())
  {
    source = sv;
    token_stream = tokens;
    alloc = allocator;
    curr_token_idx = 0;
    peek_token_idx = 1;
  }

  void next_token()
  {
    curr_token_idx = peek_token_idx;
    peek_token_idx += 1;
  }

  bool expect(Token_Type type)
  {
    if (curr_token().type == type)
    {
      next_token();
      return true;
    }
    return false;
  }

  bool expect_peek(Token_Type type)
  {
    if (peek_token().type == type)
    {
      next_token();
      return true;
    }
    return false;
  }

  void match_or_err(Token token, Token_Type type, const char *err_msg)
  {
    if (token.type != type)
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  void expect_or_err(Token_Type type, const char *err_msg)
  {
    if (!expect(type))
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  void expect_peek_or_err(Token_Type type, const char *err_msg)
  {
    if (!expect_peek(type))
    {
      fprintf(stderr, "%s%s%s\n", red, err_msg, reset);
      exit(0);
    }
  }

  Ast_Stmt *parse_stmt()
  {
    using enum Token_Type;

    Token curr = curr_token();
    Token peek = peek_token();

    switch (curr.type)
    {
      case ident:
        switch (peek.type)
        {
          case const_assign:
          case decl_assign:
            return parse_decl();
          default:
            return parse_expr_stmt();
        }
      case return_keyword:
        return parse_return_stmt();
      case if_keyword:
        return parse_if_stmt();
      default:
        return parse_expr_stmt();
    }
  }

  Ast_Stmt *parse_decl()
  {
    using enum Token_Type;

    Token curr = curr_token();
    Token peek = peek_token();
    Token ident_token = curr;

    next_token();
    next_token();

    Ast_Expr *rhs = nullptr;

    bool type_expr = false;

    curr = curr_token();
    switch (curr.type)
    {
      case Token_Type::function:
        {
          type_expr = true;
          match_or_err(peek, const_assign, "functions must be const declared");
          next_token();
          expect_or_err(lparen, "expected opening parentheses");
          Array<Ast_Expr *> params = parse_params_list();
          expect_or_err(rparen, "expected closing parentheses");

          Ast_Expr *return_type = nullptr;
          if (curr_token().type == arrow)
          {
            next_token();
            return_type = parse_expr(Precedence::lowest);
            next_token();
          }

          expect_or_err(lbrace, "expected opening brace");
          Ast_Block_Stmt *body = parse_block_stmt();
          expect_or_err(rbrace, "expected closing brace");

          rhs = Ast_Fn_Decl::init(params, return_type, body, alloc);

          break;
        }
      default:
        {
          rhs = parse_expr(Precedence::lowest);
          next_token();
          expect_or_err(semicolon, "expected semicolon");
          break;
        }
    }

    switch (peek.type)
    {
      case const_assign:
        return Ast_Const_Decl::init(ident_token, rhs, alloc);
      case decl_assign:
        return Ast_Infer_Decl::init(ident_token, rhs, alloc);
      default:
        return nullptr;
    }
  }

  Array<Ast_Expr *> parse_params_list()
  {
    using enum Token_Type;

    Array<Ast_Expr *> params_expr(16, alloc);

    while (true)
    {
      Ast_Expr *param = parse_type_decl();
      params_expr.append(param);

      Token curr = curr_token();
      if (curr.type == rparen) break;
      else expect_or_err(comma, "expected comma between parentheses");
    }

    return params_expr;
  }

  Ast_Stmt *parse_if_stmt()
  {
    using enum Token_Type;

    next_token();
    expect_or_err(lparen, "expected opening parentheses");

    Ast_Expr *condition = parse_expr(Precedence::lowest);

    next_token();
    expect_or_err(rparen, "expected closing parentheses");
    expect_or_err(lbrace, "expected opening brace");

    Ast_Block_Stmt *consequence = parse_block_stmt();
    expect_or_err(rbrace, "expected closing brace");

    Ast_Block_Stmt *alternative = nullptr;

    if (curr_token().type == else_keyword)
    {
      next_token();
      expect_or_err(lbrace, "expected opening brace");
      alternative = parse_block_stmt();
      expect_or_err(rbrace, "expected closing brace");
    }

    return Ast_If_Stmt::init(condition, consequence, alternative, alloc);
  }

  Ast_Block_Stmt *parse_block_stmt()
  {
    using enum Token_Type;

    Array<Ast_Stmt *> statements(alloc);

    while (true)
    {
      Token curr = curr_token();
      Token peek = peek_token();

      if (curr.type == rbrace) break;

      Ast_Stmt *stmt = parse_stmt();
      statements.append(stmt);
    }

    return Ast_Block_Stmt::init(statements, alloc);
  }

  Ast_Stmt *parse_return_stmt()
  {
    using enum Token_Type;

    next_token();
    Ast_Expr *rhs = parse_expr(Precedence::lowest);
    next_token();
    expect_or_err(semicolon, "expected semicolon");
    return Ast_Return::init(rhs, alloc);
  }

  Ast_Stmt *parse_expr_stmt()
  {
    using enum Token_Type;

    Token curr = curr_token();
    Ast_Expr *expr = parse_expr(Precedence::lowest);
    next_token();
    expect_or_err(semicolon, "expected semicolon");
    return Ast_Expr_Stmt::init(expr, alloc);
  }

  Ast_Expr *parse_type_decl()
  {
    using enum Token_Type;

    Token curr = curr_token();
    expect_or_err(ident, "expected identifier");
    Token ident_token = curr;
    expect_or_err(colon, "expected colon");
    Ast_Expr *type = parse_expr(Precedence::lowest);
    next_token();
    return Ast_Type_Decl::init(ident_token, type, alloc);
  }

  Ast_Expr *parse_expr(Precedence prec)
  {
    using enum Token_Type;

    Token curr = curr_token();
    Ast_Expr *left = nullptr;

    switch (curr.type)
    {
      case int_num:
        left = Ast_Int_Literal::init(curr, source, alloc);
        break;
      case ident:
        left = Ast_Ident_Expr::init(curr, alloc);
        break;
      case bang:
      case minus:
        next_token();
        left = parse_expr(Precedence::lowest);
        next_token();
        match_or_err(curr_token(), rparen,
                     "no closing parentheses");
        break;
      default:
        break;
    }

    while (true)
    {
      Token curr = curr_token();
      Token peek = peek_token();

      if ((peek.type == semicolon)
          || (peek.type == rparen)
          || (peek.type == comma)
          || (prec >= get_prec(peek.type)))
      {
        break;
      }

      switch (peek.type)
      {
        case plus:
        case minus:
        case asterisk:
        case slash:
        case lt:
        case gt:
        case eq:
        case neq:
          next_token();
          left = parse_binary_expr(left);
          break;
        default:
          return left;
      }
    }

    return left;
  }

  Ast_Expr *parse_binary_expr(Ast_Expr *left)
  {
    Token curr = curr_token();
    Precedence prec = get_prec(curr.type);
    next_token();
    Ast_Expr *right = parse_expr(prec);
    return Ast_Binary_Expr::init(curr, left, right, alloc);
  }
};

