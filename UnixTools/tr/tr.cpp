#include "stdafx.h"
#include <MyUtil.h>
#include <fcntl.h>
#include <io.h>

static bool isoctdigit(char x) {
  return '0' <= x && x <= '7';
}

static int hex2bin(int c) {
  // Convert the hex digit represented by 'c' to an int. 'c' must be one of
  // the following characters: 0123456789abcdefABCDEF
  return (isdigit(c) ? c - '0': ((toupper(c)) - 'A') + 10) & 0xf;
}

static int oct2bin(int c) {
  // Convert the octal digit represented by 'c' to an int. 'c' must be a
  // digit in the range '0'-'7'.
  return (c-'0') & 0x7;
}

/*------------------------------------------------------------*/

String escape(unsigned char *s) {
  // Map escape sequences into their equivalent symbols. Return the equivalent
  // ASCII character. If no escape
  // sequence is present, the current character is returned and s
  // is advanced by one. The following are recognized:
  //
  //  \b      backspace
  //  \f      formfeed         (
  //  \t      tab              (\9 )
  //  \n      newline          (\10)
  //  \r      carriage return  (\13)
  //  \s      space            (\32)
  //  \e      Escape character (\27)
  //  \^C     C = any letter. Control code ie C-'\x40'
  //  \\      '\'
  //  \DDD    number formed of 1-3 decimal digits
  //  \xDD    number formed of 1-2 hex digits
  //  \0DDD   number formed of 1-3 octal digits

  unsigned int asciival;
  unsigned char *buf = new unsigned char[strlen((char*)s)+1];
  int index = 0;
  while(*s) {
    if(*s != '\\') {
      asciival = *(s++);
    } else {
      s++; // Skip the '\'
      switch(toupper(*s)) {
      case 'B' :
        asciival = '\b';   s++;
        break;
      case 'F' :
        asciival = '\f';   s++;
        break;
      case 'T' :
        asciival = '\t';   s++;
        break;
      case 'N' :
        asciival = '\n';   s++;
        break;
      case 'R' :
        asciival = '\r';   s++;
        break;
      case 'S' :
        asciival = ' ';    s++;
        break;
      case 'E' :
        asciival = '\033'; s++;
        break;
      case '\\':
        asciival = '\\';   s++;
        break;

      case '^' :
        s++;
        asciival = *(s++);
        if(asciival >= '\40') {
          asciival = toupper(asciival) - '\40';
        }
        break;

      case 'X' :
        for(s++, asciival = 0; asciival < 256 && isxdigit(*s);) {
          asciival = asciival * 16 + hex2bin(*(s++));
        }
        break;

      case '0':
        for(s++, asciival = 0; asciival < 256 && isoctdigit(*s);) {
          asciival = asciival * 8 + oct2bin(*(s++));
        }
        break;

      default  :
        if(!isdigit(*s)) {
          asciival = *(s++);
        } else {
          for(asciival = 0; asciival < 256 && isdigit(*s);) {
            asciival = asciival * 10 + (*(s++) - '0');
          }
        }
        break;
      }
    }
    buf[index++] = (unsigned char)asciival;
  }
  buf[index] = '\0';
  String result = (char*)buf;
  delete[] buf;
  return result;
}

static void usage() {
  fprintf(stderr, "Usage:tr from to [file]\n"
                  "         from and to can be strings containing normal ascii-characters and/or escape-sequences as described below.\n"
                  "    \\b      backspace        (\\8)                   \n"
                  "    \\t      tab              (\\9)                   \n"
                  "    \\n      newline          (\\10)                  \n"
                  "    \\f      formfeed         (\\12)                  \n"
                  "    \\r      carriage return  (\\13)                  \n"
                  "    \\e      Escape character (\\27)                  \n"
                  "    \\s      space            (\\32)                  \n"
                  "    \\\\      '\\'                                    \n"
                  "    \\^C     C = any letter. Control code ie C-'\\x40'\n"
                  "    \\ddd    number formed as 1-3 decimal digits      \n"
                  "    \\xDD    number formed of 1-2 hex digits          \n"
                  "    \\0DDD   number formed of 1-3 octal digits        \n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  argv++;
  if(!*argv) {
    usage();
  }
  const String from       = escape((unsigned char*)*(argv++));
  const size_t fromLength = from.length();
  const TCHAR *fromStr    = from.cstr();
  if(fromLength == 0) {
    usage();
  }
  if(!*argv) {
    usage();
  }
  const String to       = escape((unsigned char*)*(argv++));
  const size_t toLength = to.length();
  const TCHAR *toStr    = to.cstr();

  try {
    FILE *in = *argv ? FOPEN(*argv, _T("r")) : stdin;
    if(isatty(in)) {
      usage();
    }
    setvbuf(in    ,nullptr,_IOFBF,0x10000);
    setvbuf(stdout,nullptr,_IOFBF,0x10000);
    setFileMode(in    , _O_BINARY);
    setFileMode(stdout, _O_BINARY);
    int ch;
    int index = 0;
    while((ch = getc(in)) != EOF) {
      if(ch == fromStr[index]) {
        index++;
        if(index == fromLength) {
          for(size_t i = 0; i < toLength; i++) {
            putc(toStr[i],stdout);
          }
          index = 0;
        }
      } else {
        if(index > 0) {
          for(int i = 0; i < index; i++) {
            putc(fromStr[i],stdout);
          }
          index = 0;
        }
        putc(ch,stdout);
      }
    }
    if(index > 0) {
      for(int i = 0; i < index; i++) {
        putc(fromStr[i],stdout);
      }
      index = 0;
    }
    fflush(stdout);
    if(in != stdin) {
      fclose(in);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("tr:%s\n"), e.what());
    return -1;
  }

  return 0;
}
