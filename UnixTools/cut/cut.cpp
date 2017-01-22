#include "stdafx.h"
#include <MyUtil.h>
#include <String.h>
#include <Array.h>
#include <Tokenizer.h>
#include <io.h>

#define MAXLEN 4096

class Interval {
public:
  UINT l,r;
  Interval(TCHAR *s, int maxpos);
};

Interval::Interval(TCHAR *s, int maxpos) {
  TCHAR *m = _tcschr(s,'-');

  if(m == NULL) {
    if(_stscanf(s, _T("%u"), &l) == 1) {
      r = l;
    } else {
      throwException(_T("Illegal interval:'%s'"), s);
    }
  } else {
    *m = _T('\0');

    if(_stscanf(s, _T("%u"), &l) < 1) {
      l = 1;
    }
    if(_stscanf(m+1, _T("%u"), &r) < 1) {
      r = maxpos;
    }
    *m = _T('-');
  }

  if((l < 1) || (maxpos > 0 && (int)r > maxpos) || (l > r)) {
    throwException(_T("Illegal interval:'%s'"), s);
  }

  l--;
}

static Array<Interval> scanIntervals(const char *cp, int maxpos) {
  Array<Interval> result;
  for(Tokenizer tok(cp,_T(",")); tok.hasNext(); ) {
    result.add(Interval(tok.next().cstr(), maxpos));
  }
  return result;
}

static void usage() {
  _ftprintf(stderr,_T("Usage: cut [-l] -clist [file]\n"));
  _ftprintf(stderr,_T("       cut [-l] -flist [-ddelimiters] [file]\n"));
  exit(-1);
}

int main(int argc, char **argv) {
  char  *cp;
  TCHAR *format;
  bool   fieldSelection = false;
  bool   linefeed       = false;
  String delimiters     = _T(" \t");
  Array<Interval> intervalList;

  try {
    for(argv++;*argv && *(cp = *argv) == '-';argv++) {
      for(cp++; *cp; cp++) {
        switch(*cp) {
        case 'f':
        case 'c':
          fieldSelection = (*cp == 'f');
          intervalList = scanIntervals(cp+1, fieldSelection ? -1 : MAXLEN);
          break;
        case 'l':
          linefeed = true;
          continue;
        case 'd':
          delimiters = cp+1;
          break;
        default:
          usage();
        }
        break;
      }
    }

    if(intervalList.size() == 0) {
      usage();
    }

    FILE *f = *argv ? FOPEN(*argv,"r") : stdin;
    if(isatty(f)) {
      usage();
    }

    format = linefeed ? _T("%s\n") : _T("%s");

    TCHAR line[MAXLEN];
    while(FGETS(line, ARRAYSIZE(line), f)) {
      if(fieldSelection) {
        Array<String> fieldArray;
        Tokenizer tok(line, delimiters);
        while(tok.hasNext()) {
          fieldArray.add(tok.next());
        }
        if(linefeed) {
          for(size_t i = 0; i < intervalList.size();i++) {
            const Interval &interval = intervalList[i];
            for(size_t j = interval.l; (j < fieldArray.size()) && (j < interval.r); j++) {
              _tprintf(_T("%s\n"), fieldArray[j].cstr());
            }
          }
        } else {
          TCHAR separator = 0;
          for(size_t i = 0; i < intervalList.size();i++) {
            const Interval &interval = intervalList[i];
            for(size_t j = interval.l; (j < fieldArray.size()) && (j < interval.r); j++) {
              if(separator) {
                putchar(separator);
              } else {
                separator = delimiters[0];
              }
              _tprintf(_T("%s"), fieldArray[j].cstr());
            }
          }
        }
      } else {
        const size_t n = _tcsclen(line);
        for(size_t i = 0; i < intervalList.size();i++) {
          const Interval &interval = intervalList[i];
          if(interval.l >= n) {
            continue;
          }
          TCHAR &ch = line[interval.r];
          TCHAR save = ch;
          ch = _T('\0');
          _tprintf(format, line + interval.l);
          ch = save;
        }
      }
      if(!linefeed) {
        _tprintf(_T("\n"));
      }
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("cut:%s\n"), e.what());
    return -1;
  }
  return 0;
}
