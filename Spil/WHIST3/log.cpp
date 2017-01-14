#include "stdafx.h"
#include <MyUtil.h>
#include <Date.h>
#include "GameTypes.h"

void log(const TCHAR *format, ...) {
  FILE *f = fopen(FileNameSplitter(getModuleFileName()).setExtension(_T("log")).getFullPath(),_T("a"));
  if(f == NULL)
    return;
  va_list argptr;
  va_start(argptr,format);
  _ftprintf(f,_T("%s:%s\n"),Timestamp::cctime().cstr(),vformat(format,argptr).cstr());
  fclose(f);
}

