D [0-9]
L [a-zA-Z_]

%{
#include <stdio.h>
#include <string.h>

#ifdef WIN32
#include "grammar.tab.h"
#else
#include "y.tab.h"
#endif

void comments();

%}

%%

"//" { comments(); }

"char" { return CHARTYPE ; }
"double" { return DOUBLETYPE; }
"float" { return FLOATTYPE; }
"int" { return INTTYPE; }
"long" { return LONGTYPE; }
"short" { return SHORTTYPE; }
"signed" { return SIGNEDTYPE; }
"struct" { return STRUCT; }
"unsigned" { return UNSIGNEDTYPE; }

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
