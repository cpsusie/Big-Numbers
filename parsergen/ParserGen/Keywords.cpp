#include "stdafx.h"
#include <CompactHashMap.h>
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

typedef CompactStrHashMap<Token,ARRAYSIZE(keywordtable)> HashMapType;

class KeyWordMap : public HashMapType {
public:
  KeyWordMap(UINT capacity) : HashMapType(capacity) {
    for(int i = 0; i < ARRAYSIZE(keywordtable); i++) {
      put(keywordtable[i].m_name, keywordtable[i].m_token);
    }
  }
};

static KeyWordMap keywords(20);

Token nameOrKeyWord( const TCHAR *name ) {
  const Token *p = keywords.get(name);
  return p ? *p : NAME;
}

// #define CHECKKEYWORDMAP

#ifdef CHECKKEYWORDMAP

class CheckKeywordMap {
private:
  void findBestHashMapSize();
public:
  CheckKeywordMap() {
    findBestHashMapSize();
  }
};

void CheckKeywordMap::findBestHashMapSize() { // static
  int bestTableSize   = -1;
  int bestChainLength = 0;
  for(int tableSize = 20; tableSize < 2000; tableSize++) {
    KeyWordMap ht(tableSize);
    const int chainLength = ht.getMaxChainLength();
    debugLog(_T("tableSize:%4d, maxChainLength:%d\n"), tableSize, chainLength);
    if((bestTableSize < 0) || (chainLength < bestChainLength)) {
      bestTableSize   = tableSize;
      bestChainLength = chainLength;
    }
    if(bestChainLength == 1) {
      break;
    }
  }
  debugLog(_T("\nTablesize=%d gives best hashmap (maxchainLength=%u\n")
          ,bestTableSize, bestChainLength);

  exit(0);
}

static CheckKeywordMap checkKeywordMap;

#endif
