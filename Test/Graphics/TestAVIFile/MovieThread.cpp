#include "stdafx.h"
#include <MFCUtil/AviFile.h>
#include "MovieThread.h"

MovieThread::MovieThread(FrameGenerator *frameGenerator) : m_frameGenerator(*frameGenerator) {
  setDeamon(true);
}

unsigned int MovieThread::run() {
  const TCHAR *fileName = _T("test.avi");
  CAviFile avi(fileName, true, 0, 15);
  HDC     dc        = NULL;
  HBITMAP bitmap    = NULL;
  HGDIOBJ oldBitmap = NULL;

  try {
    HDC screenDC = getScreenDC();
    dc     = CreateCompatibleDC(screenDC);
    const CSize size = m_frameGenerator.getFrameSize();

    bitmap = CreateCompatibleBitmap(screenDC, size.cx, size.cy);

    ::ReleaseDC(0, screenDC);

    oldBitmap = SelectObject(dc, bitmap);

    unlink(fileName);

    PixRect *pr;
    while((pr = m_frameGenerator.nextFrame()) != NULL) {
      PixRect::bitBlt(dc, 0,0,size.cx,size.cy, SRCCOPY, pr, 0, 0);
      avi.appendNewFrame(bitmap);
    }
  } catch(Exception e) {
    Message(_T("Exception:%s"), e.what());
  } catch(...) {
    Message(_T("Unknown Exception"));
  }

  if(oldBitmap != NULL) {
    SelectObject(dc, oldBitmap);
    oldBitmap = NULL;
  }
  if(dc != NULL) {
    DeleteDC(dc);
    dc = NULL;
  }
  if(bitmap != NULL) {
    DeleteObject(bitmap);
    bitmap = NULL;
  }
  return 0;
}
