#include "pch.h"
#include <PackedArray.h>
#include <Bytefile.h>

#pragma warning(disable : 4244)

void PackedArray::save(ByteOutputStream &s) const {
  s.putBytes((BYTE*)&m_bitsPerItem ,sizeof(m_bitsPerItem )); // if modified, => take care of PackedFileArray
  s.putBytes((BYTE*)&m_firstFreeBit,sizeof(m_firstFreeBit));
  m_data.save(s);
}

void PackedArray::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)&m_bitsPerItem , sizeof(m_bitsPerItem ));
  s.getBytesForced((BYTE*)&m_firstFreeBit, sizeof(m_firstFreeBit));
  m_maxValue = (1<<m_bitsPerItem) - 1;
  m_data.load(s);
}

#ifdef _DEBUG

#define CHECK_INDEX(f)            \
{ if(index >= size()) {           \
    indexError(index, _T(f));     \
  }                               \
}

#else

#define CHECK_INDEX(f)

#endif

DEFINECLASSNAME(PackedFileArray);

PackedFileArray::PackedFileArray(const String &fileName, unsigned __int64 startOffset) 
: m_data(fileName, startOffset + sizeof(unsigned int) + sizeof(unsigned __int64))
  // offset of ByteArray if startOffset + sizeof(m_bitsPerItem) + sizeof(PackedArray::m_firstFreeBit)
{
  ByteInputFile s(fileName);
  s.seek(startOffset);
  s.getBytesForced((BYTE*)&m_bitsPerItem , sizeof(m_bitsPerItem ));
  PackedArray::validateBitsPerItem(s_className, m_bitsPerItem);
  s.getBytesForced((BYTE*)&m_firstFreeBit, sizeof(m_firstFreeBit));
  m_maxValue = (1<<m_bitsPerItem)-1;
}
 
unsigned int PackedFileArray::get(unsigned __int64 index) const { // this is the same algorithm as in PackedArray::get
  CHECK_INDEX("get")
  const unsigned int p0Index = (index * m_bitsPerItem) / 32;
  const unsigned int offset  = (index * m_bitsPerItem) % 32;
  const unsigned int p0      = m_data[p0Index];
        unsigned int rest    = m_bitsPerItem;
  const unsigned int n       = min(32 - offset, rest);
        unsigned int v       = (p0 >> offset) & (m_maxValue >> (m_bitsPerItem-n));
  if(rest -= n) {
    const unsigned int p1 = m_data[p0Index+1];
    v |= (p1 & (m_maxValue >> (m_bitsPerItem-rest))) << n;
  }
  return v;
}

unsigned int PackedFileArray::select() const {
#ifdef _DEBUG
  if(isEmpty()) {
   PackedArray::selectError();
 }
#endif
  return get(randInt() % size());
}
