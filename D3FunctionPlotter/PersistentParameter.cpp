#include "stdafx.h"
#include "PersistentParameter.h"

void PersistentParameter::load(const String &fileName) {
  FILE *f = FOPEN(fileName, "r");
  try {
    read(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void PersistentParameter::save(const String &fileName) {
  FILE *f = FOPEN(fileName, "w");
  try {
    write(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

char *fgetline(char *line, int size, FILE *f) {
  if (!fgets(line, size, f)) return NULL;
  char *nl = strchr(line, '\n');
  if (nl) *nl = 0;
  return line;
}

#define nextChar() ch = fgetc(f)
String PersistentParameter::readString(FILE *f) { // static
  String result;
  int ch;
  nextChar();
  while(!feof(f) && isspace(ch)) nextChar();
  if(ch == '\"') {
    for(nextChar(); !feof(f) && (ch != '\"'); nextChar()) {
      result += ch;
    }
  } else {
    throwException(_T("Invalid input. Expected \""));
  }
  result.replace('\n', _T("\xd\xa"));
  return result;
}

String PersistentParameter::readLine(FILE *f) { // static
  char line[2024];
  if(!fgetline(line, sizeof(line), f)) {
    throwException(_T("Unexpected end-of-file"));
  }
  return line;
}

void PersistentParameter::writeString(FILE *f, const String &str) { // static
  String tmp = str;
  tmp.replace(_T("\xd\xa"), _T("\n")).trimRight();
  USES_CONVERSION;
  fprintf(f, "\"%s\"\n", T2A(tmp.cstr()));
}

String PersistentParameter::getDisplayName() const {
  return FileNameSplitter(getName()).getFileName();
}
