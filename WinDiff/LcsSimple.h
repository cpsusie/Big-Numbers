#pragma once

#include "Lcs.h"

typedef CompactArray<UINT> MatchArray;

class LcsSimple : public Lcs {
private:
  CompactArray<LcsElement> m_A, m_B;       // len(A) = n >= len(B), both indexed from 1
  Array<MatchArray>        m_tmpM;
  MatchArray             **m_matchList;
  UINT                     m_docSize[2];

  size_t findMatchlist();
  UINT findK(UINT j) const; // find k:m_thresh[k-1] < j <= m_thresh[k]. ie min(k:m_thresh[k] <= j
  void clear();
  void init(const LineArray &a, const LineArray &b);
public:
  LcsSimple(const LineArray &a, const LineArray &b, UINT *docSize, LcsComparator &cmp, CompareJob *job);
  ~LcsSimple();

  void findLcs(ElementPairArray &result);
};
