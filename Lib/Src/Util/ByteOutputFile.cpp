#include "pch.h"
#include <fcntl.h>
#include "ByteFile.h"

void ByteOutputFile::init(const String &name) {
  m_name  = name;
}

ByteOutputFile::ByteOutputFile() {
  m_file  = NULL;
}

ByteOutputFile::ByteOutputFile(const String &name) {
  m_file  = NULL;
  open(name);
}

ByteOutputFile::ByteOutputFile(FILE *f) {
  if(f == NULL) {
    throwInvalidArgumentException(_T(__FUNCTION__), _T("Illegal argument(NULL)"));
  }
  m_file    = f;
  m_oldMode = setFileMode(f, _O_BINARY);

  init("");
}

ByteOutputFile::~ByteOutputFile() {
  close();
}

void ByteOutputFile::open(const String &name) {
  close();
  m_file = MKFOPEN(name, "wb");
  init(name);
}

void ByteOutputFile::close() {
  if(!isOpen()) {
    return;
  }
  if(m_name != "") {
    fclose(m_file);
    m_name = "";
  } else {
    fflush(m_file);
    setFileMode(m_file, m_oldMode);
  }
  m_file = NULL;
}

void ByteOutputFile::putByte(BYTE c) {
  if(putc(c, m_file) != c) {
    throwException(_T("Error write byte"));
  }
}

void ByteOutputFile::putBytes(const BYTE *src, size_t n) {
  FWRITE(src, 1, n, m_file);
}

void ByteOutputFile::seek(__int64 pos) {
  FSEEK(m_file, pos);
}
