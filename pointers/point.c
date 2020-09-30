#include <stdio.h>

struct point {
  double x, y;
};


void set_point(struct point point, double x, double y)
{
  point.x = x;
  point.y = y;
}

struct point set_point_(struct point point, double x, double y)
{
  point.x = x;
  point.y = y;
  return point;
}

void set_point__(struct point *point, double x, double y)
{
  (*point).x = x;
  (*point).y = y;
}

void set_point___(struct point *point, double x, double y)
{
  point->x = x;
  point->y = y;
}

void print_point(struct point *point)
{
  printf("point <%.2f, %.2f>\n", point->x, point->y);
}

int main(void)
{
  struct point point = { .x = 0.0, .y = 0.0 };
  print_point(&point);

  set_point(point, 13, 42);
  print_point(&point);

  point = set_point_(point, 13, 42);
  print_point(&point);

  set_point___(&point, -13, -42);
  print_point(&point);


  return 0;
}
