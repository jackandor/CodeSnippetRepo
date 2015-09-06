#include <stdio.h>
#include "table.h"

extern FILE *yyin;
extern int yyparse ();

void print_struct(struct struct_def *s)
{
  while(s != NULL) {
    printf("\nstruct %s {\n", s->name);
    struct var_def *v = s->head;
    while(v != NULL) {
      printf("\t");
      if(v->sign == UNSIGNED)
        printf("unsigned ");
      else if(v->sign == SIGNED)
        printf("signed ");
      switch(v->type) {
        case CHAR:
          printf("char ");
          break;
        case SHORT:
          printf("short ");
          break;
        case INT:
          printf("int ");
          break;
        case LONG:
          printf("long ");
          break;
        case FLOAT:
          printf("float ");
          break;
        case DOUBLE:
          printf("double ");
          break;
        default:
          break;
      }
      int pointer = v->pointer;
      while(pointer--) printf("*");
      printf("%s", v->name);
      if(v->array_size > 0)
        printf("[%d]", v->array_size);
      printf(";\n");
      v = v->next;
    }
    printf("};\n");
    s = s->next;
  }
}

int main(int argc, char *argv[])
{
  if(argc < 2) {
    printf("Usage: analyzer <file>\n");
    return -1;
  }
  yyin = fopen(argv[1], "r");
  yyparse();
  print_struct(head);
  return 0;
}
