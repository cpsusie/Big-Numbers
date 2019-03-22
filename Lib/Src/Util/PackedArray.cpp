#include "pch.h"
#include <MyUtil.h>
#include <MyAssert.h>
#ifdef _DEBUG
#include <Console.h>
#endif
#include <PackedArray.h>

#ifdef _DEBUG
//#define TEST
#endif

#pragma warning(disable : 4244)

PackedArray::PackedArray(BYTE bitsPerItem) : m_bitsPerItem(bitsPerItem), m_maxValue((1<<bitsPerItem)-1) {
  validateBitsPerItem(__TFUNCTION__, bitsPerItem);
  m_firstFreeBit = 0;
}

void PackedArray::validateBitsPerItem(const TCHAR *method, UINT bitsPerItem) { // static
  if(bitsPerItem == 0) {
    throwInvalidArgumentException(method, _T("bitsPerItem=0"));
  }
  if(bitsPerItem > 31) {
    throwInvalidArgumentException(method, _T("bitsPerItem=%u. max=31"), bitsPerItem);
  }
}

static TCHAR *sprintbin(TCHAR *s, UINT v) { // low-end bits first
  TCHAR *t = s;
  for(int i = 32; i--; v >>= 1) {
    *(t++) = _T('0') + (v & 1);
  }
  *t = 0;
  return s;
}

#ifdef TEST
#define DUMP() if(trace) { dump(true); }

static String printbin(UINT v) {
  TCHAR tmp[100];
  return sprintbin(tmp, v);
}

#else
#define DUMP()
#endif

#ifdef _DEBUG

#define CHECK_INDEX                      \
{ if(index >= size()) {                  \
    indexError(__TFUNCTION__, index);    \
  }                                      \
}

#define CHECK_VALUE                      \
{ if(v > m_maxValue) {                   \
    valueError(__TFUNCTION__, v);        \
  }                                      \
}

#define CHECK_INDEX_AND_VALUE            \
{ CHECK_INDEX                            \
  CHECK_VALUE                            \
}

#else

#define CHECK_INDEX
#define CHECK_VALUE
#define CHECK_INDEX_AND_VALUE

#endif

UINT PackedArray::get(UINT64 index) const {
  CHECK_INDEX

  const UINT *p     = &m_data[(index * m_bitsPerItem) / 32];
  const UINT offset =         (index * m_bitsPerItem) % 32;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
        UINT v      = (*p >> offset) & (m_maxValue >> (m_bitsPerItem-n));
  if(rest -= n) {
    v |= (p[1] & (m_maxValue >> (m_bitsPerItem-rest))) << n;
  }
  return v;
}

UINT PackedArray::select() const {
  if(isEmpty()) selectError(__TFUNCTION__);
  return get(randInt() % size());
}

void PackedArray::set(UINT64 index, UINT v) {
  CHECK_INDEX_AND_VALUE

        UINT *p     = &m_data[(index * m_bitsPerItem) / 32];
  const UINT offset =         (index * m_bitsPerItem) % 32;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
        UINT mask   = m_maxValue >> (m_bitsPerItem - n);
  *p &= ~(mask << offset);
  *p |= (v & mask) << offset;
  if(rest -= n) {
    p[1] &= ~(mask = m_maxValue >> (m_bitsPerItem - rest));
    p[1] |= (v>>n) & mask;
  }
}

void PackedArray::or(UINT64 index, UINT v) {
  CHECK_INDEX_AND_VALUE

        UINT *p     = &m_data[(index * m_bitsPerItem) / 32];
  const UINT offset =         (index * m_bitsPerItem) % 32;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
  *p |= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] |= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::and(UINT64 index, UINT v) {
  CHECK_INDEX_AND_VALUE

        UINT *p     = &m_data[(index * m_bitsPerItem) / 32];
  const UINT offset =         (index * m_bitsPerItem) % 32;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
  *p &= (v << offset) | ~(m_maxValue << offset);
  if(rest -= n) {
    p[1] &= (v>>n) | ~(m_maxValue >> n);
  }
}

void PackedArray::xor(UINT64 index, UINT v) {
  CHECK_INDEX_AND_VALUE

        UINT *p     = &m_data[(index * m_bitsPerItem) / 32];
  const UINT offset =         (index * m_bitsPerItem) % 32;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
  *p ^= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] ^= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::add(UINT v) {
  CHECK_VALUE

  assertHasOneFreeItem();

        UINT *p     = &m_data[m_firstFreeBit / 32];
  const UINT offset =         m_firstFreeBit % 32;
  m_firstFreeBit += m_bitsPerItem;
        UINT rest   = m_bitsPerItem;
  const UINT n      = min(32 - offset, rest);
  *p |= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] |= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::add(UINT64 index, UINT v) {
  CHECK_VALUE

  addZeroes(index, 1);

  set(index, v);
}

// TODO works only for little-endian
void PackedArray::addZeroes(UINT64 index, UINT64 count) {
#ifdef _DEBUG
  if(index > size()) indexError(__TFUNCTION__, index);
#endif

  DUMP();

  const __int64 bitsToAdd  = count * m_bitsPerItem;
  const __int64 newFreeBit = m_firstFreeBit + bitsToAdd;

#ifdef TEST
  if(trace) {
    int fisk = 1;
  }
#endif

  for(__int64 bitsNeeded = bitsToAdd - (m_data.size() * 32 - m_firstFreeBit); bitsNeeded > 0; bitsNeeded -= 32) {
    m_data.add(0);
  }
  DUMP();

  if(index < size()) {
    const __int64 bitPos      = index * m_bitsPerItem;
    const   int   baseOffset  = bitPos % 32;
    UINT         *p0          = &m_data[0];
    UINT         *basep       = p0 + bitPos/32; // first integer to copy from

#ifdef TEST
    markPointer(basep, baseOffset);
#endif

    switch(bitsToAdd % 32) {
    case  0:
      if(bitsToAdd == 0) {
        return;
      }
      // continue case
    case  8:
    case 16:
    case 24:
      { char     *src         = (char*)basep + baseOffset/8;
        char     *dst         = src + bitsToAdd/8; // bitsToAdd is a positive multiplum of 8 => dst > src
        const int bytesToMove = (((char*)p0) + m_firstFreeBit/8 + ((m_firstFreeBit%8)?1:0)) - src;

#ifdef TEST
        assert(bytesToMove > 0);
        markPointer(src);
        markPointer(dst);
        assert(((char*)basep <= src) && (src < dst) && (dst+bytesToMove <= (((char*)&m_data.last()) + sizeof(UINT))));
#endif
        memmove(dst, src, bytesToMove);
        DUMP();
        const int offset = baseOffset % 8;
        if(offset) { // = dstOffset % 8
          const unsigned char mask = (1<<offset)-1;
          switch(bitsToAdd) {
          case 8 :
            *dst &= ~mask;
            DUMP();
            *src &=  mask;
            DUMP();
            break;
          case 16:
            *dst &= ~mask;
            DUMP();
            src[1] = 0;
            *src &= mask;
            DUMP();
            break;
          default:
            memset(src+1, 0, dst-src-1);
            DUMP();
            *dst &= ~mask;
            DUMP();
            *src &= mask;
            DUMP();
            break;
          }
        } else { // offset == 0
          if(bitsToAdd > 8) { // == dstOffset % 8 !
            memset(src, 0, dst-src);
            DUMP();
          } else { // bitsToAdd != 0 => bitsToAdd == 8
            *src = 0;
            DUMP();
          }
        }
      }
      break;

    default:
      { UINT              *src         = p0 + m_firstFreeBit / 32;
        UINT              *dst         = p0 + (newFreeBit-1) / 32;
        const    int       srcOffset   = m_firstFreeBit % 32;
                 int       dstOffset   = newFreeBit % 32;
        if(dstOffset == 0) dstOffset   = 32;
        const __int64      dstBitIndex = bitPos + bitsToAdd;
        UINT              *dstp        = p0 + dstBitIndex/32; // lowest address to copy to
        int shiftL, shiftR;

#ifdef TEST
        markPointer(src, srcOffset);
        markPointer(dstp);
        markPointer(dst, dstOffset);
#endif

        if(dstOffset <= srcOffset) {
          shiftR = srcOffset - dstOffset;
          shiftL = 32 - shiftR;
        } else {  // dstOffset > srcOffset
          shiftL = dstOffset - srcOffset;
          shiftR = 32 - shiftL;
        }
        if(dst == src) {
          for(;dst > dstp; dst--) {
#ifdef TEST
            markPointer(dst);
#endif
            *dst = (*(dst-1) >> shiftR) | (*dst << shiftL);
            DUMP();
          }
        } else { // dst > src
          if(dstOffset <= srcOffset) {
#ifdef TEST
            markPointer(src);
            markPointer(dst);
#endif
            *(dst--) = (*src >> shiftR);
            DUMP();
          }
          for(;dst > dstp; src--) {
            assert(src > basep);
#ifdef TEST
            markPointer(src);
            markPointer(dst);
#endif
            *(dst--) = (*(src-1) >> shiftR) | (*src << shiftL);
            DUMP();
          }
        }

        dstOffset = dstBitIndex % 32;

#ifdef TEST
        if(trace) {
          markBit(bitPos);
          markPointer(dstp, dstOffset);
          markBit(dstBitIndex);
          markBit(newFreeBit);
        }
#endif

        const UINT mask = (1<<baseOffset)-1;
        if(dstp == basep) {
          *dstp = (basep[0] & mask) | ((basep[0] & ~mask) << shiftL);
          DUMP();
        } else { // dstp > basep
          if(dstOffset > baseOffset) { // copy highend of basep to dstp[0]
            *dstp = (basep[0] & ~mask) << (dstOffset - baseOffset);
          } else {
            shiftL = baseOffset - dstOffset;
            *dstp = ((basep[0] & ~mask) >> shiftL) | ((basep[1] & mask) << (32-shiftL));
          }
          DUMP();
          basep[0] &= mask;
          DUMP();
          if(dstp > basep+1) { // set bytes between the splittet parts of the array to zero
            memset(basep+1, 0, (char*)dstp - (char*)(basep+1));
            DUMP();
          }
        }

        const int lastOffset = newFreeBit%32;
        if(lastOffset) { // cut off the last integer, ie remove garbage-bits
          DUMP();
          m_data.last() &= ((1<<lastOffset)-1);
          DUMP();
        }
      }
      break;
    }
  }

  m_firstFreeBit = newFreeBit;

  DUMP();
}

// TODO works only for little-endian
void PackedArray::remove(UINT64 index, UINT64 count) {
  const UINT64 j = index + count;
  if(j > size()) {
    indexError(__TFUNCTION__, index, count);
  }

  DUMP();

  const __int64 bitsToRemove = count * m_bitsPerItem;
  const __int64 newFreeBit   = m_firstFreeBit - bitsToRemove;

#ifdef TEST
  if(trace) {
    int fisk = 1;
  }
#endif

  if(j < size()) {
    const __int64     bitPos     = index * m_bitsPerItem;
    const   int       baseOffset = bitPos % 32;
    UINT             *p0         = &m_data[0];
    UINT             *basep      = p0 + bitPos/32; // first integer to copy to

    switch(bitsToRemove % 32) {
    case 0:
      if(count == 0) {
        return;
      }
      // continue case
    case 8 :
    case 16:
    case 24:
      { char     *dst         = (char*)basep + baseOffset/8;
        char     *src         = dst + bitsToRemove/8; // bitsToRemove is a positive multiplum of 8 => src > dst
        const int bytesToMove = (((char*)p0) + m_firstFreeBit/8 + ((m_firstFreeBit%8)?1:0)) - src;
        const char oldDst     = *dst;
#ifdef TEST
        assert(bytesToMove > 0);
        markPointer(src);
        markPointer(dst);
        assert(((char*)basep <= dst) && (dst < src) && (src+bytesToMove <= (((char*)&m_data.last()) + sizeof(UINT))));
#endif
        memmove(dst, src, bytesToMove);
        DUMP();
        const int offset = baseOffset % 8;
        if(offset) { // = dstOffset % 8
          const unsigned char mask = (1<<offset)-1;
          *dst = (oldDst & mask) | (*dst & ~mask);
          DUMP();
        }
      }
      break;

    default:
      { const  __int64     srcBitIndex = bitPos + bitsToRemove;
        const    int       srcOffset   = srcBitIndex % 32;
        UINT              *src         = p0 + srcBitIndex / 32;
        UINT              *dst         = basep;
        const    int       dstOffset   = baseOffset;
        UINT              *lastp       = &m_data.last(); // higest address to copy from
        int shiftL, shiftR;

        assert(dstOffset != srcOffset);

        if(dstOffset <= srcOffset) {
          shiftR = srcOffset - dstOffset;
          shiftL = 32 - shiftR;
        } else {  // dstOffset > srcOffset
          shiftL = dstOffset - srcOffset;
          shiftR = 32 - shiftL;
        }

#ifdef TEST
        markPointer(dst, dstOffset);
        markPointer(src, srcOffset);
#endif
        const UINT mask = (1<<dstOffset)-1;
        if(dstOffset <= srcOffset) {
          *dst = (*dst & mask) | ((src[0] >> shiftR) & ~mask);
          DUMP();
          if(src < lastp) {
            *dst |= src[1] << shiftL;
            DUMP();
          }
          src++;
        } else { // dstOffset > srcOffset
          *dst = (*dst & mask) | ((src[0] << shiftL) & ~mask);
        }
        DUMP();
        dst++;

#ifdef TEST
        markPointer(dst);
        markPointer(src);
        markPointer(lastp);
#endif
        if(dst == src) {
          for(;dst < lastp; dst++) {
#ifdef TEST
            markPointer(dst);
#endif
            *dst = (*dst >> shiftR) | (*(dst+1) << shiftL);
            DUMP();
          }
          src = dst;
        } else { // dst < src
          for(;src < lastp; src++) {
#ifdef TEST
            markPointer(dst);
            markPointer(src);
#endif
            *(dst++) = (*src >> shiftR) | (*(src+1) << shiftL);
            DUMP();
          }
        }
        assert(dst <= src);

        if(src <= lastp) {
#ifdef TEST
          markPointer(dst, dstOffset);
          markPointer(src, srcOffset);
          assert((basep <= dst) && (dst <= src) && (src <= lastp));
#endif
          *dst = *src >> shiftR;
          DUMP();
        }
      }
      break;
    }
  }

  m_firstFreeBit = newFreeBit;
  const int          lastOffset  =  newFreeBit % 32;
  const UINT         newDataSize = (newFreeBit / 32) + (lastOffset ? 1 : 0);
  if(newDataSize < (UINT)m_data.size()) {
    m_data.remove(newDataSize, m_data.size() - newDataSize);
    DUMP();
  }
  if(lastOffset) {
    m_data.last() &= (1 << lastOffset)-1; // clear the highend bits of the last element
    DUMP();
  }
}

PackedArray &PackedArray::clear() {
  m_data.clear();
  m_firstFreeBit = 0;
  return *this;
}

void PackedArray::setCapacity(UINT64 capacity) {
  if(capacity < size()) {
    capacity = size();
  }
  if(capacity == 0) {
    return;
  }
  const UINT64 bitCapacity   = capacity * m_bitsPerItem;
  const UINT64 intCapacity64 = (bitCapacity - 1) / 32 + 1;
  CHECKUINT64ISVALIDSIZET(intCapacity64);
  const size_t intCapacity   = (size_t)intCapacity64;
  m_data.setCapacity(intCapacity);
}

bool PackedArray::operator==(const PackedArray &a) const {
  if((m_bitsPerItem != a.m_bitsPerItem) || (m_firstFreeBit != a.m_firstFreeBit)) {
    return false;
  }
  return m_data == a.m_data;
}

void PackedArray::checkInvariant(const TCHAR *method) const {
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

String PackedArray::toString() const {
  String result = format(_T("Packed Array:Bits/Item:%d, Size:%s, firstFreeBit:%s. m_data.size:%s\n")
                        ,m_bitsPerItem
                        ,format1000(size()).cstr()
                        ,format1000(m_firstFreeBit).cstr()
                        ,format1000(m_data.size()).cstr());
  TCHAR tmp[40];
  int l = getBitsPerItem();
  size_t n = m_data.size() * 32 / l;
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += _T(" ");
      bitCount %= 32;
    }
    if(i % 10 == 0) {
      result += format(_T("%*s"), l, formatSize(i/10).cstr());
    } else {
      result += format(_T("%*s"), l,_T(" "));
    }
  }
  result += _T("\n");
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += _T(":");
      bitCount %= 32;
    }
    result += format(_T("%*d"), l, (int)(i%10));
  }
  result += _T("\n");

  for(size_t i = 0, bitCount = 0; i < size(); i++) { // print member of the array
    bitCount += l;
    if(bitCount > 32) {
      result += _T(":");
      bitCount %= 32;
    }
    result += format(_T("%*d"), l, get(i));
  }
  result += _T("\n");
  for(size_t i = 0; i < m_data.size(); i++) { // print all integers of m_data
    result += format(_T("%s:"), sprintbin(tmp, m_data[i]));
  }
  result += _T("\n");

  n = m_data.size() * 32;
  l = 1;
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += " ";
      bitCount %= 32;
    }
    if(i % 10 == 0) {
      result += format(_T("%*s"), l, formatSize(i/10).cstr());
    } else {
      result += format(_T("%*s"), l,_T(" "));
    }
  }
  result += _T("\n");
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += _T(":");
      bitCount %= 32;
    }
    result += format(_T("%*d"), l, (int)(i%10));
  }
  return result;
}

#ifdef _DEBUG

bool PackedArray::trace = false;
#define RECTTOP     50
#define STRINGLINES 7
#define RECTBOTTOM  (RECTTOP + STRINGLINES)

void PackedArray::dump(bool fixedPos) const {
  if(fixedPos) {
    const COORD oldPos = Console::getCursorPos();
    Console::clearRect(0,RECTTOP,Console::getWindowSize().X,RECTBOTTOM+1);
    Console::setCursorPos(0,RECTTOP);
    _tprintf(_T("%s\n"), toString().cstr());
    Console::setCursorPos(oldPos);
  } else {
    _tprintf(_T("%s\n"), toString().cstr());
  }
}

void PackedArray::markPointer(const void *p, int offset) const {
  if(!trace) return;
  if(!m_data.isEmpty()) {
    markBit(((const char*)p - (const char*)&m_data[0]) * 8 + offset);
  }
}

void PackedArray::markElement(int index) const {
  if(!trace) return;
  markBit(index * m_bitsPerItem);
}

void PackedArray::markBit(int bit) const {
  if(!trace) return;
  COORD oldPos = Console::getCursorPos();
  Console::clearRect(0,RECTBOTTOM,Console::getWindowSize().X,RECTBOTTOM);
  Console::printf(bit + (bit/32), RECTBOTTOM, _T("%c"), 24);
  Console::setCursorPos(oldPos);
}
#endif
