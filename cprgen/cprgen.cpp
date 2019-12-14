#include "stdafx.h"
#include <Date.h>
#include <InputValue.h>

static bool CPRCheck(TCHAR *cpr) {
  const int fac[] = { 4,3,2,7,6,5,4,3,2,1 };

  int sum = 0;
  for(int i = 0; i < 10; i++) {
    sum += fac[i] * (cpr[i] - _T('0'));
  }
  return sum % 11 == 0;
}

static TCHAR *CPRGenNext(TCHAR *dst) {
  TCHAR tmp[20];
  int c;
  if(_stscanf(dst+6, _T("%d"), &c) != 1) {
    c = 0;
  } else {
    c++;
    dst[6] = '\0';
  }
  int i;
  for(i = c; i <= 9999; i++) {
    _stprintf(tmp, _T("%s%04d"), dst, i);
    if(CPRCheck(tmp)) {
      _tcscpy(dst, tmp);
      return dst;
    }
  }
  if(i > 9999) {
    return NULL;
  }
  return dst;
}

#define PRINTODD  0x01
#define PRINTEVEN 0x02
#define PRINTALL  (PRINTODD | PRINTEVEN)

class CprNumberArray : public StringArray {
private:
  Date m_date;
public:
  CprNumberArray(const Date &date);
  void print(UINT count, UINT flags = PRINTALL, FILE *f = stdout);
};

CprNumberArray::CprNumberArray(const Date &date) : m_date(date) {
  TCHAR str[20];
  m_date.tostr(str, _T("ddMMyy"));
  for(;;) {
    if(CPRGenNext(str) == NULL) {
      break;
    };
    add(str);
  }
}

void CprNumberArray::print(UINT count, UINT flags, FILE *f) {
  if(count > size()) {
    count = (UINT)size();
  }
  int printed = 0;
  for(UINT i = 0; i < count; i++) {
    const String &s = (*this)[i];
    if(flags != PRINTALL) {
      const bool isOdd = (s[s.length()-1] - '0')&1;
      if(((flags & PRINTODD) != 0) != isOdd) {
        continue;
      }
    }
    _ftprintf(f, _T("%s%c"),s.cstr(),((printed%10) == 9)?'\n':' ');
    printed++;
  }
  if(printed%10) {
    _ftprintf(f, _T("\n"));
  }
  _ftprintf(f, _T("Total antal cpr numre for %s:%zd\n"), m_date.toString().cstr(), size());
}

static Date getDate(UINT n) {
  const UINT n0 = n;
  int yy = Date::adjustYear100(n % 100);
  n /= 100;
  int mm = n % 100;
  n /= 100;
  int dd = n;
  n /= 100;
  if(n) {
    throwException(_T("Too many digits in number (%u)"), n0);
  }
  return Date(dd, mm, yy);
}

static void usage() {
  fprintf(stderr, "Usage:cprgen [-dDate] [-a] [-nCount] [-e|-o]\n"
                  "             -dDate : Print cpr numbers for the specified date.\n"
                  "                      Date format is ddmmyy. Default date is today\n"
                  "             -a     : Print all cpr numbers for the actual date\n"
                  "             -nCount: Print only Count cpr numbers. Default Count is 200\n"
                  "             -e     : Print only even cpr numbers (women)\n"
                  "             -o     : Print onle odd cpr numbers (men)\n"
                  "             -i     : Interactive mode. Repeatedly ask for date, to print cpr numbers for\n"
         );
  exit(-1);
}

int main(int argc, char **argv) {
  char *cp;
  bool printAll    = false;
  bool printEven   = true;
  bool printOdd    = true;
  bool interactive = false;
  int  count       = 200;
  Date date;

  for(argv++; *argv && (*(cp = *argv) == '-'); argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case 'a':
        printAll = true;
        continue;
      case 'n':
        if((sscanf(cp+1,"%u", &count) != 1) || (count < 1)) {
          usage();
        }
        break;
      case 'd':
        try {
          UINT n;
          if(sscanf(cp+1, "%u", &n) != 1) {
            usage();
          }
          date = getDate(n);
        } catch(Exception e) {
          _ftprintf(stderr, _T("%s\n"), e.what());
          usage();
        }
        break;
      case 'e':
        if(!printEven) {
          usage();
        }
        printOdd = false;
        continue;
      case 'o':
        if(!printOdd) {
          usage();
        }
        printEven = false;
        continue;
      case 'i':
        interactive = true;
        continue;
      default:
        usage();
      }
      break;
    }
  }

  if(interactive) {
    for(;;) {
      try {
        const int n = inputValue<int>(_T("Indtast de første 6 cifre af cpr-nummeret:"));
        date = getDate(n);
        CprNumberArray list(date);
        UINT flags = (printEven?PRINTEVEN:0) | (printOdd?PRINTODD:0);
        list.print(printAll?-1:count, flags);
      } catch(Exception e) {
        _tprintf(_T("%s\n"), e.what());
      }
    }
  } else {
    CprNumberArray list(date);
    UINT flags = (printEven?PRINTEVEN:0) | (printOdd?PRINTODD:0);
    list.print(printAll?-1:count, flags);
  }
  return 0;
}
