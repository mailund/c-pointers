#include <stdio.h>
#include <stdlib.h>

typedef void * cls;
typedef struct { cls *cls; } obj;

#define basetype(x, base) ((base *)x)
#define vtbl(inst, base) \
          basetype(((obj *)inst)->cls, base)

void *alloc_cls(size_t cls_size,
                void (*cls_init)(cls *))
{
  cls *cls = malloc(cls_size);
  if (!cls) abort(); // error handling
  cls_init(cls);
  return cls;
}

#define INIT_CLS(p, init)                    \
  do {                                       \
    if (!p)                                  \
      p = alloc_cls(                         \
            sizeof *p, (void (*)(cls *))init \
          );                                 \
  } while(0)

void *alloc_obj(size_t obj_size, cls cls)
{
  obj *obj = malloc(obj_size);
  if (!obj) abort(); // error handling
  obj->cls = cls;
  return obj;
}
#define NEW_OBJ(p, cls) alloc_obj(sizeof *p, cls)


// Expressions .................

typedef struct base_expr *EXP;

// Base class, class definition
typedef struct {
  void   (*print)(EXP);
  double (*eval) (EXP);
} base_expr_cls;

void print_expr(EXP e) {
  vtbl(e, base_expr_cls)->print(e);
}
double eval_expr(EXP e) {
  return vtbl(e, base_expr_cls)->eval(e);
}

void init_base_expr_cls(base_expr_cls *cls)
{
  cls->print = 0; // abstract method
  cls->eval  = 0; // abstract method
}

// Base class, object definition
typedef struct base_expr {
  obj obj;
} base_expr;

// Base class, methods
void init_base_expr(base_expr *inst)
{
  // nothing to initialise...
}


// Value expressions
typedef struct  {
  base_expr_cls base_expr_cls;
} value_expr_cls;
typedef struct {
  base_expr base_expr;
  double value;
} value_expr;

// Concrete class, so must have a struct
value_expr_cls *VALUE_EXPR_CLS; // must be initialised

void value_expr_print(value_expr *val)
{
  printf("%.3f", val->value);
}

double value_expr_eval(value_expr *val)
{
  return val->value;
}

void init_value_expr_cls(value_expr_cls *cls)
{
  init_base_expr_cls(basetype(cls, base_expr_cls));
  basetype(cls, base_expr_cls)->print =
            (void (*)(EXP))value_expr_print;
  basetype(cls, base_expr_cls)->eval  =
            (double (*)(EXP))value_expr_eval;
}

void init_value_expr(value_expr *val, double value)
{
  init_base_expr(basetype(val, base_expr));
  val->value = value;
}

// constructor
EXP value(double value)
{
  INIT_CLS(VALUE_EXPR_CLS, init_value_expr_cls);
  value_expr *val = NEW_OBJ(val, VALUE_EXPR_CLS);
  init_value_expr(val, value);
  return (EXP)val;
}

// Binary expressoins
typedef struct {
  base_expr_cls base_expr_cls;
} binary_expr_cls;
typedef struct {
  base_expr base_expr;
  char symb;
  EXP left, right;
} binary_expr;

void print_binary_expr(binary_expr *binop)
{
  putchar('(');
    print_expr(binop->left);
  putchar(')');
  putchar(binop->symb);
  putchar('(');
    print_expr(binop->right);
  putchar(')');
}

void init_binary_expr_cls(binary_expr_cls *cls)
{
  init_base_expr_cls(basetype(cls, base_expr_cls));
  basetype(cls, base_expr_cls)->print =
          (void (*)(EXP))print_binary_expr;
}

void init_binary_expr(binary_expr *binop,
                      char symb, EXP left, EXP right)
{
  init_base_expr(basetype(binop, base_expr));
  binop->symb = symb;
  binop->left = left;
  binop->right = right;
}

// Addition
typedef struct {
  binary_expr_cls binary_expr_cls;
} add_expr_cls;
typedef struct {
  binary_expr binary_expr;
} add_expr;

// Concrete class, so must have a struct
add_expr_cls *ADD_EXPR_CLS; // must be initialised


double eval_add_expr(add_expr *expr)
{
  return eval_expr(basetype(expr, binary_expr)->left) +
         eval_expr(basetype(expr, binary_expr)->right);
}

void init_add_expr_cls(add_expr_cls *cls)
{
  init_binary_expr_cls(basetype(cls, binary_expr_cls));
  basetype(cls, base_expr_cls)->eval =
          (double (*)(EXP))eval_add_expr;
}

void init_add_expr(add_expr *expr, EXP left, EXP right)
{
  init_binary_expr(basetype(expr, binary_expr),
                   '+', left, right);
}

// constructor
EXP add(EXP left, EXP right)
{
  INIT_CLS(ADD_EXPR_CLS, init_add_expr_cls);
  add_expr *expr = NEW_OBJ(expr, ADD_EXPR_CLS);
  init_add_expr(expr, left, right);
  return (EXP)expr;
}


// Subtraction
typedef struct {
  binary_expr_cls binary_expr_cls;
} sub_expr_cls;
typedef struct {
  binary_expr binary_expr;
} sub_expr;

// Concrete class, so must have a struct
sub_expr_cls *SUB_EXPR_CLS; // must be initialised


double eval_sub_expr(sub_expr *expr)
{
  return eval_expr(basetype(expr, binary_expr)->left) -
         eval_expr(basetype(expr, binary_expr)->right);
}

void init_sub_expr_cls(sub_expr_cls *cls)
{
  init_binary_expr_cls(basetype(cls, binary_expr_cls));
  basetype(cls, base_expr_cls)->eval =
          (double (*)(EXP))eval_sub_expr;
}

void init_sub_expr(add_expr *expr, EXP left, EXP right)
{
  init_binary_expr(basetype(expr, binary_expr),
                   '-', left, right);
}

// constructor
EXP sub(EXP left, EXP right)
{
  INIT_CLS(SUB_EXPR_CLS, init_sub_expr_cls);
  add_expr *expr = NEW_OBJ(expr, SUB_EXPR_CLS);
  init_sub_expr(expr, left, right);
  return (EXP)expr;
}

int main(void)
{
  EXP expr =
    add(value(1.0),
        sub(value(10.0),
            value(2.0))
    );
  print_expr(expr); putchar('\n');
  printf("evaluates to %f\n", eval_expr(expr));

  return 0;
}
