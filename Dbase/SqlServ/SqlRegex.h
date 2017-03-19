#pragma once

#include <SqlApi.h>
#include <Regex.h>

// translates sql-like-pattern with wildcards '%' and '_' to normal regular expression
// with '_' -> '.' and % -> '.*'
class SqlRegex : private Regex {
private:
  String convertWildCards(const String &wc);
  String lastpattern;
public:
  SqlRegex(const String &pattern) { compile(pattern); }
  SqlRegex() {};
  ~SqlRegex() {}
  void compile(const String &pattern);
  bool match(const String &str) const;
  bool isLike(const String &str, const String &pattern);
#ifdef _DEBUG
  void dump(FILE *f=stdout) const { Regex::dump(f); }
  void testSqlRegex();
#endif
};

