#include "pch.h"
#include <ByteArray.h>

ByteFileArray::ByteFileArray(const String &fileName, size_t startOffset) : m_startOffset(startOffset) {
  m_f = FOPEN(fileName, _T("rb"));
  FSEEK(m_f, m_startOffset);
  size_t size;
  FREAD(&size, sizeof(size), 1, m_f);
  m_size = size;
}

ByteFileArray::~ByteFileArray() {
  fclose(m_f);
}

BYTE ByteFileArray::operator[](size_t i) const {
  FSEEK(m_f, m_startOffset + sizeof(size_t) + i);
  BYTE result;
  FREAD(&result, 1, 1, m_f);
  return result;
}
