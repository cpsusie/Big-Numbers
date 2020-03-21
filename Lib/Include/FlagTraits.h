#pragma once

#define _FLAGTRAITS(FLAGTYPE, PARAMTYPE, THISTYPE)  \
FLAGTYPE m_flags;                                   \
inline THISTYPE &setFlag(PARAMTYPE flags) {         \
  m_flags |= flags;                                 \
  return *this;                                     \
}                                                   \
inline THISTYPE &clrFlag(PARAMTYPE flags) {         \
  m_flags &= ~flags;                                \
  return *this;                                     \
}                                                   \
inline THISTYPE &setFlag(PARAMTYPE flags, bool v) { \
  return v ? setFlag(flags) : clrFlag(flags);       \
}                                                   \
inline bool isSet(PARAMTYPE flags) const {          \
  return (m_flags & flags) != 0;                    \
}

#define FLAGTRAITS(      FLAGTYPE , THISTYPE) _FLAGTRAITS(FLAGTYPE, FLAGTYPE, THISTYPE)
#define ATOMICFLAGTRAITS(PARAMTYPE, THISTYPE) _FLAGTRAITS(std::atomic<PARAMTYPE>, PARAMTYPE, THISTYPE)
