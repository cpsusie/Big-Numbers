#include "pch.h"
#include <BCDArray.h>
#include "Packer.h"

Packer &operator<<(Packer &p, const BCDArray &a) {
  return p.addElement(Packer::E_ARRAY, nullptr, 0) << (const ByteArray&)a;
}

Packer &operator>>(Packer &p, BCDArray &a) {
  return p.getElement(Packer::E_ARRAY, nullptr, 0) >> (ByteArray&)a;
}
