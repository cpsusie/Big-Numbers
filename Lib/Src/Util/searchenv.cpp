#include "pch.h"
#include <io.h>
#include <FileNameSplitter.h>
#include <Tokenizer.h>

TCHAR *searchenv(TCHAR *dst, const TCHAR *fileName, const TCHAR *envName) {
  if(_taccess(fileName, 0) == 0) {
    return _tcscpy(dst, fileName); // use working directory
  } else {
    TCHAR *env = _tgetenv(envName);
    if(env == nullptr) {
      return nullptr; // not found
    }
    for(Tokenizer tok(env, _T(";")); tok.hasNext();) {
      String s = FileNameSplitter::getChildName(tok.next(),fileName);
      if(_taccess(s.cstr(),0) == 0) {
        return _tcscpy(dst,s.cstr());
      }
    }
    return EMPTYSTRING; // not found
  }
}

String searchenv(const String &fileName, const String &envName) {
  TCHAR tmp[256];
  return searchenv(tmp,fileName.cstr(), envName.cstr());
}
