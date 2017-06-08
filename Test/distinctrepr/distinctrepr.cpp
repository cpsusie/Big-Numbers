#include "stdafx.h"
#include <BitSet.h>

class ElementPair {
public:
  size_t m_girl,m_boy;
  ElementPair() : m_girl(-1), m_boy(-1) {
  }
  ElementPair(size_t girl, size_t boy) : m_girl(girl), m_boy(boy) {
  }
};

typedef CompactArray<ElementPair> PairArray;
typedef Array<BitSet>             SetArray;

bool setsNonEmpty(const SetArray &crit) { // return true if all sets in crit is non-empty ?
  for(size_t i = 0; i < crit.size(); i++) {
    if(crit[i].isEmpty()) {
      return false;
    }
  }
  return true;
}

PairArray findDistinctRepresentatives(SetArray &crit) { // crit[i] = boys, wanted by girl i
  BitSet unmatched(crit.size());
  unmatched.invert();
  PairArray result;

  while(!unmatched.isEmpty() && setsNonEmpty(crit)) {
    size_t   p;
    intptr_t minsize = -1;
    for(Iterator<size_t> it = unmatched.getIterator(); it.hasNext();) { // find p so crit[p] is the smallest set
      const size_t   i = it.next();
      const intptr_t size = crit[i].size();
      if(minsize == -1 || size <= minsize) {
        minsize = size;
        p       = i;
      }
    }
    const size_t q = crit[p].select();
    result.add(ElementPair(p,q));  // <p,q> is a ElementPair
    unmatched -= p;
    for(Iterator<size_t> remaining = unmatched.getIterator(); remaining.hasNext();) {
      crit[remaining.next()] -= q;
    }
  }
  if(!unmatched.isEmpty()) {
    result.clear(); // no distinct representatives exists
  }
  return result;
}

BitSet makecrit(int size, ...) {
  BitSet s(size);
  va_list argptr;
  va_start(argptr,size);
  for(int a = va_arg(argptr,int); a >= 0; a = va_arg(argptr,int)) {
    s += a;
  }
  va_end(argptr);
  return s;
}

int main(int argc, char **argv) {
  SetArray crit;
// eks 7.1 in Gotlieb & Gotlieb page 246
  BitSet S1(makecrit(6,3,4,1,2,5,-1)); // g,t,a,f,v
  BitSet S2(makecrit(6,1,4      ,-1)); // a,t
  BitSet S3(makecrit(6,4,3,2    ,-1)); // t,g,f
  BitSet S4(makecrit(6,5,4      ,-1)); // v,t
  BitSet S5(makecrit(6,1,3      ,-1)); // a,g
  crit.add(S1);
  crit.add(S2);
  crit.add(S3);
  crit.add(S4);
  crit.add(S5);

  PairArray pairs = findDistinctRepresentatives(crit);
  
  for(int i = 0; i < pairs.size(); i++) {
    ElementPair &p = pairs[i];
    printf("<%zd,%zd>\n",p.m_girl+1,p.m_boy); // +1 just to make the range of girls from 1 to 5
  }
  return 0;
}
