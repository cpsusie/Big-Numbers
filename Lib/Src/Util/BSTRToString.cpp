#include "pch.h"
#include <comutil.h>

String BSTRToString(const BSTR &s) {
  int size = ((_bstr_t) s).length()+1;
  char *tmp = new char[size];
  ::WideCharToMultiByte(CP_ACP,0,s,-1,tmp,size,NULL,NULL);
  String result = tmp;
  delete[] tmp;
  return result;
}
/*
BSTR StringToBSTR(const String &s) {

int MultiByteToWideChar(UINT CodePage,         // code page
  DWORD dwFlags,         // character-type options
  LPCSTR lpMultiByteStr, // string to map
  int cbMultiByte,       // number of bytes in string
  LPWSTR lpWideCharStr,  // wide-character buffer
  int cchWideChar        // size of buffer);
*/