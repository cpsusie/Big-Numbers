#pragma once

#include <Thread.h>

class MemoryLogThread : public Thread {
public:
  unsigned int run();
};

