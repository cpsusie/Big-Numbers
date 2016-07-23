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
  const DWORD  capacity = (DWORD)set.m_capacity;

  if (capacity == 0) {
    return _T("empty");
  }
  const QWORD  startAddr  = set.m_paddr;
  DWORD        byte0Index = 0; // in bytes
  String       result;
//  return format(_T("capacity:%u, addr:%#I64x"), capacity, startAddr);
  for(DWORD bytesLeft = BYTECOUNT(capacity); bytesLeft > 0;) {
    BYTE buffer[1024];
    const int   n = min(bytesLeft, sizeof(buffer));
    bytesLeft -= n;
    m_helper->getObjectx64(buffer, startAddr + byte0Index, n);
    const BYTE *p = (BYTE*)buffer;
    for (int i = n; i--; p++) {
      if (*p) {
        int k = (byte0Index + (p - buffer)) * 8;
        for (BYTE b = *p; b; b >>= 1, k++) {
          if (b & 1) {
            TCHAR numStr[20];
            _ui64tot(k, numStr, 10);
            size_t nl = _tcslen(numStr)+1;
            size_t rl = result.length();
            if (rl + nl >= maxResult) {
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
  return result.length() ? result : _T("empty");
}

ADDIN_API HRESULT WINAPI AddIn_BitSet(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  String tmpStr;
  try {
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
  }
  catch (...) {
    tmpStr = _T("Invalid address");
  }

  USES_CONVERSION;
  const char *cp = T2A(tmpStr.cstr());
  strncpy(pResult, cp, maxResult);
  return S_OK;
}
