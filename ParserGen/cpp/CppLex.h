/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once
#include <Scanner.h>

#line 2 "C:\\Mytools2015\\ParserGen\\cpp\\Cpp.lex"
#pragma once
 /* This part goes to cpplex.h */

#include <Scanner.h>
#include <lrparser.h>

using namespace LRParsing;

class CppLex : public Scanner {
private:
  LRparser *m_parser;
public:
  CppLex() { m_parser = nullptr; }
  void setParser(LRparser *parser) { m_parser = parser; }
  int  getNextLexeme() override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  int m_token;
  int ascii;
};

#line 5 "C:\\mytools2015\\ParserGen\\lib\\lexgencpp.par"
