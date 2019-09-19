#include "pch.h"
#include <Math/Int128.h>

using namespace std;

static char *i128ToHexDbgString(char *dst, _int128 x, size_t maxResult) {
  stringstream str;
  str.width(32);
  str.precision(0);
  str.fill('0');
  str.flags(ios::right|ios::hex); // we want prefix 0x for all values. ios::showpos doesn't show 0x for 0!! so we add it manually here 
  str << x;
  return strncat(strcpy(dst, "0x"), str.str().c_str(), maxResult);
}

ADDIN_API HRESULT WINAPI AddIn__int128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    _int128 x;
    pHelper->getRealObject(&x, sizeof(x));
    if(nBase == 16) {
      i128ToHexDbgString(pResult, x, maxResult);
    } else {
      char tmp[150];
      strncpy(pResult, _i128toa(x, tmp, nBase), maxResult);
    }
  } catch(...) {
    strcpy(pResult, "");
  }
  return S_OK;
}

ADDIN_API HRESULT WINAPI AddIn__uint128(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    _uint128 x;
    pHelper->getRealObject(&x, sizeof(x));
    if(nBase == 16) {
      i128ToHexDbgString(pResult, x, maxResult);
    } else {
      char tmp[150];
      strncpy(pResult, _ui128toa(x, tmp, nBase), maxResult);
    }
  } catch(...) {
    strcpy(pResult, "");
  }
  return S_OK;
}
