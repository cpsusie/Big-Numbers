#include "pch.h"

unsigned char toAscii(UINT virtualCode) {
  BYTE keystate[256];
  if(!::GetKeyboardState(keystate)) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("GetKeyboardState"));
  }
  WORD ch;
  const int ret = ToAscii(virtualCode, 0, keystate, &ch, 0);
  return ret == 1 ? ch : 0;
}
