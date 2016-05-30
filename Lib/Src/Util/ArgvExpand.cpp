#include "pch.h"
#include <String.h>
#include <io.h>

/*
#define TEST define this macro to get a standalone program
*/

void argvExpand(int &argc, TCHAR **&argv) {
  TCHAR   path[256],*last;
  StringArray list;

  for(TCHAR **cpp = argv; *cpp; cpp++) {
    if(_tcschr(*cpp,_T('*'))==NULL && _tcschr(*cpp,_T('?'))==NULL) {
      list.add(*cpp);
    } else { // expand it
      _tcscpy(path,*cpp);
      if(last = _tcsrchr(path, _T('\\'))) {
        last++; 
      } else if(path[1]==_T(':')) {
        last = path+2;
      } else {
        last = path;
      }

      long    namecurs;
      int     done;
#ifdef UNICODE
      struct _wfinddata_t finddata;
#else
      struct _finddata_t finddata;
#endif
      for(done = 0, namecurs = _tfindfirst(*cpp, &finddata);
          namecurs >= 0 && done == 0; done = _tfindnext(namecurs, &finddata)) {
        _tcscpy(last, finddata.name);
        list.add(path);
      }
      if(namecurs>=0) {
        _findclose(namecurs);
      }
    }
  }

  argc = list.size();
  argv = new TCHAR*[list.size()+1];
  int i;
  for(i = 0; i < list.size(); i++) {
    argv[i] = STRDUP(list[i].cstr());
  }
  argv[i] = NULL;
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
