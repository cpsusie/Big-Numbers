#pragma once

typedef enum {
    TF_ASCII8
  , TF_ASCII16_BE
  , TF_ASCII16_LE
  , TF_UTF7
  , TF_UTF8
  , TF_UTF16_BE
  , TF_UTF16_LE
  , TF_UNDEFINED
} TextFormat;

class TextFormatDetecter {
private:
  static bool hasOnlyAscii(const BYTE *buf, int size, bool &bigEndian);
public:
  static TextFormat detectFormat(const BYTE *buf, int size, UINT &bytesToSkip);
  static int getTextFormatFlags(const BYTE *buf, int size);
  static int findLegalCodePage(const BYTE *buf, int size);
  static int getAsciiBytesPercent(const BYTE *buf, int size); // Calculate how many percent of bytes < 128
};

