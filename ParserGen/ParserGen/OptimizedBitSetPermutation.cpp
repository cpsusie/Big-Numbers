#include "stdafx.h"
#include "OptimizedBitSetPermutation.h"

//#define TEST_BITSETPERMUTATION

UIntPermutation::UIntPermutation(size_t capacity) {
  init((UINT)capacity);
}
UIntPermutation::UIntPermutation() {
}

void UIntPermutation::init(UINT capacity) {
  setCapacity(capacity).clear(-1).insert(0, (UINT)-1, capacity);
}

UINT UIntPermutation::findIndex(UINT v) const {
  if(v >= size()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("v=%u, size=%zu"), v, size());
  }
  const size_t n = size();
  for(UINT i = 0; i < n; i++) {
    if((*this)[i] == v) {
      return i;
    }
  }
  throwException(_T("%s dropped to the end! v=%u"), __TFUNCTION__, v);
  return -1;
}

void UIntPermutation::validate() const {
  if(isEmpty()) {
    return;
  }
  BitSet s(size());
  for(UINT v : *this) {
    if(v >= size()) {
      throwException(_T("%s:A value (=%u) is outside valid range [0..%zu]"), __TFUNCTION__, v, size());
    }
    if(s.contains(v)) {
      throwException(_T("%s:A value (=%u) is duplicated. All values in permutation must be distinct"), __TFUNCTION__, v);
    }
    s.add(v);
  }
  assert(s.size() == size());
}

// -----------------------------------------------------------------------------------------------------------------------

OptimizedBitSetPermutation::OptimizedBitSetPermutation(const BitSet &bitSet)
: UIntPermutation(bitSet.getCapacity())
{
  BitSet compl(bitSet);
  compl.invert();
  UINT index = 0;

//  debugLog(_T("%s:bitSet:%s\n"       ), __TFUNCTION__, bitSet.toRangeString().cstr());
//  debugLog(_T("%s:compl(bitSet):%s\n"), __TFUNCTION__, compl.toRangeString().cstr());

  addSet(bitSet, index);
  setNewCapacity(index);
  addSet(compl, index);
}

OptimizedBitSetPermutation &OptimizedBitSetPermutation::addSet(const BitSet &s, UINT &v) {
  for(auto it = s.getIterator(); it.hasNext();) {
    (*this)[it.next()] = v++;
  }
  return *this;
}

OptimizedBitSetPermutation &OptimizedBitSetPermutation::setNewCapacity(UINT v) {
  m_newCapacity = v;
  return *this;
}

ByteCount OptimizedBitSetPermutation::getSavedBytesByOptimizedBitSets(UINT bitSetCount) const {
  const UINT oldCapacity = getOldCapacity(), newCapacity = getNewCapacity();
  if(newCapacity >= oldCapacity) {
    return ByteCount();
  } else {
    const ByteCount oldByteCount = ByteCount::wordAlignedSize(bitSetCount * oldCapacity);
    const ByteCount newByteCount = ByteCount::wordAlignedSize(bitSetCount * newCapacity);
    return oldByteCount - newByteCount;
  }
}

void OptimizedBitSetPermutation::validate() const {
  __super::validate();
  if((m_newCapacity == 0) || (m_newCapacity > getOldCapacity())) {
    throwException(_T("%s: newCapacity(=%u) must be in range [1..%u]"), __TFUNCTION__, m_newCapacity, getOldCapacity());
  }
}
// -----------------------------------------------------------------------------------------------------------------------

static UINT getMinValue(const BitSet &set) {
  for(auto it = set.getIterator(); it.hasNext();) {
    return (UINT)it.next();
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("set is empty"));
  return -1;
}

OptimizedBitSetPermutation2::OptimizedBitSetPermutation2(const BitSet &A, const BitSet &B)
: UIntPermutation(A.getCapacity())
{
  setEmpty();

  if(A.getCapacity() != B.getCapacity()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("A.capacity=%zu, B.capacity=%zu"), A.getCapacity(), B.getCapacity());
  }
  Array<BitSet> S(4);
  S.add(A - B)       ; const BitSet &S0 = S[0];
  S.add(B - A)       ; const BitSet &S1 = S[1];
  S.add(A * B)       ; const BitSet &S2 = S[2];
  S.add(compl(A + B)); const BitSet &S3 = S[3];

#if defined(_DEBUG)
  // Check that all sets are mutually disjoint
  const UINT n = (UINT)S.size();
  for(UINT i = 1; i < n; i++) {
    for(UINT j = 0; j < i; j++) {
      assert((S[i] & S[j]).isEmpty());
    }
  }
#endif // _DEBUG

//  for(UINT i = 0; i < S.size(); i++) {
//    debugLog(_T("%s:S[%u]:%s\n"), __TFUNCTION__, i, S[i].toRangeString().cstr());
//  }

  const UINT oldCapacity = (UINT)A.getCapacity();

  BYTE code = 0;
  code |= S0.isEmpty() ? 0 : 0x1;
  code |= S1.isEmpty() ? 0 : 0x2;
  code |= S2.isEmpty() ? 0 : 0x4;

  UINT index = 0;
  switch(code) {
  case  0:                    // A-B     empty, B-A     empty, A*B     empty
    addSet(    S3   , index);
    break;
  case  1:                    // A-B not empty, B-A     empty, A*B     empty => B empty
    addSet(    S0   , index);
    setInterval(0, 0, index);
    addSet(    S3   , index);
    break;
  case  2:                    // B-A not empty, B-A     empty, A*B     empty => A empty
    addSet(    S1   , index);
    setInterval(1, 0, index);
    addSet(    S3   , index);
    break;
  case  3:                    // A-B not empty, B-A not empty, A*B     empty
    setStart(      0, index );
    addSet(    S0   , index);
    setEnd(        0, index);
    setStart(      1, index);
    addSet(    S1   , index);
    setEnd(        1, index);
    addSet(    S3   , index);
    break;
  case  4:                    // A-B     empty, B-A     empty, A*B not empty
    setStart(      0, index );
    setStart(      1, index );
    addSet(    S2   , index);
    setEnd(        0, index);
    setEnd(        1, index);
    addSet(    S3   , index);
    break;
  case  5:                    // A-B not empty, B-A     empty, A*B not empty
    setStart(      0, index );
    setStart(      1, index );
    addSet(    S2   , index);
    setEnd(        1, index);
    addSet(    S0   , index);
    setEnd(        0, index);
    addSet(    S3   , index);
    break;
  case  6:                    // A-B     empty, B-A not empty, A*B not empty
    setStart(      0, index);
    setStart(      1, index);
    addSet(    S2   , index);
    setEnd(        0, index);
    addSet(    S1   , index);
    setEnd(        1, index);
    addSet(    S3   , index);
    break;
  case  7:                    // A-B not empty, B-A not empty, A*B not empty
    setStart(   0   , 0    );
    addSet(    S0   , index);
    setStart(   1   , index);
    addSet(    S2   , index);
    setEnd(     0   , index);
    addSet(    S1   , index);
    setEnd(     1   , index);
    addSet(    S3   , index);
    break;
  }

#if defined(TEST_BITSETPERMUTATION)
  debugLog(_T("A:(size:%3zu):%s\nB:(size:%3zu):%s\nA-B:(size:%3zu):%s\nB-A:(size:%3zu):%s\nA*B:(size:%3zu):%s\nnot(A|B):(size:%3zu):%s\n")
          ,A.size(), A.toRangeString().cstr()
          ,B.size(), B.toRangeString().cstr()
          ,S0.size(),S0.toRangeString().cstr()
          ,S1.size(),S1.toRangeString().cstr()
          ,S2.size(),S2.toRangeString().cstr()
          ,S3.size(),S3.toRangeString().cstr()
          );
  for(UINT i = 0; i < ARRAYSIZE(m_interval); i++) {
    debugLog(_T("%s:interval[%u]:%s\n"), __TFUNCTION__, i, m_interval[i].toString().cstr());
  }
  for(UINT P : *this) {
    debugLog(_T("%u "), P);
  }
  debugLog(_T("\n"));
#endif // TEST_BITSETPERMUTATION

}

BitSetInterval &OptimizedBitSetPermutation2::getInterval(BYTE index) {
  assert(index < ARRAYSIZE(m_interval));
  return m_interval[index];
}

OptimizedBitSetPermutation2 &OptimizedBitSetPermutation2::addSet(const BitSet &s, UINT &v) {
  for(auto it = s.getIterator(); it.hasNext();) {
    const UINT i = (UINT)it.next();
    UINT      &n = (*this)[i];
    if(n != -1) {
      throwInvalidArgumentException(__TFUNCTION__, _T("perm[%u] already has a value (=%u)"), i, n);
    }
    n = v++;
  }
  return *this;
}

OptimizedBitSetPermutation2 &OptimizedBitSetPermutation2::setStart(BYTE index, UINT start) {
  getInterval(index).setFrom(start);
  return *this;
}
OptimizedBitSetPermutation2 &OptimizedBitSetPermutation2::setEnd(BYTE index, UINT end) {
  getInterval(index).setTo(end);
  return *this;
}

OptimizedBitSetPermutation2 &OptimizedBitSetPermutation2::setInterval(BYTE index, UINT start, UINT end) {
  getInterval(index).setFrom(start).setTo(end);
  return *this;
}

void OptimizedBitSetPermutation2::setEmpty() {
  getInterval(0).setEmpty();
  getInterval(1).setEmpty();
}

ByteCount OptimizedBitSetPermutation2::getSavedBytesByOptimizedBitSets(UINT Acount, UINT Bcount) const {
  const UINT oldCap = getOldCapacity(), Acap = getCapacity(0), Bcap = getCapacity(1);
  if((Acap == oldCap) && (Bcap == oldCap)) {
    return ByteCount();
  } else {
    const UINT totalCount = Acount + Bcount;
    const ByteCount oldBC  = ByteCount::wordAlignedSize(totalCount * oldCap);
    const ByteCount Abc    = ByteCount::wordAlignedSize(Acount     * Acap  );
    const ByteCount Bbc    = ByteCount::wordAlignedSize(Bcount     * Bcap  );
    return oldBC - (Abc + Bbc);
  }
}

void OptimizedBitSetPermutation2::validate() const {
  __super::validate();
  for(UINT i = 0; i < ARRAYSIZE(m_interval); i++) {
    const BitSetInterval &v = getInterval(i);
    const UINT            capacity = v.getCapacity();
    if((capacity == 0) || (capacity > getOldCapacity())) {
      throwException(_T("%s: bitSetInterval[%u]:%s, capacity(=%u) must be in range [1..%u]")
                    ,__TFUNCTION__, i, v.toString().cstr(), capacity, getOldCapacity());
    }
  }
}
