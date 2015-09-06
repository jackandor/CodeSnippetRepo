D [0-9]
L [a-zA-Z_]

%{
#include <stdio.h>
#include "grammar.tab.h"

void comments();
%}

%%

"//" { comments(); }

"char" { return CHAR ; }
"double" { return DOUBLE; }
"float" { return FLOAT; }
"int" { return INT; }
"long" { return LONG; }
"short" { return SHORT; }
"signed" { return SIGNED; }
"struct" { return STRUCT; }
"unsigned" { return UNSIGNED; }

{L}({L}|{D})* { yylval.string = strdup(yytext); return IDENTIFIER; }
{D}+ { yylval.string = strdup(yytext); return CONSTANT; }

";" { return ';'; }
"*" { return '*'; }
"[" { return '['; }
"]" { return ']'; }
"{" { return '{'; }
"}" { return '}'; }

[ \t\v\n\f] { /* ignore blank */}

%%

int yywrap()
{
  return 1;
}

void comments()
{
  char c;
  while((c = input()) != '\n');
}
