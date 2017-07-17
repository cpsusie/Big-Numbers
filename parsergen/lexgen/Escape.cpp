#include "stdafx.h"
#include <ctype.h>
#include "lexscanner.h"

static inline bool isOctalDigit(TCHAR x) {
  return '0' <= x && x <= '7';
}

// Convert the hex digit represented by 'c' to an int. 'c' must be one of
// the following characters: 0123456789abcdefABCDEF
static inline int hexToBin(int c) {
  return (isdigit(c) ? c - '0': ((toupper(c)) - 'A') + 10) & 0xf;
}

// Convert the octal digit represented by 'c' to an int. 'c' must be a
// digit in the range '0'-'7'.
static inline int octalToBin(int c) {
  return (c-'0') & 0x7;
}

// Map escape sequences into their equivalent symbols. Return the equivalent
// ASCII character. s is advanced past the escape sequence. If no escape
// sequence is present, the current character is returned and s
// is advanced by one. The following are recognized:
//
//  \b      backspace
//  \f      formfeed
//  \t      tab
//  \n      newline
//  \r      carriage return
//  \s      space
//  \e      ASCII ESC character ('\033')
//  \^C     C = any letter. Control code ie C-'\x40'
//  \xDD    number formed of 1-2 hex digits
//  \DDD    number formed of 1-3 octal digits
UINT escape(const _TUCHAR *&s) {

  UINT asciiVal;

  if(*s != '\\') {
    asciiVal = *(s++);
  } else {
    s++; // Skip the '\'
    switch(toupper(*s)) {
    case 'B' :
      asciiVal = '\b';
      break;
    case 'F' :
      asciiVal = '\f';
      break;
    case 'T' :
      asciiVal = '\t';
      break;
    case 'N' :
      asciiVal = '\n';
      break;
    case 'R' :
      asciiVal = '\r';
      break;
    case 'S' :
      asciiVal = ' ';
      break;
    case 'E' :
      asciiVal = '\033';
      break;
    case '^' :
      s++;
      asciiVal = *s;
      if (asciiVal >= '\40')
        asciiVal = toupper(asciiVal) - '\40';
      break;

    case 'X' :
      asciiVal = 0;
      s++;
      if (isxdigit(*s))
        asciiVal = hexToBin(*(s++));
      if (isxdigit(*s)) {
        asciiVal <<= 4;
        asciiVal |= hexToBin(*(s++));
      }
      s--;
      break;

    default  :
      if (!isOctalDigit(*s)) {
        asciiVal = *s;
      }
      else {
        s++;
        asciiVal = octalToBin(*(s++));
        if (isOctalDigit(*s)) {
          asciiVal <<= 3;
          asciiVal |= octalToBin(*(s++));
        }
        if (isOctalDigit(*s)) {
          asciiVal <<= 3;
          asciiVal |= octalToBin(*(s++));
        }
        s--;
      }
      break;
    }
    s++;
  }
  return asciiVal;
}
