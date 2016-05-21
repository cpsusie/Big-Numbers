#pragma once

#include "Regex.h"

class WildCardRegex : public Regex {
private:
  String convertWildCard(const TCHAR *wc);
public:
  WildCardRegex(const TCHAR **fnames) {
    compile(fnames);
  }
  WildCardRegex() {};
  ~WildCardRegex() {}
  void compile(const TCHAR **fnames);
  void compile(const TCHAR *str, bool ignoreCase = false, bool wholeWordOnly = false);
  void compile(const String    &str, bool ignoreCase = false, bool wholeWordOnly = false);
};
