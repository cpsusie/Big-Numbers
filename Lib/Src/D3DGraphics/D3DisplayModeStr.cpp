#include "pch.h"
#include <D3DGraphics/D3ToString.h>

String toString(const D3DDISPLAYMODE &mode) {
  return format(_T("(W,H)       :(%d,%d)\n"
                   "Refresh rate:%d\n"
                   "Format      :%s\n")
               ,mode.Width, mode.Height
               ,mode.RefreshRate
               ,formatToString(mode.Format).cstr()
               );
}
