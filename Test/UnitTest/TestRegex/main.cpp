#include "stdafx.h"
#include <MyUtil.h>
#include <Regex.h>
#include "TestRegex.h"

static void usage() {
  const String errMsg = "Usage testregex [-x] [-c]\n"
                        " -x  : runs testscript\n"
                        " -c  : runs interactive as case-insensitive\n"
                        " none: runs interactive case-sensitive\n";

  _ftprintf(stderr,_T("%s"), errMsg.cstr());
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  TCHAR *cp;
  bool ignorecase = false;

  for(argv++; *argv && *(cp=*argv) == '-'; argv++) {
    for(cp++;*cp;cp++)
      switch(*cp) {
      case 'c':
        ignorecase = true;
        break;
      case 'x':
        try {
          testRegex();
        } catch(Exception e) {
          _tprintf(_T("error:%s\n"),e.what());
          return -1;
        }
        return 0;

      default:
        usage();
    }
  }
  TCHAR target[256];
  TCHAR pat[64];
  Regex reg;

  for(;;) {
    for(;;) {
      _tprintf(_T("Enter pattern ('!'=quit):"));
      GETS(pat);
//      strcpy(pat,"\\(\\(\\`.w.*\\)\\|\\(.*w.\\'\\)\\)");
      _tprintf(_T("pattern:<%s>\n"), pat);
      if(pat[0] == '!') {
        return 0;
      }
      try {
        reg.compilePattern(pat, ignorecase ? String::upperCaseTranslate : NULL);
      } catch(Exception e) {
        _tprintf(_T("reg.compile(\"%s\"):%s\n"), pat, e.what());
        continue;
      }
      break;
    }
#ifdef _DEBUG
    reg.dump();
#endif
    for(;;) {
      _tprintf(_T("Enter target ('!'=new pattern):"));
      GETS(target);
      if(target[0] == '!') {
        break;
      }
      RegexRegisters registers;
//      bool res = reg.match(target, &registers );
      int execResult   = reg.search(target, false, -1, &registers);
      int resultLength = reg.getResultLength();
      _tprintf(_T("reg.search(\"%s\") : %d resultlen:%d\n"), target, execResult,resultLength);
      for(int i = 0; i < RE_NREGS; i++) {
        int start = registers.start[i];
        int end   = registers.end[i];
        int len   = end - start;
        if(len > 0) {
          _tprintf(_T("register[%d]:<%-*.*s>\n"), i, len, len, target+start);
        }
      }
      bool matchResult = reg.match(target);
      _tprintf(_T("reg.match(\"%s\"):%s\n"), target, matchResult?_T("true"):_T("false"));
    }
  }
  return 0;
}
