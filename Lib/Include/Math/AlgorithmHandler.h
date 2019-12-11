#pragma once

template <typename T> class AlgorithmHandler {
public:
  virtual void handleData(const T &data) = 0;
};

