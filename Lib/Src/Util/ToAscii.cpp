#include "pch.h"

unsigned char toAscii(unsigned int virtualCode) {
  BYTE keystate[256];
  ::GetKeyboardState(keystate);
  WORD ch;
  int ret = ToAscii(virtualCode, 0, keystate, &ch, 0);
  return ret == 1 ? ch : 0;
}
