#pragma once

#define meta(...)
#define tag(...)

enum struct [[meta::stringify]]
Token_Type : int
{
  tag(".illegal") illegal,
  tag(".eof")     end_of_file,

  tag(".ident")   ident,
  tag(".int_num") int_num,
  tag(".string")  string,

  tag(".assign(=)")        assign,
  tag(".const_assign(::)") const_assign,
  tag(".decl_assign(:=)")  decl_assign,

  tag(".plus(+)")     plus,
  tag(".minus(-)")    minus,
  tag(".bang(!)")     bang,
  tag(".asterisk(*)") asterisk,
  tag(".slash(/)")    slash,

  tag(".lt(<)") lt,
  tag(".gt(>)") gt,

  tag(".eq(==)")  eq,
  tag(".neq(!=)") neq,

  tag(".comma(,)")     comma,
  tag(".colon(:)")     colon,
  tag(".semicolon(;)") semicolon,
  tag(".dot(.)")       dot,
  tag(".arrow(->)")    arrow,

  tag(".lparen")   lparen,
  tag(".rparen")   rparen,
  tag(".lbrace")   lbrace,
  tag(".rbrace")   rbrace,
  tag(".lbracket") lbracket,
  tag(".rbracket") rbracket,

  tag(".arr_brackets([])") arr_brackets,

  tag(".fn") function,

  tag(".true")   true_keyword,
  tag(".false")  false_keyword,
  tag(".if")     if_keyword,
  tag(".else")   else_keyword,
  tag(".return") return_keyword,
  tag(".import") import_keyword
};

