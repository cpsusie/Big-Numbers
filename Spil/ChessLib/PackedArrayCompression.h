#pragma once

#include <ByteStream.h>
#include <PackedArray.h>
#include <Compressor.h>

class PackedArrayInputStream : public ResetableByteInputStream {
private:
  const PackedArray &m_src;
  size_t             m_currentIndex, m_size;
public:
  PackedArrayInputStream(const PackedArray &src) : m_src(src), m_size(src.size()) {
    reset();
  }
  void reset() {
    m_currentIndex = 0;
  }
  intptr_t getBytes(BYTE *dst, size_t n) { // Must return the actual number of bytes read.
    BYTE *p;
    for (p = dst; n--;) {
      const int b = getByte();
      if(b == EOF) break;
      *(p++) = (BYTE)b;
    }
    return p - dst;
  }
  int getByte() {                    // Must return EOF when end-of-stream
    if(m_currentIndex < m_size) {
      return m_src.get(m_currentIndex++);
    }
    return EOF;
  }
};

class PackedArrayOutputStream : public ByteOutputStream {
private:
  PackedArray &m_dst;
public:
  PackedArrayOutputStream(PackedArray &dst) : m_dst(dst) {
  }
  void putBytes(const BYTE *src, size_t n) {
    while (n--) putByte(*(src++));
  }
  void putByte(BYTE b) {
    m_dst.add(b);
  }
};

class PackedArrayCompressor {
private:
  Compressor &m_compressor;
public:
  PackedArrayCompressor(Compressor &compressor) : m_compressor(compressor) {
  }
  String compress(const PackedArray &src, ByteOutputStream &out) {
    const BYTE   bpi = src.getBitsPerItem();
    const UINT64 size = src.size();
    out.putBytes(&bpi, sizeof(bpi));
    out.putBytes((BYTE*)&size, sizeof(size));
    m_compressor.compress(PackedArrayInputStream(src), out);
    return m_compressor.getRatios();
  }

  String expand(ByteInputStream &in, PackedArray &dst) {
    BYTE  bpi;
    INT64 size;
    in.getBytesForced(&bpi        , sizeof(bpi));
    in.getBytesForced((BYTE*)&size, sizeof(size));
    if(bpi != dst.getBitsPerItem()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("Array.bitPerItem=%d, read BPI=%d")
                                   ,dst.getBitsPerItem(), bpi);
    }
    dst.setCapacity(size);
    m_compressor.expand(in, PackedArrayOutputStream(dst));
    return m_compressor.getRatios();
  }
};
