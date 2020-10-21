#include "pch.h"
#include <fcntl.h>
#include <MyUtil.h>
#include "ByteFile.h"

void ByteOutputFile::init(const String &name) {
  m_name  = name;
}

ByteOutputFile::ByteOutputFile() {
  m_file  = nullptr;
}

ByteOutputFile::ByteOutputFile(const String &name) {
  m_file  = nullptr;
  open(name);
}

ByteOutputFile::ByteOutputFile(FILE *f) {
  if(f == nullptr) {
    throwInvalidArgumentException(__TFUNCTION__, _T("Illegal argument(nullptr)"));
  }
  m_file    = f;
  m_oldMode = setFileMode(f, _O_BINARY);

  init(EMPTYSTRING);
}

ByteOutputFile::~ByteOutputFile() {
  close();
}

void ByteOutputFile::open(const String &name) {
  close();
  m_file = MKFOPEN(name, _T("wb"));
  init(name);
}

void ByteOutputFile::close() {
  if(!isOpen()) {
    return;
  }
  if(m_name != EMPTYSTRING) {
    fclose(m_file);
    m_name = EMPTYSTRING;
  } else {
    fflush(m_file);
    setFileMode(m_file, m_oldMode);
  }
  m_file = nullptr;
}

void ByteOutputFile::putByte(BYTE c) {
  if(putc(c, m_file) != c) {
    throwException(_T("Error write byte"));
  }
}

void ByteOutputFile::putBytes(const BYTE *src, size_t n) {
  FWRITE(src, 1, n, m_file);
}

inline __int64 ByteOutputFile::getPos() {
  return GETPOS(m_file);
}

void ByteOutputFile::seek(__int64 pos) {
  FSEEK(m_file, pos);
}

