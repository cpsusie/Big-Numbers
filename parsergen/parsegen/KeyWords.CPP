#include "stdafx.h"
#include <HashMap.h>
#include "GrammarScanner.h"

typedef struct {
  TCHAR *m_name;
  Token m_token;
} KeyWord;

static KeyWord keywordtable[] = {
  _T("token"   ) , TERM
 ,_T("term"    ) , TERM
 ,_T("nonassoc") , NONASSOC
 ,_T("left"    ) , LEFT
 ,_T("right"   ) , RIGHT
 ,_T("prec"    ) , PREC
};

typedef StrHashMap<Token> HashMapType;

class KeyWordMap : public HashMapType {
public:
  KeyWordMap() : HashMapType(10) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++) {
      put(keywordtable[i].m_name, keywordtable[i].m_token);
    }
  }
};

static KeyWordMap keywords;

Token nameOrKeyWord( const TCHAR *name ) {
  Token *p = keywords.get(name);
  return p ? *p : NAME;
}
