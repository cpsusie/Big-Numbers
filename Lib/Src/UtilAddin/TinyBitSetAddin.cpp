#include "pch.h"
#include <TinyBitSet.h>

static String tinyBitSetToString(BYTE *bits, int byteCount) {
  String result;
  for(int i = 0; i < byteCount; i++, bits++) {
    BYTE b = *bits;
    if(b) {
      for(int k = i * 8; b; k++, b >>= 1) {
        if(b & 1) {
          TCHAR numStr[20];
          _itot(k, numStr, 10);
//          size_t nl = _tcslen(numStr)+1;
          size_t rl = result.length();
          if(rl) result += _T(' ');
          result += numStr;
        }
      }
    }
  }
  if(result.length() == 0) {
    result = _T("empty");
  }
  return _T("{") +  result + _T("}");
}

ADDIN_API HRESULT WINAPI AddIn_BitSet8(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    BitSet8 set;
    pHelper->getRealObject(&set, sizeof(set));
    const String tmpStr = tinyBitSetToString((BYTE*)&set,sizeof(set));
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "undefined", maxResult);
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_BitSet16(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    BitSet16 set;
    pHelper->getRealObject(&set, sizeof(set));
    const String tmpStr = tinyBitSetToString((BYTE*)&set,sizeof(set));
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "undefined", maxResult);
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_BitSet32(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    BitSet32 set;
    pHelper->getRealObject(&set, sizeof(set));
    const String tmpStr = tinyBitSetToString((BYTE*)&set,sizeof(set));
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "undefined", maxResult);
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn_BitSet64(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    BitSet64 set;
    pHelper->getRealObject(&set, sizeof(set));
    const String tmpStr = tinyBitSetToString((BYTE*)&set,sizeof(set));
    USES_CONVERSION;
    const char *cp = T2A(tmpStr.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "undefined", maxResult);
  }
  return S_OK;
}