#pragma once

#include "token.hpp"
#include "string.hpp"
#include "array.hpp"

struct Ast_Node {};

struct Ast_Stmt : Ast_Node
{
  enum struct Kind : int
  {
    const_decl,
    infer_decl,
    return_stmt,
    expr_stmt,
    if_stmt,
    block_stmt,
  };
  Kind kind;

  Ast_Stmt() {}
  Ast_Stmt(Kind kind) : kind(kind) {}

  template<typename T>
  bool is_stmt()
  {
    if constexpr (__is_base_of(T, Ast_Stmt))
    {
      return kind == T::node_kind;
    }
  }

  template<typename T>
  const T *to_stmt()
  {
    if constexpr (__is_base_of(T, Ast_Stmt))
    {
      return is_stmt<T>() ? static_cast<const T *>(this) : nullptr;
    }
  }
};

struct Ast_Expr : Ast_Node
{
  enum struct Kind : int
  {
    int_literal,
    ident_expr,
    type_decl,
    fn_decl,
    binary_expr,
    unary_expr,
  };
  Kind kind;

  Ast_Expr() {}
  Ast_Expr(Kind kind) : kind(kind) {}

  template<typename T>
  bool is_expr()
  {
    if constexpr (__is_base_of(T, Ast_Expr))
    {
      return kind == T::node_kind;
    }
  }

  template<typename T>
  const T *to_expr()
  {
    if constexpr (__is_base_of(T, Ast_Expr))
    {
      return is_expr<T>() ? static_cast<const T *>(this) : nullptr;
    }
  }
};

struct Ast_Const_Decl : Ast_Stmt
{
  Token ident;
  Ast_Expr *rhs;

  constexpr static Kind node_kind = Kind::const_decl;

  static Ast_Const_Decl *init(Token ident, Ast_Expr *rhs, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Const_Decl)).unwrap();
    Ast_Const_Decl *decl = (Ast_Const_Decl *) ast_obj;
    decl->ident = ident;
    decl->rhs = rhs;
    decl->kind = node_kind;
    return decl;
  }
};

struct Ast_Infer_Decl : Ast_Stmt
{
  Token ident;
  Ast_Expr *rhs;

  constexpr static Kind node_kind = Kind::infer_decl;

  static Ast_Infer_Decl *init(Token ident, Ast_Expr *rhs, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Infer_Decl)).unwrap();
    Ast_Infer_Decl *decl = (Ast_Infer_Decl *) ast_obj;
    decl->ident = ident;
    decl->rhs = rhs;
    decl->kind = node_kind;
    return decl;
  }
};

struct Ast_Return : Ast_Stmt
{
  Ast_Expr *rhs;

  constexpr static Kind node_kind = Kind::return_stmt;

  static Ast_Return *init(Ast_Expr *rhs, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Return)).unwrap();
    Ast_Return *return_stmt = (Ast_Return *) ast_obj;
    return_stmt->rhs = rhs;
    return_stmt->kind = node_kind;
    return return_stmt;
  }
};

struct Ast_Expr_Stmt : Ast_Stmt
{
  Ast_Expr *expr;

  constexpr static Kind node_kind = Kind::expr_stmt;

  static Ast_Expr_Stmt *init(Ast_Expr *expr, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Expr_Stmt)).unwrap();
    Ast_Expr_Stmt *expr_stmt = (Ast_Expr_Stmt *) ast_obj;
    expr_stmt->expr = expr;
    expr_stmt->kind = node_kind;
    return expr_stmt;
  }
};

struct Ast_Block_Stmt : Ast_Stmt
{
  Array<Ast_Stmt *> stmts;

  constexpr static Kind node_kind = Kind::block_stmt;

  static Ast_Block_Stmt *init(Array<Ast_Stmt *> stmts, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Block_Stmt)).unwrap();
    Ast_Block_Stmt *block_stmt = (Ast_Block_Stmt *) ast_obj;
    block_stmt->stmts = stmts;
    block_stmt->kind = node_kind;
    return block_stmt;
  }
};

struct Ast_If_Stmt : Ast_Stmt
{
  Ast_Expr *condition;
  Ast_Block_Stmt *consequence;
  Ast_Block_Stmt *alternative;

  constexpr static Kind node_kind = Kind::if_stmt;

  static Ast_If_Stmt *init(Ast_Expr *condition, Ast_Block_Stmt *consequence,
                           Ast_Block_Stmt *alternative, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_If_Stmt)).unwrap();
    Ast_If_Stmt *if_stmt = (Ast_If_Stmt *) ast_obj;
    if_stmt->condition = condition;
    if_stmt->consequence = consequence;
    if_stmt->alternative = alternative;
    if_stmt->kind = node_kind;
    return if_stmt;
  }
};

struct Ast_Int_Literal : Ast_Expr
{
  Token token;
  int value;

  constexpr static Kind node_kind = Kind::int_literal;

  static Ast_Int_Literal *init(Token token, String8_View source,
                               Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Int_Literal)).unwrap();
    Ast_Int_Literal *int_literal = (Ast_Int_Literal *) ast_obj;

    // any 64-bit number can have at most 20 digits in decimal
    char num[21] = {0};
    strncpy(num, (char *) &source.buffer[token.position], token.length);
    int value = atoi(num);

    int_literal->token = token;
    int_literal->value = value;
    int_literal->kind = node_kind;
    return int_literal;
  }
};

struct Ast_Ident_Expr : Ast_Expr
{
  Token ident;

  constexpr static Kind node_kind = Kind::ident_expr;

  static Ast_Ident_Expr *init(Token ident, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Ident_Expr)).unwrap();
    Ast_Ident_Expr *ident_epxr = (Ast_Ident_Expr *) ast_obj;
    ident_epxr->ident = ident;
    ident_epxr->kind = node_kind;
    return ident_epxr;
  }
};

struct Ast_Type_Decl : Ast_Expr
{
  Token ident;
  Ast_Expr *type;

  constexpr static Kind node_kind = Kind::type_decl;

  static Ast_Type_Decl *init(Token ident, Ast_Expr *type, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Type_Decl)).unwrap();
    Ast_Type_Decl *type_decl = (Ast_Type_Decl *) ast_obj;
    type_decl->ident = ident;
    type_decl->type = type;
    type_decl->kind = node_kind;
    return type_decl;
  }
};

struct Ast_Fn_Decl : Ast_Expr
{
  Array<Ast_Expr *> params;
  Ast_Expr *return_type;
  Ast_Block_Stmt *body;

  constexpr static Kind node_kind = Kind::fn_decl;

  static Ast_Fn_Decl *init(Array<Ast_Expr *> params, Ast_Expr *return_type,
                           Ast_Block_Stmt *body, Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Fn_Decl)).unwrap();
    Ast_Fn_Decl *fn_decl = (Ast_Fn_Decl *) ast_obj;
    fn_decl->params = params;
    fn_decl->return_type = return_type;
    fn_decl->body = body;
    fn_decl->kind = node_kind;
    return fn_decl;
  }
};

struct Ast_Binary_Expr : Ast_Expr
{
  Token expr_operator;
  Ast_Expr *lhs;
  Ast_Expr *rhs;

  constexpr static Kind node_kind = Kind::binary_expr;

  static Ast_Binary_Expr *init(Token expr_operator,
                               Ast_Expr *lhs, Ast_Expr *rhs,
                               Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Binary_Expr)).unwrap();
    Ast_Binary_Expr *binary_expr = (Ast_Binary_Expr *) ast_obj;
    binary_expr->expr_operator = expr_operator;
    binary_expr->lhs = lhs;
    binary_expr->rhs = rhs;
    binary_expr->kind = node_kind;
    return binary_expr;
  }
};

struct Ast_Unary_Expr : Ast_Expr
{
  Token expr_operator;
  Ast_Expr *rhs;

  constexpr static Kind node_kind = Kind::unary_expr;

  static Ast_Unary_Expr *init(Token expr_operator, Ast_Expr *rhs,
                              Allocator *allocator)
  {
    void *ast_obj = allocator->allocate(sizeof(Ast_Unary_Expr)).unwrap();
    Ast_Unary_Expr *unary_expr = (Ast_Unary_Expr *) ast_obj;
    unary_expr->expr_operator = expr_operator;
    unary_expr->rhs = rhs;
    unary_expr->kind = node_kind;
    return unary_expr;
  }
};


