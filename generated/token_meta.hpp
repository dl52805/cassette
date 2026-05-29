#pragma once

#include "token.hpp"

enum struct Token_Type : int;
static const char *str_from_token_type(Token_Type e)
{
  switch (e)
  {
    case Token_Type::illegal:
      return ".illegal";
    case Token_Type::end_of_file:
      return ".eof";
    case Token_Type::ident:
      return ".ident";
    case Token_Type::int_num:
      return ".int_num";
    case Token_Type::string:
      return ".string";
    case Token_Type::assign:
      return ".assign(=)";
    case Token_Type::const_assign:
      return ".const_assign(::)";
    case Token_Type::decl_assign:
      return ".decl_assign(:=)";
    case Token_Type::plus:
      return ".plus(+)";
    case Token_Type::minus:
      return ".minus(-)";
    case Token_Type::bang:
      return ".bang(!)";
    case Token_Type::asterisk:
      return ".asterisk(*)";
    case Token_Type::slash:
      return ".slash(/)";
    case Token_Type::lt:
      return ".lt(<)";
    case Token_Type::gt:
      return ".gt(>)";
    case Token_Type::eq:
      return ".eq(==)";
    case Token_Type::neq:
      return ".neq(!=)";
    case Token_Type::comma:
      return ".comma(,)";
    case Token_Type::colon:
      return ".colon(:)";
    case Token_Type::semicolon:
      return ".semicolon(;)";
    case Token_Type::dot:
      return ".dot(.)";
    case Token_Type::arrow:
      return ".arrow(->)";
    case Token_Type::lparen:
      return ".lparen";
    case Token_Type::rparen:
      return ".rparen";
    case Token_Type::lbrace:
      return ".lbrace";
    case Token_Type::rbrace:
      return ".rbrace";
    case Token_Type::lbracket:
      return ".lbracket";
    case Token_Type::rbracket:
      return ".rbracket";
    case Token_Type::arr_brackets:
      return ".arr_brackets([])";
    case Token_Type::function:
      return ".fn";
    case Token_Type::true_keyword:
      return ".true";
    case Token_Type::false_keyword:
      return ".false";
    case Token_Type::if_keyword:
      return ".if";
    case Token_Type::else_keyword:
      return ".else";
    case Token_Type::return_keyword:
      return ".return";
    case Token_Type::import_keyword:
      return ".import";
  }
  return "";
}

static int token_type_max_field_len()
{
  return 17;
}

