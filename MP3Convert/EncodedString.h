#pragma once

#include <ByteArray.h>
#include <id3.h>

String toString(ID3_FrameID   id  );
String toString(ID3_FieldID   id  );
String toString(ID3_FieldType type);
String toString(ID3_TextEnc   enc );
String toString(ID3_V2Spec    spec);

class EncodedString : public ByteArray {
private:
  ID3_TextEnc m_encoding;
public:
  inline EncodedString(ID3_TextEnc encoding, const BYTE *data, size_t nbytes)
    : m_encoding(encoding)
    , ByteArray(data, nbytes)
  {
  }
  EncodedString(const String &str);
  inline ID3_TextEnc getEncoding() const {
    return m_encoding;
  }
  String toString(bool hexdump) const;
};
