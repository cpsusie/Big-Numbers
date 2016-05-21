#pragma once

#include "MyString.h"
#include "StreamParameters.h"

class StrStream : public StreamParameters, public String { // used instead of standardclass strstream, which is slow!!! (at least in windows)
public:
  StrStream(int precision=6, int width=0, int flags=0);
  StrStream(const StreamParameters &param) : StreamParameters(param) {
  }
  StrStream(tostream &stream);

  void clear() {
    String::operator=(_T(""));
  }
  
  inline TCHAR getLast() const {
    return last();
  }
  
  static void formatZero(String &result, int precision, long flags, int maxPrecision = 0);

  TCHAR unputc();
  StrStream &append(    const String             &str  ); // append str to stream without using any format-specifiers
  StrStream &append(    const TCHAR              *str  );
  StrStream &operator<<(TCHAR                     ch   );
  StrStream &operator<<(unsigned char             ch   );
  StrStream &operator<<(const TCHAR              *str  );
  StrStream &operator<<(const unsigned char      *str  );
  StrStream &operator<<(const String             &str  );
  StrStream &operator<<(         int              n    );
  StrStream &operator<<(unsigned int              n    );
  StrStream &operator<<(         long             n    );
  StrStream &operator<<(unsigned long             n    );
  StrStream &operator<<(         __int64          n    );
  StrStream &operator<<(unsigned __int64          n    );
  StrStream &operator<<(float                     f    );
  StrStream &operator<<(double                    d    );
  StrStream &operator<<(const StrStream          &s    );
  StrStream &operator<<(const StreamParameters   &param);

#ifdef UNICODE
  StrStream &append(    const char *str);
  StrStream &operator<<(const char *str);
  StrStream &operator<<(char        ch);
#endif
};
