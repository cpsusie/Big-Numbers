#pragma once

#include <MyUtil.h>
#include <TinyBitSet.h>

class FieldSet : public BitSet64 {
public:
  inline FieldSet() {
  }
  explicit FieldSet(int f,...); // terminate with -1
  String toString() const;
#if defined(_DEBUG)
  void dump() const;
#endif _DEBUG
};

