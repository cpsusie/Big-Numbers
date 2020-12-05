#pragma once

#include "TermActionPairArray.h"

class Grammar;
class TermActionPairMatrix : public Array<TermActionPairArray> {
public:
  static constexpr BYTE SELECT_SHIFTACTIONS  = 0x01;
  static constexpr BYTE SELECT_REDUCEACTIONS = 0x02;
  static constexpr BYTE SELECT_ALLACTIONS    = SELECT_SHIFTACTIONS | SELECT_REDUCEACTIONS;
  TermActionPairMatrix(const Grammar &grammar, BYTE selectFlags = SELECT_ALLACTIONS);
  inline UINT getRowCount() const {
    return (UINT)size();
  }
};

