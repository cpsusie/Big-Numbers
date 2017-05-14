#include "pch.h"
#include <MFCUtil/FileSrc.h>
#include <pldebug.h>

FileSource::FileSource(FILE *f) {
  char buffer[4096];
  int bufferSize = sizeof(buffer);
  m_dataBuffer   = MALLOC(PLBYTE,bufferSize);
  int n;
  for(m_bufferSize = 0; (n = (int)fread(buffer,1,sizeof(buffer),f)) > 0; m_bufferSize += n) {
    if(m_bufferSize + n > bufferSize) {
      if(n == sizeof(buffer)) {
        bufferSize *= 2;
      } else {
        bufferSize += n;
      }
      m_dataBuffer = REALLOC(m_dataBuffer,PLBYTE,bufferSize);
    }
    memcpy(m_dataBuffer + m_bufferSize,buffer,n);
  }

  if(bufferSize > m_bufferSize && m_bufferSize > 0) {
    m_dataBuffer = REALLOC(m_dataBuffer,PLBYTE,m_bufferSize);
  }
  m_currentPosition = 0;
  PLDataSource::Open("file", m_bufferSize);
}

FileSource::~FileSource() {
  PLDataSource::Close();
  FREE(m_dataBuffer);
}

PLBYTE *FileSource::ReadNBytes(int n) {
  return m_dataBuffer + (m_currentPosition += n);
}

// Read but don't advance file pointer.
PLBYTE *FileSource::GetBufferPtr(int MinBytesInBuffer) {
  PLASSERT(MinBytesInBuffer < 4096);
  return m_dataBuffer + m_currentPosition;
}

PLBYTE *FileSource::ReadEverything() {
  throwException(_T("%s:Not implemented"), __TFUNCTION__);
  return NULL;
}

void FileSource::Seek(int n) {
  m_currentPosition = n;
  PLDataSource::Seek(n);
}

