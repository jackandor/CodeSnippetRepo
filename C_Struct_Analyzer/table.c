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
