#include "stdafx.h"
#include <MyUtil.h>
#include <FileNameSplitter.h>
#include <Date.h>
#include "GameTypes.h"

void log(_In_z_ _Printf_format_string_ TCHAR const * const format, ...) {
  FILE *f = fopen(FileNameSplitter(getModuleFileName()).setExtension(_T("log")).getFullPath(),_T("a"));
  if(f == nullptr) {
    return;
  }
  va_list argptr;
  va_start(argptr,format);
  _ftprintf(f,_T("%s:%s\n"),Timestamp::cctime().cstr(),vformat(format,argptr).cstr());
  fclose(f);
}

