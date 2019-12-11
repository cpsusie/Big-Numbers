#pragma once

#include "StringArray.h"
#include "Iterator.h"

#ifdef DEBUG_REGISTRY

void registryLog(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

#define REGISTRYLOG(s, p) registryLog(_T("%s %lu\n"), _T(s), p)
#else
#define REGISTRYLOG(s, p)
#endif

class RegistryValueInfo {
private:
  const String m_name;
  const ULONG  m_type, m_size;
public:
  RegistryValueInfo(const String &name, ULONG type, ULONG size)
    : m_name(name), m_type(type), m_size(size)
  {
  }
  inline const String &name() const {
    return m_name;
  }
  inline ULONG type() const {
    return m_type;
  }
  inline ULONG size() const {
    return m_size;
  }
  inline String typeName() const {
    return typeName(m_type);
  }
  static String typeName(ULONG type);
};

class RegistryValue {
private:
  String  m_valueName;
  ULONG   m_type;
  ULONG   m_size;  // in bytes
  BYTE   *m_buffer;

  void init(const String &valueName, ULONG type, ULONG size);
  void cleanup();
  inline void setBuffer(const void *src) {
    if(m_size != 0) memcpy(m_buffer, src, m_size);
  }
  void initValue(const String &valueName, ULONG type, ULONG  value);
  void initValue(const String &valueName, ULONG type, UINT64 value);
public :
  RegistryValue(const RegistryValue &src);
  RegistryValue();
  RegistryValue(       const RegistryValueInfo &info, const BYTE *bytes);
  RegistryValue(       const String &valueName, const BYTE      *bytes, ULONG size, ULONG type = REG_BINARY);
  RegistryValue(       const String &valueName, const String    &str              , ULONG type = REG_SZ);
  inline RegistryValue(const String &valueName, int              value            , ULONG type = REG_DWORD) {
    initValue(valueName, type, (ULONG)value);
  }
  inline RegistryValue(const String &valueName, UINT             value            , ULONG type = REG_DWORD) {
    initValue(valueName, type, (ULONG)value);
  }
  inline RegistryValue(const String &valueName, long             value            , ULONG type = REG_DWORD) {
    initValue(valueName, type, (ULONG)value);
  }
  inline RegistryValue(const String &valueName, ULONG            value            , ULONG type = REG_DWORD) {
    initValue(valueName, type, (ULONG)value);
  }
  inline RegistryValue(const String &valueName, INT64            value            , ULONG type = REG_QWORD) {
    initValue(valueName, type, (UINT64)value);
  }
  inline RegistryValue(const String &valueName, UINT64           value            , ULONG type = REG_QWORD) {
    initValue(valueName, type, (UINT64)value);
  }
  // type = REG_MULTI_SZ
  RegistryValue(const String &valueName, const StringArray &strings);
  ~RegistryValue() {
    cleanup();
  }
  RegistryValue &operator=(const RegistryValue &src);

  // m_name = "", m_type = REG_NONE
  void reset();

  inline RegistryValueInfo getValueInfo() const {
    return RegistryValueInfo(m_valueName, m_type, m_size);
  }
  inline const String &name() const {
    return m_valueName;
  }
  inline ULONG type() const {
    return m_type;
  }
  inline String typeName() const {
    return RegistryValueInfo::typeName(m_type);
  }
  // type must be REG_DWORD, REG_DWORD_LITTLE_ENDIAN, REG_DWORD_BIG_ENDIAN
  operator ULONG()       const; // same as int
  // type must be REG_DWORD, REG_DWORD_LITTLE_ENDIAN, REG_DWORD_BIG_ENDIAN
  inline operator UINT()        const {
    return (ULONG)(*this);
  }
  // type must be REG_QWORD
  operator UINT64()      const;
  // type must be REG_EXPAND_SZ, REG_SZ
  operator String()      const;
  // type must be REG_MULTI_SZ, REG_EXPAND_SZ, REG_SZ
  operator StringArray() const;
  // type must be REG_BINARY
  operator ByteArray()   const;
  inline ULONG getBufSize() const {
    return m_size;
  }
  inline const BYTE *getBuffer() const {
    return m_buffer;
  }

  static String &RegistryValue::bytesToString(String &dst, const void *data, size_t size);
  static inline String bytesToString(const void *data, size_t size) {
    String tmp;
    return bytesToString(tmp, data, size);
  }

  bool operator==(const RegistryValue &r) const;
  inline bool operator!=(const RegistryValue &r) const {
    return !(*this == r);
  }
  String toString() const;
};

template <typename T, typename B> class ReferenceCountedType {
private :
  long m_count;
protected :
  virtual void preRelease() = 0;
  T m_obj;

  virtual ~ReferenceCountedType() {
  }

  ReferenceCountedType(const ReferenceCountedType &src);              // Restrict copying. Not implemented
  ReferenceCountedType &operator=(const ReferenceCountedType &src);   // Restrict copying. Not implemented

public:
  ReferenceCountedType(T obj) : m_obj(obj), m_count(1) {
    REGISTRYLOG("new", this);
  }

  const T &getObject() const {
    return m_obj;
  }

  B *addRef() {
    InterlockedIncrement(&m_count);
    return (B*)this;
  }

  B *release() {
    if(InterlockedDecrement(&m_count) == 0)  {
      try {
        preRelease();
      } catch (...) {
      }
      REGISTRYLOG("delete", this);
      TRACE_DELETE(this); delete this;
    }
    return NULL;
  }

  static B *copy(B *p) {
    return p ? p->addRef() : NULL;
  }
};

class ReferenceCountedRegKey : public ReferenceCountedType<HKEY, ReferenceCountedRegKey> {
private:
  ReferenceCountedRegKey(const ReferenceCountedRegKey &rhs);            // Restrict copying. Not implemented
  ReferenceCountedRegKey &operator=(const ReferenceCountedRegKey &rhs); // Restrict copying. Not implemented
protected:
  void preRelease() {
    REGISTRYLOG("close", m_obj);
    RegCloseKey(m_obj);
  }

  ~ReferenceCountedRegKey() {
  }

public:
  explicit ReferenceCountedRegKey(const HKEY key) : ReferenceCountedType<HKEY, ReferenceCountedRegKey>(key) {
  }
};

class RegistryKey {
private:
  ReferenceCountedRegKey *m_key;
  String                  m_name;
  static ReferenceCountedRegKey *newCountedKey(HKEY key, bool closeKeyOnFailure = false);
  static void checkResult(long result, const TCHAR *function, const TCHAR *regFunction);
  static void checkResult(long result, const TCHAR *function, const TCHAR *regFunction, const String &argument);
  static void createChildName(String &dst, const String &parentName, const String &name);
  RegistryValueInfo getValueInfo(const TCHAR *method, const String &valueName) const;
  BYTE             *getValueData(const TCHAR *method, const RegistryValueInfo &info, BYTE *bytes) const;
  void              setValueData(const TCHAR *method, const String &valueName, ULONG type, const BYTE *bytes, DWORD size) const;
  friend class SubKeyIterator;
  friend class RegValueIterator;
  static const TCHAR *getRootName(HKEY key);

public:
  // Valid values for key are:
  // HKEY_CLASSES_ROOT
  // HKEY_CURRENT_USER
  // HKEY_LOCAL_MACHINE
  // HKEY_USERS
  // HKEY_PERFORMANCE_DATA
  // HKEY_CURRENT_CONFIG
  // HKEY_DYN_DATA
  explicit RegistryKey(HKEY key);
  RegistryKey(const String &remoteMachine, HKEY key);
  RegistryKey(HKEY key, const String &subKey, REGSAM samDesired = KEY_ALL_ACCESS, const String &remoteMachine=EMPTYSTRING);
  RegistryKey(const RegistryKey &src);
  ~RegistryKey();

  RegistryKey &operator=(const RegistryKey &src);

  RegistryKey openKey(const String &subKey, REGSAM samDesired = KEY_ALL_ACCESS) const;

  RegistryKey createKey(        const String         &subKey
                               ,const String         &keyClass           = EMPTYSTRING
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  RegistryKey createOrOpenKey(  const String         &subKey
                               ,ULONG                *disposition        = NULL
                               ,const String         &keyClass           = EMPTYSTRING
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  RegistryKey createOrOpenPath( const String         &path
                               ,ULONG                *disposition        = NULL
                               ,const String         &keyClass           = EMPTYSTRING
                               ,ULONG                 options            = REG_OPTION_NON_VOLATILE
                               ,REGSAM                samDesired         = KEY_ALL_ACCESS
                               ,SECURITY_ATTRIBUTES  *securityAttributes = NULL) const;

  void deleteKey(          const String &keyName) const;
  void deleteKeyAndSubkeys(const String &keyName) const;
  void deleteSubKeys() const;
  void deleteValue(        const String &valueName) const;
  void deleteValues() const;
  bool hasSubkey(          const String &subkey, REGSAM samDesired = KEY_ALL_ACCESS) const;

  RegistryKey connectRegistry(const String &machineName) const;

  void flushKey() const;

  const String &name() const;
  String getClass();

  void setValue(       const String &valueName, const String &value , ULONG type = REG_SZ   ) const;
  void setValue(       const String &valueName, UINT64        value , ULONG type = REG_QWORD) const;
  inline void setValue(const String &valueName, INT64         value , ULONG type = REG_QWORD) const {
    setValue(valueName, (UINT64)value, type);
  }
  void setValue(       const String &valueName, ULONG         value , ULONG type = REG_DWORD) const;
  inline void setValue(const String &valueName, long          value , ULONG type = REG_DWORD) const {
    setValue(valueName, (ULONG)value, type);
  }
  inline void setValue(const String &valueName, UINT          value, ULONG type = REG_DWORD) const {
    setValue(valueName, (ULONG)value, type);
  }
  inline void setValue(const String &valueName, int           value, ULONG type = REG_DWORD) const {
    setValue(valueName, (ULONG)value, type);
  }
  inline void setValue(const String &valueName, USHORT        value, ULONG type = REG_DWORD) const {
    setValue(valueName, (ULONG)value, type);
  }
  inline void setValue(const String &valueName, short         value, ULONG type = REG_DWORD) const {
    setValue(valueName, (long)value, type);
  }
  inline void setValue(const String &valueName, bool          value) const {
    const ULONG tmp = value ? 1 : 0;
    setValue(valueName, tmp);
  }
  // type = REG_BINARY
  inline void setValue(const String &valueName, const BYTE *bytes, ULONG size) const {
    setValue(RegistryValue(valueName, bytes, size));
  }
  inline void setValue(const RegistryValue &value) const {
    setValueData(__TFUNCTION__, value.name(), value.type(), value.getBuffer(), value.getBufSize());
  }

        String  &getValue(const String &valueName, String    &value) const;
        UINT64  &getValue(const String &valueName, UINT64    &value) const;
  inline INT64  &getValue(const String &valueName, INT64     &value) const {
    UINT64 tmp;
    getValue(valueName, tmp);
    value = (INT64)tmp;
    return value;
  }
        ULONG   &getValue(const String &valueName, ULONG     &value, ULONG wantedType = REG_DWORD) const;
  inline LONG   &getValue(const String &valueName, LONG      &value, ULONG wantedType = REG_DWORD) const {
    ULONG tmp;
    getValue(valueName, tmp, wantedType);
    value = (LONG)tmp;
    return value;
  }
  inline UINT   &getValue(const String &valueName, UINT      &value, ULONG wantedType = REG_DWORD) const {
    ULONG tmp;
    getValue(valueName, tmp, wantedType);
    value = (UINT)tmp;
    return value;
  }
  inline INT    &getValue(const String &valueName, INT        &value, ULONG wantedType = REG_DWORD) const {
    ULONG tmp;
    getValue(valueName, tmp, wantedType);
    value = (INT)tmp;
    return value;
  }
  inline USHORT &getValue(const String &valueName, USHORT    &value, ULONG wantedType = REG_DWORD) const {
    ULONG tmp;
    getValue(valueName, tmp, wantedType);
    value = (USHORT)tmp;
    return value;
  }
  inline short &getValue(const String &valueName, short     &value, ULONG wantedType = REG_DWORD) const {
    ULONG tmp;
    getValue(valueName, tmp, wantedType);
    value = (short)tmp;
    return value;
  }
  inline bool &getValue(const String &valueName, bool      &value) const {
    ULONG tmp;
    getValue(valueName, tmp);
    value = tmp ? true : false;
    return value;
  }
  RegistryValue &getValue(const  String &valueName, RegistryValue   &value) const;
  String  getString(const String &valueName, const String    &defaultValue) const;
  INT64   getInt64( const String &valueName, INT64            defaultValue) const;
  UINT64  getUint64(const String &valueName, UINT64           defaultValue) const;
  int     getInt(   const String &valueName, int              defaultValue) const;
  UINT    getUint(  const String &valueName, UINT             defaultValue) const;
  short   getShort( const String &valueName, short            defaultValue) const;
  USHORT  getUshort(const String &valueName, USHORT           defaultValue) const;
  bool    getBool(  const String &valueName, bool             defaultValue) const;

  Iterator<String>        getSubKeyIterator() const;
  Iterator<RegistryValue> getValueIterator()  const;
};
