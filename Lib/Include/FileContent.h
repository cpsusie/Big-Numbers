#pragma once

#include "MyUtil.h"
#include "ByteArray.h"


class FileContent : public ByteArray {
public:
  FileContent() {
  }
  FileContent(const String &fileName);
  FileContent(      FILE *f);
  FileContent &load(FILE *f);                 // return this
  FileContent &load(const String &fileName);  // return this
  FileContent &save(FILE *f);
  FileContent &save(const String &fileName);
#ifdef UNICODE
  String converToString(UINT codePage = CP_UTF8) const;
#else
  String converToString() const;
#endif
  bool hasUTF8BOM() const;
};

