/* MD5.H - header file for MD5C.CPP
 *
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 *
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 *
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 *
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 *
 *
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

#pragma once

#include <ByteMemoryStream.h>

class MD5HashCode {
private:
  BYTE m_v[16];
  inline void init(const BYTE *v) {
    memcpy(m_v, v, sizeof(m_v));
  }
  inline void reset() {
    memset(m_v, 0, sizeof(m_v));
  }
public:
  inline MD5HashCode() {
    reset();
  }
  inline MD5HashCode(const MD5HashCode &src) {
    init(src.m_v);
  }
  inline MD5HashCode(const BYTE *v) {
    init(v);
  }
  inline void clear(const BYTE *v = nullptr) {
    if(v) init(v); else reset();
  }
  inline const BYTE &operator[](UINT index) const {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  inline BYTE &operator[](UINT index) {
    assert(index < ARRAYSIZE(m_v));
    return m_v[index];
  }
  String toString(bool upper = true) const;
};

std::ostream  &operator<<(std::ostream  &out, const MD5HashCode &code);
std::wostream &operator<<(std::wostream &out, const MD5HashCode &code);

class MD5 {
public:
  static MD5HashCode &getHashCode(MD5HashCode &dst, ByteInputStream &s);
  static MD5HashCode &getHashCode(MD5HashCode &dst, const ByteArray &a) {
    return getHashCode(dst, ByteMemoryInputStream(a));
  }
};
