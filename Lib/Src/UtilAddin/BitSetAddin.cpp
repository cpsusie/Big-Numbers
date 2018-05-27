#include "pch.h"

class BitSetx86 {
public:
  DWORD m_paddr;
  DWORD m_capacity;
};

class BitSetx64 {
public:
  QWORD m_paddr;
  QWORD m_capacity;
};

template<class BitSet> class BitSetAddIn {
private:
  DEBUGHELPER *m_helper;
public:
  BitSetAddIn(DEBUGHELPER *pHelper) : m_helper(pHelper) {
  }
  String toString(BitSet &s, size_t maxResult) const;
};

#define BYTECOUNT(size) (((size)-1) / 8 + 1)

template<class BitSet> String BitSetAddIn<BitSet>::toString(BitSet &set, size_t maxResult) const {
  const QWORD  capacity = set.m_capacity;

  if(capacity == 0) {
    return _T("empty");
  }
  maxResult -= 2;
  const QWORD  startAddr  = set.m_paddr;
  QWORD        byte0Index = 0; // in bytes
  String       result;
  const int    maxBufCount = 30; // to prevent long responstime for very large bitsets
  int          buffersRead = 0;
//  return format(_T("capacity:%u, addr:%#I64x"), capacity, startAddr);
  for(QWORD bytesLeft = BYTECOUNT(capacity); bytesLeft > 0;) {
    BYTE      buffer[4096];
    const int n = (int)min(bytesLeft, sizeof(buffer));
    if(buffersRead++ == maxBufCount) {
      if(result.length() == 0) {
        result = format(_T("It will take a long time to expand BitSet. Capacity=%I64u."), capacity);
      } else {
        for(size_t rl = result.length(); rl < maxResult; rl++) {
          result += _T('.');
        }
      }
      break;
    }
    bytesLeft -= n;
    m_helper->getObjectx64(buffer, startAddr + byte0Index, n);
    const BYTE *p = (BYTE*)buffer;
    for(int i = n; i--; p++) {
      if(*p) {
        QWORD k = (byte0Index + (p - buffer)) * 8;
        for(BYTE b = *p; b; b >>= 1, k++) {
          if(b & 1) {
            TCHAR numStr[20];
            _ui64tot(k, numStr, 10);
            size_t nl = _tcslen(numStr)+1;
            size_t rl = result.length();
            if(rl + nl >= maxResult) {
              while(rl < maxResult) {
                result += _T('.'); rl++;
              }
              return result;
            }
            if(rl) result += _T(' ');
            result += numStr;
          }
        }
      }
    }
    byte0Index += n;
  }
  return result.length() ? (_T("{") + result + _T("}")) : _T("empty");
}

ADDIN_API HRESULT WINAPI AddIn_BitSet(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    String tmpStr;
    switch (pHelper->getProcessorType()) {
    case PRTYPE_X86:
      { BitSetx86 set;
        pHelper->getRealObject(&set, sizeof(set));
        tmpStr = BitSetAddIn<BitSetx86>(pHelper).toString(set, maxResult - 1);
      }
      break;
    case PRTYPE_X64:
      { BitSetx64 set;
        pHelper->getRealObject(&set, sizeof(set));
        tmpStr = BitSetAddIn<BitSetx64>(pHelper).toString(set, maxResult - 1);
      }
      break;
    }
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "undefined", maxResult);
  }
  return S_OK;
}
