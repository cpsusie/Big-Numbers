#include "pch.h"
#include <Random.h>

Random _standardRandomGenerator;

UINT randInt() {
  return _standardRandomGenerator.nextInt();
}

UINT randInt(UINT n) {
  return _standardRandomGenerator.nextInt(n);
}

int randInt(int from, int to) {
  return randInt() % (to-from+1) + from;
}

__int64 randInt64() {
  return _standardRandomGenerator.nextInt64();
}

__int64 randInt64(unsigned __int64 n) {
  return _standardRandomGenerator.nextInt64(n);
}
