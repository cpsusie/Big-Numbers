#pragma once

class Runnable {
public:
  virtual UINT run() = 0;
  virtual ~Runnable() {
  }
};
