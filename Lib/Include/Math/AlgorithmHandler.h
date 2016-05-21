#pragma once

template <class T> class AlgorithmHandler {
public:
  virtual void handleData(const T &data) = 0;
};

