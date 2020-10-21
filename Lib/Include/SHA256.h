#pragma once

/**
 * SHA-256 implemented according to the specification:
 * http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 */

#include "CompactArray.h"
#include <ByteMemoryStream.h>

class Block512Bit {
private:
  UINT32 m_v[16];
public:
  inline const UINT32 &operator[](UINT index) const {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  inline UINT32 &operator[](UINT index) {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
};

class SHA256HashCode {
private:
  UINT32 m_v[8];
  inline void init(const UINT32 *v) {
    memcpy(m_v, v, sizeof(m_v));
  }
  inline void reset() {
    memset(m_v, 0, sizeof(m_v));
  }
public:
  inline SHA256HashCode() {
    reset();
  }
  inline SHA256HashCode(const SHA256HashCode &src) {
    init(src.m_v);
  }
  inline SHA256HashCode(const UINT32 *v) {
    init(v);
  }
  inline void clear(const UINT32 *v = nullptr) {
    if(v) init(v); else reset();
  }
  inline const UINT32 &operator[](UINT index) const {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  inline UINT32 &operator[](UINT index) {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  String toString(bool upper = true) const;
};

std::ostream  &operator<<(std::ostream  &out, const SHA256HashCode &code);
std::wostream &operator<<(std::wostream &out, const SHA256HashCode &code);

class SHA256 {
private:
  static const UINT32 s_K[64];    // Constants used in hash algorithm
  static const UINT32 s_hash0[8]; // Initial value of m_hashedMsg, before shuffling bits with 512-bit blocks
public:
  static SHA256HashCode &getHashCode(SHA256HashCode &dst, ByteInputStream &s);
  static SHA256HashCode &getHashCode(SHA256HashCode &dst, const ByteArray &a) {
    return getHashCode(dst, ByteMemoryInputStream(a));
  }
};
