#include "pch.h"
#include <ByteArray.h>

ByteFileArray::ByteFileArray(const String &fileName, unsigned int startOffset) : m_startOffset(startOffset) {
  m_f = FOPEN(fileName, _T("rb"));
  FSEEK(m_f, m_startOffset);
  unsigned int size;
  FREAD(&size, sizeof(size), 1, m_f);
  m_size = size;
}

ByteFileArray::~ByteFileArray() {
  fclose(m_f);
}

BYTE ByteFileArray::operator[](unsigned int i) const {
  FSEEK(m_f, m_startOffset + sizeof(unsigned int) + i);
  BYTE result;
  FREAD(&result, 1, 1, m_f);
  return result;
}
