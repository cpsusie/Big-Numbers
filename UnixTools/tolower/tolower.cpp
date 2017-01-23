#include "stdafx.h"
#include <MyUtil.h>
#include <ctype.h>
#include <io.h>

static bool escapeStrings    = false;

static char charIsUpper[256];
static unsigned char upperCaseCharacters[256];
static unsigned char lowerCaseCharacters[256];

static unsigned char charToUpper(unsigned char ch) {
  switch(ch) {
  case 0x91: return 0x92;
  case 0x9b: return 0x9d;
  case 0x86: return 0x8f;
  case (unsigned char)'æ': return (unsigned char)'Æ';
  case (unsigned char)'ø': return (unsigned char)'Ø';
  case (unsigned char)'å': return (unsigned char)'Å';
  default                : return toupper(ch);
  }
}

static unsigned char charToLower(unsigned char ch) {
  switch(ch) {
  case 0x92: return 0x91;
  case 0x9d: return 0x9b;
  case 0x8f: return 0x86;
  case (unsigned char)'Æ': return (unsigned char)'æ';
  case (unsigned char)'Ø': return (unsigned char)'ø';
  case (unsigned char)'Å': return (unsigned char)'å';
  default                : return tolower(ch);
  }
}

static unsigned char convertToUpper(unsigned char ch) {
  return charIsUpper[ch] ? ch : charToUpper(ch);
}

static unsigned char convertToLower(unsigned char ch) {
  return charIsUpper[ch] ? charToLower(ch) : ch;
}

static void initCharTable(bool useDanishCharacters) {
  for(int i = 0; i < ARRAYSIZE(charIsUpper); i++) {
    if(isupper(i)) {
      charIsUpper[i] = true;
      continue;
    } else if(useDanishCharacters) {
      switch(i) {
      case 0x91:
      case 0x9b:
      case 0x86:
      case (unsigned char)'æ':
      case (unsigned char)'ø':
      case (unsigned char)'å':
        charIsUpper[i] = false;
        continue;
      case 0x92:
      case 0x9d:
      case 0x8f:
      case (unsigned char)'Æ':
      case (unsigned char)'Ø':
      case (unsigned char)'Å':
        charIsUpper[i] = true;
        continue;
      }
    }
    charIsUpper[i] = false;
  }
  for(int i = 0; i < ARRAYSIZE(upperCaseCharacters); i++) {
    upperCaseCharacters[i] = convertToUpper(i);
  }
  for(int i = 0; i < ARRAYSIZE(lowerCaseCharacters); i++) {
    lowerCaseCharacters[i] = convertToLower(i);
  }
}

static unsigned char *conversionTable = lowerCaseCharacters;

static char *strConvert(char *s) {
  bool instr = false;

  if(escapeStrings) {
    for(unsigned char *t = (unsigned char*)s; *t; t++) {
      if(*t == '"') {
        instr = !instr;
      } else if(!instr) {
        *t = conversionTable[*t];
      }
    }
  } else {
    for(unsigned char *t = (unsigned char*)s; *t; t++) {
      *t = conversionTable[*t];
    }
  }
  return s;
}

static void usage() {
  _ftprintf(stderr,_T("Usage:tolower [-u] [-e] [-d] [inputfile]\n"));
  exit(-1);
}

int main(int argc,char **argv) {
  char *cp;
  bool useDanishCharacters = false;

  for(argv++; *argv && ((*(cp = *argv) == '-')); argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'u': conversionTable     = upperCaseCharacters; continue;
      case 'e': escapeStrings       = true; continue;
      case 'd': useDanishCharacters = true; continue;
      default : usage();
      }
      break;
    }
  }

  initCharTable(useDanishCharacters);
  try {
    FILE *in = *argv ? FOPEN(*argv,_T("r")) : stdin;
    if(isatty(in)) {
      usage();
    }
    char line[4096];
    while(fgets(line,sizeof(line),in)) {
      printf("%s",strConvert(line));
    }
  } catch(Exception e) {
    _ftprintf(stderr,_T("tolower:%s\n"), e.what());
    return -1;
  }
  return 0;
}
