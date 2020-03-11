#include "pch.h"
#include <MyUtil.h>
#include <CompactArray.h>
#include <String.h>
#include <io.h>

/*
#define TEST define this macro to get a standalone program
*/

template<typename Ctype> void expandArgv(int &argc, Ctype **&argv) {
  Ctype path[256],*last;
  CompactArray<Ctype*> list;

  for(Ctype **cpp = argv; *cpp; cpp++) {
    if(strChr(*cpp,(Ctype)'*')==NULL && strChr(*cpp, (Ctype)'?')==NULL) {
      list.add(strDup(*cpp));
    } else { // expand it
      strCpy(path,*cpp);
      if(last = strChr(path, (Ctype)'\\')) {
        last++;
      } else if(path[1]==':') {
        last = path+2;
      } else {
        last = path;
      }

      intptr_t namecurs;
      int      done;

      if(sizeof(Ctype) == 1) {
        struct _finddata_t finddata;
        for(done = 0, namecurs = _findfirst((char*)*cpp, &finddata);
          namecurs >= 0 && done == 0; done = _findnext(namecurs, &finddata)) {
          strCpy(last, finddata.name);
          list.add(strDup(path));
        }
      } else {
        struct _wfinddata_t finddata;
        for(done = 0, namecurs = _wfindfirst((wchar_t*)*cpp, &finddata);
            namecurs >= 0 && done == 0; done = _wfindnext(namecurs, &finddata)) {
          strCpy(last, finddata.name);
          list.add(strDup(path));
        }
      }
      if(namecurs>=0) {
        _findclose(namecurs);
      }
    }
  }

  argc = (int)list.size();
  argv = new Ctype*[list.size()+1];
  size_t i;
  for(i = 0; i < list.size(); i++) {
    argv[i] = list[i];
  }
  argv[i] = NULL;
  list.clear();
}

void argvExpand(int &argc, wchar_t **&argv) {
  expandArgv(argc, argv);
}

void argvExpand(int &argc, char **&argv) {
  expandArgv(argc, argv);
}

#ifdef TEST

int _tmain(int argc, TCHAR **argv) {
  argvExpand(argc, argv);

  _tprintf(_T("argc:%d\n"),argc);
  for(;*argv;argv++) {
    _tprintf(_T("%s\n"),*argv);
  }
  return(0);
}
#endif
