#include "pch.h"
#include <fcntl.h>
#include <MyUtil.h>
#include <Scanner.h>

#if !defined(UNICODE)

LexFileStream::LexFileStream() {
  m_f       = stdin;
/*
  if(!isatty(m_f)) {
    m_oldMode = setFileMode(m_f,_O_BINARY);
  }
*/
}

LexFileStream::LexFileStream(const String &name) {
  m_f       = nullptr;
  open(name);
}

bool LexFileStream::open(const String &name) {
  close();
  m_f = fopen(name, _T("rb"));
  return ok();
}

void LexFileStream::initQueues() {
  // do nothing cause there are no queues
}

int LexFileStream::getChars(_TUCHAR *dst, UINT n) {
  return fread(dst, 1, n, m_f);
}

bool LexFileStream::eof() {
  return feof(m_f) != 0;
}

#else // UNICODE

LexFileStream::LexFileStream() {
  m_f       = stdin;
/*
  if(!isatty(m_f)) {
    m_oldMode = setFileMode(m_f,_O_BINARY);
  }
*/
}

LexFileStream::LexFileStream(const String &name) {
  m_f       = nullptr;
  open(name);
}

bool LexFileStream::open(const String &name) {
  close();
  m_f = fopen(name, _T("rb"));
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

  while(!eof()) {
    const intptr_t done = d - dst;
    if(done >= (intptr_t)n) return done;
    const intptr_t needed = (intptr_t)n - done;
    const intptr_t got  = m_convertedQueue.get(d, needed);
    if(got) {
      d += got;
    }
  }
  return d - dst;
}

// s point to the beginning of the buffer to search
static inline BYTE *memrchr(const BYTE *s, BYTE ch, size_t n) {
  for(s += n; n--;) {
    if(*(--s) == ch) {
      return (BYTE*)s;
    }
  }
  return nullptr;
}

bool LexFileStream::eof() {
  return (feof(m_f) != 0) && m_rawQueue.isEmpty() && m_convertedQueue.isEmpty();
}

// -------------------------------------------------------------------

bool ByteQueue::hasFullLine() const {
  return findLastNewLine() != nullptr;
}

const BYTE *ByteQueue::findLastNewLine() const {
  return isEmpty() ? nullptr : memrchr(getData(), '\n', size());
}

size_t ByteQueue::readUntilHasNewLine(FILE *f) {
  const BYTE *nl = findLastNewLine();
  while(((nl == nullptr) || (size() < 1000)) && !feof(f)) {
    BYTE tmp[4096];
    const intptr_t n = fread(tmp, 1, sizeof(tmp), f);
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
  TCHAR  tmp[4096];
  TCHAR *buffer     = tmp;
  size_t bufferSize = ARRAYSIZE(tmp);
  String result;

  if(m_firstRead) {
    m_firstRead = false;
    UINT bytesToSkip = 0;
    m_textFormat = TextFormatDetecter::detectFormat(getData(), (int)size(), bytesToSkip);
    if(bytesToSkip) {
      remove(0, bytesToSkip);
      count = (count >= bytesToSkip) ? (count - bytesToSkip) : 0;
    }
  }

  switch(m_textFormat) {
  case TF_UNDEFINED: // just read it as ascii bytes
  case TF_ASCII8:
    { for(size_t i = 0; (i < count) && !isEmpty();) {
        const size_t rest      = count - i; // in bytes
        const size_t charCount = min(rest, bufferSize - 1); // characters to copy = bytes to copy
        const BYTE  *src       = getData();
        const BYTE  *lastByte  = src + charCount;
        _TUCHAR     *dst       = (_TUCHAR*)buffer;
        while(src < lastByte) {
          *(dst++) = *(src++);
        }
        *dst = 0;
        result += buffer;
        remove(0, charCount);
        i += charCount;
      }
    }
    break;
  case TF_ASCII16_BE:
    throwException(_T("%s:Textformat TF_ASCII16_BE not implemented yet"), __TFUNCTION__);
    break;
  case TF_ASCII16_LE:
    { if(count & 1) {  // make sure count is even, or we'll get an infinite loop
        if(count > 1) {
          count--;
        } else if(++count > size()) {
          add(0); // add a 0-byte to end the stream
        }
      }
      for(size_t i = 0; (i < count) && !isEmpty();) {
        const size_t rest       = count - i; // in bytes
        const size_t tcharCount = min(rest / sizeof(TCHAR), bufferSize - 1); // in TCHAR's
        TMEMCPY(buffer, (const TCHAR*)getData(), tcharCount);
        buffer[tcharCount] = 0;
        result += buffer;
        const size_t byteCount = tcharCount * sizeof(TCHAR);
        remove(0, byteCount);
        i += byteCount;
      }
    }
    break;
  case TF_UTF8:
  case TF_UTF16_BE:
  case TF_UTF16_LE:
    try {
      const int requiredSize = MultiByteToWideChar(CP_UTF8, 0, (char*)getData(), (int)count, nullptr, 0);
      if(requiredSize == 0) {
        throwLastErrorOnSysCallException(__TFUNCTION__, _T("MultiByteToWideChar"));
      }
      if((UINT)requiredSize + 1 > bufferSize) {
        if(buffer != tmp) {
          SAFEDELETEARRAY(buffer);
        }
        bufferSize = requiredSize + 1;
        buffer = new TCHAR[bufferSize]; TRACE_NEW(buffer);
      }
      const int ret = MultiByteToWideChar(CP_UTF8, 0, (char*)getData(), (int)count, buffer, requiredSize);
      if(ret == 0) {
        throwLastErrorOnSysCallException(__TFUNCTION__, _T("MultiByteToWideChar"));
      }
      buffer[requiredSize] = 0;
      result = buffer;
      if(buffer != tmp) {
        SAFEDELETEARRAY(buffer);
      }
      remove(0, count);
    } catch(...) {
      if(buffer != tmp) {
        SAFEDELETEARRAY(buffer);
      }
      throw;
    }
    break;
  }
  return result;
}

// --------------------------------------------------------------------------------------------

intptr_t CharQueue::get(_TUCHAR *dst, size_t n) {
  n = min(length(), n);
  TMEMCPY(dst, cstr(), n);
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
  m_f = nullptr;
}
