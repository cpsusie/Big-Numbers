#include "pch.h"
#include <FileContent.h>

FileContent::FileContent(const String &fileName) {
  load(fileName);
}

FileContent::FileContent(FILE *f) {
  load(f);
}

FileContent &FileContent::load(const String &fileName) {
  FILE *f = NULL;
  try {
    f = FOPEN(fileName,_T("rb"));
    load(f);
    fclose(f);
    f = NULL;
    return *this;
  } catch(...) {
    if(f) fclose(f);
    throw;
  }
}

FileContent &FileContent::load(FILE *f) {
  clear();

  BYTE buffer[4096];
  int n;
  while((n = FREAD(buffer,1,sizeof(buffer),f)) > 0) {
    append(buffer, n);
  }
  return *this;
}

FileContent &FileContent::save(FILE *f) {
  unsigned int s = size();
  if(s > 0) {
    FWRITE(getData(), 1, s, f);
  }
  return *this;
}

FileContent &FileContent::save(const String &fileName) {
  FILE *f = MKFOPEN(fileName,_T("wb"));
  try {
    save(f);
    fclose(f);
    return *this;
  } catch(...) {
    fclose(f);
    throw;
  }
}

#ifdef UNICODE

static const int legalCodePages[] = {
  CP_ACP
 ,CP_UTF7
 ,CP_OEMCP
 ,CP_SYMBOL
};

String FileContent::converToString(UINT codePage) const {
  if(size() == 0) {
    return _T("");
  }

  const BYTE *data      = getData();
  int         byteCount = size();
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
    TCHAR *buffer = new TCHAR[bufferSize];
    const int ret = MultiByteToWideChar(codePage, MB_ERR_INVALID_CHARS, (char*)data, byteCount, buffer, requiredSize);
    if(ret == 0) {
      throwLastErrorOnSysCallException(_T("MultiByteToWideChar"));
    }
    buffer[requiredSize] = 0;
    const String result = buffer;
    delete[] buffer;
    buffer = NULL;
    return result;
  } catch(...) {
    if(buffer) delete[] buffer;
    throw;
  }
}

#else

String FileContent::converToString() const {
  char *buffer = NULL;
  
  try {
    buffer = new char[size()+1];
    memcpy(buffer, getData(), size());
    buffer[size()] = 0;
    const String result = buffer;
    delete[] buffer;
    buffer = NULL;
    return result;
  } catch(...) {
    if(buffer) delete[] buffer;
    throw;
  }
}

#endif

bool FileContent::hasUTF8BOM() const {
  const BYTE utf8signature[] = { 0xEF, 0xBB, 0xBF };
  return ((size() >= 3) && (memcmp(getData(), utf8signature, 3) == 0));
}
