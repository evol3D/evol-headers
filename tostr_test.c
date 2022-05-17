#include <stdio.h>
#include "ev_types.h"
#include "ev_numeric.h"

typedef struct {
  char *name;
  int age;
  char *desc;
} Person;

DEFINE_TOSTR_FUNCTION(Person, PERSON_PRINT)
{
  sprintf(out, "%s:\n\tage: %d\n\tdesc: %s", self->name, self->age, self->desc);
}

TYPEDATA_GEN(Person,
    TOSTR(PERSON_PRINT),
    DEFAULT(
        .name = "sisyphus",
        .age = 9999,
        .desc = "One can only imagine him happy"
    ),
    INVALID(
        .name = NULL,
        .age = -1,
        .desc = NULL
    )
);

int main()
{
  puts("");
  char out[256] = {};

  Person sisyphue = EV_DEFAULT(Person);
  EV_TOSTR(Person)(&sisyphue, out);

  puts(out);
  return 0;
}
