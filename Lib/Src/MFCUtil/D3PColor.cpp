#include "pch.h"
#include <MFCUtil/ColorSpace.h>

String D3PCOLOR::toString(bool showAlpha) const {
  const D3DCOLOR cc = *this;
  if(showAlpha) {
    return format(_T("R:%3d G:%3d B:%3d A:%3d"), ARGB_GETRED(cc), ARGB_GETGREEN(cc), ARGB_GETBLUE(cc), ARGB_GETALPHA(cc));
  } else {
    return format(_T("R:%3d G:%3d B:%3d"), ARGB_GETRED(cc), ARGB_GETGREEN(cc), ARGB_GETBLUE(cc));
  }
}
