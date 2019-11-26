#pragma once

#include "CompactArray.h"

class Runnable {
public:
  virtual UINT run() = 0;
  virtual ~Runnable() {
  }
};

class RunnableArray : public CompactArray<Runnable*> {
public:
  RunnableArray() : CompactArray<Runnable*>() {
  }
  explicit RunnableArray(size_t capacity) : CompactArray<Runnable*>(capacity) {
  }
};
