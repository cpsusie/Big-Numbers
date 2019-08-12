#include "pch.h"
#include "D80ToDbgString.h"

using namespace std;

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Double80 d;
    pHelper->getRealObject(&d, sizeof(d));
    char str[150];
    strCpy(str,D80ToDbgString(d).cstr());
    strncpy(pResult, str, maxResult);
  } catch(...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}

String D80ToDbgString(const Double80 &d80) {
  String result;
  int d80class;
  switch(d80class = _fpclass(d80)) {
  case _FPCLASS_PINF:
    StrStream::formatpinf(result); return result;
  case _FPCLASS_NINF:
    StrStream::formatninf(result); return result;
  case _FPCLASS_QNAN:
    StrStream::formatqnan(result); return result;
  case _FPCLASS_SNAN:
    StrStream::formatsnan(result); return result;
  }
  const int expo10 = Double80::getExpo10(d80);
  if(expo10 < -4 || (expo10 >= 18)) {
    TCHAR str[50];
    d80tot(str, d80);
    if(d80class & (_FPCLASS_PD | _FPCLASS_ND)) { // denormalized
      wcscat(str, _T("#DEN"));
    }
    return str;
  } else {
    return toString(d80, 19 - expo10, 21, ios::fixed | ios::left);
  }
}
