#pragma once

class RollingAvg {
private:
  bool  m_enabled;
  UINT  m_queueSize;
public:
  RollingAvg(bool enabled=false, UINT queueSize=10)
    : m_enabled(false)
  {
    setQueueSize(queueSize);
  }
  inline void setEnabled(bool enabled) {
    m_enabled = enabled;
  }
  inline bool isEnabled() const {
    return m_enabled;
  }
  void setQueueSize(UINT queueSize);
  inline UINT getQueueSize() const {
    return m_queueSize;
  }
  static UINT getMinQueueSize() {
    return 1;
  }
  static UINT getMaxQueueSize() {
    return 10000;
  }
  static inline bool isValidQueueSize(UINT queueSize) {
    return (getMinQueueSize() <= queueSize) && (queueSize <= getMaxQueueSize());
  }
  // throws Exception i !isValidQueueSize(queueSize)
  static void checkQueueSize(UINT queueSize);
  static RollingAvg s_default;
};

bool operator==(const RollingAvg &r1, const RollingAvg &r2);
inline bool operator!=(const RollingAvg &r1, const RollingAvg &r2) {
  return !(r1 == r2);
}
