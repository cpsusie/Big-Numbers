#include "pch.h"
#include <fcntl.h>
#include <Scanner.h>

#ifndef UNICODE

LexFileStream::LexFileStream() {
  m_f       = stdin;
/*
  if(!isatty(m_f)) {
    m_oldMode = setFileMode(m_f,_O_BINARY);
  }
*/
}

LexFileStream::LexFileStream(const String &name) {
  m_f       = NULL;
  open(name);
}

bool LexFileStream::open(const String &name) {
  close();
  m_f = fopen(name.cstr(), _T("rb"));
  return ok();
}

void LexFileStream::initQueues() {
  // do nothing cause there are no queues
}

int LexFileStream::getChars(_TUCHAR *dst, unsigned int n) {
  return fread(dst, 1, n, m_f);
}

bool LexFileStream::eof() {
  return feof(m_f) != 0;
}

#else // UNICODE

LexFileStream::LexFileStream(UINT codePage, DWORD flags) : m_rawQueue(codePage, flags) {
  m_f       = stdin;
/*
  if(!isatty(m_f)) {
    m_oldMode = setFileMode(m_f,_O_BINARY);
  }
*/
}
;
LexFileStream::LexFileStream(const String &name, UINT codePage, DWORD flags) : m_rawQueue(codePage, flags) {
  m_f       = NULL;
  open(name);
}

bool LexFileStream::open(const String &name) {
  close();
  m_f = fopen(name.cstr(), _T("rb"));
  initQueues();
  return ok();
}

void LexFileStream::initQueues() {
  m_rawQueue.clear();
  m_convertedQueue.clear();
}

intptr_t LexFileStream::getChars(_TUCHAR *dst, size_t n) {
  _TUCHAR *d = dst;

  while((m_convertedQueue.size() < n) && !(m_rawQueue.isEmpty() && feof(m_f))) {
    m_convertedQueue.put(m_rawQueue.getConvertedString(m_rawQueue.readUntilHasNewLine(m_f)));
  }

  while((d - dst < (int)n) && !eof()) {
    const intptr_t rest = n - (d-dst);
    const intptr_t got  = m_convertedQueue.get(d, rest);
    if(got) {
      d += got;
    }
  }
  return d - dst;
}

static inline BYTE *memrchr(const BYTE *s, BYTE ch, size_t n) { // s point to the beginning of the buffer to search
  for(s += n; n--;) {
    if(*(--s) == ch) {
      return (BYTE*)s;
    }
  }
  return NULL;
}

bool LexFileStream::eof() {
  return (feof(m_f) != 0) && m_rawQueue.isEmpty() && m_convertedQueue.isEmpty();
}

// -------------------------------------------------------------------

bool ByteQueue::hasFullLine() const {
  return findLastNewLine() != NULL;
}

const BYTE *ByteQueue::findLastNewLine() const {
  return isEmpty() ? NULL : memrchr(getData(), '\n', size());
}

size_t ByteQueue::readUntilHasNewLine(FILE *f) {
  const BYTE *nl = findLastNewLine();;
  while((nl == NULL) && !feof(f)) {
    BYTE tmp[4096];
    const size_t n = fread(tmp, 1, sizeof(tmp), f);
    if(n > 0) {
      const size_t oldSize = size();
      append(tmp, n);
      nl = memrchr(tmp, '\n', n);
      if(nl) {
        nl = getData() + oldSize + (nl - tmp);
      }
    }
  }
  return nl ? (nl - getData() + 1) : size();
}

String ByteQueue::getConvertedString(size_t count) {
  TCHAR tmp[4096];
  TCHAR *buffer     = tmp;
  int    bufferSize = ARRAYSIZE(tmp);
  try {
    const int requiredSize = MultiByteToWideChar(m_codePage, m_flags, (char*)getData(), (int)count, NULL, 0);
    if(requiredSize == 0) {
      throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
    }
    if(requiredSize + 1 > bufferSize) {
      if(buffer != tmp) {
        delete[] buffer;
      }
      bufferSize = requiredSize + 1;
      buffer = new TCHAR[bufferSize];
    }
    const int ret = MultiByteToWideChar(m_codePage, m_flags, (char*)getData(), (int)count, buffer, requiredSize);
    if(ret == 0) {
      throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
    }
    buffer[requiredSize] = 0;
    const String result = buffer;
    if(buffer != tmp) {
      delete[] buffer;
    }
    remove(0, count);
    return result; 
  } catch(...) {
    if(buffer != tmp) {
      delete[] buffer;
    }
    throw;
  }
}

intptr_t CharQueue::get(_TUCHAR *dst, size_t n) {
  n = min(length(), n);
  MEMCPY(dst, cstr(), n);
  remove(0, (int)n);
  return n;
}

#endif

void LexFileStream::close() {
  if(m_f == stdin) {
//    setFileMode(stdin, m_oldMode);
  } else if(m_f) { // never close stdin
    fclose(m_f);
  }
  initQueues();
  m_f = NULL;
}
