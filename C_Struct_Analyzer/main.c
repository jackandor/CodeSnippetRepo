#include <stdio.h>
#include <crtdbg.h>
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
      if(v->sign == UNSIGNEDTYPE)
        printf("unsigned ");
      else if (v->sign == SIGNEDTYPE)
        printf("signed ");
      switch(v->type) {
      case CHARTYPE:
          printf("char ");
          break;
      case SHORTTYPE:
          printf("short ");
          break;
      case INTTYPE:
          printf("int ");
          break;
      case LONGTYPE:
          printf("long ");
          break;
      case FLOATTYPE:
          printf("float ");
          break;
      case DOUBLETYPE:
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

char buffer[10 * 1024 * 1024];
int offset;
#define print(fmt, ...) offset += sprintf(&buffer[offset], fmt, ##__VA_ARGS__)

int indent;

void print_indent()
{
  for(int i = 0; i < indent; i++)
    print("  ");
}

#define println(fmt, ...) do { \
  print_indent(); \
  print(fmt, ##__VA_ARGS__); \
  print("\n"); \
} while(0)

void gen_name_list(struct var_def *v)
{
  println("const char *name_list[] = {");
  indent++;
  while(v != NULL) {
    println("\"%s\",", v->name);
    v = v->next;
  }
  indent--;
  println("};");
}

void gen_get_value_from_idx(char *struct_name, struct var_def *v)
{
  println("char *get_value_from_idx(%s *p, char *field_name, int subscript)", struct_name);
  println("{");
  indent++;
  println("char buf[100];");
  println("memset(buf, 0, sizeof(buf));");
  struct var_def *first = v;
  while(v != NULL) {
    print_indent();
    if(first != v)
      print("else ");
    switch(v->type) {
    case CHARTYPE:
    case SHORTTYPE:
    case INTTYPE:
    case LONGTYPE:
        if(v->array_size >0)
          print("if(!strcmp(field_name, \"%s\")) sprintf(buf, \"%%d\", p->%s[subscript]);\n", v->name, v->name);
        else
          print("if(!strcmp(field_name, \"%s\")) sprintf(buf, \"%%d\", p->%s);\n", v->name, v->name);
        break;
    case FLOATTYPE:
    case DOUBLETYPE:
        if(v->array_size >0)
          print("if(!strcmp(field_name, \"%s\")) sprintf(buf, \"%%f\", p->%s[subscript]);\n", v->name, v->name);
        else
          print("if(!strcmp(field_name, \"%s\")) sprintf(buf, \"%%f\", p->%s);\n", v->name, v->name);
        break;
      default:
        break;
    }
    println("else sprintf(buf, \"\");");
    println("return strdup(buf);");
    v = v->next;
  }
  indent--;
  println("}");
}

int main(int argc, char *argv[])
{
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
  //_CrtSetBreakAlloc(81);
  if(argc < 2) {
    printf("Usage: analyzer <file>\n");
    return -1;
  }
  yyin = fopen(argv[1], "r");
  yyparse();
  yylex_destroy();
  print_struct(head);
/*
  struct struct_def *s = head;
  while(s != NULL) {
    gen_name_list(s->head);
    gen_get_value_from_idx(s->name, s->head);
    s = s->next;
  }
  printf("%s", buffer);
*/
  free_all(head);
  return 0;
}
