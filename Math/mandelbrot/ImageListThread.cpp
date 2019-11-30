#include "stdafx.h"
#include <FileNameSplitter.h>
#include "ImageListThread.h"

ImageListThread::ImageListThread(FrameGenerator *frameGenerator) : m_frameGenerator(*frameGenerator) {
  setDemon(true);
}

UINT ImageListThread::run() {
  const String dirName = m_frameGenerator.getDirName();

  try {
    const CSize size = m_frameGenerator.getFrameSize();

    HBITMAP bm;
    int frameIndex = 0;
    while((bm = m_frameGenerator.nextBitmap()) != NULL) {
      frameIndex++;
      const String fileName = FileNameSplitter::getChildName(dirName, format(_T("frame%05d.jpg"), frameIndex));
      DLOG(_T("got frame\n"));
      writeAsJPG(bm, ByteOutputFile(fileName));
    }
  } catch(Exception e) {
    DLOG(_T("ImageListThread got Exception:%s\n"), e.what());
    showException(e);
  } catch(...) {
    DLOG(_T("ImageListThread got unknown Exception\n"));
    showError(_T("Unknown Exception"));
  }
  return 0;
}
