#include "pch.h"
#include <CompactHashMap.h>

template<typename T> struct NameValue {
  const T      m_value;
  const TCHAR *m_str;
};

template<typename T, const NameValue<T> *table, UINT n> class NameLookup : public CompactStrIHashMap<T> {
protected:
  NameLookup() : CompactStrIHashMap(31) {
    for(UINT i = 0; i < n; i++) {
      const NameValue<T> &nv = table[i];
      put(nv.m_str, nv.m_value);
    }
  }
public:
  T lookupName(const String &name) const {
    const T *v = get(name.cstr());
    if(v == NULL) {
      throwInvalidArgumentException(__TFUNCTION__, _T("name=%s"), name.cstr());
    }
    return *v;
  }
};

#define DefineNameLookupClass(Type,Table,lookupTableNamePrefix)                               \
class lookupTableNamePrefix##LookupTable : public NameLookup<Type, Table, ARRAYSIZE(Table)> { \
public:                                                                                       \
  static const lookupTableNamePrefix##LookupTable &getInstance() {                            \
    static const lookupTableNamePrefix##LookupTable instance;                                 \
    return instance;                                                                          \
  }                                                                                           \
}

// used for small tables
template<typename T> T linearStringSearch(const String &str, const T *table, size_t n, String (*toString)(T)) {
  for(const T *endp = table + n; table < endp; table++) {
    if(str.equalsIgnoreCase(toString(*table))) {
      return *table;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("s=%s"), str.cstr());
  return (T)0;
}
