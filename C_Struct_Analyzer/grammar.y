%token IDENTIFIER CONSTANT
%token CHAR DOUBLE FLOAT INT LONG SHORT SIGNED UNSIGNED
%token STRUCT

%start struct_specifier_list

%{
#include <stdio.h>

void yyerror(const char *s);
%}

%%

struct_specifier_list
  : struct_specifier ';'
  | struct_specifier_list struct_specifier ';'

struct_specifier
  : STRUCT IDENTIFIER '{' struct_declaration_list '}'
  | STRUCT '{' struct_declaration_list '}'
  | STRUCT IDENTIFIER
  ;

struct_declaration_list
  : struct_declaration
  | struct_declaration_list struct_declaration
  ;

struct_declaration
  : specifier_qualifier_list struct_declarator_list ';'
  ;

specifier_qualifier_list
  : type_specifier specifier_qualifier_list
  | type_specifier
  ;

struct_declarator_list
  : struct_declarator
  | struct_declarator_list struct_declarator
  ;

struct_declarator
  : declarator
  ;

declarator
  : pointer direct_declarator
  | direct_declarator
  ;

direct_declarator
  : IDENTIFIER
  | direct_declarator '[' CONSTANT ']'
  ;

pointer
  : '*'
  | '*' pointer
  ;

type_specifier
  : CHAR
  | DOUBLE
  | FLOAT
  | INT
  | LONG
  | SHORT
  | SIGNED
  | UNSIGNED
  ;

%%

void yyerror(const char *s)
{
  fflush(stdout);
  fprintf(stderr, "*** %s\n", s);
}
