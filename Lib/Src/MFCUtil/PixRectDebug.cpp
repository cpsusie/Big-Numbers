#include "stdafx.h"
#include <PixRect.h>
#include <WinTools.h>

void DebugPixRect::showPixRect(const PixRect *pr) { // static paint pr in upper left corner of the screen.
  CBitmap bm;
  pr->toBitmap(bm);

  DebugBitmap::showBitmap(bm);
}
