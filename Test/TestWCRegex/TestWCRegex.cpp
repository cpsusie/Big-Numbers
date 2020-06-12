#include "stdafx.h"
#include <MyUtil.h>
#include <WildCardRegex.h>
#include <Scandir.h>

int _tmain(int argc, TCHAR **argv) {
  *argv++;
  TCHAR **origargv = argv;

  argvExpand(argc,argv);
  _tprintf(_T("Found by argvExpand:\n"));
  while(*argv) {
    _tprintf(_T("  <%s>\n"), *argv++);
  }

  WildCardRegex wc((const TCHAR **)origargv);
#if defined(_DEBUG)
  _tprintf(_T("wc.dump:\n%s"), wc.toString().cstr());
#endif
  DirList files = scandir(_T("*.*"));
  _tprintf(_T("Found by WildCardRegex:\n"));
  for(size_t i = 0; i < files.size(); i++) {
    if(wc.match(files[i].name)) {
      _tprintf(_T("  <%s>\n"), files[i].name);
    }
  }
  return 0;
}
