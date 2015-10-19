#include <stdlib.h>
#include <string.h>

#include "table.h"

struct var_def *alloc_var_def()
{
	struct var_def *p = malloc(sizeof(struct var_def));
	memset(p, 0, sizeof(struct var_def));
  return p;
}

struct struct_def *alloc_struct_def()
{
  struct struct_def *p = malloc(sizeof(struct struct_def));
  memset(p, 0, sizeof(struct struct_def));
  return p;
}

void free_all(struct struct_def *h)
{
  while (h != NULL) {
    struct struct_def *p = h;
    h = h->next;
    struct var_def *v = p->head;
    while (v != NULL) {
      struct var_def *p = v;
      v = v->next;
      free(p->name);
      free(p);
    }
    free(p->name);
    free(p);
  }
}

struct struct_def *head;

struct struct_def *find_struct(const char *name)
{
    struct struct_def *p = head;
    while(strcmp(name, p->name)) p = p->next;
    return p;
}

int size_of_struct(struct struct_def *sd)
{
    int ret = 0;
    struct var_def *v = sd->head;
    while(!v) {
        int sz = 0;
        if(v->pointer)
            sz = 4;
        else {
            switch(v->type) {
            case CHARTYPE:
                sz = 1;
                break;
            case SHORTTYPE:
                sz = 2;
                break;
            case INTTYPE:
            case LONGTYPE:
            case FLOATTYPE:
                sz = 4;
                break;
            case DOUBLETYPE:
                sz = 8;
                break;
            }
        }
        if(v->array_size)
            sz *= v->array_size;
        ret += sz;
        v = v->next;
    }
    return ret;
}
