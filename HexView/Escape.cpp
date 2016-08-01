#include "stdafx.h"

bool isOctDigit(TCHAR ch) {
  return ('0' <= ch) && (ch <= '7');
}

/* Convert the octal digit represented by ch to a BYTE. ch must be a
 * digit in the range '0'-'7'.
 */
BYTE octToByte(int ch) {
  return (ch-'0') & 0x7;
}

/* Convert the decimal digit represented by ch to a BYTE. ch must be a
 * digit in the range '0'-'9'.
 */
BYTE decToByte(int ch) {
  return (ch-'0');
}

/* Convert the hex digit represented by ch to a BYTE. ch must be one of
 * the following characters: 0123456789abcdefABCDEF
 */
BYTE hexToByte(int ch) {
  return (_istdigit(ch) ? (ch-'0') : ((toupper(ch))-'A')+10) & 0xf;
}

BYTE escape(const TCHAR *&s) {
  /* Map escape sequences into their equivalent symbols. Return the equivalent
   * ASCII character. s is advanced past the escape sequence. If no escape
   * sequence is present, the current character is returned and the String
   * is advanced by one. The following are recognized:
   *
   *  \b      backspace
   *  \f      formfeed
   *  \t      tab
   *  \n      newline
   *  \r      carriage return
   *  \e      ASCII ESC character ('\033')
   *  \0ddd   number formed of 1-3 octal digits
   *  \ddd    number formed of 1-3 decimal digits
   *  \xdd    number formed of 1-2 hex digits
   */

  UINT asciiValue;
  const TCHAR *start = s;

  if(*s != '\\') {
    asciiValue = *(s++);
  } else {
    s++; // Skip the '\' 
    switch(toupper(*s)) {
    case '\\':
      asciiValue = '\\'  ;
      break;
    case 'B' :
      asciiValue = '\b'  ;
      break;
    case 'F' :
      asciiValue = '\f'  ;
      break;
    case 'N' :
      asciiValue = '\n'  ;
      break;
    case 'R' :
      asciiValue = '\r'  ;
      break;
    case 'T' :
      asciiValue = '\t'  ;
      break;
    case 'E' :
      asciiValue = '\033';
      break;
    case '\0':
      return 0;

    case '0': // expect an octal number
      asciiValue = 0;
      s++;
      if(isOctDigit(*s)) {
        asciiValue = octToByte(*(s++));
      }
      if(isOctDigit(*s)) {
        asciiValue = asciiValue * 8 + octToByte(*(s++));
      }
      if(isOctDigit(*s) && (asciiValue <= '\037')) { // assure we don't make value greater than 255
        asciiValue = asciiValue * 8 + octToByte(*(s++));
      }
      s--;
      break;
    case 'X' : // expect a hexadecimal number
      asciiValue = 0;
      s++;
      if(isxdigit(*s)) {
        asciiValue = hexToByte(*(s++));
      }
      if(isxdigit(*s)) {
        asciiValue = asciiValue * 16 + hexToByte(*(s++));
      }
      s--;
      break;

    default:
      if(!isdigit(*s)) {
        asciiValue = *s;
      } else {
        asciiValue = decToByte(*(s++));
        if(isdigit(*s)) {
          asciiValue = asciiValue * 10 + decToByte(*(s++));
        }
        if(isdigit(*s)) {
          const BYTE v = decToByte(*s);
          if(asciiValue <= 25 && ((v <= 5) || (asciiValue <= 24))) { // assure we don't make value greater than 255
            asciiValue = asciiValue * 10 + v;
            s++;
          }
        }
        s--;
      }
      break;
    }
    s++;
  }
  if(asciiValue > 255) {
    const int length = (int)(s - start);
    throwException(_T("Invalid escape sequence [%*.*s]. Value cannot be in an 8-bit byte"), length, length, start);
  }
  return asciiValue;
}
