#include "pch.h"
#include <MyUtil.h>
#include <CompactArray.h>
#include <String.h>
#include <io.h>

#ifdef STRDUP
#undef STRDUP
#endif
/*
#define TEST define this macro to get a standalone program
*/

#define STRLEN(str)      ((sizeof(Ctype)==1) ?          strlen((char*)str)             : wcslen(         (wchar_t*)str))
#define STRCPY(dst, src) ((sizeof(Ctype)==1) ? (Ctype*) strcpy((char*)dst, (char*)src) : (Ctype*) wcscpy((wchar_t*)dst, (wchar_t*)src))
#define STRREV(str)      ((sizeof(Ctype)==1) ? (Ctype*)_strrev((char*)str)             : (Ctype*)_wcsrev((wchar_t*)str))
#define STRCHR(str,ch)   ((sizeof(Ctype)==1) ? (Ctype*) strchr((char*)str, (char)ch)   : (Ctype*) wcschr((wchar_t*)str, (wchar_t)ch ))
#define STRDUP(str)      ((sizeof(Ctype)==1) ? (Ctype*)_strdup((char*)str)             : (Ctype*)_wcsdup((wchar_t*)str))

template<typename Ctype> void expandArgv(int &argc, Ctype **&argv) {
  Ctype path[256],*last;
  CompactArray<Ctype*> list;

  for(Ctype **cpp = argv; *cpp; cpp++) {
    if(STRCHR(*cpp,'*')==NULL && STRCHR(*cpp,'?')==NULL) {
      list.add(STRDUP(*cpp));
    } else { // expand it
      STRCPY(path,*cpp);
      if(last = STRCHR(path, '\\')) {
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
          STRCPY(last, finddata.name);
          list.add(STRDUP(path));
        }
      }
      else {
        struct _wfinddata_t finddata;
        for(done = 0, namecurs = _wfindfirst((wchar_t*)*cpp, &finddata);
            namecurs >= 0 && done == 0; done = _wfindnext(namecurs, &finddata)) {
          STRCPY(last, finddata.name);
          list.add(STRDUP(path));
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
  expandArgv<wchar_t>(argc, argv);
}

void argvExpand(int &argc, char **&argv) {
  expandArgv<char>(argc, argv);
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
