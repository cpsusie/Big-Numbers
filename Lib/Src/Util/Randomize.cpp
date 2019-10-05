#include "pch.h"
#include <objbase.h>
#include <Random.h>

#pragma comment(lib, "ole32.lib")

void randomize() {
  const INT64 seed = RandomGenerator::getRandomSeed();
  srand((UINT)seed);
  _standardRandomGenerator->setSeed(seed);
}

INT64 RandomGenerator::getRandomSeed() { // static
  GUID guid;
  CoCreateGuid(&guid);
  const short w4 = ((short)guid.Data4[0] << 8) | guid.Data4[1];
  const short w5 = ((short)guid.Data4[2] << 8) | guid.Data4[3];
  const short w6 = ((short)guid.Data4[4] << 8) | guid.Data4[5];
  const short w7 = ((short)guid.Data4[6] << 8) | guid.Data4[7];

  INT64 seed = ((((((INT64)guid.Data1 * 151i64
                + guid.Data2) * 641
                + guid.Data3) * 983
                + w4        ) * 337
                + w5        ) * 757
                + w6        ) * 599
                + w7;
  return seed;
}
