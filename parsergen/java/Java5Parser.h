/* DO NOT EDIT THIS FILE - it is machine generated */
#pragma once

#include <LRparser.h>

#line 5 "C:\\Mytools2015\\ParserGen\\java\\Java5.y"
#pragma once

#include <LRparser.h>
#include "Java5Symbol.h"
#include "Java5Lex.h"

class Java5Parser : public LRparser {
public:
  static const ParserTables *Java5Tables;
private:
  int reduceAction(         UINT prod  ) override;

  void userStackInit()                   override { // Called before the first parsecycle
  }

  void userStackShiftSymbol(UINT symbol) override { // Called when LRparser shift in inputtoken
  }

  void userStackPopSymbols( UINT count ) override { // Pop count symbols from userstack
  }

  void userStackShiftLeftSide()          override { // Push($$) to userstack. called at the end of each reduction
  }

  void defaultReduce(       UINT prod  ) override { // $$ = $1
  }

  int *getStackTop(int fromtop) {
    return NULL;
  }
  String m_fileName;
public:
  Java5Parser(const TCHAR *fileName, Scanner *lex=NULL, UINT stacksize = 1000) : LRparser(*Java5Tables,lex,stacksize) {
    m_fileName = fileName;
  }
  ~Java5Parser() override {
  }
};

#line 22 "C:\\mytools2015\\parsergen\\lib\\parsergencpp.par"

