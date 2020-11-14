#pragma once

class Macro {
private:
  mutable StateSet m_stateSet;
  mutable UINT     m_stateSetSize;
  int              m_index;     // index in array m_actionCode
  String           m_name;
  const String     m_value, m_comment;
public:
  Macro(UINT stateCount, UINT state0, const String &value, const String &comment)
    : m_stateSet(stateCount)
    , m_stateSetSize(0)
    , m_index(  -1         )
    , m_value(  value      )
    , m_comment(comment    )
  {
    addState(state0);
  }
  inline int getIndex() const {
    return m_index;
  }
  Macro &setIndex(UINT index) {
    m_index = index;
    return *this;
  }
  Macro &setName(const String &name) {
    m_name = name;
    return *this;
  }
  inline const String &getName() const {
    return m_name;
  }
  inline const String &getValue() const {
    return m_value;
  }
  String getComment() const;
  inline const StateSet &getStateSet() const {
    return m_stateSet;
  }
  inline UINT getStateSetSize() const {
    return m_stateSetSize;
  }
  inline void addState(UINT state) const {
    m_stateSet.add(state);
    m_stateSetSize++;
  }
  void print(MarginFile &f) const;
};

inline bool operator==(const Macro &m1, const Macro &m2) {
  return (m1.getName() == m2.getName()) && (m1.getValue() == m2.getValue());
}

inline int macroCmpByName(const Macro &m1, const Macro &m2) {
  return stringCmp(m1.getName(), m2.getName());
}

inline int macroCmpByIndex(const Macro &m1, const Macro &m2) {
  return m1.getIndex() - m2.getIndex();
}
