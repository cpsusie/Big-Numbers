%{
#pragma once

 /* This part goes to SqlLex.h */

#include <Scanner.h>
#include <LrParser.h>

class SqlLex : public Scanner {
private:
  LRparser *m_parser; // redirect errors to parser
public:
  SqlLex(LRparser *parser=NULL);
  int getNextLexeme() override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  static void findBestHashMapSize();
};

%}

%{

 /* This part goes to the first part of sqllex.cpp */

#include "stdafx.h"
#include <HashMap.h>
#include "SqlLex.h"
#include "SqlSymbol.h"

static int nameOrKeyWord(const TCHAR *lexeme);

%}

let     [_a-zA-Z\x80-\xff]                                /* Letter                                */
alnum   [_a-zA-Z0-9\x80-\xff]                             /* Alphanumeric character                */
d       [0-9]		                                      /* Decimal digit				           */
white   [\x00-\x09\x0b\s]                                 /* White space: all control chars but \n */
date    {d}{d}?[\-\./]{d}{d}?[\-\./]{d}{d}?{d}?{d}?
time    {d}{d}?:{d}{d}?(:{d}{d}?)?{white}*(([AaPp])[Mm])?

%%

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)? return NUMBER;
#{white}*{date}{white}*#                      return DATECONST;
#{white}*{time}{white}*#                      return TIMECONST;
#{white}*{date}{white}+{time}{white}*#        return TIMESTAMPCONST;
{alnum}+                return nameOrKeyWord((TCHAR*)getText());
\[[^\]]+\]              return NAME;
\"([^\"]|\"\")*\"	    return STRING;
'(\\.|[^\\'])*\'	    return STRING;
\"\"+                   return STRING;
\"([^\"]|\"\")*\r?\n    error(getPos(),_T("Adding missing \" to string constant"));
					    return STRING;

"<"		return RELOPLT;
"<="	return RELOPLE;
"<>"	return NOTEQ;
">"		return RELOPGT;
">="	return RELOPGE;
"="		return EQUAL;
"-"		return MINUS;
"+"		return PLUS;
"*"		return STAR;
"/"		return DIVOP;
"**"	return EXPO;
"&" 	return CONCAT;
"."		return DOT;
"!"		return EXCLAMATION;
","		return COMMA;
";"		return SEMI;
"("		return LPAR;
")"		return RPAR;
"?"		return PARAM;

{white}+	;
\n+			;
\r+			;

%%

  /* This part goes to the last part of sqllex.cpp */

typedef struct {
  TCHAR *m_name;
  int    m_token;
} KeyWord;

static KeyWord keyWordTable[] = {
  _T("ALL") 			 ,ALL 				,
  _T("AND") 			 ,AND 				,
  _T("AS")   			 ,AS 				,
  _T("ASC") 			 ,ASCENDING			,
  _T("ASCENDING")		 ,ASCENDING			,
  _T("BETWEEN") 		 ,BETWEEN 			,
  _T("BIT")              ,TYPEBIT           ,
  _T("BY")				 ,BY				,
  _T("CAST")		     ,CAST				,
  _T("COUNT")			 ,COUNT				,
  _T("CURRENTDATE")      ,CURRENTDATE       ,
  _T("CURRENTTIME")      ,CURRENTTIME       ,
  _T("CURRENTTIMESTAMP") ,CURRENTTIMESTAMP  ,
  _T("DATETIME")         ,TYPEDATETIME  	,
  _T("DELETE")			 ,DELETESYM			,
  _T("DESC")			 ,DESCENDING		,
  _T("DESCENDING")		 ,DESCENDING		,
  _T("DISTINCT")		 ,DISTINCT			,
  _T("DISTINCTROW")		 ,DISTINCTROW		,
  _T("DOUBLE")			 ,TYPEDOUBLE		,
  _T("DROP")			 ,DROP				,
  _T("EXISTS")			 ,EXISTS			,
  _T("FALSE")            ,BOOLFALSE         ,
  _T("FROM")			 ,FROM				,
  _T("GROUP")			 ,GROUp				,
  _T("HAVING")			 ,HAVING			,
  _T("IEEEDOUBLE")       ,TYPEDOUBLE        ,
  _T("IIF")              ,IIF               ,
  _T("IN")				 ,INSYM				,
  _T("INDEX")			 ,INDEX				,
  _T("INNER")			 ,INNER				,
  _T("INSERT")			 ,INSERT			,
  _T("INTERSECT")		 ,INTERSECT			,
  _T("INTO")			 ,INTO				,
  _T("IS")				 ,IS				,
  _T("JOIN")			 ,JOIN				,
  _T("LEFT")			 ,LEFT				,
  _T("LIKE")			 ,LIKE				,
  _T("LONG")			 ,TYPELONG			,
  _T("MINUS")			 ,SETDIFFERENCE		, // symbol MINUS is -
  _T("MOD")			     ,MODOP				,
  _T("NOT") 			 ,NOT 				,
  _T("NULL")			 ,NULLVAL 			,
  _T("ON")				 ,ON				,
  _T("OR")				 ,OR				,
  _T("ORDER")			 ,ORDER				,
  _T("OUTER")			 ,OUTER				,
  _T("PARAMETERS")       ,PARAMETERS        ,
  _T("PERCENT")          ,PERCENT           ,
  _T("PIVOT")            ,PIVOT             ,
  _T("RIGHT")            ,RIGHT             ,
  _T("SELECT")			 ,SELECT			,
  _T("SET") 			 ,SET 				,
  _T("TABLE")			 ,TABLE				,
  _T("TEXT")             ,TYPETEXT          ,
  _T("TOP")              ,TOP               ,
  _T("TRANSFORM")        ,TRANSFORM         ,
  _T("TRUE")             ,BOOLTRUE          ,
  _T("UNION")			 ,UNION				,
  _T("UPDATE")			 ,UPDATE			,
  _T("VALUE") 			 ,TYPEVALUE	        ,
  _T("VALUES")			 ,VALUES			,
  _T("WHERE")			 ,WHERE
};

typedef StrHashMap<int> HashMapType;

class KeyWordHashMap : public HashMapType {
public:
  KeyWordHashMap(int tablesize) : HashMapType(tablesize) {
    for(int i = 0; i < ARRAYSIZE(keyWordTable); i++)
      put(keyWordTable[i].m_name, keyWordTable[i].m_token);

//	  printf("maxlength:%d\n",maxlength());
//	  iterator it = getiterator();
//	  for(it.first(); it.current(); it.next()) {
//		printf("%-20s %d\n",it.key(),it.elem());
//	  }
  }
};

static KeyWordHashMap keywords(299);

static int nameOrKeyWord(const TCHAR *lexeme) {
  TCHAR tmp[100];
  int *p = keywords.get(strToUpperCase(_tcscpy(tmp, lexeme)));
  return p ? *p : NAME;
}

SqlLex::SqlLex(LRparser *parser) {
  m_parser = parser;
}

void SqlLex::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  if(m_parser != NULL) {
    m_parser->verror(pos, format, argptr); // redirect to parser
  } else {
    _vtprintf(format, argptr);
  }
}

void SqlLex::findBestHashMapSize() {
  for(int tableSize = 57; tableSize < 2000; tableSize++) {
    KeyWordHashMap ht(tableSize);
	if(ht.getMaxChainLength() == 1) {
	  _tprintf(_T("Tablesize:%d gives best hashmap\n"), tableSize);
	  exit(0);
	}
  }
}
