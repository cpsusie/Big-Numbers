#pragma once

#include "TermActionPairArray.h"
#include "NTermNewStatePairArray.h"

class StateResult {
public:
  UINT                   m_index; // state
  StringArray            m_errors, m_warnings;
  TermActionPairArray    m_termActionArray;
  NTermNewStatePairArray m_ntermNewStateArray;
  inline StateResult(UINT index) : m_index(index) {
  }
  void sortArrays();
};

inline int stateResultCmpByIndex(const StateResult &sr1, const StateResult &sr2) {
  return (int)sr1.m_index - (int)sr2.m_index;
}
