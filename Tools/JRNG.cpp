#include "JRNG.h"

static uint64_t seed;

void jsrand (unsigned s)
{
  seed = s;
}

float jrand (void)
{
  seed = 2373697824597063315ULL*seed + 1;
  return ((float) (seed >> 33) / (float) (RAND_MAX));
}
