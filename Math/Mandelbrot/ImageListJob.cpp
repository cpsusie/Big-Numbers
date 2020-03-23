#include "stdafx.h"
#include "ImageListJob.h"

ImageListJob::ImageListJob(FrameGenerator *frameGenerator) : m_frameGenerator(*frameGenerator) {
}

UINT ImageListJob::safeRun() {
  const String dirName = m_frameGenerator.getDirName();
  const CSize size = m_frameGenerator.getFrameSize();

  HBITMAP bm;
  int frameIndex = 0;
  while((bm = m_frameGenerator.nextBitmap()) != NULL) {
    frameIndex++;
    const String fileName = FileNameSplitter::getChildName(dirName, format(_T("frame%05d.jpg"), frameIndex));
    DEBUGLOG(_T("got frame\n"));
    writeAsJPG(bm, ByteOutputFile(fileName));
  }
  return 0;
}
