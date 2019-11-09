#include "pch.h"


ADDIN_API HRESULT WINAPI AddIn_EndGameResult(DWORD dwAddress, DEBUGHELPER *pHelper, int nBase, BOOL bUniStrings, char *pResult, size_t maxResult, DWORD /*reserved*/) {
  try {
    String str;
    EndGameResult egr;
    pHelper->getRealObject(&egr, sizeof(egr));
    if(egr.isPrunedWinnerResult()) {
      str = _T("Pruned winner");
    } else if(!egr.exists()) {
      str = _T("Non existing");
    } else {
      const EndGamePositionStatus status     = egr.getStatus();
      const UINT                  pliesToEnd = egr.getPliesToEnd();
      switch(status) {
      case EG_UNDEFINED:
        str = _T("Status=UNDEFINED");
        break;
      case EG_DRAW     :
        str = format(_T("Status=DRAW, pliesToEnd=%3u"), pliesToEnd);
        switch(pliesToEnd) {
        case 0: str += _T(" (=Stalemate)"    ); break;
        case 1: str += _T(" (=Terminal Draw)"); break;
        }
        break;
      case EG_WHITEWIN :
        str = format(_T("Status=WHITEWIN(%3d plies)"), pliesToEnd);
        if(pliesToEnd==0) str += _T(" Black is mate");
        break;
      case EG_BLACKWIN :
        str = format(_T("Status=BLACKWIN(%3d plies)"), pliesToEnd);
        if(pliesToEnd==0) str += _T(" White is mate");
        break;
      }
    }
    USES_CONVERSION;
    const char *cp = T2A(str.cstr());
    strncpy(pResult, cp, maxResult);
  } catch (...) {
    strncpy(pResult, "", maxResult);
  }
  return S_OK;
}
