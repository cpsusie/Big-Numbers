#include "pch.h"
#include <fcntl.h>
#include <MyUtil.h>
#include "ByteFile.h"

ByteInputFile::ByteInputFile() {
  m_file  = nullptr;
}

ByteInputFile::ByteInputFile(const String &name) {
  m_file  = nullptr;
  open(name);
}

ByteInputFile::ByteInputFile(FILE *f) {
  if(f == nullptr) {
    throwInvalidArgumentException(__TFUNCTION__, _T("f=nullptr"));
  }
  m_file    = f;
  m_oldMode = setFileMode(f, _O_BINARY);

  init(EMPTYSTRING);
}

ByteInputFile::~ByteInputFile() {
  close();
}

void ByteInputFile::init(const String &name) {
  m_name     = name;
  m_startPos = GETPOS(m_file);
}

void ByteInputFile::open(const String &name) {
  close();
  m_file = FOPEN(name, _T("rb"));
  init(name);
}

void ByteInputFile::close() {
  if(!isOpen()) {
    return;
  }
  if(m_name != EMPTYSTRING) {
    fclose(m_file);
  } else {
    setFileMode(m_file, m_oldMode);
  }
  m_file = nullptr;
}

int ByteInputFile::getByte() {
  return getc(m_file);
}

intptr_t ByteInputFile::getBytes(BYTE *dst, size_t n) {
  return fread(dst, 1, n, m_file);
}

void ByteInputFile::reset() {
  seek(m_startPos);
}

void ByteInputFile::seek(__int64 pos) {
  FSEEK(m_file, pos);
}
