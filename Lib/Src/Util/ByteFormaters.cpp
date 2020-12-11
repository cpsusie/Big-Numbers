#include "pch.h"
#include <ctype.h>

class StdByteFormater : public ByteFormater {
private:
  const bool m_printableToAscii;
public:
  StdByteFormater(bool printableToAscii) : m_printableToAscii(printableToAscii) {
  }
  String toString(const BYTE &ch) final {
    if(m_printableToAscii) {
      return format(isprint(ch) ? _T("%c") : _T("\\x%02x"), (_TUCHAR)ch);
    } else {
      return format(_T("\\x%02x"), ch);
    }
  }
};

#pragma warning(disable : 4073)
#pragma init_seg(lib)

static StdByteFormater _stdAsciiFormater(true );
static StdByteFormater _hexFormater(     false);

ByteFormater &ByteFormater::stdAsciiFormater  = _stdAsciiFormater;
ByteFormater &ByteFormater::hexFormater       = _hexFormater;
