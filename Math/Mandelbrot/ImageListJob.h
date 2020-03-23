#pragma once

class FrameGenerator {
public:
  virtual CSize    getFrameSize()  = 0;
  virtual HBITMAP  nextBitmap() {    // should return NULL when no more frames.
    return NULL;
  }
  virtual String   getDirName() = 0;
};

class ImageListJob : public SafeRunnable {
private:
  FrameGenerator  &m_frameGenerator;
public:
  ImageListJob(FrameGenerator *frameGenerator);
  UINT safeRun();
};
