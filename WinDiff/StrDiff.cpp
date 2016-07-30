#include "stdafx.h"
#include "Lcs.h"
#include "Diff.h"

void StrDiff::syncArrays(const LineArray &a, const LineArray &b, int &ai, int ato, int &bi, int bto) {
  if(ai < ato && bi < bto) {
    int n = max(ato-ai,bto-bi);
    for(int i = 0; i < n; i++) {
      if(ai < ato && bi < bto) {
        d1.add(CHANGEDLINES);
        d2.add(CHANGEDLINES);
        ai++; bi++;
      }
      else if(ai < ato) {
        d1.add(CHANGEDLINES);
        ai++;
      }
      else {
        d2.add(CHANGEDLINES);
        bi++;
      }
    }
  }
  for(;ai < ato; ai++) d1.add(DELETEDLINES);
  for(;bi < bto; bi++) d2.add(INSERTEDLINES);
}

void StrDiff::makeDiffLines(const LineArray &a, const LineArray &b, const ElementPairArray &pairs) {
  int ai = 0;
  int bi = 0;
  d1.clear();
  d2.clear();
  for(size_t p = 0; p < pairs.size(); p++) {
    const ElementPair &e = pairs[p];
    syncArrays(a,b,ai,e.m_aIndex,bi,e.m_bIndex);
    d1.add(EQUALLINES);
    d2.add(EQUALLINES);
    ai = e.m_aIndex+1;
    bi = e.m_bIndex+1;
  }
  syncArrays(a,b,ai,(int)a.size(),bi,(int)b.size());
}

void StrDiff::compareStrings(const TCHAR *s1, const TCHAR *s2, LcsComparator &cmp) {
  LineArray a1,a2;
  TCHAR tmp[2];
  tmp[1] = 0;
  for(const TCHAR *cp = s1; *cp; cp++) { tmp[0] = *cp; a1.add(tmp); }
  for(const TCHAR *cp = s2; *cp; cp++) { tmp[0] = *cp; a2.add(tmp); }

  LcsBitSet lcs(a1,a2, cmp);

  ElementPairArray p;
  lcs.findLcs(p);

  makeDiffLines(a1,a2,p);
}
