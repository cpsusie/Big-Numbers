#include "pch.h"
#include <Random.h>

Random _standardRandomGenerator;

UINT randInt() {
  return _standardRandomGenerator.nextInt();
}

UINT randInt(UINT n) {
  return randInt() % n;
}

int randInt(int from, int to) {
  return randInt(to-from+1) + from;
}

UINT64 randInt64() {
  return _standardRandomGenerator.nextInt64();
}

UINT64 randInt64(UINT64 n) {
  return randInt64() % n;
}

INT64 randInt64(INT64 from, INT64 to) {
  return randInt64(to-from+1) + from;
}
