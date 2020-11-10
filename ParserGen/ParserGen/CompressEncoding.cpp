#include "stdafx.h"
#include "CompressEncoding.h"

void checkMax15Bits(const TCHAR *method, int line, int v, const TCHAR *varName) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("%s:(%d):value %s=(%08x) cannot be contained in 15 bits"), method, line, varName, v);
  }
}

#if defined(_DEBUG)
void checkCodeBits(const TCHAR *method, UINT v, const TCHAR *varName) {
  if(((v >> 15) & 3) != 0) { // Same code as ParserTables::getActMethodCode
    MessageBox(NULL, format(_T("%s:Encoded variable %s has value %08x, with non-zero value in encoding bits")
              ,method, varName, v).cstr()
              ,_T("Error")
              ,MB_ICONSTOP
              );
    exit(-1);
  }
}
#endif // _DEBUG

UINT encodeValue(CompressionMethod method, int highEnd, int lowEnd) {
  CHECKMAX15BITS(abs(highEnd));
  CHECKMAX15BITS(abs(lowEnd));
  const UINT v = (highEnd << 17) | lowEnd;
  CHECKCODEBITS(v);
  return (v | encodeCompressMethod(method));
}

String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd) {
  return format(_T("0x%08x"), encodeValue(method, highEnd, lowEnd));
}

static const TCHAR *methodNames[] = {
  _T("TermList"    )
 ,_T("SplitNode"   )
 ,_T("OneItem"     )
 ,_T("TermSet"     )
};

const TCHAR *compressMethodToString(CompressionMethod method) {
  assert(method < ARRAYSIZE(methodNames));
  return methodNames[method];
}
