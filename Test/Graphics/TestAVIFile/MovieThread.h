#pragma once

#include <Thread.h>

class FrameGenerator {
public:
  virtual CSize    getFrameSize()  = 0;
  virtual PixRect *nextFrame()     = 0; // should return NULL when no more frames.
};

class MovieThread : public Thread {
private:
  FrameGenerator  &m_frameGenerator;
public:
  MovieThread(FrameGenerator *frameGenerator);
  unsigned int run();
};

