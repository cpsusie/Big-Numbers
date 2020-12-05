#pragma once

class StatePair {
public:
  static constexpr UINT NoFromStateCheck = AbstractParserTables::_NoFromStateCheck;
  UINT m_fromState;
  UINT m_newState;
  UINT m_fromStateCount;
  bool m_shiftText;
  inline StatePair() : m_fromState(0), m_newState(0) {
  }
  StatePair(UINT fromState, UINT newState, bool shiftText, UINT fromStateCount=1);
  inline bool isNoFromStateCheck() const {
    return m_fromState == NoFromStateCheck;
  }
  inline UINT getFromStateCount() const {
    return m_fromStateCount;
  }
  String toString() const;
};

inline std::wostream &operator<<(std::wostream &out, const StatePair &p) {
  out << p.toString();
  return out;
}
