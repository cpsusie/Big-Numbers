%{

class DummyLex : public Scanner {
public:
  int getNextLexeme();
};
%}

%{

#include "stdafx.h"
#include <HashMap.h>
#include "dummylex.h"
#include "dummysymbol.h"

static int nameOrKeyWord(const TCHAR *lexeme);

%}

let     [Ê¯Â∆ÿ≈_a-zA-Z]			/* Letter					*/
alnum   [Ê¯Â∆ÿ≈_a-zA-Z0-9]		/* Alphanumeric character	*/
d       [0-9]					/* Decimal digit			*/
white   [\x00-\x09\x0b\s\n]		/* White space:				*/
%%

"a"|"A"   return a;
"b"|"B"   return b;
{white}+	;

%%

#include "stdafx.h"

#ifdef __NEVER__

/* This part goes to the last part of dummylex.cpp */

typedef struct {
  TCHAR *m_name;
  int    m_token;
} KeyWord;

static KeyWord keywordtable[] = {
 _T("WHILE")    ,WHILE
,_T("DO")		,DO
,_T("IF")       ,IF
,_T("THEN")     ,THEN
,_T("ELSE")     ,ELSE
,_T("BEGIN")    ,BEGIN
,_T("END")		,END
,_T("SELECT")	,SELECT
};

typedef StrHashMap<int> HashMapType;

class KeyWordMap : public HashMapType {
public:
  KeyWordMap() : HashMapType(10) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++)
      insert(keywordtable[i].m_name,keywordtable[i].m_token);
  }
};

static KeyWordMap keywords;

static int nameOrKeyWord(const TCHAR *lexeme) {
  TCHAR tmp[100];
  int *p = keywords.find(strToUpperCase(tcscpy(tmp,lexeme)));
  if(p) return *p;
  _tprintf(_T("lexeme:<%s>\n"), lexeme);
  return NAME;
}

#endif
