#include <stdio.h>

#define EV_MAP_IMPLEMENTATION
#include "ev_map.h"

int main()
{
  int x = 10;
  int y = 100;
  int x2 = 300;
  int y2 = 123;

  ev_map(i32, i32)* m = ev_map_init(i32, i32);
  ev_map_add(m, (&x, &y), (&x2, &y2));

  i32 out;
  ev_map_get(m, &x, &out);

  ev_map_fini(m);

  return 0;
}
