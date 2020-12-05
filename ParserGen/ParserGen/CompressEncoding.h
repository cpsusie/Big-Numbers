#pragma once

#include "BitSetInterval.h"

typedef enum {
  CompCodeBinSearch = AbstractParserTables::CompCodeBinSearch
 ,CompCodeSplitNode = AbstractParserTables::CompCodeSplitNode
 ,CompCodeImmediate = AbstractParserTables::CompCodeImmediate
 ,CompCodeBitSet    = AbstractParserTables::CompCodeBitSet
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
