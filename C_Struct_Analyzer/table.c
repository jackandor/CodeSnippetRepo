#include <stdlib.h>

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

struct struct_def *head;