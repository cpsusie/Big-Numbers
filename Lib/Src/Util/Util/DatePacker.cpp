#include "pch.h"
#include <Date.h>

Packer &operator<<(Packer &p, const Date &d) {
  p << d.m_factor;
  return p;
}

Packer &operator>>(Packer &p, Date &d) {
  p >> d.m_factor;
  return p;
}
