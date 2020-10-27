#include "stdafx.h"
#include <MFCUtil/AviFile.h>
#include "MovieThread.h"

MovieThread::MovieThread(FrameGenerator *frameGenerator) : m_frameGenerator(*frameGenerator) {
  setDemon(true);
}

unsigned int MovieThread::run() {
  const TCHAR *fileName = _T("test.avi");
  CAviFile avi(fileName, true, 0, 15);
  HDC     dc        = nullptr;
  HBITMAP bitmap    = nullptr;
  HGDIOBJ oldBitmap = nullptr;

  try {
    HDC screenDC = getScreenDC();
    dc     = CreateCompatibleDC(screenDC);
    const CSize size = m_frameGenerator.getFrameSize();

    bitmap = CreateCompatibleBitmap(screenDC, size.cx, size.cy);

    ::ReleaseDC(0, screenDC);

    oldBitmap = SelectObject(dc, bitmap);

    unlink(fileName);

    PixRect *pr;
    while((pr = m_frameGenerator.nextFrame()) != nullptr) {
      PixRect::bitBlt(dc, 0,0,size.cx,size.cy, SRCCOPY, pr, 0, 0);
      avi.appendNewFrame(bitmap);
    }
  } catch(Exception e) {
    showException(e);
  } catch(...) {
    showError(_T("Unknown Exception"));
  }

  if(oldBitmap != nullptr) {
    SelectObject(dc, oldBitmap);
    oldBitmap = nullptr;
  }
  if(dc != nullptr) {
    DeleteDC(dc);
    dc = nullptr;
  }
  if(bitmap != nullptr) {
    DeleteObject(bitmap);
    bitmap = nullptr;
  }
  return 0;
}
