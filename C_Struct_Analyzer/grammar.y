%{
#include <stdio.h>
#include <stdlib.h>

#include "table.h"

extern int yylex (void);
void yyerror(const char *s);
%}

%union {
  char *string;
  struct var_def *var_def;
  struct struct_def *struct_def;
}

%token <string> IDENTIFIER CONSTANT
%token <var_def> CHAR DOUBLE FLOAT INT LONG SHORT SIGNED UNSIGNED
%token STRUCT

%type <var_def> type_specifier specifier_qualifier_list pointer declarator direct_declarator struct_declarator struct_declarator_list struct_declaration struct_declaration_list
%type <struct_def> struct_specifier struct_specifier_list

%start translation_start

%%

translation_start
  : struct_specifier_list { head = $1; }

struct_specifier_list
  : struct_specifier ';' { $$ = $1; }
  | struct_specifier_list struct_specifier ';' {
	struct struct_def *p = $1;
	while(p->next != NULL) p = p->next;
	p->next = $2;
	$$ = $1;
  }

struct_specifier
  : STRUCT IDENTIFIER '{' struct_declaration_list '}' {
    $$ = alloc_struct_def();
	$$->name = $2;
	$$->head = $4;
  }
  | STRUCT '{' struct_declaration_list '}' {
    $$ = alloc_struct_def();
	$$->head = $3;
  }
  | STRUCT IDENTIFIER {
    $$ = alloc_struct_def();
	$$->name = $2;
  }
  ;

struct_declaration_list
  : struct_declaration { $$ = $1; }
  | struct_declaration_list struct_declaration {
    struct var_def *v = $1;
	while(v->next != NULL) v = v->next;
	v->next = $2;
	$$ = $1;
  }
  ;

struct_declaration
  : specifier_qualifier_list struct_declarator_list ';' {
    $$ = $2;
	$$->sign = $1->sign;
	$$->type = $1->type;
	free($1);
  }
  ;

specifier_qualifier_list
  : type_specifier specifier_qualifier_list {
    struct var_def *v1 = $1;
    struct var_def *v2 = $2;
    v2->sign = v1->sign;
    free(v1);
    $$ = v2;
  }
  | type_specifier { $$ = $1; }
  ;

struct_declarator_list
  : struct_declarator { $$ = $1; }
  | struct_declarator_list struct_declarator {
    struct var_def *v1 = $1;
	struct var_def *v2 = $2;
  }
  ;

struct_declarator
  : declarator { $$ = $1;  }
  ;

declarator
  : pointer direct_declarator {
    struct var_def *v1 = $1;
    struct var_def *v2 = $2;
    v2->pointer = v1->pointer;
    free(v1);
    $$ = v2;
  }
  | direct_declarator { $$ = $1; }
  ;

direct_declarator
  : IDENTIFIER {
    $$ = alloc_var_def();
    $$->name = $1;
  }
  | direct_declarator '[' CONSTANT ']' {
    $$ = $1;
	$$->array_size = atoi($3);
	free($3);
  }
  ;

pointer
  : '*' {
    $$ = alloc_var_def();
    $$->pointer = 1;
  }
  | '*' pointer {
    $$ = $2;
    $$->pointer++;
  }
  ;

type_specifier
  : CHAR {
    $$ = alloc_var_def();
    $$->type = CHAR;
  }
  | DOUBLE {
    $$ = alloc_var_def();
    $$->type = DOUBLE;
  }
  | FLOAT {
    $$ = alloc_var_def();
    $$->type = FLOAT;
  }
  | INT {
    $$ = alloc_var_def();
    $$->type = INT;
  }
  | LONG {
    $$ = alloc_var_def();
    $$->type = LONG;
  }
  | SHORT {
    $$ = alloc_var_def();
    $$->type = SHORT;
  }
  | SIGNED {
    $$ = alloc_var_def();
    $$->sign = SIGNED;
    $$->type = INT;
  }
  | UNSIGNED {
    $$ = alloc_var_def();
    $$->sign = UNSIGNED;
    $$->type = INT;
  }
  ;

%%

void yyerror(const char *s)
{
  fflush(stdout);
  fprintf(stderr, "*** %s\n", s);
}
