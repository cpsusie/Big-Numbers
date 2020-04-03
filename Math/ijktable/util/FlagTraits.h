#pragma once

template<typename M, typename P> class FlagSet {
public:
  M m_flags;
  inline FlagSet(P initialValue = 0) : m_flags(initialValue) {
  }
  inline FlagSet &set(P s) {
    m_flags |= s; return *this;
  }
  inline FlagSet &clr(P s) {
    m_flags &= ~s; return *this;
  }
  inline FlagSet &set(P s, bool v) {
    return v ? set(s) : clr(s);
  }
  // Return true if any bits specified in s are also set in FlagSet. Else return false
  inline bool anySet(P s) const {
    return (m_flags & s) != 0;
  }
  // Return true if all bits specified in s are also set in FlagSet. Else return false
  inline bool allSet(P s) const {
    return (m_flags & s) == s;
  }
  inline operator M&() {
    return m_flags;
  }
  inline operator const M&() const {
    return m_flags;
  }
};

#define _FLAGTRAITS(THISTYPE, FLAGTYPE, PARAMTYPE, varName)  \
FlagSet<FLAGTYPE,PARAMTYPE> varName;                         \
inline THISTYPE &setFlag(PARAMTYPE _##varName) {             \
  varName.set(_##varName);                                   \
  return *this;                                              \
}                                                            \
inline THISTYPE &clrFlag(PARAMTYPE _##varName) {             \
  varName.clr(_##varName);                                   \
  return *this;                                              \
}                                                            \
inline THISTYPE &setFlag(PARAMTYPE _##varName, bool v) {     \
  varName.set(_##varName, v);                                \
  return *this;                                              \
}                                                            \
inline bool isSet(PARAMTYPE _##varName) const {              \
  return varName.anySet(_##varName);                         \
}

#define FLAGTRAITS(      THISTYPE, FLAGTYPE , varName) _FLAGTRAITS(THISTYPE, FLAGTYPE              , FLAGTYPE , varName)
#define ATOMICFLAGTRAITS(THISTYPE, PARAMTYPE, varName) _FLAGTRAITS(THISTYPE, std::atomic<PARAMTYPE>, PARAMTYPE, varName)
