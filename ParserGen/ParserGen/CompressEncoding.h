#pragma once

typedef BYTE CompressionMethod;

const TCHAR *compressMethodToString(CompressionMethod method);

inline UINT encodeCompressMethod(CompressionMethod method) {
  return (((UINT)method) << 15);
}

UINT   encodeValue(     CompressionMethod method, int highEnd, int lowEnd);
String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd);
