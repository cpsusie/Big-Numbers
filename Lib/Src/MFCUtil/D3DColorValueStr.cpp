#include "pch.h"
#include <MFCUtil/ColorSpace.h>

String toString(const D3DCOLORVALUE &c, bool showAlpha) {
  if(showAlpha) {
    return format(_T("R:%.2f G:%.2f B:%.2f A:%.2f"), c.r,c.g,c.b, c.a);
  } else {
    return format(_T("R:%.2f G:%.2f B:%.2f"), c.r,c.g,c.b);
  }
}
