#pragma once

#include "ByteStream.h"
#include "Tcp.h"
#include "BitSet.h"


#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__

template<typename T, typename C> intptr_t readItemArrayBE(ByteInputStream &s, T *buf, size_t n) {
  s.getBytesForced((BYTE*)buf, n * sizeof(T));
  const T *end = buf + n;
  for(T *p = buf; p < end; p++) *p = C::ntoh(*p);
  return n;
}

template<typename T, typename C> void writeItemArrayBE(ByteOutputStream &s, const T *buf, size_t n) {
  T tmp[4096], *dst = tmp, *endtmp = dst+ARRAYSIZE(tmp);
  const T *endbuf = buf + n;
  while(buf < endbuf) {
    *(dst++) = C::hton(*(buf++));
    if(dst == endtmp) {
      s.putBytes((BYTE*)tmp, (dst-tmp) * sizeof(T));
      dst = tmp;
    }
  }
  if(dst > tmp) {
    s.putBytes((BYTE*)tmp, (dst-tmp) * sizeof(T));
  }
}

class BEShort {
public:
  static inline USHORT hton(USHORT n) { return htons(n); }
  static inline USHORT ntoh(USHORT n) { return ntohs(n); }
};

class BELong {
public:
  static inline ULONG hton(ULONG n) { return htonl(n); }
  static inline ULONG ntoh(ULONG n) { return ntohl(n); }
};

class BELongLong {
public:
  static inline ULONGLONG hton(ULONGLONG n) { return htonll(n); }
  static inline ULONGLONG ntoh(ULONGLONG n) { return ntohll(n); }
};

#endif

class BigEndianOutputStream : public ByteOutputStream {
private:
  ByteOutputStream &m_out;
public:
  BigEndianOutputStream(ByteOutputStream &out) : m_out(out) {
  }
  void putByte(BYTE b) {
    m_out.putByte(b);
  }
  void putBytes(const BYTE *src, size_t n) {
    m_out.putBytes(src, n);
  }
  inline void putShorts(USHORT *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<USHORT, BEShort>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(USHORT));
#endif
  }
  inline void putShorts(SHORT *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<SHORT, BEShort>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(SHORT));
#endif
  }
  inline void putLongs(ULONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<ULONG, BELong>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(ULONG));
#endif
  }
  inline void putLongs(LONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<LONG, BELong>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(LONG));
#endif
  }
  inline void putLongLongs(ULONGLONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<ULONGLONG, BELongLong>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(ULONGLONG));
#endif
  }
  inline void putLongLongs(LONGLONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    writeItemArrayBE<LONGLONG, BELongLong>(m_out, buf, n);
#else
    m_out.putBytes((BYTE*)buf, n*sizeof(LONGLONG));
#endif
  }

  inline BigEndianOutputStream &operator<<(bool b) {
    m_out.putByte(b?1:0);
    return *this;
  }
  inline BigEndianOutputStream &operator<<(BYTE n) {
    m_out.putByte(n);
    return *this;
  }
  inline BigEndianOutputStream &operator<<(char n) {
    m_out.putByte(n);
    return *this;
  }
  inline BigEndianOutputStream &operator<<(USHORT n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htons(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  inline BigEndianOutputStream &operator<<(SHORT n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htons(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  inline BigEndianOutputStream &operator<<(ULONG n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htonl(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  inline BigEndianOutputStream &operator<<(LONG n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htonl(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  inline BigEndianOutputStream &operator<<(UINT64 n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htonll(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  inline BigEndianOutputStream &operator<<(INT64 n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = htonll(n);
#endif
    m_out.putBytes((BYTE*)&n, sizeof(n));
    return *this;
  }
  BigEndianOutputStream &operator<<(const BitSet &s);
};

class BigEndianInputStream : public ByteInputStream {
private:
  ByteInputStream &m_in;
public:
  BigEndianInputStream(ByteInputStream &in) : m_in(in) {
  }
  int getByte() {
    return m_in.getByte();
  }
  intptr_t getBytes(BYTE *dst, size_t n) {
    return m_in.getBytes(dst, n);
  }
  inline void getShorts(USHORT *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<USHORT, BEShort>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(USHORT));
#endif
  }
  inline void getShorts(SHORT *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<SHORT, BEShort>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(SHORT));
#endif
  }
  inline void getLongs(ULONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<ULONG, BELong>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(ULONG));
#endif
  }
  inline void getLongs(LONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<LONG, BELong>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(LONG));
#endif
  }
  inline void getLongLongs(ULONGLONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<ULONGLONG, BELongLong>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(ULONGLONG));
#endif
  }
  inline void getLongLongs(LONGLONG *buf, size_t n) {
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    readItemArrayBE<LONGLONG, BELongLong>(m_in, buf, n);
#else
    m_in.getBytesForced(buf, n*sizeoF(LONGLONG));
#endif
  }

  inline BigEndianInputStream &operator>>(bool &b) {
    BYTE tmp;
    *this >> tmp;
    b = tmp?true:false;
    return *this;
  }

  inline BigEndianInputStream &operator>>(BYTE &n) {
    m_in.getBytesForced(&n, 1);
    return *this;
  }
  inline BigEndianInputStream &operator>>(char &n) {
    m_in.getBytesForced((BYTE*)&n, 1);
    return *this;
  }

  inline BigEndianInputStream &operator>>(USHORT &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohs(n);
#endif
    return *this;
  }
  inline BigEndianInputStream &operator>>(SHORT &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohs(n);
#endif
    return *this;
  }

  inline BigEndianInputStream &operator>>(ULONG &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohl(n);
#endif
    return *this;
  }
  inline BigEndianInputStream &operator>>(LONG &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohl(n);
#endif
    return *this;
  }

  inline BigEndianInputStream &operator>>(UINT64 &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohll(n);
#endif
    return *this;
  }
  inline BigEndianInputStream &operator>>(INT64 &n) {
    m_in.getBytesForced((BYTE*)&n, sizeof(n));
#if __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
    n = ntohll(n);
#endif
    return *this;
  }
  BigEndianInputStream &operator>>(BitSet &s);
};
