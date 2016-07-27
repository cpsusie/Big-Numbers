#include "stdafx.h"
#include "Wizard.h"

void wizard(FILE *output, const String &templateName, const String &className) {
  String wizardTemplate = searchenv(templateName, "LIB");
  if(wizardTemplate.length() == 0) {
    _ftprintf(stderr, _T("Template <%s> not found in environment LIB-path\n"), templateName.cstr());
    exit(-1);
  }
  TCHAR line[512];
  FILE *input = FOPEN(wizardTemplate.cstr(), "r");
  while(_fgetts(line, ARRAYSIZE(line), input)) {
    TCHAR dst[512];
    strReplace(dst, line, _T("$CLASSNAME"), className.cstr());
    _fputts(dst, output);
  }
  fclose(input);
}
