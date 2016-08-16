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

PackedArray::PackedArray(unsigned char bitsPerItem) : m_bitsPerItem(bitsPerItem), m_maxValue((1<<bitsPerItem)-1) {
  validateBitsPerItem(bitsPerItem);
  m_firstFreeBit = 0;
}

void PackedArray::validateBitsPerItem(unsigned int bitsPerItem) { // static
  DEFINEMETHODNAME;

  if(bitsPerItem == 0) {
    throwInvalidArgumentException(method, _T("bitsPerItem=0"));
  }
  if(bitsPerItem > 31) {
    throwInvalidArgumentException(method, _T("bitsPerItem=%d. max=31"), bitsPerItem);
  }
}

static TCHAR *sprintbin(TCHAR *s, unsigned int v) { // low-end bits first
  TCHAR *t = s;
  for(int i = 32; i--; v >>= 1) {
    *(t++) = _T('0') + (v & 1);
  }
  *t = 0;
  return s;
}

#ifdef TEST
#define DUMP() if(trace) { dump(true); }

static String printbin(unsigned int v) {
  TCHAR tmp[100];
  return sprintbin(tmp, v);
}

#else
#define DUMP()
#endif

#ifdef _DEBUG

#define CHECK_INDEX                      \
{ if(index >= size()) {                  \
    indexError(index, __TFUNCTION__); \
  }                                      \
}

#define CHECK_VALUE                      \
{ if(v > m_maxValue) {                   \
    valueError(v, __TFUNCTION__);     \
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

unsigned int PackedArray::get(unsigned __int64 index) const {
  CHECK_INDEX

  const unsigned int *p     = &m_data[(index * m_bitsPerItem) / 32];
  const unsigned int offset =         (index * m_bitsPerItem) % 32;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
        unsigned int v      = (*p >> offset) & (m_maxValue >> (m_bitsPerItem-n));
  if(rest -= n) {
    v |= (p[1] & (m_maxValue >> (m_bitsPerItem-rest))) << n;
  }
  return v;
}

unsigned int PackedArray::select() const {
#ifdef _DEBUG
  if(isEmpty()) {
   selectError();
 }
#endif
  return get(randInt() % size());
}

void PackedArray::set(unsigned __int64 index, unsigned int v) {
  CHECK_INDEX_AND_VALUE

        unsigned int *p     = &m_data[(index * m_bitsPerItem) / 32];
  const unsigned int offset =         (index * m_bitsPerItem) % 32;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
        unsigned int mask   = m_maxValue >> (m_bitsPerItem - n);
  *p &= ~(mask << offset);
  *p |= (v & mask) << offset;
  if(rest -= n) {
    p[1] &= ~(mask = m_maxValue >> (m_bitsPerItem - rest));
    p[1] |= (v>>n) & mask;
  }
}

void PackedArray::or( unsigned __int64 index, unsigned int v) {
  CHECK_INDEX_AND_VALUE

        unsigned int *p     = &m_data[(index * m_bitsPerItem) / 32];
  const unsigned int offset =         (index * m_bitsPerItem) % 32;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
  *p |= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] |= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::and(unsigned __int64 index, unsigned int v) {
  CHECK_INDEX_AND_VALUE

        unsigned int *p     = &m_data[(index * m_bitsPerItem) / 32];
  const unsigned int offset =         (index * m_bitsPerItem) % 32;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
  *p &= (v << offset) | ~(m_maxValue << offset);
  if(rest -= n) {
    p[1] &= (v>>n) | ~(m_maxValue >> n);
  }
}

void PackedArray::xor(unsigned __int64 index, unsigned int v) {
  CHECK_INDEX_AND_VALUE

        unsigned int *p     = &m_data[(index * m_bitsPerItem) / 32];
  const unsigned int offset =         (index * m_bitsPerItem) % 32;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
  *p ^= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] ^= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::add(unsigned int v) {
  CHECK_VALUE

  assertHasOneFreeItem();

        unsigned int *p     = &m_data[m_firstFreeBit / 32];
  const unsigned int offset =         m_firstFreeBit % 32;
  m_firstFreeBit += m_bitsPerItem;
        unsigned int rest   = m_bitsPerItem;
  const unsigned int n      = min(32 - offset, rest);
  *p |= (v & (m_maxValue >> (m_bitsPerItem - n))) << offset;
  if(rest -= n) {
    p[1] |= (v>>n) & (m_maxValue >> (m_bitsPerItem - rest));
  }
}

void PackedArray::add(unsigned __int64 index, unsigned int v) {
  CHECK_VALUE

  addZeroes(index, 1);

  set(index, v);
}

// TODO works only for little-endian
void PackedArray::addZeroes(unsigned __int64 index, unsigned __int64 count) {
#ifdef _DEBUG
  if(index > size()) {
    indexError(index, _T("addZeroes"));
  }
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
    unsigned int *p0          = &m_data[0];
    unsigned int *basep       = p0 + bitPos/32; // first integer to copy from

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
        assert(((char*)basep <= src) && (src < dst) && (dst+bytesToMove <= (((char*)&m_data.last()) + sizeof(unsigned int))));
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
      { unsigned int      *src         = p0 + m_firstFreeBit / 32;
        unsigned int      *dst         = p0 + (newFreeBit-1) / 32;
        const    int       srcOffset   = m_firstFreeBit % 32;
                 int       dstOffset   = newFreeBit % 32;
        if(dstOffset == 0) dstOffset   = 32;
        const __int64      dstBitIndex = bitPos + bitsToAdd;
        unsigned int      *dstp        = p0 + dstBitIndex/32; // lowest address to copy to
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

        const unsigned int mask = (1<<baseOffset)-1;
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
void PackedArray::remove(unsigned __int64 index, unsigned __int64 count) {
  const unsigned int j = index + count;
#ifdef _DEBUG
  if(j > size()) {
    indexError(j, format(_T("remove(%lu,%lu):"), index, count).cstr());
  }
#endif

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
    unsigned int      *p0        = &m_data[0];
    unsigned int      *basep     = p0 + bitPos/32; // first integer to copy to

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
        assert(((char*)basep <= dst) && (dst < src) && (src+bytesToMove <= (((char*)&m_data.last()) + sizeof(unsigned int))));
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
        unsigned int      *src         = p0 + srcBitIndex / 32;
        unsigned int      *dst         = basep;
        const    int       dstOffset   = baseOffset;
        unsigned int      *lastp       = &m_data.last(); // higest address to copy from
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
        const unsigned int mask = (1<<dstOffset)-1;
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
  const unsigned int newDataSize = (newFreeBit / 32) + (lastOffset ? 1 : 0);
  if(newDataSize < (unsigned int)m_data.size()) {
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

void PackedArray::setCapacity(unsigned __int64 capacity) {
  if(capacity < size()) {
    capacity = size();
  }
  if(capacity == 0) {
    return;
  }
  unsigned __int64 bitCapacity = capacity * m_bitsPerItem;
  unsigned int     intCapacity = (bitCapacity - 1) / 32 + 1;
  m_data.setCapacity(intCapacity);
}

bool PackedArray::operator==(const PackedArray &a) const {
  if((m_bitsPerItem != a.m_bitsPerItem) || (m_firstFreeBit != a.m_firstFreeBit)) {
    return false;
  }
  return m_data == a.m_data;
}

void PackedArray::checkInvariant() const {
  const int expectedSize = m_firstFreeBit ? ((m_firstFreeBit-1) / 32 + 1) : 0;
  if(m_data.size() != expectedSize) {
    throwException(_T("PackedArray:Bits/Item:%d. m_firstFreeBit:%d, m_data.size=%d, Should be %d")
                   ,m_bitsPerItem, m_firstFreeBit, m_data.size(), expectedSize);
  }
  if(expectedSize > 0 && (m_firstFreeBit%32)) {
    const unsigned int lastInt = m_data[expectedSize-1];
    if((lastInt & ~((1<<(m_firstFreeBit%32))-1)) != 0) {
      throwException(_T("PackedArray:Bits/Item:%d. m_firstFreeBit:%d, m_data.size=%d, Garbagebits in last element:%08x")
                     ,m_bitsPerItem, m_firstFreeBit, m_data.size(), lastInt);
    }
  }
}

String PackedArray::toString() const {
  String result = format(_T("Packed Array:Bits/Item:%d, Size:%d, firstFreeBit:%d. m_data.size:%d\n")
                        ,m_bitsPerItem, size(), m_firstFreeBit, m_data.size());
  TCHAR tmp[40];
  int l = getBitsPerItem();
  size_t n = m_data.size() * 32 / l;
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += " ";
      bitCount %= 32;
    }
    if(i % 10 == 0) {
      result += format(_T("%*d"), l, i/10);
    } else {
      result += format(_T("%*s"), l," ");
    }
  }
  result += _T("\n");
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += ":";
      bitCount %= 32;
    }
    result += format(_T("%*d"), l, i%10);
  }
  result += _T("\n");

  for(size_t i = 0, bitCount = 0; i < size(); i++) { // print member of the array
    bitCount += l;
    if(bitCount > 32) {
      result += ":";
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
      result += format(_T("%*d"), l, i/10);
    } else {
      result += format(_T("%*s"), l," ");
    }
  }
  result += _T("\n");
  for(size_t i = 0, bitCount = 0; i < n; i++) { // print indices
    bitCount += l;
    if(bitCount > 32) {
      result += ":";
      bitCount %= 32;
    }
    result += format(_T("%*d"), l, i%10);
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
