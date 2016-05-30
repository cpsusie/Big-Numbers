#include "stdafx.h"
#include <MyUtil.h>
#include <FileContent.h>
#include <MFCUtil/WinTools.h>

void setWindowCursor(CWnd *wnd, const TCHAR *name) {
  setWindowCursor(wnd, ::LoadCursor(NULL,name));
}

void setWindowCursor(CWnd *wnd, int resId) { 
  setWindowCursor(wnd,AfxGetApp()->LoadCursor(resId));
}

void setWindowCursor(CWnd *wnd, HCURSOR cursor) {
  SetClassLong(wnd->m_hWnd,GCL_HCURSOR,(long)cursor);
}

void setSystemCursor(int id, const TCHAR *name) {
  setSystemCursor(id, ::LoadCursor(NULL,name));
}

void setSystemCursor(int id, int resId) {
  setSystemCursor(id, AfxGetApp()->LoadCursor(resId));
}

void setSystemCursor(int id, HCURSOR cursor) {
  ::SetSystemCursor(CopyCursor(cursor),id);
}

HCURSOR createCursor(const ByteArray &bytes) {
  HCURSOR cursor = (HCURSOR)CreateIconFromResource((BYTE*)bytes.getData(), bytes.size(), FALSE, 0x00030000);
  if(cursor == NULL) {
    throwLastErrorOnSysCallException(_T("CreateIconFromResource"));
  }
  return cursor;
}

HCURSOR loadCursor(FILE *f) {
  return createCursor(FileContent(f));
}

HCURSOR loadCursor(const String &fileName) {
  return createCursor(FileContent(fileName));
}
