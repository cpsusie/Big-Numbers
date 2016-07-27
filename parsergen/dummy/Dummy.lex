%{

class DummyLex : public Scanner {
public:
  int getNextLexeme();
};
%}

%{

#include <MyUtil.h>
#include <string.h>
#include <hashmap.h>
#include "dummylex.h"
#include "dummysymbol.h"

static int nameOrKeyWord(const char *lexeme);

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

#ifdef __NEVER__
  /* This part goes to the last part of dummylex.cpp */

typedef struct {
  char *m_name;
  int   m_token;
} KeyWord;

static KeyWord keywordtable[] = {
 "WHILE"    ,WHILE
,"DO"		,DO
,"IF"       ,IF
,"THEN"     ,THEN
,"ELSE"     ,ELSE
,"BEGIN"    ,BEGIN
,"END"		,END
,"SELECT"	,SELECT
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

static int nameOrKeyWord(const char *lexeme) {
  char tmp[100];
  int *p = keywords.find(strToUpperCase(strcpy(tmp,lexeme)));
  if(p) return *p;
  else {
    printf("lexeme:<%s>\n",lexeme);
    return NAME;
  }
}
#endif
