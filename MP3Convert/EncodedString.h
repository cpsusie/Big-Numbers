#pragma once

#include <ByteArray.h>
#include <id3.h>

class EncodedString : public ByteArray {
private:
  ID3_TextEnc m_encoding;
public:
  EncodedString(ID3_TextEnc encoding, const BYTE *data, size_t nbytes)
    : m_encoding(encoding)
    , ByteArray(data, nbytes)
  {
  }
  ID3_TextEnc getEncoding() const {
    m_encoding;
  }
  String toString() const;
};
