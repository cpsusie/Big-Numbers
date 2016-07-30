#include "pch.h"
#include <winsock.h>
#include <Tokenizer.h>
#include <Exception.h>
#include <Registry.h>

#pragma comment(lib,"wsock32.lib")

#ifdef DEBUG_REGISTRY

static const TCHAR *thisFile = _T(__FILE__);
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

void registryLog(const TCHAR *format,...) {
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
    throwException(_T("RegistryKey::%s:%s:%s"), function, regFunction, getErrorText(result).cstr());
  }
}

void RegistryKey::checkResult(long result, const TCHAR *function, const TCHAR *regFunction, const String &argument) { // static 
  if(result != ERROR_SUCCESS) {
    throwException(_T("RegistryKey::%s:%s(\"%s\"):%s"), function, regFunction, argument.cstr(), getErrorText(result).cstr());
  }
}

RegistryKey::RegistryKey(HKEY key) : m_key(newCountedKey(key, true)) {
  m_name = getRootName(key);
}

RegistryKey::RegistryKey(const String &remoteMachine, HKEY key) : m_key(NULL) {
  HKEY theKey = key;
  long result = RegConnectRegistry(remoteMachine.cstr(), key, &theKey);
  checkResult(result, _T("RegistryKey"), _T("RegConnectRegistry"), remoteMachine);
  m_key = newCountedKey(theKey, true);
  REGISTRYLOG("open",theKey);
  m_name = getRootName(key);
}

RegistryKey::RegistryKey(HKEY key, const String &subKey, REGSAM samDesired, const String &remoteMachine) : m_key(NULL) {
   HKEY theKey = key;

   // if we're passed a remote machine name...
   // do a connect registry first

   if(remoteMachine.length() > 0)  {
     long result = RegConnectRegistry(remoteMachine.cstr(), key, &theKey);
     checkResult(result, _T("RegistryKey"), _T("RegConnectRegistry"), remoteMachine);
     REGISTRYLOG("open",theKey);
   }

   HKEY newKey;

   long result = RegOpenKeyEx(theKey, subKey.cstr(), 0, samDesired, &newKey);
   checkResult(result, _T("RegistryKey"), _T("RegOpenKeyEx"), subKey);
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
    result = new ReferenceCountedRegKey(key);
  } catch (...) {
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
   // deleteKey will always fail to delete a key with sub keys 
   // and deleteKeyAndSubKeys will always work.

  RegistryKey key = openKey(keyName); 
  Iterator<String> it = key.getSubKeyIterator();
  if(it.hasNext()) {
    throwException(_T("RegistryKey::deleteKey(\"%s\") failed. Subkey exist"), key.getName().cstr());
  }

  long result = RegDeleteKey(m_key->getObject(), keyName.cstr());
  checkResult(result,_T("deleteKey"), _T("RegDeleteKey"), keyName);
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
                                  ,unsigned long options
                                  ,REGSAM samDesired
                                  ,LPSECURITY_ATTRIBUTES securityAttributes) const {
  unsigned long disposition;

  RegistryKey key = createOrOpenKey(subKey, &disposition, keyClass, options, samDesired, securityAttributes);
   
  if(disposition != REG_CREATED_NEW_KEY) {
    REGISTRYLOG("close",key.m_key->getObject());
    RegCloseKey(key.m_key->getObject());
    throwException(_T("createKey(\"%s\") failed. Key alredy exist"), subKey.cstr());
  }
  return key;
}

RegistryKey RegistryKey::createOrOpenKey(const String  &subKey
                                        ,unsigned long *disposition
                                        ,const String  &keyClass
                                        ,unsigned long  options
                                        ,REGSAM         samDesired
                                        ,LPSECURITY_ATTRIBUTES securityAttributes) const {
  unsigned long disp;

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
  checkResult(result,_T("createOrOpenKey"), _T("RegCreateKeyEx"), subKey);
  REGISTRYLOG("open",key);

  RegistryKey newKey(key);
  createChildName(newKey.m_name, m_name, subKey);
  return newKey;
}

RegistryKey RegistryKey::createOrOpenPath( const String         &path
                                          ,unsigned long        *disposition
                                          ,const String         &keyClass
                                          ,unsigned long         options
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
  checkResult(result,_T("connectRegistry"), _T("RegConnectRegistry"), machineName);
  return RegistryKey(key);
}

void RegistryKey::flushKey() const {
  long result = RegFlushKey(m_key->getObject());
  checkResult(result,_T("flushKey"), _T("RegFlushKey"));
}

void RegistryKey::deleteValue(const String &valueName) const {
  long result = RegDeleteValue(m_key->getObject(), valueName.cstr());
  checkResult(result,_T("deleteValue"), _T("RegDeleteValue"), valueName);
}

void RegistryKey::deleteValues() const {
  for(Iterator<RegistryValue> it = getValueIterator(); it.hasNext();) {
    it.next();
    it.remove();
  }
}

const String &RegistryKey::getName() const {
  return m_name;
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
  checkResult(result,"getName", "RegQueryInfoKey");
*/
}

// --------------------------------------------getValue---------------------------------------------

void RegistryKey::getValue(const String &valueName, String &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  if(type == REG_EXPAND_SZ || type == REG_SZ)  {
    unsigned char *buffer = new unsigned char[bufSize+1];
    result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, buffer, &bufSize);
    try {
      checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
      buffer[bufSize] = 0;
      value = (TCHAR*)buffer;
      delete[] buffer;
    } catch(...) {
      delete[] buffer;
      throw;
    }
  }
}

void RegistryKey::getValue(const String &valueName, unsigned __int64 &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  switch(type) {
  case REG_QWORD:
    if(bufSize != sizeof(__int64)) {
      throwException(_T("getValue:Value has illegal size (=%d) Must be %d"), bufSize, sizeof(__int64));
    }
    result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, (LPBYTE)&value, &bufSize);
    checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
    break;
  default:
    throwException(_T("getValue:Illegal type for value %s (=%d). Must be %d"), valueName.cstr()
                   ,type,REG_QWORD);
  }
}

void RegistryKey::getValue(const String &valueName, __int64 &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  switch(type) {
  case REG_QWORD:
    if(bufSize != sizeof(__int64)) {
      throwException(_T("getValue:Value has illegal size (=%d) Must be %d"), bufSize, sizeof(__int64));
    }
    result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, (LPBYTE)&value, &bufSize);
    checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
    break;
  default:
    throwException(_T("getValue:Illegal type for value %s (=%d). Must be %d"),valueName.cstr()
                   ,type,REG_QWORD);
  }
}


void RegistryKey::getValue(const String &valueName, unsigned long &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  switch(type) {
  case REG_DWORD:
  case REG_DWORD_BIG_ENDIAN:
    if(bufSize != sizeof(unsigned long)) {
      throwException(_T("getValue:Value has illegal size (=%d) Must be %d"), bufSize, sizeof(long));
    }
    result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, (LPBYTE)&value, &bufSize);
    checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
    break;
  default:
    throwException(_T("getValue:Illegal type for value %s (=%d). Must be {%d,%d}"), valueName.cstr()
                   ,type,REG_DWORD,REG_DWORD_BIG_ENDIAN);
  }
}

void RegistryKey::getValue(const String &valueName, long &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  switch(type) {
  case REG_DWORD:
  case REG_DWORD_BIG_ENDIAN:
    if(bufSize != sizeof(unsigned long)) {
      throwException(_T("getValue:Value has illegal size (=%d) Must be %d"), bufSize, sizeof(long));
    }
    result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, (LPBYTE)&value, &bufSize);
    checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
    break;
  default:
    throwException(_T("getValue:Illegal type for value %s (=%d). Must be {%d,%d}"),valueName.cstr()
                   ,type,REG_DWORD,REG_DWORD_BIG_ENDIAN);
  }
}

void RegistryKey::getValue(const String &valueName, unsigned int &value) const {
  unsigned long tmp;
  getValue(valueName,tmp);
  value = (unsigned int)tmp;
}

void RegistryKey::getValue(const String &valueName, int &value) const {
  long tmp;
  getValue(valueName, tmp);
  value = (int)tmp;
}

void RegistryKey::getValue(const String &valueName, unsigned short &value) const {
  unsigned long tmp;
  getValue(valueName,tmp);
  value = (unsigned short)tmp;
}

void RegistryKey::getValue(const String &valueName, short &value) const {
  long tmp;
  getValue(valueName, tmp);
  value = (short)tmp;
}

void RegistryKey::getValue(const String &valueName, bool &value) const {
  unsigned long tmp;
  getValue(valueName,tmp);
  value = tmp ? true : false;
}

void RegistryKey::getValue(const String &valueName, RegistryValue &value) const {
  unsigned long type;
  unsigned long bufSize = 0;

  long result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, NULL, &bufSize);
  checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);

  unsigned char *buffer = new unsigned char[bufSize];

  result = RegQueryValueEx(m_key->getObject(), valueName.cstr(), 0, &type, buffer, &bufSize);
  try {
    checkResult(result, _T("getValue"), _T("RegQueryValueEx"), valueName);
    value = RegistryValue(valueName,buffer, bufSize, type);
    delete[] buffer;
  } catch(...) {
    delete[] buffer;
    throw;
  }
}

String RegistryKey::getString(const String &name, const String &defaultValue) const {
  String result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

__int64 RegistryKey::getInt64(const String &name, __int64 defaultValue) const {
  __int64 result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

unsigned __int64 RegistryKey::getUint64(const String &name, unsigned __int64 defaultValue) const {
  unsigned __int64 result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

int RegistryKey::getInt(const String &name, int defaultValue) const {
  int result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

unsigned int RegistryKey::getUint(const String &name, unsigned int defaultValue) const {
  unsigned int result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

short RegistryKey::getShort(const String &name, short defaultValue) const {
  short result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

unsigned short RegistryKey::getUshort(const String &name, unsigned short defaultValue) const {
  unsigned short result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

bool RegistryKey::getBool(const String &name, bool defaultValue) const {
  bool result;
  try {
    getValue(name, result);
  } catch(Exception) {
    result = defaultValue;
  }
  return result;
}

// --------------------------------------------setValue---------------------------------------------

void RegistryKey::setValue(const String &valueName, const String &value, unsigned long type) const {
  if (type != REG_SZ && type != REG_EXPAND_SZ) {
    throwException(_T("RegistryKey::SetValue():Illegal type=%d. Must be {REG_SZ,REG_EXPAND_SZ}"), type);
  }

  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, (BYTE*)value.cstr(), sizeof(TCHAR)*((DWORD)value.length()+1));
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}

void RegistryKey::setValue(const String &valueName, unsigned __int64 value, unsigned long type) const {
  if(type != REG_QWORD && type != REG_QWORD_LITTLE_ENDIAN) {
    throwException(_T("RegistryKey::SetValue():Illegal type=%d. Must be {REG_QWORD,REG_QWORD_LITTLE_ENDIAN}"), type);
  }

  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, (BYTE*)&value, sizeof(unsigned long));
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}

void RegistryKey::setValue(const String &valueName, __int64 value, unsigned long type) const {
  if(type != REG_QWORD && type != REG_QWORD_LITTLE_ENDIAN) {
    throwException(_T("RegistryKey::SetValue():Illegal type=%d. Must be {REG_QWORD,REG_QWORD_LITTLE_ENDIAN}"), type);
  }

  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, (BYTE*)&value, sizeof(long));
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}


void RegistryKey::setValue(const String &valueName, unsigned long value, unsigned long type) const {
  if(type != REG_DWORD && type != REG_DWORD_LITTLE_ENDIAN && type != REG_DWORD_BIG_ENDIAN) {
    throwException(_T("RegistryKey::SetValue():Illegal type=%d. Must be {REG_DWORD,REG_DWORD_LITTLE_ENDIAN,REG_DWORD_BIG_ENDIAN}"), type);
  }

  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, (BYTE*)&value, sizeof(unsigned long));
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}

void RegistryKey::setValue(const String &valueName, long value, unsigned long type) const {
  if(type != REG_DWORD && type != REG_DWORD_LITTLE_ENDIAN && type != REG_DWORD_BIG_ENDIAN) {
    throwException(_T("RegistryKey::SetValue():Illegal type=%d. Must be {REG_DWORD,REG_DWORD_LITTLE_ENDIAN,REG_DWORD_BIG_ENDIAN}"), type);
  }

  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, type, (BYTE*)&value, sizeof(long));
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}

void RegistryKey::setValue(const String &valueName, unsigned int value, unsigned long type) const {
  setValue(valueName, (unsigned long)value, type);
}

void RegistryKey::setValue(const String &valueName, int value, unsigned long type) const {
  setValue(valueName, (long)value, type);
}

void RegistryKey::setValue(const String &valueName, unsigned short value, unsigned long type) const {
  setValue(valueName, (unsigned long)value, type);
}

void RegistryKey::setValue(const String &valueName, short value, unsigned long type) const {
  setValue(valueName, (long)value, type);
}

void RegistryKey::setValue(const String &valueName, bool value) const {
  unsigned long tmp = value ? 1 : 0;
  setValue(valueName, tmp);
}

void RegistryKey::setValue(const String &valueName, BYTE *bytes , unsigned long size) const {
  long result = RegSetValueEx(m_key->getObject(), valueName.cstr(), 0, REG_BINARY, bytes, size);
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), valueName);
}

void RegistryKey::setValue(const RegistryValue &value) const {
  long result = RegSetValueEx(m_key->getObject(), value.getName().cstr(), 0, value.getType(), value.getBuffer(), value.getBufSize());
  checkResult(result, _T("setValue"), _T("RegSetValueEx"), value.getName());
}

// ---------------------------------------------RegistryValue-------------------------------------------------------

RegistryValue::RegistryValue() {
  init(_T(""), REG_NONE, 0);
}

RegistryValue::RegistryValue(const String &name, const BYTE *bytes, unsigned long size, unsigned long type) {
  init(name, type, size);
  setBuffer(bytes);
}

RegistryValue::RegistryValue(const String &name, const String &str, unsigned long type) {
  if(type != REG_EXPAND_SZ &&  type != REG_SZ) {
    throwException(_T("RegistryValue::Illegal type=%d for value <%s>. Must be {REG_EXPAND_SZ, REG_SZ}. Str=<%s>")
                  ,type, name.cstr(), str.cstr());
  }
  init(name, type, sizeof(TCHAR)*((DWORD)str.length() + 1));
  setBuffer(str.cstr());
}

RegistryValue::RegistryValue(const String &name, unsigned long value,  unsigned long type) {
  if(type != REG_DWORD && type != REG_DWORD_LITTLE_ENDIAN && type != REG_DWORD_BIG_ENDIAN) {
    throwException(_T("RegistryValue::Illegal type=%d for value <%s>. Must be {REG_DWORD,REG_DWORD_BIG_ENDIAN}. Value=%lu")
                  ,type, name.cstr(), value);
  }
  init(name, type, sizeof(unsigned long));
  setBuffer(&value);
}

// After use of result returned by stringArrayToCharPointer, call delete.
static TCHAR *stringArrayToCharPointer(const StringArray &strings, unsigned long &charCount) {
  charCount = 0;
  for(int i = 0; i < (int)strings.size(); i++) {
    charCount += (int)strings[i].length() + 1;
  }
  charCount++;
  if(charCount== 1) {
    charCount++;
  }
  TCHAR *result = new TCHAR[charCount];
  MEMSET(result,0,charCount);
  TCHAR *cp = result;;
  for(int i = 0; i < (int)strings.size(); i++) {
    _tcscpy(cp,strings[i].cstr());
    cp += strings[i].length() + 1;
  }
  *(cp++) = 0;
  if(cp < result + charCount) {
    MEMSET(cp, 0, (result+charCount)-cp);
  }
  return result;
}

RegistryValue::RegistryValue(const String &name, const StringArray &strings) {
  unsigned long charCount;
  TCHAR *buf = stringArrayToCharPointer(strings, charCount);
  init(name, REG_MULTI_SZ, charCount*sizeof(TCHAR));
  setBuffer(buf);
  delete[] buf;
}

RegistryValue::RegistryValue(const RegistryValue &src) {
  init(src.m_name,src.m_type,src.m_bufSize);
  setBuffer(src.m_buffer);
}

RegistryValue::~RegistryValue() {
  cleanup();
}

void RegistryValue::init(const String &name, unsigned long type, unsigned long bufSize) {
  m_name    = name;
  m_type    = type;
  m_bufSize = bufSize;
  m_buffer  = new BYTE[m_bufSize];
}

void RegistryValue::cleanup() {
  if(m_buffer != NULL) {
    delete[] m_buffer;
    m_buffer  = NULL;
  }
  m_bufSize = 0;
}

void RegistryValue::reset() {
  cleanup();
  init(_T(""),REG_NONE,0);
}

void RegistryValue::setBuffer(const void *src) {
  if(m_bufSize != 0) {
    memcpy(m_buffer,src,m_bufSize);
  }
}

RegistryValue &RegistryValue::operator=(const RegistryValue &src) {
  if(this == &src) {
    return *this;
  }

  cleanup();
  init(src.m_name,src.m_type,src.m_bufSize);
  setBuffer(src.m_buffer);
  return *this;
}

RegistryValue::operator unsigned long() const {
  if(m_type != REG_DWORD && m_type != REG_DWORD_LITTLE_ENDIAN && m_type != REG_DWORD_BIG_ENDIAN) {
    throwException(_T("RegistryValue::operator unsigned long():Illegal type=%d for value <%s>. Must be {REG_DWORD,REG_DWORD_BIG_ENDIAN}")
                  ,m_type, m_name.cstr());
  }
  return *(unsigned long*)m_buffer;
}

RegistryValue::operator String() const {
  if(m_type != REG_EXPAND_SZ &&  m_type != REG_SZ) {
    throwException(_T("RegistryValue::operator String:Illegal type=%d for value <%s>. Must be {REG_SZ,REG_EXPAND_SZ}")
                  ,m_type, m_name.cstr());
  }
  return String((TCHAR*)m_buffer);
}

RegistryValue::operator StringArray() const {
  StringArray result;

  switch(m_type) {
  case REG_EXPAND_SZ:
  case REG_SZ       :
    result.add((TCHAR*)m_buffer); // will return an array with 1 element
    break;
  case REG_MULTI_SZ :
    { const TCHAR *bufEnd = (TCHAR*)(m_buffer + m_bufSize);
      for(const TCHAR *cp = (TCHAR*)m_buffer; *cp && cp < bufEnd;) {
        String tmp = (TCHAR*)cp;
        result.add(tmp);
        cp += tmp.length()+1;
      }
    }
    break;
  default:
    throwException(_T("RegistryValue::operator Array<String>:Illegal type=%d for value <%s>. Must be {REG_MULTI_SZ,REG_SZ,REG_EXPAND_SZ}")
                  ,m_type, m_name.cstr());
  }
  return result;
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
    return format(_T("%lu"),*(unsigned long*)m_buffer);

  case REG_DWORD_BIG_ENDIAN:
    return format(_T("%lu"),ntohl(*(unsigned long*)m_buffer));

  case REG_BINARY:
    return bytesToString(m_buffer, m_bufSize);

  case REG_NONE:
    return EMPTYSTRING;

  default:
	return bytesToString(m_buffer, m_bufSize);
  }
}

String RegistryValue::typeAsString(unsigned long type) { // static 
  switch(type) {
  case REG_NONE                      : return _T("REG_NONE");
  case REG_SZ                        : return _T("REG_SZ");
  case REG_EXPAND_SZ                 : return _T("REG_EXPAND_SZ");
  case REG_BINARY                    : return _T("REG_BINARY");
  case REG_DWORD                     : return _T("REG_DWORD");
  case REG_DWORD_BIG_ENDIAN          : return _T("REG_DWORD_BIG_ENDIAN");
  case REG_LINK                      : return _T("REG_LINK");
  case REG_MULTI_SZ                  : return _T("REG_MULTI_SZ");
  case REG_RESOURCE_LIST             : return _T("REG_RESOURCE_LIST");
  case REG_FULL_RESOURCE_DESCRIPTOR  : return _T("REG_FULL_RESOURCE_DESCRIPTOR");
  case REG_RESOURCE_REQUIREMENTS_LIST: return _T("REG_RESOURCE_REQUIREMENTS_LIST");
  default                            : return format(_T("Type=%d"),type);
  }
}

// ---------------------------------------------SubKeyIterator-------------------------------------------------------

class SubKeyIterator : public AbstractIterator {
private:
  DECLARECLASSNAME;
  RegistryKey         m_key;
  unsigned long       m_subKeyCount;
  unsigned long       m_maxNameLength;
  unsigned long       m_index;
  TCHAR              *m_nameBuffer;
  String              m_resultBuffer;

  void queryKeyInfo();
  void init(unsigned long subKeyCount, unsigned long maxNameLength);
  void cleanup();

  unsigned long getNameBufferSize() const {
    return m_maxNameLength+1;
  }

public:
  SubKeyIterator(const RegistryKey &key);
  SubKeyIterator(const SubKeyIterator &src); // copy constructor
  ~SubKeyIterator();
  AbstractIterator *clone();
  bool hasNext() const;
  void *next();
  void remove();
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
  unsigned long subKeyCount   = 0;
  unsigned long maxNameLength = 0;
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
  RegistryKey::checkResult(result, s_className, _T("RegQueryInfoKey"), m_key.getName());
  init(subKeyCount, maxNameLength);
}

void SubKeyIterator::init(unsigned long subKeyCount, unsigned long maxNameLength) {
  m_subKeyCount   = subKeyCount;
  m_maxNameLength = maxNameLength;
  m_nameBuffer    = new TCHAR[getNameBufferSize()];
}

void SubKeyIterator::cleanup() {
  delete[] m_nameBuffer;
  m_nameBuffer = NULL;
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

    unsigned long nameBufferSize = getNameBufferSize();
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
      RegistryKey::checkResult(result,_T("SubKeyIterator::next"), _T("RegEnumKeyEx"),m_key.getName());
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
  m_resultBuffer = "";
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
  unsigned long       m_valueCount;
  unsigned long       m_maxNameLength;
  unsigned long       m_maxValueLength;
  unsigned long       m_index;
  TCHAR              *m_nameBuffer;
  BYTE               *m_valueBuffer;
  RegistryValue       m_resultBuffer;

  void queryKeyInfo();
  void init(unsigned long valueCount, unsigned long maxNameLength, unsigned long maxValueLength);
  void cleanup();

  unsigned long getNameBufferSize() const {
    return m_maxNameLength+1;
  }

  unsigned long getValueBufferSize() const {
    return m_maxValueLength+1;
  }

public:
  RegValueIterator(const RegistryKey &key);
  RegValueIterator(const RegValueIterator &src); // copy constructor
  ~RegValueIterator();
  AbstractIterator *clone();
  bool hasNext() const;
  void *next();
  void remove();
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
  unsigned long valueCount     = 0;
  unsigned long maxNameLength  = 0;
  unsigned long maxValueLength = 0;
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
  RegistryKey::checkResult(result, s_className, _T("RegQueryInfoKey"), m_key.getName());
  init(valueCount,maxNameLength,maxValueLength);
}

void RegValueIterator::init(unsigned long valueCount, unsigned long maxNameLength, unsigned long maxValueLength) {
  m_valueCount     = valueCount;
  m_maxNameLength  = maxNameLength;
  m_maxValueLength = maxValueLength;
  m_nameBuffer     = new TCHAR[getNameBufferSize()];
  m_valueBuffer    = new BYTE[getValueBufferSize()];
}

void RegValueIterator::cleanup() {
  delete[] m_nameBuffer;
  m_nameBuffer = NULL;
  m_maxNameLength = 0;

  delete[] m_valueBuffer;
  m_valueBuffer = NULL;
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

    unsigned long nameBufferSize  = getNameBufferSize();
    unsigned long valueBufferSize = getValueBufferSize();
    unsigned long type;
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
      RegistryKey::checkResult(result,_T("RegValueIterator::next"), _T("RegEnumValue"),m_key.getName());
      break;
    }
  }
  return &m_resultBuffer;
}

void RegValueIterator::remove() {
  if(m_resultBuffer.getType() == REG_NONE) {
    noCurrentElementError(s_className);
  }
  m_key.deleteValue(m_resultBuffer.getName());
  m_resultBuffer.reset();
  if(m_valueCount > 0) {
    m_index--;
    m_valueCount--;
  }
}

Iterator<RegistryValue> RegistryKey::getValueIterator() const {
  return Iterator<RegistryValue>(new RegValueIterator(*this));
}
