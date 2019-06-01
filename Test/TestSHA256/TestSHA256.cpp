#include "stdafx.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>


/**
 * SHA-256 implemented according to the specification:
 * http://csrc.nist.gov/publications/fips/fips180-4/fips-180-4.pdf
 */

using namespace std;

typedef unsigned char       BYTE;
typedef unsigned int        UINT32;
typedef unsigned long long  UINT64;
typedef vector<UINT32>      UINT32Vector;

class SHA256HashCode : public UINT32Vector {
public:
  SHA256HashCode() {
  }
  SHA256HashCode(const UINT32Vector &v) : UINT32Vector(v) {
  }
  string toString() const;
};

ostream &operator<<(ostream &out, const SHA256HashCode &code) {
  for(int i = 0; i < 8;) {
    out << hex << setw(8) << setfill('0') << code[i++];
  }
  return out;
}

string SHA256HashCode::toString() const {
  ostringstream ostr;
  ostr << *this;
  return ostr.str();
}

class SHA256 {
private:
  static const UINT32         s_K[64];         // Constants used in hash algorithm
  static const SHA256HashCode s_hash0;         // Initial value of m_hashedMsg, before shuffling bits with 512-bit blocks (m_msg[0..N-1])

  vector<BYTE>                m_bytes;         // Plain and padded message bytes
  vector<UINT32Vector>        m_msg;           // Message to be hashed
  vector<SHA256HashCode>      m_hashedMsg;     // Hashed message
  UINT64                      m_bitCount;             // Message length in bits

  void clear();                                // Clear all working vectors and variables.
  void storeHexBytes(   const string &hexStr);
  void storeStringBytes(const string &str   );

  int  calcPadding() const;                    // Calculate the required padding of the message. Return the required padding.
  void padBytes();                             // Pad m_bytes according to the specification.
  int  parseBytes();                           // Parse m_bytes into 512-bit blocks split up into UINT32's. and return number of blocks
  const SHA256HashCode &computeHash();         // Do the bit-shuffling

public:
  SHA256() {
    clear();
  }
  SHA256HashCode &getHashHexStr(SHA256HashCode &dst, const string &hexStr);
  SHA256HashCode &getHashStr(   SHA256HashCode &dst, const string &str   );
};

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

const SHA256HashCode SHA256::s_hash0 = UINT32Vector(
{ 0x6a09e667
 ,0xbb67ae85
 ,0x3c6ef372
 ,0xa54ff53a
 ,0x510e527f
 ,0x9b05688c
 ,0x1f83d9ab
 ,0x5be0cd19
});

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
void SHA256::storeHexBytes(const string &hexStr) {
  m_bytes.clear();
  const size_t n = hexStr.length();
  for(size_t j = 0; j < n;) {
    switch(n-j) {
    case 1 :
      { const BYTE byte = stoi(hexStr.substr(j, 1), nullptr, 16);
        m_bytes.push_back(byte);
        j++;
      }
      break;
    default:
      { const BYTE byte = stoi(hexStr.substr(j, 2), nullptr, 16);
        m_bytes.push_back(byte);
        j += 2;
      }
      break;
    }
  }
  m_bitCount = m_bytes.size() * 8;
}

void SHA256::storeStringBytes(const string &str) {
  m_bytes.clear();
  for(const char *cp = str.c_str(); *cp;) {
    m_bytes.push_back(*(cp++));
  }
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
  m_bytes.push_back(0x80);

  // Append the remaining zeroes
  for(int i = (calcPadding() - 7) / 8; i--;) {
    m_bytes.push_back(0);
  }

  // Finally append the length in binary to the message as the least
  // significant bits, assuming a 64-bit number
  for(int i = 56; i >= 0; i -= 8) {
    m_bytes.push_back((BYTE)(m_bitCount >> i));
  }
}

int SHA256::parseBytes() {
  padBytes();
  UINT32 n = 0;
  m_msg.clear();
  const size_t count = m_bytes.size() / 64;
  for(int i = 0; n < count; n++) {
    UINT32Vector block(16);
    for(int j = 0; j < 16; j++) {
      UINT32 word = m_bytes[i++];
      for(int k = 3; k--;) {
        word <<= 8;
        word |= m_bytes[i++];
      }
      block[j] = word;
    }
    m_msg.push_back(block);
  }
  return n;
}

// Rotate right function ROTR^n(x) in hash algorithm.
inline UINT32 ROTR(UINT32 x, UINT32 n) {
  return (x >> n) | (x << (32 - n));
}

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
  const int n = parseBytes();
  m_hashedMsg.clear();
  m_hashedMsg.push_back(s_hash0);
  UINT32Vector hashBlock(8);
  for(int i = 0; i < n; i++) {
    const UINT32Vector &Mi = m_msg[i];
    UINT32 W[64];
    // Prepare message schedule
    for(int t = 0; t < 16; t++) {
      W[t] = Mi[t];
    }
    for(int t = 16; t < 64; t++) {
      W[t] = ssigma1(W[t-2]) + W[t-7] + ssigma0(W[t-15]) + W[t-16];
    }

    UINT32Vector &Hi = m_hashedMsg[i];
    // Initialise working variables with previous hash value
    UINT32 a = Hi[0];
    UINT32 b = Hi[1];
    UINT32 c = Hi[2];
    UINT32 d = Hi[3];
    UINT32 e = Hi[4];
    UINT32 f = Hi[5];
    UINT32 g = Hi[6];
    UINT32 h = Hi[7];

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
    hashBlock[0] = a + Hi[0];
    hashBlock[1] = b + Hi[1];
    hashBlock[2] = c + Hi[2];
    hashBlock[3] = d + Hi[3];
    hashBlock[4] = e + Hi[4];
    hashBlock[5] = f + Hi[5];
    hashBlock[6] = g + Hi[6];
    hashBlock[7] = h + Hi[7];
    m_hashedMsg.push_back(hashBlock);
  }
  return m_hashedMsg[n];
}

SHA256HashCode &SHA256::getHashHexStr(SHA256HashCode &dst, const string &hexStr) {
  storeHexBytes(hexStr);
  dst = computeHash();
  clear();
  return dst;
}

SHA256HashCode &SHA256::getHashStr(SHA256HashCode    &dst, const string &str) {
  storeStringBytes(str);
  dst = computeHash();
  clear();
  return dst;
}

static void testSuite() {
  static const char *testNames[] ={
     "512"
    ,"empty"
    ,"large"
    ,"leading_zero_hash"
    ,"less_512"
    ,"massive"
    ,"massive_deluxe"
    ,"more_512"
    ,"sample"
    ,"test_vectors"
  };
  const int n = 10;
  bool allOk = true;
  for(int i = 0; i < 10; i++) {
    string inName = "data\\";
    inName += testNames[i];
    string answerName = inName + ".ans";
    inName += ".in";
    ifstream input(inName);
    ifstream answer(answerName);

    int lineCount = 0;
    for(string line; getline(input, line);) {
      lineCount++;
      SHA256HashCode code;
      string hashCode = SHA256().getHashHexStr(code, line).toString();
      string expected;
      getline(answer, expected);
      if(hashCode != expected) {
        cout << "error in" << inName << " line " << lineCount << endl;
        cout << "  hash(" << line << ") = \"" << hashCode << "\"" << endl;
        cout << "  expected:\"" << expected << "\"" << endl;
        allOk = false;
      }
    }
  }
  if(allOk) {
    cout << "All ok!" << endl;
  }
}

typedef enum {
  CMD_UNKNOWN
 ,CMD_TESTSUITE
 ,CMD_HASHTEXT
} Command;

int main(int argc, char **argv) {
  char *cp;
  Command cmd = CMD_UNKNOWN;
  for(argv++; *argv && (*(cp = *argv) == '-'); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 't':
        cmd = CMD_TESTSUITE;
        break;
      case 'f':
        cmd = CMD_HASHTEXT;
        break;
      }
      break;
    }
  }
  switch(cmd) {
  case CMD_TESTSUITE:
    testSuite();
    break;
  case CMD_HASHTEXT:
    { istream *input = *argv ? new ifstream(*argv) : &cin;
      string fileContent;
      for(string line; getline(*input, line); fileContent += line);
      SHA256HashCode code;
      cout << SHA256().getHashStr(code, fileContent) << endl;
    }
    break;
  }
  return 0;
}
