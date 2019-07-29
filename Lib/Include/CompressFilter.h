#pragma once

#include "ByteArray.h"

typedef enum {
  DEFAULT_COMPRESSION    = -1
 ,COMPRESSION_BESTSPEED  = 1
 ,COMPRESSION_LEVEL1     = COMPRESSION_BESTSPEED
 ,COMPRESSION_LEVEL2
 ,COMPRESSION_LEVEL3
 ,COMPRESSION_LEVEL4
 ,COMPRESSION_LEVEL5
 ,COMPRESSION_LEVEL6
 ,COMPRESSION_LEVEL7
 ,COMPRESSION_LEVEL8
 ,COMPRESSION_LEVEL9
 ,COMPRESSION_BESTSPACE  = COMPRESSION_LEVEL9
} CompressionLevel;

class CompressFilter : public ByteOutputStream {
private:
  ByteOutputStream &m_dst;
  ByteArray         m_inputBuffer;
  ByteArray         m_outputBuffer;
  BYTE             *m_buffer;   // Temporary buffer to receeive the compressed data
  void             *m_zStreamp; // Actually z_stream*. Need zlib.h to get definition right. See CompressFilter.cpp

  CompressFilter(const CompressFilter &src);            // Not defined. Class not cloneable
  CompressFilter &operator=(const CompressFilter &src); // Not defined. Class not cloneable

  void flushInput();
  void flushOutput();
  void getCompressedData();
  void finalFlush();
  void setAvailableOut();
public:
  CompressFilter(ByteOutputStream &dst, CompressionLevel level = DEFAULT_COMPRESSION);
  virtual ~CompressFilter();

  void putBytes(const BYTE *src, size_t n);
  void putByte(BYTE b);

  void flush();
};

class DecompressFilter : public ByteInputStream {
private:
  ByteInputStream  &m_src;
  ByteArray         m_inputBuffer;
  ByteArray         m_outputBuffer;
  intptr_t          m_pos;
  BYTE             *m_buffer;
  void             *m_zStreamp; // Actually z_stream*. Need zlib.h to get definition right. See DecompressFilter.cpp
  bool              m_eos, m_eoz;

  DecompressFilter(const DecompressFilter &src);            // Not defined. Class not cloneable
  DecompressFilter &operator=(const DecompressFilter &src); // Not defined. Class not cloneable

  void fillInputBuffer();
  void fillOutputBuffer();
  void setAvailableOut();
  void decompress();
  void getDecompressedData();
public:
  DecompressFilter(ByteInputStream &src);
  virtual ~DecompressFilter();
  intptr_t getBytes(BYTE *dst, size_t n);
  int getByte();
};
