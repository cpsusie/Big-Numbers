#pragma once

#include "GrammarCode.h"
#include "UsedByBitSet.h"

class Macro {
private:
  mutable UsedByBitSet m_usedBySet;
  mutable UINT         m_usedByCount; // == m_usedBySet.size()
  int                  m_index;       // index in array
  String               m_name, m_comment;
  const String         m_value;
public:
  Macro(const BitSetParameters &usedByParam, UINT usedByV0, const String &value, const String &comment);
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
  Macro &setComment(const String &comment) {
    m_comment = comment;
    return *this;
  }
  // commentWidth1 = min width of comment excl usedBy-string
  String getComment(bool includeUsedBy = false, UINT commentWidth1 = 0, const AbstractSymbolNameContainer *nameContainer = nullptr) const;
  inline const UsedByBitSet &getUsedBySet() const {
    return m_usedBySet;
  }
  inline UINT getUsedByCount() const {
    return m_usedByCount;
  }
  inline void addUsedByValue(UINT usedBy) const {
    m_usedBySet.add(usedBy);
    m_usedByCount++;
  }
  void print(MarginFile &f, UINT commentWidth1 = 0, const AbstractSymbolNameContainer *nameContainer = nullptr) const;
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
