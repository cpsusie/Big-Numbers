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

#include "MyString.h"

class MD5Context {
private:
  unsigned long m_state[4];        // state (ABCD)
  unsigned long m_count[2];        // number of bits, modulo 2^64 (lsb first)
  unsigned char m_buffer[64];      // input buffer
  char          m_strDigest[33];   // result-String
public:
  MD5Context();
  void init();
  void update(const unsigned char *input, UINT inputLen);
  void update(const          char *input, UINT inputLen) {
    update((const unsigned char*)input, inputLen);
  };
  void final(unsigned char digest[16]);
  char *strFinal(char *dst);       // Returns the digest result in a String containing hexadecimal digits
  char *digest(const char *src);   // Digests a String (src) and returns the result
  String digest(const String &s);
};
