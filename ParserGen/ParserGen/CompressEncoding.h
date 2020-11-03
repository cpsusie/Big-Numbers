#pragma once

typedef enum {
  UNCOMPRESSED
 ,SPLITNODECOMPRESSION
 ,ONEITEMCOMPRESSION
 ,REDUCEBYSAMEPRODCOMPRESSION
 ,MAXCOMPRESSIONVALUE = REDUCEBYSAMEPRODCOMPRESSION // we have only 2 bits to encode compressionmethod in parsertables. So keep maxvalue < 4 !!
} CompressionMethod;

const TCHAR *compressMethodToString(CompressionMethod method);

inline UINT encodeCompressMethod(CompressionMethod method) {
  return (((UINT)method) << 15);
}

void checkMax15Bits(const TCHAR *method, int line, int v, const TCHAR *varName);

#define CHECKMAX15BITS(v) checkMax15Bits(__TFUNCTION__,__LINE__,v,_T(#v))

#if defined(_DEBUG)

// check that bits, used for encoding the compression method are all zero, give error-message and exit(-1) if this is not the case
void checkCodeBits( const TCHAR *method, UINT v, const TCHAR *varName);

#define CHECKCODEBITS(v) checkCodeBits(__TFUNCTION__,v,_T(#v))

#else

#define CHECKCODEBITS(v)

#endif // _DEBUG

UINT   encodeValue(     CompressionMethod method, int highEnd, int lowEnd);
String encodeMacroValue(CompressionMethod method, int highEnd, int lowEnd);
