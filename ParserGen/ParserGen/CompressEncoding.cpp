#include "stdafx.h"
#include "CompressEncoding.h"

#if defined(_DEBUG)
static inline void checkMax15Bits(const TCHAR *method, int v, const TCHAR *varName) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("%s:Value %s=(%#08x) cannot be contained in 15 bits (15,16)"), method, varName, v);
  }
}

static inline void checkCodeBits(const TCHAR *method, UINT v, const TCHAR *varName) {
  if(((v >> 15) & 3) != 0) { // Same code as ParserTables::getActMethodCode
    throwException(_T("%s:Encoded variable %s has value %#08x, with non-zero value in encoding bits"), method, varName, v);
  }
}
#define CHECKMAX15BITS(v) checkMax15Bits(__TFUNCTION__,v,_T(#v))
#define CHECKCODEBITS( v) checkCodeBits( __TFUNCTION__,v,_T(#v))

#else

static inline void checkMax15Bits(int v) {
  if((v & 0xffff8000) != 0) {
    throwException(_T("Value %#08x cannot be contained in 15 bits"), v);
  }
}

static inline void checkCodeBits(UINT v) {
  if(((v >> 15) & 3) != 0) { // Same code as ParserTables::getActMethodCode
    throwException(_T("Encoded value %#08x has non-zero value in encoding bits (15,16)"), v);
  }
}
#define CHECKMAX15BITS(v) checkMax15Bits(v)
#define CHECKCODEBITS( v) checkCodeBits( v)

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
  _T("TermList"  )
 ,_T("SplitNode" )
 ,_T("OneItem"   )
 ,_T("TermSet"   )
};

const TCHAR *compressMethodToString(CompressionMethod method) {
  assert(method < ARRAYSIZE(methodNames));
  return methodNames[method];
}
