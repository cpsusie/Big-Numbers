#pragma once

#include "NTermNewStatePairArray.h"

class Grammar;
class NTermNewStatePairMatrix : public Array<NTermNewStatePairArray> {
public:
  NTermNewStatePairMatrix(const Grammar &grammar);
  inline UINT getRowCount() const {
    return (UINT)size();
  }
};
