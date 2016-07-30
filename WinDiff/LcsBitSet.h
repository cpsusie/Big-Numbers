#pragma once

#include "Lcs.h"

class LcsBitSet : public Lcs {
private:
  UINT   m_symbolCount;
  size_t m_m;           // Number of elements in smallest input-Array
  UINT  *m_X, *m_Y;
  void clear();
  void init1(const LineArray &a, const LineArray &b);
  void init(const LineArray &a, const LineArray &b);
  UINT findK(UINT i) const;
public:
  LcsBitSet(const LineArray &a, const LineArray &b, LcsComparator &cmp);
  ~LcsBitSet();

  void findLcs(ElementPairArray &p);
};
