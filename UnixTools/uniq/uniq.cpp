#include "stdafx.h"
#include <MyUtil.h>
#include <io.h>
#include <Comparator.h>

#define LINESIZE 4000

static void removeDuplicates(FILE *f, Comparator<TCHAR*> &comp) {
  TCHAR buf1[LINESIZE],buf2[LINESIZE];
  TCHAR *current = buf1;
  TCHAR *last    = buf2;

  last = FGETS(last,LINESIZE,f);
  while(current = FGETS(current,LINESIZE,f)) {
    if(comp.compare(last,current) != 0) {
      _tprintf(_T("%s\n"), last);
    }
    std::swap(last,current);
  }
  if(last) {
    _tprintf(_T("%s\n"), last);
  }
}

static void countDuplicates(FILE *f, Comparator<TCHAR*> &comp) {
  TCHAR buf1[LINESIZE],buf2[LINESIZE];
  TCHAR *current = buf1;
  TCHAR *last    = buf2;
  ULONG  count;

  if(last = FGETS(last,LINESIZE,f)) {
    count = 1;
  } else {
    count = 0;
  }

  while(current = FGETS(current,LINESIZE,f)) {
    if(comp.compare(last,current) == 0) {
      count++;
    } else {
      _tprintf(_T("%lu %s\n"), count, last);
      count = 1;
      std::swap(last,current);
    }
  }
  if(count > 0) {
    _tprintf(_T("%lu %s\n"), count, last);
  }
}

static void usage() {
  _ftprintf(stderr,_T("Usage:uniq [-ci] [file]\n"
                      "      -c: Count number of duplicate consecutive lines.\n"
                      "      -i: Ignore case when comparing lines.\n")
           );
  exit(-1);
}

typedef enum {
  CountDuplicates ,
  RemoveDuplicates
} Command;

class StringComparator : public Comparator<TCHAR *> {
public:
  bool m_ignorecase;
  int compare(TCHAR * const &s1, TCHAR * const &s2);
  StringComparator() {
    m_ignorecase = false;
  }
  AbstractComparator *clone() const {
    return new StringComparator(*this);
  }
};

int StringComparator::compare(TCHAR * const &s1, TCHAR * const &s2) {
  return m_ignorecase ? streicmp(s1,s2) : _tcscmp(s1,s2);
}

int main(int argc, char **argv) {
  char *cp;
  Command command = RemoveDuplicates;
  StringComparator compare;

  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'c': command = CountDuplicates;   continue;
      case 'i': compare.m_ignorecase = true; continue;
      default : usage();
      }
      break;
    }
  }

  try {
    FILE *in = *argv ? FOPEN(*argv, _T("r")) : stdin;
    if(isatty(in)) {
      usage();
    }
    setvbuf(in,NULL,_IOFBF,0x10000);
    switch(command) {
    case RemoveDuplicates  : 
      removeDuplicates(in,compare);
      break;
    case CountDuplicates:
      countDuplicates(in,compare);
      break;
    }
    if(in != stdin) {
      fclose(in);
    }
  } catch(Exception e) {
    _ftprintf(stderr, _T("uniq:%s\n"), e.what());
    return -1;
  }
  return 0;
}
