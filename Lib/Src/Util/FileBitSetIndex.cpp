#include "pch.h"
#include <BitSet.h>
#include <CountedByteStream.h>

FileBitSetIndex::FileBitSetIndex(const String &fileName, UINT64 startOffset)
: m_startOffset(startOffset)
, m_f(fileName)
 {
  m_f.seek(startOffset);
  ByteCounter            byteCounter;
  CountedByteInputStream in(byteCounter, m_f);

  m_shift = in.getByte();                                         // load 1 byte:m_shift
  UINT64 capacity64;
  in.getBytesForced((BYTE*)&capacity64, sizeof(capacity64));      // load 8 bytes:setCapacity
  CHECKUINT64ISVALIDSIZET(capacity64)
  m_rangeTable.load(in);                                          // load rangeTable
  m_loadedIntervals   = new BitSet(m_rangeTable.size()+1); TRACE_NEW(m_loadedIntervals);
  m_bitSet            = new BitSet((size_t)capacity64);    TRACE_NEW(m_bitSet); // allocate bitset. dont read it
  m_bitsStartOffset   = startOffset + byteCounter.getCount() + sizeof(capacity64); // bits begin after bitet.capacity
}

void BitSetIndex::save(ByteOutputStream &s) const {
  s.putByte(m_shift);                                             // save 1 byte:m_shift
  const UINT64 setCapacity64 = m_bitSet.getCapacity();
  s.putBytes((BYTE*)&setCapacity64, sizeof(setCapacity64));       // save 8 bytes:setCapacity
  m_rangeTable.save(s);                                           // save rangetable
  m_bitSet.save(s);                                               // save m_bitset
}

FileBitSetIndex::~FileBitSetIndex() {
  SAFEDELETE(m_bitSet);
  SAFEDELETE(m_loadedIntervals);
}

#define BYTESINATOM sizeof(m_bitSet->m_p[0])

intptr_t FileBitSetIndex::getIndex(size_t i) const {
  if(i >= m_bitSet->getCapacity()) {
    return -1;
  }
  const intptr_t rangeIndex = m_rangeTable.binarySearchLE(i, int64HashCmp);

  const size_t bitInterval = (rangeIndex < 0) ? 0 : (rangeIndex+1);
  const size_t startBit    =  (size_t)(bitInterval ? m_rangeTable[rangeIndex] : 0);
  if(!m_loadedIntervals->contains(bitInterval)) {
    const size_t lastBit     = (size_t)((bitInterval < m_rangeTable.size()) ? m_rangeTable[bitInterval] : (m_bitSet->getCapacity()-1));
    const size_t startAtom   = m_bitSet->getAtomIndex(startBit);
    const size_t atomsToRead = m_bitSet->getAtomIndex(lastBit) - startAtom + 1;
    m_f.seek(m_bitsStartOffset + startAtom * BYTESINATOM);
    m_f.getBytesForced((BYTE*)(m_bitSet->m_p + startAtom), BYTESINATOM * atomsToRead);
    m_loadedIntervals->add(bitInterval);
  }

  if(!m_bitSet->contains(i)) {
    return -1;
  }
  return (bitInterval << m_shift) + m_bitSet->getCount(startBit, i) - 1;
}
