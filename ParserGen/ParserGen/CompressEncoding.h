#pragma once

#include "BitSetInterval.h"

typedef enum {
  CompCodeBinSearch = SearchFunctions::CompCodeBinSearch
 ,CompCodeSplitNode = SearchFunctions::CompCodeSplitNode
 ,CompCodeImmediate = SearchFunctions::CompCodeImmediate
 ,CompCodeBitSet    = SearchFunctions::CompCodeBitSet
} CompressionMethod;

const TCHAR *compressMethodToString(CompressionMethod method);

inline UINT encodeCompressMethod(CompressionMethod method) {
  return (((UINT)method) << 15);
}

UINT   encodeValue(     CompressionMethod method, int highEnd, int lowEnd);

#define TEST_ENCODING
#if defined(TEST_ENCODING)

String tencodeMacroValue(CompressionMethod method, int highEnd, int lowEnd, const TCHAR *func, int line, const TCHAR *hstr,const TCHAR *lstr);
#define encodeMacroValue(method, highEnd, lowEnd) \
  tencodeMacroValue(method, highEnd, lowEnd, __TFUNCTION__, __LINE__, _T(#highEnd), _T(#lowEnd))

#else
String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd);
#endif
