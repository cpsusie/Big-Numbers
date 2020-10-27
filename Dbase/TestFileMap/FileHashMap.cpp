#include "stdafx.h"
#include <ByteFile.h>
#include "FileHashMap.h"

#define SETCONST(dst,src,type) { type *p = (type*)&(dst); *p = src; }

KeyFileWrapper::KeyFileWrapper(const String &fileName, int keySize, int valueSize)
: m_fileName(fileName)
, m_keySize(keySize)
, m_valueSize(valueSize)
, m_keyFieldCount(0)
, m_valueOffset(0)
{
  if(!DbFile::exist(m_fileName)) {
    create(fileName);
  }

  m_file = new KeyFile(m_fileName, DBFMODE_READWRITE, nullptr);

  const KeyFileDefinition &keydef = m_file->getDefinition();
  SETCONST(m_keyFieldCount, keydef.getKeyFieldCount(), UINT);
  SETCONST(m_valueOffset  , keydef.getFieldOffset(m_keyFieldCount), UINT);
  m_key.init();
  m_fileKey.init();
  m_size = m_file->getKeyCount();
}

KeyFileWrapper::~KeyFileWrapper() {
  flush();
  delete m_file;
}

void KeyFileWrapper::create(const String &fileName) {
  KeyFileDefinition keydef;
  int i;
  for(i = m_keySize; i >= 4; i -= sizeof(UINT)) {
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UINT);
  }
  for(; i >= 2; i -= sizeof(USHORT)) {
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_USHORT);
  }
  if(i == 1) {
    keydef.addKeyField(SORT_ASCENDING,DBTYPE_UCHAR);
  }

  for(i = m_valueSize; i >= 4; i -= sizeof(UINT)) {
    keydef.addDataField(DBTYPE_UINT);
  }
  for(; i >= 2; i -= sizeof(USHORT)) {
    keydef.addDataField(DBTYPE_USHORT);
  }
  if(i == 1) {
    keydef.addDataField(DBTYPE_UCHAR);
  }
  KeyFile::create(fileName, keydef);
}

void KeyFileWrapper::flush() {
  if(memcmp(getValue(m_key), getValue(m_fileKey), m_valueSize) != 0) {
    update(m_key.m_data, getValue(m_key));
    m_fileKey = m_key;
  }
}

void *KeyFileWrapper::get(const void *key) {
  flush();
  memcpy(m_fileKey.m_data,key, m_keySize);
  if(m_file->searchMin(RELOP_EQ, m_fileKey, m_keyFieldCount)) {
    m_key = m_fileKey;
    return getValue(m_key);
  } else {
    m_fileKey = m_key;
    return nullptr;
  }
}

bool KeyFileWrapper::put(const void *key, const void *value) {
  flush();
  memcpy(m_fileKey.m_data, key, m_keySize);
  memcpy(getValue(m_fileKey), value, m_valueSize);
  try {
    m_file->insert(m_fileKey);
    m_key = m_fileKey;
    m_size++;
    return true;
  } catch(sqlca ca) {
    if(ca.sqlcode == SQL_DUPKEY) {
      m_fileKey = m_key;
      return false;
    } else {
      throwException(_T("FileMap::put:%s"), ca.sqlerrmc);
      return false;
    }
  }
}

bool KeyFileWrapper::remove(const void *key) {
  flush();
  memcpy(m_fileKey.m_data, key, m_keySize);
  try {
    m_file->remove(m_fileKey);
    m_fileKey.init();
    m_key.init();
    m_size--;
    return true;
  } catch(sqlca ca) {
    if(ca.sqlcode == SQL_NOT_FOUND) {
      m_fileKey = m_key;
      return false;
    } else {
      throwException(_T("%s:%s"), __TFUNCTION__, ca.sqlerrmc);
      return false;
    }
  }
}

void KeyFileWrapper::update(const void *key, const void *value) {
  KeyType tmpKey;
  memcpy(tmpKey.m_data, key, m_keySize);
  memcpy(getValue(tmpKey), value, m_valueSize);
  try {
    m_file->update(tmpKey);
  } catch(sqlca ca) {
    throwException(_T("%s:%s"), __TFUNCTION__, ca.sqlerrmc);
  }
}

void KeyFileWrapper::clear() {
  try {
    m_file->truncate();
    m_fileKey.init();
    m_key.init();
    m_size = 0;
  } catch(sqlca ca) {
    throwException(_T("%s:%s"), __TFUNCTION__, ca.sqlerrmc);
  }
}

void KeyFileWrapper::save(ByteOutputStream &s) const {
  KeyFileWrapper *me = (KeyFileWrapper*)this;
  me->flush();
  ByteInputFile src(m_fileName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = src.getBytes(buffer,sizeof(buffer))) > 0) {
    s.putBytes(buffer,n);
  }
}

void KeyFileWrapper::load(ByteInputStream &s) {
  DbFile::destroy(m_fileName);
  ByteOutputFile dst(m_fileName);
  BYTE buffer[4096];
  intptr_t n;
  while((n = s.getBytes(buffer,sizeof(buffer))) > 0) {
    dst.putBytes(buffer,n);
  }
  m_key.init();
  m_fileKey.init();
  m_size = m_file->getKeyCount();
}

DEFINECLASSNAME(FileHashMapKeyIterator);

FileHashMapKeyIterator::FileHashMapKeyIterator(KeyFileWrapper &keyFile)
  : m_keyFile(keyFile)
  , m_keyCursor(keyFile.getFile(), RELOP_TRUE, nullptr, 0, RELOP_TRUE, nullptr, 0, SORT_ASCENDING)
{
  m_hasElement = false;
}

AbstractIterator *FileHashMapKeyIterator::clone() {
  return new FileHashMapKeyIterator(*this);
}

bool FileHashMapKeyIterator::hasNext() const {
  return m_keyCursor.hasNext();
}

void *FileHashMapKeyIterator::next() {
  if(!hasNext()) {
    noNextElementError(s_className);
  }
  m_keyCursor.next(m_buffer);
  m_hasElement = true;
  return m_buffer.m_data;
}

void FileHashMapKeyIterator::remove() {
  if(!m_hasElement) {
    noCurrentElementError(s_className);
  }
  m_hasElement = false;
  m_keyFile.remove(&m_buffer);
}

DEFINECLASSNAME(FileHashMapEntryIterator);

void FileHashMapEntryIterator::flush() {
  if(memcmp(m_buffer.m_value, m_fileValue, m_valueSize) != 0) {
    m_keyFile->update(m_buffer.m_key, m_buffer.m_value);
  }
}

FileHashMapEntryIterator::FileHashMapEntryIterator(KeyFileWrapper &keyFile)
  : m_keyFile(&keyFile)
  , m_keyCursor(keyFile.getFile(), RELOP_TRUE, nullptr, 0, RELOP_TRUE, nullptr, 0, SORT_ASCENDING)
{
  m_buffer.m_key   = m_key.m_data;
  m_buffer.m_value = m_keyFile->getValue(m_key);
  m_fileValue      = m_keyFile->getValue(m_fileKey);
  m_valueSize      = m_keyFile->getValueSize();
  m_hasElement     = false;
  m_fileKey.init();
  m_key.init();
}

FileHashMapEntryIterator::FileHashMapEntryIterator(FileHashMapEntryIterator &src)
  : m_keyFile(src.m_keyFile)
  , m_keyCursor(src.m_keyCursor)
{
  m_buffer.m_key   = m_key.m_data;
  m_buffer.m_value = m_keyFile->getValue(m_key);
  m_fileValue      = m_keyFile->getValue(m_fileKey);
  m_valueSize      = src.m_valueSize;
  m_hasElement     = src.m_hasElement;
  m_fileKey        = src.m_fileKey;
  m_key            = src.m_key;
}

FileHashMapEntryIterator &FileHashMapEntryIterator::operator=(FileHashMapEntryIterator &src) {
  if(&src == this) {
    return *this;
  }
  flush();

  m_keyFile        = src.m_keyFile;
  m_keyCursor      = src.m_keyCursor;
  m_buffer.m_key   = m_key.m_data;
  m_buffer.m_value = m_keyFile->getValue(m_key);
  m_fileValue      = m_keyFile->getValue(m_fileKey);
  m_valueSize      = src.m_valueSize;
  m_hasElement     = src.m_hasElement;
  m_fileKey        = src.m_fileKey;
  m_key            = src.m_key;
  return *this;
}

FileHashMapEntryIterator::~FileHashMapEntryIterator() {
  flush();
}

AbstractIterator *FileHashMapEntryIterator::clone() {
  return new FileHashMapEntryIterator(*this);
}

bool FileHashMapEntryIterator::hasNext() const {
  return m_keyCursor.hasNext();
}

void *FileHashMapEntryIterator::next() {
  if(!hasNext()) {
    noNextElementError(s_className);
  }
  flush();
  m_keyCursor.next(m_key);
  m_fileKey    = m_key;
  m_hasElement = true;
  return &m_buffer;
}

void FileHashMapEntryIterator::remove() {
  if(!m_hasElement) {
    noCurrentElementError(s_className);
  }
  m_keyFile->remove(m_fileKey.m_data);
  m_hasElement = false;

  m_fileKey.init();
  m_key.init();
}
