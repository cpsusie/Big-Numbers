#include "pch.h"
#include <PackedArray.h>
#include <Bytefile.h>

#pragma warning(disable : 4244)

void PackedArray::save(ByteOutputStream &s) const {
  checkInvariant(__TFUNCTION__);
  s.putBytes((BYTE*)&m_bitsPerItem ,sizeof(m_bitsPerItem )); // if modified, => take care of PackedFileArray
  s.putBytes((BYTE*)&m_firstFreeBit,sizeof(m_firstFreeBit));
  m_data.save(s);
}

void PackedArray::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)&m_bitsPerItem , sizeof(m_bitsPerItem ));
  s.getBytesForced((BYTE*)&m_firstFreeBit, sizeof(m_firstFreeBit));
  m_maxValue = (1<<m_bitsPerItem) - 1;
  m_data.load(s);
  checkInvariant(__TFUNCTION__);
}

PackedFileArray::PackedFileArray(const String &fileName, UINT64 startOffset)
: m_data(fileName, startOffset + sizeof(UINT) + sizeof(UINT64))
  // offset of ByteArray if startOffset + sizeof(m_bitsPerItem) + sizeof(PackedArray::m_firstFreeBit)
{
  ByteInputFile s(fileName);
  s.seek(startOffset);
  s.getBytesForced((BYTE*)&m_bitsPerItem , sizeof(m_bitsPerItem ));
  PackedArray::validateBitsPerItem(__TFUNCTION__, m_bitsPerItem);

  s.getBytesForced((BYTE*)&m_firstFreeBit, sizeof(m_firstFreeBit));
  m_maxValue = (1<<m_bitsPerItem)-1;
  m_size     = m_firstFreeBit / m_bitsPerItem;
  checkInvariant(__TFUNCTION__);
}

void PackedFileArray::indexError(const TCHAR *method, UINT64 index) const {
  throwIndexOutOfRangeException(method, index, size());
}

void PackedFileArray::emptyArrayError(const TCHAR *method) { // static
  throwEmptyArrayException(method);
}

// this is the same algorithm as in PackedArray::get
UINT PackedFileArray::get(UINT64 index) const {
  if(index >= size()) indexError(__TFUNCTION__, index);

  const UINT p0Index = (index * m_bitsPerItem) / 32;
  const UINT offset  = (index * m_bitsPerItem) % 32;
  const UINT p0      = m_data[p0Index];
        UINT rest    = m_bitsPerItem;
  const UINT n       = min(32 - offset, rest);
        UINT v       = (p0 >> offset) & (m_maxValue >> (m_bitsPerItem-n));
  if(rest -= n) {
    const UINT p1 = m_data[p0Index+1];
    v |= (p1 & (m_maxValue >> (m_bitsPerItem-rest))) << n;
  }
  return v;
}

void PackedFileArray::checkInvariant(const TCHAR *method) const {
  const UINT64 expectedDataSize64 = m_firstFreeBit ? ((m_firstFreeBit-1) / 32 + 1) : 0;
  CHECKUINT64ISVALIDSIZET(expectedDataSize64);
  const size_t expectedDataSize = (size_t)expectedDataSize64;
  if(m_data.size() != expectedDataSize) {
    throwException(_T("%s:Bits/Item:%u. m_firstFreeBit:%s, m_data.size=%s, Should be %zu")
                  ,method
                  ,m_bitsPerItem
                  ,format1000(m_firstFreeBit).cstr()
                  ,format1000(m_data.size()).cstr()
                  ,expectedDataSize);
  }
  if((expectedDataSize > 0) && (m_firstFreeBit%32)) {
    const UINT lastInt = m_data[expectedDataSize-1];
    if((lastInt & ~((1<<(m_firstFreeBit%32))-1)) != 0) {
      throwException(_T("%s:Bits/Item:%u. m_firstFreeBit:%s, m_data.size=%s, Garbagebits in last element:%08x")
                    ,method
                    ,m_bitsPerItem
                    ,format1000(m_firstFreeBit).cstr()
                    ,format1000(m_data.size()).cstr()
                    ,lastInt);
    }
  }
}
