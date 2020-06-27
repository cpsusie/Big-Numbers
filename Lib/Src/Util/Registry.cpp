#include "pch.h"
#include <winsock.h>
#include <MyUtil.h>
#include <TinyBitSet.h>
#include <Tokenizer.h>
#include <Registry.h>

#pragma comment(lib,"wsock32.lib")

#if defined(DEBUG_REGISTRY)

static const TCHAR *thisFile = __TFILE__;
static FILE *logFile = NULL;

class InitLogFile {
public:
  InitLogFile();
  ~InitLogFile();
};

InitLogFile::InitLogFile() {
  String fileName = FileNameSplitter(thisFile).setExtension(_T("log")).getFullPath();
  logFile = fopen(fileName,_T("w"));
}

InitLogFile::~InitLogFile() {
  if(logFile != NULL) {
    fclose(logFile);
    logFile = NULL;
  }
}

static InitLogFile dummy;

void registryLog(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  if(logFile != NULL) {
    va_list argptr;
    va_start(argptr,format);
    _vftprintf(logFile,format,argptr);
    va_end(argptr);
  }
}

#endif

const TCHAR *RegistryKey::getRootName(HKEY key) { // static
  switch((LONG)((ULONG_PTR)key)) {
#define caseKey(k) case ((LONG)((ULONG_PTR)k)): return _T(#k)

  caseKey(HKEY_CLASSES_ROOT     );
  caseKey(HKEY_CURRENT_USER);
  caseKey(HKEY_LOCAL_MACHINE);
  caseKey(HKEY_USERS);
  caseKey(HKEY_PERFORMANCE_DATA);
  caseKey(HKEY_CURRENT_CONFIG);
  caseKey(HKEY_DYN_DATA);
  default: return EMPTYSTRING;
  }
}

void RegistryKey::checkResult(long result, const TCHAR *function, const TCHAR *regFunction) { // static
  if(result != ERROR_SUCCESS) {
    throwException(_T("%s:%s:%s"), function, regFunction, getErrorText(result).cstr());
  }
}

void RegistryKey::checkResult(long result, const TCHAR *function, const TCHAR *regFunction, const String &argument) { // static
  if(result != ERROR_SUCCESS) {
    throwException(_T("%s:%s(\"%s\"):%s"), function, regFunction, argument.cstr(), getErrorText(result).cstr());
  }
}

RegistryKey::RegistryKey(HKEY key) : m_key(newCountedKey(key, true)) {
  m_name = getRootName(key);
}

RegistryKey::RegistryKey(const String &remoteMachine, HKEY key) : m_key(NULL) {
  HKEY theKey = key;
  long result = RegConnectRegistry(remoteMachine.cstr(), key, &theKey);
  checkResult(result, __TFUNCTION__, _T("RegConnectRegistry"), remoteMachine);
  m_key = newCountedKey(theKey, true);
  REGISTRYLOG("open",theKey);
  m_name = getRootName(key);
}

RegistryKey::RegistryKey(HKEY key, const String &subKey, REGSAM samDesired, const String &remoteMachine) : m_key(NULL) {
  DEFINEMETHODNAME;
   HKEY theKey = key;

   // if we're passed a remote machine name...
   // do a connect registry first

   if(remoteMachine.length() > 0)  {
     long result = RegConnectRegistry(remoteMachine.cstr(), key, &theKey);
     checkResult(result, method, _T("RegConnectRegistry"), remoteMachine);
     REGISTRYLOG("open",theKey);
   }

   HKEY newKey;

   long result = RegOpenKeyEx(theKey, subKey.cstr(), 0, samDesired, &newKey);
   checkResult(result, method, _T("RegOpenKeyEx"), subKey);
   REGISTRYLOG("open",newKey);

   m_key  = newCountedKey(newKey, true);
   createChildName(m_name,getRootName(key),subKey);
}

RegistryKey::RegistryKey(const RegistryKey &src) : m_key(src.m_key->addRef()) {
  m_name = src.m_name;
}

RegistryKey::~RegistryKey() {
  m_key->release();
}

RegistryKey &RegistryKey::operator=(const RegistryKey &src) {
  if(this != &src) {
    m_key->release();
    m_key  = src.m_key->addRef();
    m_name = src.m_name;
  }
  return *this;
}

ReferenceCountedRegKey *RegistryKey::newCountedKey(HKEY key, bool closeKeyOnFailure) {
  ReferenceCountedRegKey *result = NULL;

  try {
    result = new ReferenceCountedRegKey(key); TRACE_NEW(result);
  } catch(...) {
    result = NULL;
  }

  if(result == NULL) {
    if(closeKeyOnFailure) {
      REGISTRYLOG("close",key);
      RegCloseKey(key);
    }
    throwException(_T("RegistryKey::newCountedKey() failed"));
  }
  return result;
}

void RegistryKey::createChildName(String &dst, const String &parentName, const String &name) {
  dst = parentName + "\\" + name;
}

RegistryKey RegistryKey::openKey(const String &subKey, REGSAM samDesired) const {
  RegistryKey result(m_key->getObject(), subKey.cstr(), samDesired);
  createChildName(result.m_name, m_name, subKey);
  return result;
}

void RegistryKey::deleteKey(const String &keyName) const {
  DEFINEMETHODNAME;
   // deleteKey will always fail to delete a key with sub keys
   // and deleteKeyAndSubKeys will always work.

  RegistryKey key = openKey(keyName);
  Iterator<String> it = key.getSubKeyIterator();
  if(it.hasNext()) {
    throwException(_T("%s(\"%s\") failed. Subkey exist"), method, key.name().cstr());
  }

  long result = RegDeleteKey(m_key->getObject(), keyName.cstr());
  checkResult(result,method, _T("RegDeleteKey"), keyName);
}

void RegistryKey::deleteSubKeys() const {
  for(Iterator<String> it = getSubKeyIterator(); it.hasNext();) {
    openKey(it.next()).deleteSubKeys();;
    it.remove();
  }
}

void RegistryKey::deleteKeyAndSubkeys(const String &keyName) const {
  openKey(keyName).deleteSubKeys();
  deleteKey(keyName);
}

RegistryKey RegistryKey::createKey(const String &subKey
                                  ,const String &keyClass
                                  ,ULONG         options
                                  ,REGSAM        samDesired
                                  ,LPSECURITY_ATTRIBUTES securityAttributes) const {
  ULONG disposition;

  RegistryKey key = createOrOpenKey(subKey, &disposition, keyClass, options, samDesired, securityAttributes);

  if(disposition != REG_CREATED_NEW_KEY) {
    REGISTRYLOG("close",key.m_key->getObject());
    RegCloseKey(key.m_key->getObject());
    throwException(_T("%s(\"%s\") failed. Key alredy exist"), __TFUNCTION__, subKey.cstr());
  }
  return key;
}

RegistryKey RegistryKey::createOrOpenKey(const String  &subKey
                                        ,ULONG         *disposition
                                        ,const String  &keyClass
                                        ,ULONG          options
                                        ,REGSAM         samDesired
                                        ,LPSECURITY_ATTRIBUTES securityAttributes) const {
  ULONG disp;
  HKEY key;
  long result = RegOpenKeyEx(m_key->getObject(), subKey.cstr(), 0, samDesired, &key);

  if(result == ERROR_SUCCESS) {
    RegistryKey newKey(key);
    createChildName(newKey.m_name, m_name, subKey);
    return newKey;
  }

  if(disposition == NULL) {
    disposition = &disp;
  }
  String tmpClass = keyClass;
  result = RegCreateKeyEx(m_key->getObject(), subKey.cstr(), 0, tmpClass.cstr(), options
                          ,samDesired, securityAttributes, &key
                          ,disposition);
  checkResult(result,__TFUNCTION__, _T("RegCreateKeyEx"), subKey);
  REGISTRYLOG("open",key);

  RegistryKey newKey(key);
  createChildName(newKey.m_name, m_name, subKey);
  return newKey;
}

RegistryKey RegistryKey::createOrOpenPath( const String         &path
                                          ,ULONG                *disposition
                                          ,const String         &keyClass
                                          ,ULONG                 options
                                          ,REGSAM                samDesired
                                          ,SECURITY_ATTRIBUTES  *securityAttributes) const {
  RegistryKey key(*this);
  Exception error;
  bool ok = true;
  for(Tokenizer tok(path,"\\"); tok.hasNext(); ) {
    try {
      key = key.createOrOpenKey(tok.next(),disposition,keyClass,options,samDesired,securityAttributes);
      ok = true;
    } catch(Exception e) {
      error = e;
      ok = false;
    }
  }
  if(!ok) {
    throw error;
  }
  return key;
}

bool RegistryKey::hasSubkey(const String &subKey, REGSAM samDesired) const {
  bool hasKey = false;
  HKEY key;
  long result = RegOpenKeyEx(m_key->getObject(), subKey.cstr(), 0, samDesired, &key);

  if(result == ERROR_SUCCESS) {
    hasKey = true;

    REGISTRYLOG("open",key);
    REGISTRYLOG("close",key);

    RegCloseKey(key);
  } else { // should check for errors that mean we dont have it, or we
           // have it but not with the right access and throw if a real error
    hasKey = false;
  }
  return hasKey;
}

RegistryKey RegistryKey::connectRegistry(const String &machineName) const {
  HKEY key;
  long result = RegConnectRegistry(machineName.cstr(), m_key->getObject(), &key);
  REGISTRYLOG("open",key);
  checkResult(result,__TFUNCTION__, _T("RegConnectRegistry"), machineName);
  return RegistryKey(key);
}

void RegistryKey::flushKey() const {
  long result = RegFlushKey(m_key->getObject());
  checkResult(result,__TFUNCTION__, _T("RegFlushKey"));
}

void RegistryKey::deleteValue(const String &valueName) const {
  long result = RegDeleteValue(m_key->getObject(), valueName.cstr());
  checkResult(result,__TFUNCTION__, _T("RegDeleteValue"), valueName);
}

void RegistryKey::deleteValues() const {
  for(Iterator<RegistryValue> it = getValueIterator(); it.hasNext();) {
    it.next();
    it.remove();
  }
}

const String &RegistryKey::name() const {
  return m_name;
}

RegistryValueInfo RegistryKey::getValueInfo(const TCHAR *method, const String &valueName) const {
  ULONG      type;
  ULONG      bufSize = 0;
  const long result  = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, method, _T("RegQueryValueEx"), valueName);
  return RegistryValueInfo(valueName, type, bufSize);
}

BYTE *RegistryKey::getValueData(const TCHAR *method, const RegistryValueInfo &info, BYTE *bytes) const {
  ULONG      type    = info.type();
  ULONG      bufSize = info.size();
  const long result  = RegQueryValueEx(m_key->getObject(), info.name().cstr(), 0, &type, bytes, &bufSize);
  checkResult(result, method, _T("RegQueryValueEx"), info.name());
  return bytes;
}

void RegistryKey::setValueData(const TCHAR *method, const String &valueName, ULONG type, const BYTE *bytes, DWORD size) const {
  const long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, bytes, size);
  checkResult(result, method, _T("RegSetValueEx"), valueName);
}


String RegistryKey::getClass() {
  return _T("unknown class");
/*
  long result = RegQueryInfoKey(key.m_key->getObject()
                               ,NULL    // Not interested in this key's class
                               ,NULL    // ditto
                               ,NULL    // Reserved
                               ,&subKeyCount
                               ,&maxNameLength
                               ,&maxClassLength
                               ,NULL    // Not interested in number of values
                               ,NULL    // Not interested in max length of value name
                               ,NULL    // Not interested in max length of value buffer
                               ,NULL    // Not interested in length of security descriptor
                               ,NULL);  // Not interested in last write time
  checkResult(result,"name", "RegQueryInfoKey");
*/
}

class ValueTypeStringifier : public AbstractStringifier<UINT> {
public:
  String toString(const UINT &type) {
    return RegistryValueInfo::typeName(type);
  }
};

class RegValueTypeSet : BitSet32 {
public:
// terminate list with -1. Values should be (defined in winnt.h)
// REG_NONE,REG_SZ,REG_EXPAND_SZ,REG_BINARY,
// REG_DWORD,REG_DWORD_LITTLE_ENDIAN,REG_DWORD_BIG_ENDIAN
// REG_LINK,REG_MULTI_SZ,REG_RESOURCE_LIST,REG_FULL_RESOURCE_DESCRIPTOR
// REG_RESOURCE_REQUIREMENTS_LIST
// REG_QWORD,REG_QWORD_LITTLE_ENDIAN
  RegValueTypeSet(int t1, ...);
  // throws Exception if type not in set
  void checkIsValidType(const TCHAR *method, UINT type, const String &valueName = EMPTYSTRING) const;
  inline void checkIsValidType(const TCHAR *method, const RegistryValueInfo &info) const {
    checkIsValidType(method, info.type(), info.name());
  }
  inline String toString() const {
    ValueTypeStringifier sf;
    return __super::toString(&sf);
  }
};

RegValueTypeSet::RegValueTypeSet(int t1, ...) {
  va_list argptr;
  va_start(argptr, t1);
  add(t1);
  for(int t = va_arg(argptr, int); t >= 0; t = va_arg(argptr, int)) {
    add(t);
  }
}

void RegValueTypeSet::checkIsValidType(const TCHAR *method, UINT type, const String &valueName) const {
  if(!contains(type)) {
    throwException(_T("%s:Illegal type for value %s (=%s). Must be in {%s}")
                  ,method
                  ,valueName.cstr()
                  ,RegistryValueInfo::typeName(type).cstr()
                  ,toString().cstr());
  }
}

// --------------------------------------------getValue---------------------------------------------

#pragma warning(disable : 4073)
#pragma init_seg(lib)

// { REG_DWORD, REG_DWORD_BIG_ENDIAN }
static const RegValueTypeSet regDWORDTypeSet(REG_DWORD , REG_DWORD_BIG_ENDIAN, -1);
// { REG_QWORD }
static const RegValueTypeSet regQWORDTypeSet(REG_QWORD , -1);
// { REG_EXPAND_SZ , REG_SZ }
static const RegValueTypeSet regSZTypeSet(   REG_SZ    , REG_EXPAND_SZ , -1);
// { REG_BINARY }
static const RegValueTypeSet regBinTypeSet(  REG_BINARY, -1);

static void validateBufSize(const TCHAR *method, const String &valueName, size_t bufSize, size_t expectedSize) {
  if(bufSize != expectedSize) {
    throwException(_T("%s:Value %s has illegal size (=%zu) Must be %zu")
                  ,method
                  ,valueName.cstr()
                  ,bufSize
                  ,expectedSize);
  }
}

static inline void validateBufSize(const TCHAR *method, const RegistryValueInfo &info, size_t expectedSize) {
  validateBufSize(method, info.name(), info.size(), expectedSize);
}

static void throwUnexpectedType(const TCHAR *method, int type) {
  throwException(_T("%s:Unexpected type:%s"), method, RegistryValueInfo::typeName(type).cstr());
}

#define UNEXPECTEDTYPEERROR(info) throwUnexpectedType(method, info.type())

String &RegistryKey::getValue(const String &valueName, String &value) const {
  DEFINEMETHODNAME;
  const RegistryValueInfo info = getValueInfo(method, valueName);
  regSZTypeSet.checkIsValidType(method, info.type(), valueName);

  BYTE *buffer = new BYTE[info.size()+1]; TRACE_NEW(buffer);
  try {
    getValueData(method, info, buffer);
    buffer[info.size()] = 0;
    value = (TCHAR*)buffer;
    SAFEDELETEARRAY(buffer);
  } catch(...) {
    SAFEDELETEARRAY(buffer);
    throw;
  }
  return value;
}

UINT64 &RegistryKey::getValue(const String &valueName, UINT64 &value) const {
  DEFINEMETHODNAME;
  const RegistryValueInfo info = getValueInfo(method, valueName);
  regQWORDTypeSet.checkIsValidType(method, info);

  switch(info.type()) {
  case REG_QWORD:
    validateBufSize(method, info, sizeof(INT64));
    getValueData(method, info, (BYTE*)&value);
    break;
  default:
    UNEXPECTEDTYPEERROR(info);
  }
  return value;
}

ULONG &RegistryKey::getValue(const String &valueName, ULONG &value, ULONG wantedType) const {
  DEFINEMETHODNAME;
  const RegistryValueInfo info = getValueInfo(method , valueName);
  regDWORDTypeSet.checkIsValidType(method, info);
  regDWORDTypeSet.checkIsValidType(method, wantedType, valueName);
  switch(info.type()) {
  case REG_DWORD:
  case REG_DWORD_BIG_ENDIAN:
    validateBufSize(method, info, sizeof(ULONG));
    getValueData(method, info, (BYTE*)&value);
    if(wantedType != info.type()) {
      value = htonl(value);
    }
    break;
  default:
    UNEXPECTEDTYPEERROR(info);
  }
  return value;
}

RegistryValue &RegistryKey::getValue(const String &valueName, RegistryValue &value) const {
  DEFINEMETHODNAME;
  BYTE *buffer = NULL;
  try {
    RegistryValueInfo info = getValueInfo(method, valueName);
    buffer = new BYTE[info.size()]; TRACE_NEW(buffer);
    getValueData(method, info, buffer);
    value = RegistryValue(info, buffer);
    SAFEDELETEARRAY(buffer);
  } catch(...) {
    SAFEDELETEARRAY(buffer);
    throw;
  }
  return value;
}

String RegistryKey::getString(const String &valueName, const String &defaultValue) const {
  String result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

INT64 RegistryKey::getInt64(const String &valueName, INT64 defaultValue) const {
  INT64 result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

UINT64 RegistryKey::getUint64(const String &valueName, UINT64 defaultValue) const {
  UINT64 result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

int RegistryKey::getInt(const String &valueName, int defaultValue) const {
  int result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

UINT RegistryKey::getUint(const String &valueName, UINT defaultValue) const {
  UINT result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

short RegistryKey::getShort(const String &valueName, short defaultValue) const {
  short result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

USHORT RegistryKey::getUshort(const String &valueName, USHORT defaultValue) const {
  USHORT result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

bool RegistryKey::getBool(const String &valueName, bool defaultValue) const {
  bool result;
  try {
    getValue(valueName, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

// --------------------------------------------setValue---------------------------------------------

void RegistryKey::setValue(const String &valueName, const String &value, ULONG type) const {
  DEFINEMETHODNAME;
  regSZTypeSet.checkIsValidType(method, type, valueName);
  setValueData(method, valueName, type, (BYTE*)value.cstr(), sizeof(TCHAR)*((DWORD)value.length() + 1));
}

void RegistryKey::setValue(const String &valueName, UINT64 value, ULONG type) const {
  DEFINEMETHODNAME;
  regQWORDTypeSet.checkIsValidType(method, type, valueName);
  setValueData(method, valueName, type, (BYTE*)&value, sizeof(UINT64));
}

void RegistryKey::setValue(const String &valueName, ULONG value, ULONG type) const {
  DEFINEMETHODNAME;
  regDWORDTypeSet.checkIsValidType(method, type, valueName);
  setValueData(method, valueName, type, (BYTE*)&value, sizeof(ULONG));
}

// ---------------------------------------------RegistryValue-------------------------------------------------------

RegistryValue::RegistryValue() {
  init(EMPTYSTRING, REG_NONE, 0);
}

RegistryValue::RegistryValue(const String &valueName, const BYTE *bytes, ULONG size, ULONG type) {
  init(valueName, type, size);
  setBuffer(bytes);
}

RegistryValue::RegistryValue(const RegistryValueInfo &info, const BYTE *bytes) {
  init(info.name(), info.type(), info.size());
  setBuffer(bytes);
}

RegistryValue::RegistryValue(const String &valueName, const String &str, ULONG type) {
  regSZTypeSet.checkIsValidType(__TFUNCTION__, type, valueName);
  init(valueName, type, sizeof(TCHAR)*((DWORD)str.length() + 1));
  setBuffer(str.cstr());
}

RegistryValue::RegistryValue(const String &valueName, const StringArray &strings) {
  const String buf       = strings.getAsDoubleNullTerminatedString();
  const ULONG  charCount = (ULONG)buf.length() + 1;
  init(valueName, REG_MULTI_SZ, charCount*sizeof(TCHAR));
  setBuffer(buf.cstr());
}

RegistryValue::RegistryValue(const RegistryValue &src) {
  init(src.m_valueName,src.m_type,src.m_size);
  setBuffer(src.m_buffer);
}

void RegistryValue::initValue(const String &valueName, ULONG type, ULONG  value) {
  regDWORDTypeSet.checkIsValidType(__TFUNCTION__, type, valueName);
  init(valueName, type, sizeof(ULONG));
  setBuffer(&value);
}
void RegistryValue::initValue(const String &valueName, ULONG type, UINT64 value) {
  regQWORDTypeSet.checkIsValidType(__TFUNCTION__, type, valueName);
  init(valueName, type, sizeof(UINT64));
  setBuffer(&value);
}

void RegistryValue::init(const String &valueName, ULONG type, ULONG size) {
  m_valueName = valueName;
  m_type      = type;
  m_size      = size;
  m_buffer    = new BYTE[m_size]; TRACE_NEW(m_buffer);
}

void RegistryValue::cleanup() {
  SAFEDELETEARRAY(m_buffer);
  m_size = 0;
}

void RegistryValue::reset() {
  cleanup();
  init(EMPTYSTRING,REG_NONE,0);
}

RegistryValue &RegistryValue::operator=(const RegistryValue &src) {
  if(this == &src) {
    return *this;
  }
  cleanup();
  init(src.m_valueName,src.m_type,src.m_size);
  setBuffer(src.m_buffer);
  return *this;
}

bool RegistryValue::operator==(const RegistryValue &r) const {
  return (m_valueName == r.m_valueName)
      && (m_type      == r.m_type     )
      && (m_size      == r.m_size     )
      && (memcmp(getBuffer(), r.getBuffer(), r.getBufSize()) == 0);
}

RegistryValue::operator ULONG() const {
  regDWORDTypeSet.checkIsValidType(__TFUNCTION__, m_type, m_valueName);
  return *(ULONG*)m_buffer;
}

RegistryValue::operator UINT64() const {
  regQWORDTypeSet.checkIsValidType(__TFUNCTION__, m_type, m_valueName);
  return *(UINT64*)m_buffer;
}

RegistryValue::operator String() const {
  regSZTypeSet.checkIsValidType(__TFUNCTION__, m_type, m_valueName);
  return String((TCHAR*)m_buffer);
}

RegistryValue::operator StringArray() const {
  DEFINEMETHODNAME;
  StringArray result;
  switch(m_type) {
  case REG_EXPAND_SZ:
  case REG_SZ       :
    result.add((TCHAR*)m_buffer); // will return an array with 1 element
    break;
  case REG_MULTI_SZ :
    { const TCHAR *bufEnd = (TCHAR*)(m_buffer + m_size);
      for(const TCHAR *cp = (TCHAR*)m_buffer; *cp && cp < bufEnd;) {
        String tmp = (TCHAR*)cp;
        result.add(tmp);
        cp += tmp.length()+1;
      }
    }
    break;
  default:
    UNEXPECTEDTYPEERROR(getValueInfo());
  }
  return result;
}

RegistryValue::operator ByteArray() const {
  regBinTypeSet.checkIsValidType(__TFUNCTION__, m_type, m_valueName);
  return ByteArray(getBuffer(), getBufSize());
}

String &RegistryValue::bytesToString(String &dst, const void *data, size_t size) { // static
  BYTE * p = (BYTE*)data;
  dst = EMPTYSTRING;
  for(size_t i = 0; i < size; i++) {
    TCHAR tmp[10];
    _stprintf(tmp, _T("%02x "), p[i]);
    dst += tmp;
  }
  return dst;
}

String RegistryValue::toString() const {
  switch(m_type) {
  case REG_EXPAND_SZ:
  case REG_SZ       :
    return (TCHAR*)m_buffer;

  case REG_MULTI_SZ:
    { StringArray tmp = *this;
      return tmp.toString();
    }

  case REG_DWORD:
    return format(_T("%lu"),*(ULONG*)m_buffer);

  case REG_QWORD:
    return format(_T("%llu"),*(UINT64*)m_buffer);

  case REG_DWORD_BIG_ENDIAN:
    return format(_T("%lu"),ntohl(*(ULONG*)m_buffer));

  case REG_BINARY:
    return bytesToString(m_buffer, m_size);

  case REG_NONE:
    return EMPTYSTRING;

  default:
	return bytesToString(m_buffer, m_size);
  }
}

String RegistryValueInfo::typeName(ULONG type) { // static
  switch(type) {
#define caseStr(c) case c: return _T(#c);
  caseStr(REG_NONE                      );
  caseStr(REG_SZ                        );
  caseStr(REG_EXPAND_SZ                 );
  caseStr(REG_BINARY                    );
  caseStr(REG_DWORD                     );
  caseStr(REG_DWORD_BIG_ENDIAN          );
  caseStr(REG_QWORD                     );
  caseStr(REG_LINK                      );
  caseStr(REG_MULTI_SZ                  );
  caseStr(REG_RESOURCE_LIST             );
  caseStr(REG_FULL_RESOURCE_DESCRIPTOR  );
  caseStr(REG_RESOURCE_REQUIREMENTS_LIST);
  default: return format(_T("Type=%d"), type);
  }
#undef caseStr
}

// ---------------------------------------------SubKeyIterator-------------------------------------------------------

class SubKeyIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  RegistryKey         m_key;
  ULONG               m_subKeyCount;
  ULONG               m_maxNameLength;
  ULONG               m_index;
  TCHAR              *m_nameBuffer;
  String              m_resultBuffer;

  void queryKeyInfo();
  void init(ULONG subKeyCount, ULONG maxNameLength);
  void cleanup();

  ULONG getNameBufferSize() const {
    return m_maxNameLength+1;
  }

public:
  SubKeyIterator(const RegistryKey &key);
  SubKeyIterator(const SubKeyIterator &src); // copy constructor
  ~SubKeyIterator()               override;
  AbstractIterator *clone()       override;
  bool hasNext()            const override;
  void *next()                    override;
  void remove()                   override;
};

DEFINECLASSNAME(SubKeyIterator);

SubKeyIterator::SubKeyIterator(const RegistryKey &key) : m_key(key) {
  m_index = 0;
  queryKeyInfo();
}

SubKeyIterator::SubKeyIterator(const SubKeyIterator &src) : m_key(src.m_key) {
  m_index = src.m_index;
  init(src.m_subKeyCount,src.m_maxNameLength);
}

SubKeyIterator::~SubKeyIterator() {
  cleanup();
}

void SubKeyIterator::queryKeyInfo() {
  ULONG subKeyCount   = 0;
  ULONG maxNameLength = 0;
  long result = RegQueryInfoKey(m_key.m_key->getObject()
                               ,NULL    // Not interested in this key's class
                               ,NULL    // ditto
                               ,NULL    // Reserved
                               ,&subKeyCount
                               ,&maxNameLength
                               ,NULL
                               ,NULL    // Not interested in number of values
                               ,NULL    // Not interested in max length of value name
                               ,NULL    // Not interested in max length of value buffer
                               ,NULL    // Not interested in length of security descriptor
                               ,NULL);  // Not interested in last write time
  RegistryKey::checkResult(result, __TFUNCTION__, _T("RegQueryInfoKey"), m_key.name());
  init(subKeyCount, maxNameLength);
}

void SubKeyIterator::init(ULONG subKeyCount, ULONG maxNameLength) {
  m_subKeyCount   = subKeyCount;
  m_maxNameLength = maxNameLength;
  m_nameBuffer    = new TCHAR[getNameBufferSize()]; TRACE_NEW(m_nameBuffer);
}

void SubKeyIterator::cleanup() {
  SAFEDELETEARRAY(m_nameBuffer);
  m_maxNameLength = 0;
}

AbstractIterator *SubKeyIterator::clone() {
  return new SubKeyIterator(*this);
}

bool SubKeyIterator::hasNext() const {
  return m_index < m_subKeyCount;
}

void *SubKeyIterator::next() {
  bool done = false;
  while(!done) {
    if(!hasNext()) {
      noNextElementError(s_className);
    }

    ULONG nameBufferSize = getNameBufferSize();
    FILETIME m_lastWriteTime;
    long result = RegEnumKeyEx(m_key.m_key->getObject(),m_index,m_nameBuffer,&nameBufferSize,NULL,NULL,NULL,&m_lastWriteTime);

    switch(result) {
    case ERROR_SUCCESS:
      m_resultBuffer = m_nameBuffer;
      m_index++;
      done = true;
      break;

    case ERROR_NO_MORE_ITEMS:
      noNextElementError(s_className);
      break;

    case ERROR_MORE_DATA: // Size has changed since we started
      cleanup();
      queryKeyInfo();
      break;

    default:
      RegistryKey::checkResult(result,__TFUNCTION__, _T("RegEnumKeyEx"),m_key.name());
      break;
    }
  }
  return &m_resultBuffer;
}

void SubKeyIterator::remove() {
  if(m_resultBuffer.length() == 0) {
    noCurrentElementError(s_className);
  }
  m_key.deleteKey(m_resultBuffer);
  m_resultBuffer = EMPTYSTRING;
  if(m_subKeyCount > 0) {
    m_index--;
    m_subKeyCount--;
  }
}

Iterator<String> RegistryKey::getSubKeyIterator() const {
  return Iterator<String>(new SubKeyIterator(*this));
}

// ---------------------------------------------RegValueIterator-------------------------------------------------------

class RegValueIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  RegistryKey         m_key;
  ULONG               m_valueCount;
  ULONG               m_maxNameLength;
  ULONG               m_maxValueLength;
  ULONG               m_index;
  TCHAR              *m_nameBuffer;
  BYTE               *m_valueBuffer;
  RegistryValue       m_resultBuffer;

  void queryKeyInfo();
  void init(ULONG valueCount, ULONG maxNameLength, ULONG maxValueLength);
  void cleanup();

  ULONG getNameBufferSize() const {
    return m_maxNameLength+1;
  }

  ULONG getValueBufferSize() const {
    return m_maxValueLength+1;
  }

public:
  RegValueIterator(const RegistryKey &key);
  RegValueIterator(const RegValueIterator &src); // copy constructor
  ~RegValueIterator()             override;
  AbstractIterator *clone()       override;
  bool hasNext()            const override;
  void *next()                    override;
  void remove()                   override;
};

DEFINECLASSNAME(RegValueIterator);

RegValueIterator::RegValueIterator(const RegistryKey &key) : m_key(key) {
  m_index = 0;
  queryKeyInfo();
}

RegValueIterator::RegValueIterator(const RegValueIterator &src) : m_key(src.m_key) {
  m_index = src.m_index;
  init(src.m_valueCount, src.m_maxNameLength, src.m_maxValueLength);
}

RegValueIterator::~RegValueIterator() {
  cleanup();
}

void RegValueIterator::queryKeyInfo() {
  ULONG valueCount     = 0;
  ULONG maxNameLength  = 0;
  ULONG maxValueLength = 0;
  long result = RegQueryInfoKey(m_key.m_key->getObject()
                               ,NULL     // Not interested in this key's class
                               ,NULL     // ditto
                               ,NULL     // Reserved
                               ,NULL     // Not interested in number of sub keys
                               ,NULL     // Not interested in max sub key name length
                               ,NULL     // Not interested in max sub key class length
                               ,&valueCount
                               ,&maxNameLength
                               ,&maxValueLength
                               ,NULL     // Not interested in length of security descriptor
                               ,NULL);   // Not interested in last write time
  RegistryKey::checkResult(result, __TFUNCTION__, _T("RegQueryInfoKey"), m_key.name());
  init(valueCount,maxNameLength,maxValueLength);
}

void RegValueIterator::init(ULONG valueCount, ULONG maxNameLength, ULONG maxValueLength) {
  m_valueCount     = valueCount;
  m_maxNameLength  = maxNameLength;
  m_maxValueLength = maxValueLength;
  m_nameBuffer     = new TCHAR[getNameBufferSize()]; TRACE_NEW(m_nameBuffer );
  m_valueBuffer    = new BYTE[getValueBufferSize()]; TRACE_NEW(m_valueBuffer);
}

void RegValueIterator::cleanup() {
  SAFEDELETEARRAY(m_nameBuffer);
  m_maxNameLength = 0;

  SAFEDELETEARRAY(m_valueBuffer);
  m_maxValueLength = 0;
}

AbstractIterator *RegValueIterator::clone() {
  return new RegValueIterator(*this);
}

bool RegValueIterator::hasNext() const {
  return m_index < m_valueCount;
}

void *RegValueIterator::next() {
  bool done = false;
  while(!done) {
    if(!hasNext()) {
      noNextElementError(s_className);
    }

    ULONG nameBufferSize  = getNameBufferSize();
    ULONG valueBufferSize = getValueBufferSize();
    ULONG type;
    long result = RegEnumValue(m_key.m_key->getObject()
                              ,m_index
                              ,m_nameBuffer, &nameBufferSize
                              ,NULL
                              ,&type
                              ,m_valueBuffer,&valueBufferSize);

    switch(result) {
    case ERROR_SUCCESS:
      m_resultBuffer = RegistryValue(m_nameBuffer, m_valueBuffer, valueBufferSize, type);
      m_index++;
      done = true;
      break;

    case ERROR_NO_MORE_ITEMS:
      noNextElementError(s_className);
      break;

    case ERROR_MORE_DATA:                // Size has changed since we started
      cleanup();
      queryKeyInfo();
      break;

    default:
      RegistryKey::checkResult(result,__TFUNCTION__, _T("RegEnumValue"),m_key.name());
      break;
    }
  }
  return &m_resultBuffer;
}

void RegValueIterator::remove() {
  if(m_resultBuffer.type() == REG_NONE) {
    noCurrentElementError(s_className);
  }
  m_key.deleteValue(m_resultBuffer.name());
  m_resultBuffer.reset();
  if(m_valueCount > 0) {
    m_index--;
    m_valueCount--;
  }
}

Iterator<RegistryValue> RegistryKey::getValueIterator() const {
  return Iterator<RegistryValue>(new RegValueIterator(*this));
}
