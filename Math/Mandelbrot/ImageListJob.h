#pragma once

class FrameGenerator {
public:
  virtual CSize    getFrameSize()  = 0;
  virtual HBITMAP  nextBitmap() {    // should return nullptr when no more frames.
    return nullptr;
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
