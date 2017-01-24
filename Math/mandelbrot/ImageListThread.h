#pragma once

class FrameGenerator {
public:
  virtual CSize    getFrameSize()  = 0;
  virtual HBITMAP  nextBitmap() {    // should return NULL when no more frames.
    return NULL;
  }
  virtual String   getDirName() = 0;
};

class ImageListThread : public Thread {
private:
  FrameGenerator  &m_frameGenerator;
public:
  ImageListThread(FrameGenerator *frameGenerator);
  UINT run();
};
