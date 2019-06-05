#pragma once

/**
 * SHA-256 implemented according to the specification:
 * http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 */

#include "CompactArray.h"

class Block512Bit {
private:
  UINT32 m_v[16];
public:
  inline const UINT32 &operator[](UINT index) const {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  inline UINT32 &operator[](int index) {
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
  inline void clear(const UINT32 *v = NULL) {
    if(v) init(v); else reset();
  }
  inline const UINT32 &operator[](UINT index) const {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  inline UINT32 &operator[](int index) {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  String toString(bool upper = true) const;
};

std::ostream  &operator<<(std::ostream  &out, const SHA256HashCode &code);
std::wostream &operator<<(std::wostream &out, const SHA256HashCode &code);

class SHA256 {
private:
  static const UINT32         s_K[64];         // Constants used in hash algorithm
  static const UINT32         s_hash0[8];      // Initial value of m_hashedMsg, before shuffling bits with 512-bit blocks (m_msg[0..N-1])
  ByteArray                   m_bytes;         // Plain and padded message bytes
  CompactArray<Block512Bit>   m_msg;           // Message to be hashed
  SHA256HashCode              m_hashedMsg;     // Hashed message
  UINT64                      m_bitCount;      // Message length in bits

  void clear();                                // Clear all working vectors and variables.
  void storeHexStr(   const String    &hexStr);
  void storeByteArray(const ByteArray &a     );

  int    calcPadding() const;                  // Calculate the required padding of the message. Return the required padding.
  void   padBytes();                           // Pad m_bytes according to the specification.
  size_t parseBytes();                         // Parse m_bytes into 512-bit blocks split up into UINT32's. and return number of blocks
  const SHA256HashCode &computeHash();         // Do the bit-shuffling

public:
  SHA256() {
    clear();
  }
  SHA256HashCode &getHashCode(SHA256HashCode &dst, const String    &hexStr);
  SHA256HashCode &getHashCode(SHA256HashCode &dst, const ByteArray &a     );
};
