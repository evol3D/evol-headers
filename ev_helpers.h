/*!
 * \file ev_helpers.h
 */
#ifndef EV_HELPERS_HEADER
#define EV_HELPERS_HEADER

#include "ev_macros.h"
#include "ev_str.h"
#if defined(EV_HELPERS_SHARED)
# if defined (EV_HELPERS_IMPL)
#  define EV_HELPERS_API EV_EXPORT
# else
#  define EV_HELPERS_API EV_IMPORT
# endif
#else
# define EV_HELPERS_API
#endif

EV_HELPERS_API evstring
evstring_readFile(
  evstring filePath);

#ifdef EV_HELPERS_IMPLEMENTATION
#undef EV_HELPERS_IMPLEMENTATION

evstring
evstring_readFile(
  evstring filePath)
{
  FILE* f = fopen(filePath, "r");
  if(f == NULL) return EV_INVALID(evstring);

  fseek(f, 0, SEEK_END);
  u32 buflen = ftell(f);
  fseek(f, 0, SEEK_SET);

  evstring res = evstring_new("");
  evstring_setLength(&res, buflen);
  fread(res, 1, buflen, f);
  fclose(f);

  return res;
}

#endif

#endif
