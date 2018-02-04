#include "pch.h"

ICONINFO getIconInfo(HICON icon) {
  ICONINFO info;
  if(GetIconInfo(icon, &info) == 0) {
    throwException(_T("GetIconSize failed:%s. icon=%p"), getLastErrorText().cstr(), icon);
  }
  return info;
}

CSize getIconSize(HICON icon) {
  ICONINFO info = getIconInfo(icon);
  const CSize result = getBitmapSize(info.hbmColor);
  closeIconInfo(info);
  return result;
}

void closeIconInfo(ICONINFO &info) {
  DeleteObject(info.hbmColor);
  DeleteObject(info.hbmMask);
  info.hbmColor = NULL;
  info.hbmMask  = NULL;
}
