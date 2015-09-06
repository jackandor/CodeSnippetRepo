#include "y.tab.h"
#ifdef __cplusplus
extern "C" {
#endif

struct var_def {
  enum yytokentype sign;
  enum yytokentype type;
  int pointer;
  int array_size;
  char *name;
  struct var_def *next;
};

struct struct_def {
  char *name;
  struct var_def *head;
  struct struct_def *next;
};

extern struct var_def *alloc_var_def();
extern struct struct_def *alloc_struct_def();

extern struct struct_def *head;
#ifdef __cplusplus
}
#endif
