#include "pch.h"
#include <Date.h>

Packer &operator<<(Packer &p, const Time &t) {
  p << t.m_factor;
  return p;
}

Packer &operator>>(Packer &p, Time &t) {
  p >> t.m_factor;
  return p;
}

