#pragma once

#include <stdio.h>
#include <sqlapi.h>
#include <Regex.h>

// translates sql-like-pattern with wildcards '%' and '_' to normal regular expression
// with '_' -> '.' and % -> '.*'
class SqlRegex : private Regex {
private:
  String convertwc(const TCHAR *wc);
  String lastpattern;
public:
  SqlRegex(const TCHAR *pattern) { compile(pattern); }
  SqlRegex() {};
  ~SqlRegex() {}
  void compile(const TCHAR *pattern);
  bool match(const TCHAR *str) const;
  bool isLike(const TCHAR *str, const TCHAR *pattern);
#ifdef _DEBUG
  void dump(FILE *f=stdout) const { Regex::dump(f); }
#endif
};

#ifdef _DEBUG
void testSqlRegex();
#endif
