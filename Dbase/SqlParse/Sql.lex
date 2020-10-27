%{
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

%}

%{

 /* This part goes to the first part of SqlLex.cpp */

#include "stdafx.h"
#include <HashMap.h>
#include "SqlLex.h"
#include "SqlSymbol.h"

static int nameOrKeyWord(const TCHAR *lexeme);

%}

let     [_a-zA-Z]              /* Letter                                */
alnum   [_a-zA-Z0-9]           /* Alphanumeric character                */
d       [0-9]		           /* Decimal digit				            */
white   [\x00-\x09\x0b\s]      /* White space: all control chars but \n */
extchar [\x80-\xfe]
date    {d}{d}?[\-\./]{d}{d}?[\-\./]{d}{d}?{d}?{d}?
time    {d}{d}?:{d}{d}?(:{d}{d}?)?

%%

({d}+|{d}+\.{d}*|{d}*\.{d}+)([eE][\-+]?{d}+)? return NUMBER;
#{date}#                                      return DATECONST;
#{time}#                                      return TIMECONST;
#{date}{white}+{time}#                        return TIMESTAMPCONST;

\?{let}{alnum}*       return PARAM;
:{let}{alnum}*	      return HOSTVAR;
{let}{alnum}*	      return nameOrKeyWord((TCHAR*)getText());

\"(\\.|[^\\\"])*\"	  return STRING;

\"(\\.|[^\\\"])*\r?\n error(getPos(),_T("Adding missing \" to string constant"));
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
"%"		return MODOP;
"**"	return EXPO;
"||"	return CONCAT;
"."		return DOT;
","		return COMMA;
"("		return LPAR;
")"		return RPAR;
"/*"		{   int i;
				while( i = input() ) {
				  if( i < 0 )
					flushBuf(); 	/* Discard lexeme. */
				  else if( i == '*'  &&  look(1) == '/' ) {
					input();
					break;          	/* Recognized comment.*/
				  }
				}

				if( i == 0 )
				  error( getPos(), _T("End of file in comment"));
			}
"//"		{   int i;
				while( i = input() ) {
				  if( i < 0 )
					flushBuf();   		/* Discard lexeme. */
				  else 
					if( i == '\n' ) break;
				}
			}

{white}+	;
\n+			;
\r+			;

%%

  /* This part goes to the last part of SqlLex.cpp */

typedef struct {
  TCHAR *m_name;
  int    m_token;
} KeyWord;

static KeyWord keywordTable[] = {
  _T("ACCESS")			 ,ACCESSS			,
  _T("ALL") 			 ,ALL 				,
  _T("AND") 			 ,AND 				,
  _T("AS")   			 ,AS 				,
  _T("ASC") 			 ,ASCENDING			,
  _T("ASCENDING")		 ,ASCENDING			,
  _T("BETWEEN") 		 ,BETWEEN 			,
  _T("BROWSE")			 ,BROWSE			,
  _T("BY")				 ,BY				,
  _T("CAST")		     ,CAST				,
  _T("CHAR")			 ,TYPECHAR			,
  _T("COUNT")			 ,COUNT				,
  _T("CREATE")			 ,CREATE			,
  _T("CURRENTDATE")      ,CURRENTDATE       ,
  _T("CURRENTTIME")      ,CURRENTTIME       ,
  _T("CURRENTTIMESTAMP") ,CURRENTTIMESTAMP  ,
  _T("CURSOR")			 ,CURSOR			,
  _T("DATE")     		 ,TYPEDATE  		,
  _T("DECLARE") 		 ,DECLARE 			,
  _T("DEFAULT") 		 ,DEFAULT 			,
  _T("DELETE")			 ,DELETESYM			,
  _T("DESC")			 ,DESCENDING		,
  _T("DESCENDING")		 ,DESCENDING		,
  _T("DISTINCT")		 ,DISTINCT			,
  _T("DOUBLE")			 ,TYPEDOUBLE		,
  _T("DROP")			 ,DROP				,
  _T("EXCLUSIVE")		 ,EXCLUSIVE			,
  _T("EXISTS")			 ,EXISTS			,
  _T("EXPLAIN") 		 ,EXPLAIN 			,
  _T("FETCH")            ,FETCH             ,
  _T("FLOAT")			 ,TYPEFLOAT			,
  _T("FOR") 			 ,FOR 				,
  _T("FROM")			 ,FROM				,
  _T("GROUP")			 ,GROUP				,
  _T("HAVING")			 ,HAVING			,
  _T("IN")				 ,INSYM				,
  _T("INDEX")			 ,INDEX				,
  _T("INDICATOR")		 ,INDICATOR			,
  _T("INNER")			 ,INNER				,
  _T("INSERT")			 ,INSERT			,
  _T("INT") 			 ,TYPEINT 			,
  _T("INTERSECT")		 ,INTERSECT			,
  _T("INTO")			 ,INTO				,
  _T("IS")				 ,IS				,
  _T("JOIN")			 ,JOIN				,
  _T("KEY") 			 ,KEY 				,
  _T("LEFT")			 ,LEFT				,
  _T("LIKE")			 ,LIKE				,
  _T("LONG")			 ,TYPELONG			,
  _T("MAX") 			 ,MAX 				,
  _T("MIN") 			 ,MIN 				,
  _T("MODE")			 ,MODE				,
  _T("NOT") 			 ,NOT 				,
  _T("NULL")			 ,NULLVAL 			,
  _T("ON")				 ,ON				,
  _T("OR")				 ,OR				,
  _T("ORDER")			 ,ORDER				,
  _T("PRIMARY") 		 ,PRIMARY 			,
  _T("REPEATABLE")		 ,REPEATABLE		,
  _T("SELECT")			 ,SELECT			,
  _T("SET") 			 ,SET 				,
  _T("MINUS")			 ,SETDIFFERENCE		, // symbol MINUS is -
  _T("SHARE")			 ,SHARE				,
  _T("SHORT")			 ,TYPESHORT			,
  _T("SIGNED")			 ,SIGNED			,
  _T("STABLE")			 ,STABLE			,
  _T("SUBSTRING")		 ,SUBSTRING			,
  _T("SUM") 			 ,SUM 				,
  _T("TABLE")			 ,TABLE				,
  _T("TIME")             ,TYPETIME          ,
  _T("TIMESTAMP")        ,TYPETIMESTAMP     ,
  _T("UNION")			 ,UNION				,
  _T("UNIQUE")			 ,UNIQUE			,
  _T("UNSIGNED")		 ,UNSIGNED			,
  _T("UPDATE")			 ,UPDATE			,
  _T("VALUES")			 ,VALUES			,
  _T("VARCHAR") 		 ,TYPEVARCHAR 	    ,
  _T("WCHAR")			 ,TYPEWCHAR			,
  _T("WHERE")			 ,WHERE				
};

typedef StrHashMap<int> HashMapType;

class KeyWordMap : public HashMapType {
public:
  KeyWordMap(int tablesize) : HashMapType(tablesize) {
    for(int i = 0; i < ARRAYSIZE(keywordTable); i++) {
      put(keywordTable[i].m_name,keywordTable[i].m_token);
	}
  }
};

static KeyWordMap keywords(752);

static int nameOrKeyWord(const TCHAR *lexeme) {
  TCHAR tmp[100];
  int *p = keywords.get(strToUpperCase(_tcscpy(tmp,lexeme)));
  return p ? *p : NAME;
}

SqlLex::SqlLex(LRparser *parser) {
  m_parser = parser;
}

void SqlLex::verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) {
  if(m_parser != nullptr) {
    m_parser->verror(pos, format, argptr); // redirect to parser
  } else {
    _vtprintf(format, argptr);
  }
}

void SqlLex::findBestHashMapSize() { // static
  for(int tablesize = 57; tablesize < 2000; tablesize++) {
    KeyWordMap ht(tablesize);
	const int maxcl = ht.getMaxChainLength();
	_tprintf(_T("(%4d %d) "), tablesize, maxcl);
	if(maxcl == 1) {
	  _tprintf(_T("\nTablesize=%d gives best hashmap\n"), tablesize);
	  exit(0);
	}
  }
}
