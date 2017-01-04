#pragma once

#include <MyUtil.h>
#include <TinyBitSet.h>

class FieldSet : public BitSet64 {
public:
  inline FieldSet() {
  }
  explicit FieldSet(int f,...); // terminate with -1
  String toString() const;
  void dump() const;
};

