#include "pch.h"
#include <ByteFile.h>
#include <FileContent.h>

FileContent::FileContent(const String &fileName) {
  load(fileName);
}

FileContent::FileContent(FILE *f) {
  load(ByteInputFile(f));
}

FileContent::FileContent(ByteInputStream &in) {
  load(in);
}

FileContent &FileContent::load(const String &fileName) {
  return load(ByteInputFile(fileName));
}

FileContent &FileContent::load(FILE *f) {
  return load(ByteInputFile(f));
}

FileContent &FileContent::load(ByteInputStream &in) {
  clear();

  BYTE buffer[4096];
  intptr_t n;
  while((n = in.getBytes(buffer,sizeof(buffer))) > 0) {
    add(buffer, n);
  }
  return *this;
}

FileContent &FileContent::save(const String &fileName) {
  return save(ByteOutputFile(fileName));
}

FileContent &FileContent::save(FILE *f) {
  return save(ByteOutputFile(f));
}

FileContent &FileContent::save(ByteOutputStream &out) {
  const size_t s = size();
  if(s > 0) {
    out.putBytes(getData(), s);
  }
  return *this;
}


#if defined(UNICODE)

static const int legalCodePages[] = {
  CP_ACP
 ,CP_UTF7
 ,CP_OEMCP
 ,CP_SYMBOL
};

String FileContent::converToString(UINT codePage) const {
  if(size() == 0) {
    return EMPTYSTRING;
  }

  const BYTE *data      = getData();
  int         byteCount = (int)size();
  int         requiredSize;

  if(hasUTF8BOM()) {
    data      += 3;
    byteCount -= 3;
    codePage  = CP_UTF8;
    requiredSize = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, (char*)data, byteCount, NULL, 0);
    if(requiredSize == 0) {
      throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
    }
  } else {
    for(int i = 0; i < ARRAYSIZE(legalCodePages); i++) {
      requiredSize = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, (char*)data, byteCount, NULL, 0);
      if(requiredSize > 0) {
        break;
      }
      if(GetLastError() != ERROR_NO_UNICODE_TRANSLATION) {
        throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
      } else {
        codePage = legalCodePages[i];
      }
    }
    if(requiredSize == 0) {
      throwException(_T("File is not a textfile in any known format"));
    }
  }

  TCHAR *buffer = NULL;
  try {
    const int bufferSize = requiredSize + 1;
    TCHAR *buffer = new TCHAR[bufferSize]; TRACE_NEW(buffer);
    const int ret = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, (char*)data, byteCount, buffer, requiredSize);
    if(ret == 0) {
      throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
    }
    buffer[requiredSize] = 0;
    const String result = buffer;
    SAFEDELETEARRAY(buffer);
    return result;
  } catch(...) {
    SAFEDELETEARRAY(buffer);
    throw;
  }
}

#else

String FileContent::converToString() const {
  char *buffer = NULL;

  try {
    buffer = new char[size()+1]; TRACE_NEW(buffer);
    memcpy(buffer, getData(), size());
    buffer[size()] = 0;
    const String result = buffer;
    SAFEDELETEARRAY(buffer);
    return result;
  } catch(...) {
    SAFEDELETEARRAY(buffer);
    throw;
  }
}

#endif

bool FileContent::hasUTF8BOM() const {
  const BYTE utf8signature[] = { 0xEF, 0xBB, 0xBF };
  return ((size() >= 3) && (memcmp(getData(), utf8signature, 3) == 0));
}
