#include "pch.h"
#include <SHA256.h>

using namespace std;

ostream  &operator<<(std::ostream  &out, const SHA256HashCode &code) {
  USES_ACONVERSION;
  const bool uppercase = (out.flags() & ios_base::uppercase) != 0;
  const String tmp = code.toString(uppercase);

  return out << TSTR2ASTR(tmp.cstr());
}

wostream &operator<<(wostream &out, const SHA256HashCode &code) {
  USES_WCONVERSION;
  const bool uppercase = (out.flags() & ios_base::uppercase) != 0;
  const String tmp = code.toString(uppercase);

  return out << TSTR2WSTR(tmp.cstr());
}

String SHA256HashCode::toString(bool upper) const {
  const TCHAR *fstr = upper ? _T("%08X") : _T("%08x");
  String result;
  for(size_t i = 0; i < ARRAYSIZE(m_v); i++) {
    result += format(fstr, m_v[i]);
  }
  return result;
}

const UINT32 SHA256::s_K[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5
 ,0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174
 ,0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da
 ,0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967
 ,0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85
 ,0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070
 ,0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3
 ,0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

const UINT32 SHA256::s_hash0[] = {
  0x6a09e667
 ,0xbb67ae85
 ,0x3c6ef372
 ,0xa54ff53a
 ,0x510e527f
 ,0x9b05688c
 ,0x1f83d9ab
 ,0x5be0cd19
};

void SHA256::clear() {
  m_bytes.clear();
  m_msg.clear();
  m_hashedMsg.clear();
  m_bitCount = 0;
}

/**
 * Take the given hexadecimal string and store the bytes in a global vector.
 * Also update the message length.
 */
void SHA256::storeHexStr(const String &hexStr) {
  m_bytes.clear();
  const size_t n = hexStr.length();
  for(size_t j = 0; j < n;) {
    switch(n-j) {
    case 1 :
      { const BYTE byte = (BYTE)ttoi(substr(hexStr, j, 1).cstr(), nullptr, 16);
        m_bytes.append(byte);
        j++;
      }
      break;
    default:
      { const BYTE byte = (BYTE)ttoi(substr(hexStr, j, 2).cstr(), nullptr, 16);
        m_bytes.append(byte);
        j += 2;
      }
      break;
    }
  }
  m_bitCount = m_bytes.size() * 8;
}

void SHA256::storeByteArray(const ByteArray &a) {
  m_bytes = a;
  m_bitCount = m_bytes.size() * 8;
}

int SHA256::calcPadding() const {
  int k = 0;
  while((m_bitCount + 1 + k) % 512 != 448) { // 448 = 512 - 64
    k++;
  }
  return k;
}

void SHA256::padBytes() {
  // Append 1 followed by zeroes before the least significant bit, assuming full bytes as input
  m_bytes.append(0x80);

  // Append the remaining zeroes
  const int zeroCount = (calcPadding() - 7) / 8;
  m_bytes.appendZeroes(zeroCount);

  // Finally append the length in binary to the message as the least
  // significant bits, assuming a 64-bit number
  for(int i = 56; i >= 0; i -= 8) {
    m_bytes.append((BYTE)(m_bitCount >> i));
  }
}

size_t SHA256::parseBytes() {
  padBytes();
  const size_t count = m_bytes.size() / 64;
  m_msg.clear(count);
  for(size_t i = 0, n = 0; n < count; n++) {
    Block512Bit block;
    for(int j = 0; j < 16; j++) {
      UINT32 word = m_bytes[i++];
      for(int k = 3; k--;) {
        word <<= 8;
        word |= m_bytes[i++];
      }
      block[j] = word;
    }
    m_msg.add(block);
  }
  return count;
}

#define HASROTR
#ifdef HASROTR
#define ROTR(x,n) _rotr(x, n)
#else
// Rotate right function ROTR^n(x) in hash algorithm.
inline UINT32 ROTR(UINT32 x, UINT32 n) {
  return (x >> n) | (x << (32 - n));
}
#endif

// Right shift function SHR^n(x) in hash algorithm.
inline UINT32 SHR(UINT32 x, UINT32 n) {
  return x >> n;
}

// Logical function Ch(x, y, z) in hash algorithm.
inline UINT32 Ch(UINT32 x, UINT32 y, UINT32 z) {
  return (x & y) ^ (~x & z);
}

// Logical function Maj(x, y, z) in hash algorithm.
inline UINT32 Maj(UINT32 x, UINT32 y, UINT32 z) {
  return (x & y) ^ (x & z) ^ (y & z);
}

// Logical function (large) sigma^256_0(x) in hash algorithm.
inline UINT32 lsigma0(UINT32 x) {
  return ROTR(x,2) ^ ROTR(x,13) ^ ROTR(x,22);
}

// Logical function (large) sigma^256_1(x) in hash algorithm.
inline UINT32 lsigma1(UINT32 x) {
  return ROTR(x,6) ^ ROTR(x,11) ^ ROTR(x,25);
}

// Logical function (small) sigma^256_0(x) in hash algorithm.
inline UINT32 ssigma0(UINT32 x) {
  return ROTR(x,7) ^ ROTR(x,18) ^ SHR(x,3);
}

// Logical function (small) sigma^256_1(x) in hash algorithm.
inline UINT32 ssigma1(UINT32 x) {
  return ROTR(x,17) ^ ROTR(x,19) ^ SHR(x,10);
}

const SHA256HashCode &SHA256::computeHash() {
  const size_t n = parseBytes();
  m_hashedMsg.clear(s_hash0);
  SHA256HashCode &H = m_hashedMsg;
  for(size_t i = 0; i < n; i++) {
    const Block512Bit &Mi = m_msg[i];
    UINT32 W[64];
    // Prepare message schedule
    for(int t = 0; t < 16; t++) {
      W[t] = Mi[t];
    }
    for(int t = 16; t < 64; t++) {
      W[t] = ssigma1(W[t-2]) + W[t-7] + ssigma0(W[t-15]) + W[t-16];
    }

    // Initialise working variables with previous hash value
    UINT32 a = H[0];
    UINT32 b = H[1];
    UINT32 c = H[2];
    UINT32 d = H[3];
    UINT32 e = H[4];
    UINT32 f = H[5];
    UINT32 g = H[6];
    UINT32 h = H[7];

    // Perform logical operations
    for(int t = 0; t < 64; t++) {
      const UINT32 T1 = h + lsigma1(e) + Ch(e, f, g) + s_K[t] + W[t];
      const UINT32 T2 = lsigma0(a) + Maj(a, b, c);
      h = g;
      g = f;
      f = e;
      e = d + T1;
      d = c;
      c = b;
      b = a;
      a = T1 + T2;
    }

    // Compute intermediate hash values by assigning them to hash^i
    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
  }
  return m_hashedMsg;
}

SHA256HashCode &SHA256::getHashCode(SHA256HashCode &dst, const String    &hexStr) {
  storeHexStr(hexStr);
  dst = computeHash();
  clear();
  return dst;
}

SHA256HashCode &SHA256::getHashCode(SHA256HashCode &dst, const ByteArray &a     ) {
  storeByteArray(a);
  dst = computeHash();
  clear();
  return dst;
}
