#pragma once

#include "ByteArray.h"

class FileContent : public ByteArray {
public:
  FileContent() {
  }
  FileContent(      const String     &fileName);
  FileContent(      FILE             *f       );
  FileContent(      ByteInputStream  &in      );
  FileContent &load(const String     &fileName);  // Return *this
  FileContent &load(FILE             *f       );  // Return *this
  FileContent &load(ByteInputStream  &in      );  // Return *this
  FileContent &save(const String     &fileName);  // Return *this
  FileContent &save(FILE             *f       );  // Return *this
  FileContent &save(ByteOutputStream &out     );  // Return *this
#if defined(UNICODE)
  String converToString(UINT codePage = CP_UTF8) const;
#else
  String converToString() const;
#endif
  bool hasUTF8BOM() const;
};

