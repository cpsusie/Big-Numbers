%{
#pragma once

 /* This part goes to the h file */

#include <LRParser.h>

class ExpressionLex : public Scanner {
private:
  LRparser *m_parser; // if set, redirect errors to parsers verror-routine
public:
  ExpressionLex(LexStream *inputStream = NULL) : Scanner(inputStream) {
    m_parser = NULL;
  }
  void setParser(LRparser *parser) {
    m_parser = parser;
  }
  int  getNextLexeme();
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr);
};

%}

%{

 /* This part goes to the first part of the cpp file */

#include "pch.h"
#include <HashMap.h>
#include <Math/Expression/ExpressionSymbol.h>
#include <Math/Expression/ExpressionLex.h>

static int nameOrKeyWord(const _TUCHAR *lexeme);

%}

let     [_a-zA-Z]              /* Letter                                */
alnum   [_a-zA-Z0-9]           /* Alphanumeric character                */
d       [0-9]		               /* Decimal digit                         */
white   [\x00-\x09\x0b\s\r\n]  /* White space: all control chars        */

%%

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)?   return NUMBER;

"/*"						{ int i;
                  const SourcePosition startpos = getPos();
                  while(i = input()) {
                    if(i < 0) {
                      flushBuf();  /* Discard lexeme.     */
                    } else if(i == '*' && look(1) == '/') {
                     input();
                     break;        /* Recognized comment. */
                    }
                  }
                  if(i == 0) {
                    error(startpos,_T("End of file in comment\n") );
                  }
                }

"//"						{ int i;
                  while(i = input()) {
                    if(i < 0) {
                      flushBuf();  /* Discard lexeme. */
                    } else if(i == '\n') {
                      break;
                    }
                  }
                }

"+"		    return PLUS;
"-"		    return MINUS;
"*"		    return PROD;
"/"		    return QUOT;
"%"		    return MOD;
"**"|"^"	return POW;
","		    return COMMA;
"("		    return LPAR;
")"		    return RPAR;
"["		    return LB;
"]"		    return RB;
"&&"	    return AND;
"||"	    return OR;
"!"		    return NOT;
"=="	    return EQ;
"<"       return LT;
"<="      return LE;
">"       return GT;
">="      return GE;
"!="      return NE;
"<>"      return NE;
"="		    return ASSIGN;
";"		    return SEMI;

{let}{alnum}*   return nameOrKeyWord((_TUCHAR*)getText());
{white}+		;

%%

  /* This part goes to the last part of exprlex.cpp */

typedef struct {
  const TCHAR *m_name;
  const int    m_token;
} KeyWord;

static const KeyWord keywordtable[] = {
 _T("ABS")       ,ABS
,_T("ACOS")      ,ACOS
,_T("ACOSH")     ,ACOSH
,_T("ACOT")      ,ACOT
,_T("ACSC")      ,ACSC
,_T("AND")       ,AND
,_T("ASEC")      ,ASEC
,_T("ASIN")      ,ASIN
,_T("ASINH")     ,ASINH
,_T("ATAN")      ,ATAN
,_T("ATAN2")     ,ATAN2
,_T("ATANH")     ,ATANH
,_T("BINOMIAL")  ,BINOMIAL
,_T("CEIL")      ,CEIL
,_T("CHI2DENS")  ,CHI2DENS
,_T("CHI2DIST")  ,CHI2DIST
,_T("LINCGAMMA") ,LINCGAMMA
,_T("COS")       ,COS
,_T("COSH")      ,COSH
,_T("COT")       ,COT
,_T("CSC")       ,CSC
,_T("ERF")       ,ERF
,_T("EXP")       ,EXP
,_T("FAC")       ,FAC
,_T("FLOOR")     ,FLOOR
,_T("GAMMA")     ,GAMMA
,_T("GAUSS")     ,GAUSS
,_T("IF")        ,IIF
,_T("INVERF")    ,INVERF
,_T("LN")        ,LN
,_T("LOG10")     ,LOG10
,_T("MAX")       ,MAX
,_T("MIN")       ,MIN
,_T("MOD")       ,MOD
,_T("NORM")      ,NORM
,_T("NORMRAND")  ,NORMRAND
,_T("NORMRANDOM"),NORMRAND
,_T("NORMRND")   ,NORMRAND
,_T("NOT")       ,NOT
,_T("OR")        ,OR
,_T("POLY")      ,POLY
,_T("PROBIT")    ,PROBIT
,_T("PRODUCT")   ,INDEXEDPRODUCT
,_T("RAND")      ,RAND
,_T("RANDOM")    ,RAND
,_T("ROOT")      ,ROOT
,_T("RND")       ,RAND
,_T("SEC")       ,SEC
,_T("SIGN")      ,SIGN
,_T("SIN")       ,SIN
,_T("SINH")      ,SINH
,_T("SQR")       ,SQR
,_T("SQRT")      ,SQRT
,_T("SUM")       ,INDEXEDSUM
,_T("TAN")       ,TAN
,_T("TANH")      ,TANH
,_T("TO")        ,TO
};

typedef StrIHashMap<int> HashMapType;

class ExpressionKeyWordMap : public HashMapType {
public:
  ExpressionKeyWordMap() : HashMapType(23) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++) {
      put(keywordtable[i].m_name,keywordtable[i].m_token);
    }
  }
};

static ExpressionKeyWordMap keywords;

static int nameOrKeyWord(const _TUCHAR *lexeme) {
  const int *p = keywords.get(lexeme);
  return p ? *p : NAME;
}

void ExpressionLex::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  if(m_parser) {
    m_parser->verror(pos, format, argptr);
  } else {
    Scanner::verror(pos, format, argptr);
  }
}
