#include <stdio.h>
#include <stdlib.h>

typedef void * cls;
typedef struct { cls *cls; } obj;

#define basetype(x, base) ((base *)x)
#define vtbl(inst, base)  basetype(((obj *)inst)->cls, base)

typedef void (*cls_init)(cls *);
void *alloc_cls(size_t cls_size, cls_init cls_init)
{
  cls *cls = malloc(cls_size);
  if (!cls) abort(); // error handling
  cls_init(cls);
  return cls;
}
#define INIT_CLS(p, init)                     \
do {                                          \
  if (!p)                                     \
    p = alloc_cls(sizeof *p, (cls_init)init); \
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

// dynamic dispatch
void   print(EXP e) { vtbl(e, base_expr_cls)->print(e); }
double eval (EXP e) { return vtbl(e, base_expr_cls)->eval(e); }

void init_base_expr_cls(base_expr_cls *cls)
{
  cls->print = 0; // abstract method
  cls->eval  = 0; // abstract method
}

// Base class, object definition
typedef struct { obj obj; } base_expr;

// Base class, methods (init does nothing)
void init_base_expr(base_expr *inst) {}


// Value expressions
typedef struct {
  base_expr_cls base_expr_cls;
} value_expr_cls;
typedef struct {
  base_expr base_expr;
  double value;
} value_expr;

// Concrete class, so must have a struct
value_expr_cls *VALUE_EXPR_CLS = 0; // must be initialised

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
  // override virtual functions
  base_expr_cls *base_expr = basetype(cls, base_expr_cls);
  base_expr->print = (void (*)(EXP))value_expr_print;
  base_expr->eval  = (double (*)(EXP))value_expr_eval;
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

// Binary expressions
typedef struct {
  base_expr_cls base_expr_cls;
} binexpr_cls;
typedef struct {
  base_expr base_expr;
  char symb; EXP left, right;
} binexpr;

void print_binexpr(binexpr *binop)
{
  putchar('('); print(binop->left); putchar(')');
  putchar(binop->symb);
  putchar('('); print(binop->right); putchar(')');
}

void init_binexpr_cls(binexpr_cls *cls)
{
  init_base_expr_cls(basetype(cls, base_expr_cls));
  base_expr_cls *base_expr = basetype(cls, base_expr_cls);
  base_expr->print = (void (*)(EXP))print_binexpr;
}

void init_binexpr(binexpr *binop, char symb,
                  EXP left, EXP right)
{
  init_base_expr(basetype(binop, base_expr));
  binop->symb = symb;
  binop->left = left;
  binop->right = right;
}

// Addition
typedef struct { binexpr_cls binexpr_cls; } add_expr_cls;
typedef struct { binexpr binexpr;         } add_expr;

// Concrete class, so must have a struct
add_expr_cls *ADD_EXPR_CLS = 0; // must be initialised

double eval_add_expr(add_expr *expr)
{
  binexpr *base = basetype(expr, binexpr);
  return eval(base->left) + eval(base->right);
}

void init_add_expr_cls(add_expr_cls *cls)
{
  init_binexpr_cls(basetype(cls, binexpr_cls));
  base_expr_cls *base_expr = basetype(cls, base_expr_cls);
  base_expr->eval = (double (*)(EXP))eval_add_expr;
}

void init_add_expr(add_expr *expr, EXP left, EXP right)
{
  init_binexpr(basetype(expr, binexpr), '+', left, right);
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
typedef struct { binexpr_cls binexpr_cls; } sub_expr_cls;
typedef struct { binexpr binexpr;         } sub_expr;

// Concrete class, so must have a struct
sub_expr_cls *SUB_EXPR_CLS = 0; // must be initialised

double eval_sub_expr(sub_expr *expr)
{
  binexpr *base = basetype(expr, binexpr);
  return eval(base->left) - eval(base->right);
}

void init_sub_expr_cls(sub_expr_cls *cls)
{
  init_binexpr_cls(basetype(cls, binexpr_cls));
  base_expr_cls *base_expr = basetype(cls, base_expr_cls);
  base_expr->eval = (double (*)(EXP))eval_sub_expr;
}

void init_sub_expr(add_expr *expr, EXP left, EXP right)
{
  init_binexpr(basetype(expr, binexpr), '-', left, right);
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
  print(expr); putchar('\n');
  printf("evaluates to %f\n", eval(expr));

  return 0;
}
