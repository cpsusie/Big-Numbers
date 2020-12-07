#include "pch.h"
#include <FileContent.h>

void setWindowCursor(HWND wnd, const TCHAR *name) {
  setWindowCursor(wnd, ::LoadCursor(nullptr,name));
}

void setWindowCursor(HWND wnd, int resId) {
  setWindowCursor(wnd, AfxGetApp()->LoadCursor(resId));
}

void setWindowCursor(HWND wnd, HCURSOR cursor) {
  SetClassLongPtr(wnd,GCLP_HCURSOR,(LONG_PTR)cursor);
}

void setSystemCursor(int id, const TCHAR *name) {
  setSystemCursor(id, ::LoadCursor(nullptr,name));
}

void setSystemCursor(int id, int resId) {
  setSystemCursor(id, AfxGetApp()->LoadCursor(resId));
}

void setSystemCursor(int id, HCURSOR cursor) {
  ::SetSystemCursor(CopyCursor(cursor),id);
}

HCURSOR createCursor(const ByteArray &bytes) {
  HCURSOR cursor = (HCURSOR)CreateIconFromResource((BYTE*)bytes.getData(), (DWORD)bytes.size(), FALSE, 0x00030000);
  if(cursor == nullptr) {
    throwLastErrorOnSysCallException(__TFUNCTION__, _T("CreateIconFromResource"));
  }
  return cursor;
}

HCURSOR loadCursor(FILE *f) {
  return createCursor(FileContent(f));
}

HCURSOR loadCursor(const String &fileName) {
  return createCursor(FileContent(fileName));
}
