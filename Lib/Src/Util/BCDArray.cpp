#include "pch.h"
#include <BCDArray.h>

#define C_MINUS 10
#define C_COMMA 11
#define C_EXP   12
#define C_SLASH 13
#define C_RESERVED // Escape-sequence for 14
#define C_END   15
#define ENDEND ((C_END<<4)|C_END)

#define DECLARE_BCDBUF(name)   BYTE name##_byte=0; BYTE name##_head=0;                     \
                               const BYTE *name##_bp = getData(),                          \
                                          *name##_endp=name##_bp+size()
#define BCDBUF_ISEMPTY(name)   (name##_head == 0)
#define BCDBUF_FLUSH(  name)   { add(name##_byte); name##_head = 0; }
#define BCDBUF_ADD(    name,v) if(BCDBUF_ISEMPTY(name)) {                                  \
                                 name##_byte=(v)<<4; name##_head++;                        \
                               } else {                                                    \
                                 name##_byte|=(v); BCDBUF_FLUSH(name);                     \
                               }

#define BCDBUF_EOS(    name)   (name##_bp >= name##_endp)
#define BCDBUF_FILL(   name)   { name##_byte = BCDBUF_EOS(name) ? ENDEND : *(name##_bp++); \
                                 name##_head=2;                                            \
                               }

#define BCDBUF_GET(name,ch)    { if(BCDBUF_ISEMPTY(name)) BCDBUF_FILL(name);               \
                                 if(--name##_head) {                                       \
                                   ch = name##_byte>>4;                                    \
                                 } else {                                                  \
                                   ch = name##_byte&0xf;                                   \
                                 }                                                         \
                               }

BCDArray::BCDArray(const String &numstr) {
  const _TUCHAR *cp       = numstr.cstr();
  bool           gotDigit = false;
  bool           gotComma = false;
  bool           gotExpo  = false;

  while(iswspace(*cp)) cp++;

  DECLARE_BCDBUF(bcd);
  if(*cp == '+') cp++;
  else if(*cp == '-') {
    BCDBUF_ADD(bcd,C_MINUS);
    cp++;
  }
  while(iswdigit(*cp)) {
    const BYTE d = *(cp++) - '0';
    BCDBUF_ADD(bcd,d);
    gotDigit = true;
  }
  if(*cp == '.') {
    BCDBUF_ADD(bcd,C_COMMA);
    gotComma = true;
    cp++;
    while(iswdigit(*cp)) {
      const BYTE d = *(cp++) - '0';
      BCDBUF_ADD(bcd,d);
      gotDigit = true;
    }
  }
  if((*cp == 'e') || (*cp == 'E')) {
    BCDBUF_ADD(bcd,C_EXP);
    gotExpo = true;
    cp++;
    if(*cp == '+') cp++;
    else if(*cp == '-') {
      BCDBUF_ADD(bcd,C_MINUS);
      cp++;
    }
    while(iswdigit(*cp)) {
      const BYTE d = *(cp++) - '0';
      BCDBUF_ADD(bcd,d);
    }
  }
  if(gotDigit && !gotComma && !gotExpo) {
    if((*cp == '/') && iswdigit(cp[1])) {
      BCDBUF_ADD(bcd, C_SLASH);
      cp++;
      while(iswdigit(*cp)) {
        const BYTE d = *(cp++) - '0';
        BCDBUF_ADD(bcd, d);
      }
    }
  }
  if(!BCDBUF_ISEMPTY(bcd)) {
    BCDBUF_ADD(bcd,C_END);
  }
  if(!gotDigit) {
    throwInvalidArgumentException(__TFUNCTION__, _T("numstr=%s"), numstr.cstr());
  }
}

String BCDArray::toString() const {
  String result;
  DECLARE_BCDBUF(bcd);
  for(;;) {
    BYTE b;
    BCDBUF_GET(bcd,b);
    TCHAR ch;
    if(b <= 9) {
      ch = '0' + b;
    } else {
      switch(b) {
      case C_MINUS : ch = '-'; break;
      case C_COMMA : ch = '.'; break;
      case C_EXP   : ch = 'e'; break;
      case C_SLASH : ch = '/'; break;
      case C_END   : return result;
      default      :
        throwException(_T("Unknown BCD-byte:%d. current result:%s"), b, result.cstr());
      }
    }
    result += ch;
  }
}
