#include "pch.h"
#include <objbase.h>
#include <Random.h>

void randomize() {
  const __int64 seed = getRandomSeed();
  srand((UINT)seed);
  _standardRandomGenerator.setSeed(seed);
}

__int64 getRandomSeed() {
  GUID guid;
  CoCreateGuid(&guid);
  short w4 = ((short)guid.Data4[0] << 8) | guid.Data4[1];
  short w5 = ((short)guid.Data4[2] << 8) | guid.Data4[3];
  short w6 = ((short)guid.Data4[4] << 8) | guid.Data4[5];
  short w7 = ((short)guid.Data4[6] << 8) | guid.Data4[7];

  __int64 seed = ((((((__int64)guid.Data1 * 151i64
                + guid.Data2) * 641
                + guid.Data3) * 983
                + w4        ) * 337
                + w5        ) * 757 
                + w6        ) * 599
                + w7;
  return seed;
}
