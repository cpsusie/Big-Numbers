#pragma once

class Macro {
private:
  mutable BitSet   m_usedBySet;
  mutable UINT     m_usedByCount; // == m_usedBySet.size()
  int              m_index;       // index in array m_actionCode
  String           m_name;
  const String     m_value, m_comment;
public:
  Macro(UINT usedBySetSize, UINT usedByV0, const String &value, const String &comment)
    : m_usedBySet(usedBySetSize)
    , m_usedByCount(0          )
    , m_index(  -1             )
    , m_value(  value          )
    , m_comment(comment        )
  {
    addUsedByValue(usedByV0);
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
  inline const BitSet &getUsedBySet() const {
    return m_usedBySet;
  }
  inline UINT getUsedByCount() const {
    return m_usedByCount;
  }
  inline void addUsedByValue(UINT usedBy) const {
    m_usedBySet.add(usedBy);
    m_usedByCount++;
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
