/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once
#include <Scanner.h>

#line 2 "C:\\Mytools2015\\Dbase\\SqlParse\\Sql.lex"
#pragma once

 /* This part goes to SqlLex.h */

#include <Scanner.h>
#include <LRparser.h>

class SqlLex : public Scanner {
  LRparser *m_parser; // redirect errors to parser
public:
  SqlLex(LRparser *parser = nullptr);
  int  getNextLexeme() override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  static void findBestHashMapSize();
};

#line 5 "C:\\mytools2015\\parsergen\\lib\\lexgencpp.par"
