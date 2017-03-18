#include "stdafx.h"
#include "GraphParameters.h"

GraphParameters::GraphParameters(const String &name, COLORREF color, int rollSize, GraphStyle style) {
  m_name     = name;
  m_color    = color;
  m_rollSize = rollSize;
  m_style    = style;
};

void GraphParameters::load(const String &fileName) {
  FILE *f = FOPEN(fileName, _T("r"));
  try {
    readTextFile(f);
    fclose(f);
    setName(fileName);
  } catch(...) {
    fclose(f);
    throw;
  }
}

void GraphParameters::save(const String &fileName) {
  FILE *f = NULL;
  try {
    f = FOPEN(fileName, _T("wb"));
    setName(fileName);
    writeTextFile(f);
    fclose(f); f = NULL;
  } catch(Exception e) {
    if(f) {
      fclose(f);
    }
    throw;
  }
}

void GraphParameters::setName(const String &name) {
  m_name = name;
}

String GraphParameters::getPartialName() const {
  return FileNameSplitter(m_name).getFileName();
}

static const TCHAR *styleName[] = {
  _T("Curve")
 ,_T("Point")
 ,_T("Cross")
};

const TCHAR *GraphParameters::graphStyleToString(GraphStyle style) {  // static
  if(style < 0 || style > 2) {
    throwInvalidArgumentException(__TFUNCTION__, _T("style=%d. Must be [0..2]"), style);
  }
  return styleName[style];
}

GraphStyle GraphParameters::graphStyleFromString(const String &s) {  // static
  for(int i = 0; i < ARRAYSIZE(styleName); i++) {
    if(s.equalsIgnoreCase(styleName[i])) {
      return (GraphStyle)i;
    }
  }
  return GSCURVE;
}

const TCHAR *GraphParameters::trigonometricModeToString(TrigonometricMode mode) {  // static
  switch(mode) {
  case RADIANS : return _T("RADIANS");
  case DEGREES : return _T("DEGREES");
  case GRADS   : return _T("GRADS");
  default      : return _T("RADIANS");
  }
}

TrigonometricMode GraphParameters::trigonometricModeFromString(const String &str) { // static
  if(str.equalsIgnoreCase(_T("radians"))) {
    return RADIANS;
  } else if(str.equalsIgnoreCase(_T("DEGREES"))) {
    return DEGREES;
  } else if(str.equalsIgnoreCase(_T("GRADS"))) {
    return GRADS;
  } else {
    return RADIANS;
  }
}

char *fgetline(char *line, int size, FILE *f) {
  if (!fgets(line, size, f)) return NULL;
  char *nl = strchr(line, '\n');
  if (nl) *nl = 0;
  return line;
}

#define nextChar() ch = fgetc(f)
String GraphParameters::readString(FILE *f) { // static
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

String GraphParameters::readLine(FILE *f) { // static
  char line[2024];
  if(!fgetline(line, sizeof(line), f)) {
    throwException(_T("Unexpected end-of-file"));
  }
  return line;
}

void GraphParameters::writeString(FILE *f, const String &str) { // static
  String tmp = str;
  tmp.replace(_T("\xd\xa"), _T("\n")).trimRight();
  USES_CONVERSION;
  fprintf(f, "\"%s\"\n", T2A(tmp.cstr()));
}

void GraphParameters::readTextFile(FILE *f) {
  throwUnsupportedOperationException(__TFUNCTION__);
}

void GraphParameters::writeTextFile(FILE *f) {
  throwUnsupportedOperationException(__TFUNCTION__);
}
