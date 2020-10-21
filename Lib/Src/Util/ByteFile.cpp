#include "pch.h"
#include <fcntl.h>
#include <MyUtil.h>
#include "ByteFile.h"

ByteFile::ByteFile() {
  m_file  = nullptr;
}

ByteFile::ByteFile(const String &name, FileMode mode) {
  m_file  = nullptr;
  open(name, mode);
}

ByteFile::ByteFile(FILE *f, FileMode mode) {
  if(f == nullptr) {
    throwInvalidArgumentException(__TFUNCTION__, _T("f=nullptr"));
  }
  m_file    = f;
  m_oldMode = setFileMode(f, _O_BINARY);

  init("", mode);
}

ByteFile::~ByteFile() {
  close();
}

void ByteFile::init(const String &name, FileMode mode) {
  m_name     = name;
  m_mode     = mode;
  m_startPos = getPos();
}

void ByteFile::open(const String &name, FileMode mode) {
  close();
  switch(mode) {
  case READMODE : m_file = FOPEN(  name, _T("rb")); break;
  case WRITEMODE: m_file = MKFOPEN(name, _T("wb")); break;
  }
  init(name, mode);
}

void ByteFile::close() {
  if(!isOpen()) {
    return;
  }
  if(m_name != "") {
    fclose(m_file);
  } else {
    fflush(m_file);
    setFileMode(m_file, m_oldMode);
  }
  m_file = nullptr;
}

void ByteFile::putByte(BYTE c) {
#if defined(_DEBUG)
  if(m_mode != WRITEMODE) {
    throwException(_T("%s:File is readonly"), __TFUNCTION__);
  }
#endif
  if(putc(c, m_file) != c) {
    throwException(_T("%s:Error write byte"), __TFUNCTION__);
  }
}

int ByteFile::getByte() {
#if defined(_DEBUG)
  if(m_mode != READMODE) {
    throwException(_T("%s:File is writeonly"), __TFUNCTION__);
  }
#endif
  return getc(m_file);
}

void ByteFile::putBytes(const BYTE *src, size_t n) {
#if defined(_DEBUG)
  if(m_mode != WRITEMODE) {
    throwException(_T("%s:File is readonly"), __TFUNCTION__);
  }
#endif
  FWRITE(src, 1, n, m_file);
}

intptr_t ByteFile::getBytes(BYTE *dst, size_t n) {
#if defined(_DEBUG)
  if(m_mode != READMODE) {
    throwException(_T("%s:File is writeonly"), __TFUNCTION__);
  }
#endif
  return fread(dst, 1, n, m_file);
}

void ByteFile::reset() {
  seek(m_startPos);
}

inline __int64 ByteFile::getPos() {
  return GETPOS(m_file);
}

void ByteFile::seek(__int64 pos) {
  FSEEK(m_file, pos);
}
