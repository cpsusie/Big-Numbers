/* DO NOT EDIT THIS FILE - it is machine generated */

#line 102 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.y"
#include "stdafx.h"
#include "CppParser.h"

#line 26 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"



#pragma warning(disable : 4060)

int CppParser::reduceAction(unsigned int prod) {
  switch(prod) {
  }
  return 0;
}

#line 400 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.y"
void CppParser::verror(const SourcePosition &pos, const TCHAR *form, va_list argptr) {
  const String tmp = vformat(form,argptr);
  String tmp2 = format(_T("Error in line %d: %s"), pos.getLineNumber(), tmp.cstr() );
  tmp2.replace('\n',' ');
  appendError(_T("%s"),tmp2.cstr());
}

void CppParser::appendError(_In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr,format);
  _vtprintf(format,argptr);
  va_end(argptr);
}
#line 39 "C:\\mytools2015\\ParserGen\\lib\\parsergencpp.par"


