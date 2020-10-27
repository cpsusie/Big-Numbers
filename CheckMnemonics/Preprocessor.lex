%{
#pragma once

 /* This part goes to the PreprocessorLex.h file */

#include <LRParser.h>

class PreprocessorLex : public Scanner {
private:
  LRparser *m_parser; // if set, redirect errors to parsers verror-routine
public:
  PreprocessorLex(LexStream *inputStream = nullptr) : Scanner(inputStream) {
    m_parser = nullptr;
  }
  void setParser(LRparser *parser) {
    m_parser = parser;
  }
  int  getNextLexeme() override;
  void verror(const SourcePosition &pos, const TCHAR *format, va_list argptr) override;
  static void findBestHashMapSize();
};

%}

%{

 /* This part goes to the first part of the PreprocessorLex.cpp file */

#include "stdafx.h"
#include <HashMap.h>
#include <TreeMap.h>
#include "PreprocessorSymbol.h"

static PreprocessorInputSymbol nameOrKeyWord(const char *lexeme);
static PreprocessorInputSymbol preprocssessorDirective(const char *lexeme);

%}

preplet [a-z]
let     [_a-zA-Z]             /* Letter                                 */
alnum   [_a-zA-Z0-9]          /* Alphanumeric character                 */
d       [0-9]		          /* Decimal digit				            */
h		[0-9a-fA-F]			  /* Hexadecimal digit					    */
white   [\x00-\x09\x0b\s]     /* White space: all control chars but \n  */

%%

{d}+					|
0[xX]{h}+				return NUMBER;
\"(\\.|[^\\\"])*\"		return STRING;

"|"	                    return OR;
","		                return COMMA;
"//"                    { int i;
                          while(i = input()) {
                            if(i < 0)
                              flushBuf();  /* Discard lexeme. */
                            else if(i == '\n')
                              break;
                          }
                        }

#{preplet}+             return preprocssessorDirective((char*)getText());
{let}{alnum}*           return nameOrKeyWord((char*)getText());
{white}+		        ;
\n+				        ;
\r+				        ;

%%

  /* This part goes to the last part of PreprocessorLex.cpp */

typedef struct {
  char                   *m_name;
  PreprocessorInputSymbol m_token;
} KeyWord;

#define KW(w)  #w, w
#define UKW(w) #w, _##w


static const KeyWord prepKeywordTable[] = {
  KW(DEFINE)
 ,KW(ENDIF)
 ,KW(IF)
 ,KW(IFDEF)
 ,KW(INCLUDE)
 ,KW(PRAGMA)
 ,KW(UNDEF)
};

typedef StrHashMap<PreprocessorInputSymbol> HashMapType;

class PreprocessorKeyWordMap : public HashMapType {
public:
  PreprocessorKeyWordMap(int capacity, const KeyWord *initTable, int size) : HashMapType(capacity) {
    for(int i = 0; i < size; i++) {
      put(initTable[i].m_name, initTable[i].m_token);
    }
  }
};

#define DEFINE_KEYWORDTABLE(name, capacity, initTable) \
  static PreprocessorKeyWordMap name(capacity, initTable, ARRAYSIZE(initTable));

DEFINE_KEYWORDTABLE(keywords    , 1314, keywordTable    )
DEFINE_KEYWORDTABLE(prepKeywords,   11, prepKeywordTable)

static PreprocessorInputSymbol nameOrKeyWord(const char *lexeme) {
  PreprocessorInputSymbol *p = keywords.get(lexeme);
  return p ? *p : NAME;
}

static PreprocessorInputSymbol preprocssessorDirective(const char *lexeme) {
  PreprocessorInputSymbol *p = prepKeywords.get(lexeme+1); // skip '#'
  if(p) return *p;
  error(getPos(), "unknown preprocessordirective:%s\n", lexeme);
}

#ifdef _DEBUG

class MapValue {
public:
  int      m_maxChainLength;
  IntArray m_chainCount;
  MapValue(int maxChainLength, const IntArray &chainCount) 
  : m_maxChainLength(maxChainLength), m_chainCount(chainCount) {
  }
};

void PreprocessorLex::findBestHashMapSize() {
  IntTreeMap<MapValue> cl;
  int bestCapacity;
  int currentMax = -1;
  for(int capacity = 1314; capacity < 1315; capacity++) {
    PreprocessorKeyWordMap ht(capacity);
    MapValue v(ht.getMaxChainLength(), ht.getLength());
	cl.put(ht.getCapacity(), v);
	if((currentMax < 0) || v.m_maxChainLength < currentMax) {
	  bestCapacity = capacity;
      currentMax = v.m_maxChainLength;
	}
    if(currentMax == 1) {
      break;
    }
  }

  for(Iterator<Entry<int, MapValue> > it = cl.entrySet().getIterator(); it.hasNext();) {
    Entry<int, MapValue > &e = it.next();
    const int capacity = e.getKey();
	const MapValue &v = e.getValue();
    if(v.m_maxChainLength == currentMax && v.m_chainCount[1] >= 213) {
	  printf("Capacity %4d:", capacity);
	  for(int l = 0; l < v.m_chainCount.size(); l++) {
        printf(" (%d,%3d)", l, v.m_chainCount[l]);
	  }
	  printf("\n");
    }
  }
  printf("Capacity:%d gives best hashmap. max chainlenght:%d\n",bestCapacity, currentMax);
}

#endif

void PreprocessorLex::verror(const SourcePosition &pos, const char *format, va_list argptr) {
  if(m_parser) {
    m_parser->verror(pos,format,argptr);
  } else {
    Scanner::verror(pos,format,argptr);
  }
}

