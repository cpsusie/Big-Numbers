#include "stdafx.h"
#include <ctype.h>
#include "ByteStringExtractor.h"

static int charclass[256];

static void setCharClass(unsigned char c, int value) {
  charclass[c] = value;
}

static void initCharClass(bool danish) {
  for(int i = 0; i < 256; i++) {
    setCharClass(i,isprint(i)?1:0);
  }
  setCharClass(' ',1);
  if(danish) {
    // danish characters in dos
    setCharClass(0x92,1); setCharClass(0x91,1);
    setCharClass(0x9d,1); setCharClass(0x9b,1);
    setCharClass(0x8f,1); setCharClass(0x86,1);
    // danish characters in windows
    setCharClass((unsigned char)'æ',1);
    setCharClass((unsigned char)'ø',1);
    setCharClass((unsigned char)'å',1);
    setCharClass((unsigned char)'Æ',1);
    setCharClass((unsigned char)'Ø',1);
    setCharClass((unsigned char)'Å',1);
  }
}

#define ISALPHA(ch) charclass[ch]

ByteStringExtractor::ByteStringExtractor(UINT minLength, bool showNames, bool verbose, bool danish)
: StringExtractor(minLength, showNames, verbose)
{
  m_strLength = 0;
  m_bufLength = 0x10000;
  m_buf       = MALLOC(char,m_bufLength);
  initCharClass(danish);
}

ByteStringExtractor::~ByteStringExtractor() {
  FREE(m_buf);
}

void ByteStringExtractor::extractStrings(const TCHAR *fname) {
  FILE *f = FOPEN(fname, _T("rb"));
  setvbuf(f, NULL, _IOFBF, 0x10000);

  int ch;
  m_strLength = 0;
  while((ch = fgetc(f)) != EOF) {
    if(ISALPHA(ch)) {
      appendChar(ch);
    } else {
      outputString();
    }
  }
  outputString();
  fclose(f);
}

void ByteStringExtractor::extendBuffer() {
  m_bufLength *= 2;
  m_buf        = REALLOC(m_buf,char,m_bufLength);
}

void ByteStringExtractor::appendChar(char ch) {
  if(m_strLength >= m_bufLength) {
    extendBuffer();
  }
  m_buf[m_strLength++] = ch;
}

void ByteStringExtractor::outputString() {
  if(m_strLength >= (int)m_minLength) {
    appendChar('\0');
    printString(m_buf);
  }
  m_strLength = 0;
}
