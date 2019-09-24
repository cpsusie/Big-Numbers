#include "pch.h"
#include <StrStream.h>
#include "D80ToDbgString.h"

ADDIN_API HRESULT WINAPI AddIn_Double80(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    Double80 d;
    pHelper->getRealObject(&d, sizeof(d));
    strncpy(pResult, D80ToDbgString(d).c_str(), maxResult);
  } catch(...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}

string tostring(const Double80 &x, StreamSize precision, StreamSize width, FormatFlags flags) {
  return (TostringStream(precision, width, flags) << x).str().c_str();
}

string D80ToDbgString(const Double80 &d80) {
  String result;
  if(!isfinite(d80)) {
    char tmp[100];
    return StrStream::formatUndefined(tmp, _fpclass(d80));
  }
  const int expo10 = Double80::getExpo10(d80);
  if((expo10 < -4) || (expo10 >= 18)) {
    char str[50];
    d80toa(str, d80);
    if(_fpclass(d80) & (_FPCLASS_PD | _FPCLASS_ND)) { // denormalized
      strcat(str, "#DEN");
    }
    return str;
  } else {
    return tostring(d80, 19 - expo10, 21, ios::fixed | ios::left);
  }
}
