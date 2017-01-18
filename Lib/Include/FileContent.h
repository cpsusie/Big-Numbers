#pragma once

#include "MyUtil.h"
#include "ByteArray.h"

class FileContent : public ByteArray {
public:
  FileContent() {
  }
  FileContent(      const String     &fileName);
  FileContent(      FILE             *f       );
  FileContent(      ByteInputStream  &in      );
  FileContent &load(const String     &fileName);  // return this
  FileContent &load(FILE             *f       );  // return this
  FileContent &load(ByteInputStream  &in      );  // return this
  FileContent &save(const String     &fileName);  // return this
  FileContent &save(FILE             *f       );  // return this
  FileContent &save(ByteOutputStream &out     );  // return this
#ifdef UNICODE
  String converToString(UINT codePage = CP_UTF8) const;
#else
  String converToString() const;
#endif
  bool hasUTF8BOM() const;
};

