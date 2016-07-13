#include "pch.h"
#include <TextFormatDetecter.h>

TextFormat TextFormatDetecter::detectFormat(const BYTE *buf, int size, UINT &bytesToSkip) {
  bytesToSkip = 0;
  if (size < 2) {
    return TF_UNDEFINED;
  }
  if (buf[0] == 0xfe && buf[1] == 0xff) { // FE FF UTF16 big endian signature (BOM mark)
    bytesToSkip = 2;
    bool be;
    if (hasOnlyAscii(buf + 2, size - 2, be)) {
      return be ? TF_ASCII16_BE : TF_ASCII16_LE;
    }
    return TF_UTF16_BE;
  }
  if (buf[0] == 0xff && buf[1] == 0xfe) { // FF FE UTF16 little endian signature (BOM mark)
    bytesToSkip = 2;
    bool be;
    if (hasOnlyAscii(buf + 2, size - 2, be)) {
      return be ? TF_ASCII16_BE : TF_ASCII16_LE;
    }
    return TF_UTF16_LE;
  }
  if (size >= 3 && buf[0] == 0xef && buf[1] == 0xbb && buf[2] == 0xbf) { // EF BB BF UTF8 signature
    bytesToSkip = 3;
    return TF_UTF8;
  }
  const int formatFlags = getTextFormatFlags(buf, size);
  if (formatFlags & IS_TEXT_UNICODE_STATISTICS) {
    if (formatFlags & IS_TEXT_UNICODE_SIGNATURE) {
      bytesToSkip = 2; buf += 2; size -= 2;
    }
    bool be = false;
    if ((formatFlags & IS_TEXT_UNICODE_ASCII16) || hasOnlyAscii(buf, size, be)) {
      return be ? TF_ASCII16_BE : TF_ASCII16_LE;
    }
    return TF_UTF16_BE;
  }
  if (formatFlags & IS_TEXT_UNICODE_REVERSE_STATISTICS) {
    if (formatFlags & IS_TEXT_UNICODE_REVERSE_SIGNATURE) {
      bytesToSkip = 2; buf += 2; size -= 2;
    }
    bool be = true;
    if ((formatFlags & IS_TEXT_UNICODE_REVERSE_ASCII16) || hasOnlyAscii(buf, size, be)) {
      return be ? TF_ASCII16_BE : TF_ASCII16_LE;
    }
    return TF_UTF16_LE;
  }
  if (getAsciiBytesPercent(buf, size) > 95) {
    return TF_ASCII8;
  }
  const int codePage = findLegalCodePage(buf, size);
  if (codePage < 0) {
    return TF_ASCII8;
  }
  else {
    switch (codePage) {
    case CP_UTF8: return TF_UTF8;
    case CP_UTF7: return TF_UTF7;
    default:
      return TF_ASCII8;
    }
  }
}

#pragma comment(lib, "Advapi32.lib")

#define HAS_NONZERO_EVENBYTE 0x1
#define HAS_NONZERO_ODDBYTE  0x2

bool TextFormatDetecter::hasOnlyAscii(const BYTE *buf, int size, bool &bigEndian) { // static
  // reversed will be true if ascii
  BYTE nonZeroFlags = 0;
  const BYTE *endp = buf + size;

  for (const BYTE *s = buf; s < endp; s += 2) {
    if (*s != 0) {
      nonZeroFlags |= HAS_NONZERO_EVENBYTE;
      break;
    }
  }
  for (const BYTE *s = buf+1; s < endp; s += 2) {
    if (*s != 0) {
      nonZeroFlags |= HAS_NONZERO_ODDBYTE;
      break;
    }
  }
  switch (nonZeroFlags) {
  case 0: // all bytes are zero. Cannot detect endianess
    bigEndian = false;
    return true;
  case HAS_NONZERO_EVENBYTE:
    bigEndian = false;
    return true;
  case HAS_NONZERO_ODDBYTE:
    bigEndian = true;
    return true;
  case HAS_NONZERO_EVENBYTE | HAS_NONZERO_ODDBYTE: // nonzero bytes in both even and odd positions. Not ascii16
    return false;
  default:
    return false; // should not come here
  }
}

int TextFormatDetecter::getTextFormatFlags(const BYTE *buf, int size) { // static
  int result = IS_TEXT_UNICODE_UNICODE_MASK | IS_TEXT_UNICODE_REVERSE_MASK;
  const BOOL ok = IsTextUnicode(buf, size, &result);
  return result;
}

int TextFormatDetecter::findLegalCodePage(const BYTE *buf, int size) { // static. return -1 of none
  static const int codePages[] = {
    CP_UTF8
   ,CP_UTF7
  };
  for(int i = 0; i < ARRAYSIZE(codePages); i++) {
    const int cp = codePages[i];
    const int ret = MultiByteToWideChar(cp, MB_ERR_INVALID_CHARS, (char*)buf, size, NULL, 0);
    if (ret) return cp;
  }
  return -1;
}

int TextFormatDetecter::getAsciiBytesPercent(const BYTE *buf, int size) { // static Calculate how many percent of bytes < 128
  int asciiCount = 0;

  for (const BYTE *p = buf + size - 1; p >= buf;) {
    if (*(p--) <= 127) {
      asciiCount++;
    }
  }
  return asciiCount * 100 / size;
}
