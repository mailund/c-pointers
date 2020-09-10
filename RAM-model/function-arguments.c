#include <stdio.h>
#include <stdlib.h>

void doesnt_mutate(int i)
{
  i += 42;
}

void mutates(int *i)
{
  *i += 42;
}

struct point {
  double x, y;
};

void set_point(struct point point, double x, double y)
{
  point.x = x;
  point.y = y;
}

void set_point_(struct point *point, double x, double y)
{
  (*point).x = x;
  (*point).y = y;
}

void set_point__(struct point *point, double x, double y)
{
  point->x = x;
  point->y = y;
}

void foo(int *ip)
{
  ip++;
}

void bar(int **ip)
{
  (*ip)++;
}

int main(int argc, char **argv)
{
  int i = 0;
  doesnt_mutate(i);
  printf("i is %d\n", i);
  mutates(&i);
  printf("i is %d\n", i);

  struct point point;
  set_point(point, 13.0, 42.0);
  printf("point = (%f,%f)\n", point.x, point.y);
  set_point_(&point, 13.0, 42.0);
  printf("point = (%f,%f)\n", point.x, point.y);
  set_point__(&point, 42.0, 13.0);
  printf("point = (%f,%f)\n", point.x, point.y);

  int *ip = 0;
  foo(ip);
  printf("%p\n", ip);
  bar(&ip);
  printf("%p\n", ip);

  return EXIT_SUCCESS;
}
