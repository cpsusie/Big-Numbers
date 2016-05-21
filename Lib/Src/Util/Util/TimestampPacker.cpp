#include "pch.h"
#include <Date.h>

Packer &operator<<(Packer &p, const Timestamp &ts) {
  p << ts.m_factor;
  return p;
}

Packer &operator>>(Packer &p, Timestamp &ts) {
  p >> ts.m_factor;
  return p;
}

