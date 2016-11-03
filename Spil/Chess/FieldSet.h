#pragma once

#include <MyUtil.h>
#include <TinyBitSet.h>

class FieldSet : public BitSet64 {
public:
  FieldSet() {
  }
  explicit FieldSet(int f,...); // terminate with -1
  String toString() const;
};

