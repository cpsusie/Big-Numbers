#include "pch.h"
#include <Math.h>

double fraction(double x) {
  if(x < 0) {
    return -fraction(-x);
  } else {
    return x - floor(x);
  }
}

float fraction(float x) {
  if(x < 0) {
    return -fraction(-x);
  } else {
    return x - floor(x);
  }
}

