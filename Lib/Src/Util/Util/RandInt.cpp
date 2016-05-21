#include "pch.h"
#include <Random.h>

Random _standardRandomGenerator;

unsigned int randInt() {
  return _standardRandomGenerator.nextInt();
}

unsigned int randInt(unsigned int n) {
  return randInt() % n;
}

int randInt(int from, int to) {
  return randInt() % (to-from+1) + from;
}
