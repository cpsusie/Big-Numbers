#include "stdafx.h"
#include <BitStream.h>
#include "IndexedMap.h"

#ifdef TABLEBASE_BUILDER

#ifdef _DEBUG

UINT PackedIndexedMap::getCheckedIndex(const EndGameKey &key) const {
  const UINT result = m_keydef.keyToIndex(key);
  if(result >= m_indexSize) {
    UINT ii = m_keydef.keyToIndex(key);
    throwException(_T("Position [%s] has index %lu. Max=%lu"), key.toString(m_keydef).cstr(), result, m_indexSize-1);
  }
  return result;
}

#define GETELEMENT(key) m_statusArray.get(getCheckedIndex(key))

#else

#define GETELEMENT(key) m_statusArray.get(m_keydef.keyToIndex(key))

#endif

void PackedIndexedMap::rethrowException(Exception &e, const EndGameKey &key) const {
  throwException(_T("%s. key:[%s], index:%s, size:%s\n"), e.what()
                ,key.toString(m_keydef).cstr()
                ,format1000(m_keydef.keyToIndex(key)).cstr()
                ,format1000(m_statusArray.size()).cstr());
}

static const EndGamePositionStatus statusConversion[] = {
  EG_DRAW
 ,EG_DRAW
 ,EG_WHITEWIN
 ,EG_BLACKWIN
};

PackedIndexedMap::PackedIndexedMap(const EndGameKeyDefinition &keydef, bool enableGetResult, UINT maxPlies) 
: m_keydef(keydef)
, m_indexSize(keydef.getIndexSize())
, m_getResultEnabled(enableGetResult)
, m_statusArray(enableGetResult ? findBitsPerItem(maxPlies) : 2)
{
}

void PackedIndexedMap::load(ByteInputStream &s) {
  UINT size;
  s.getBytesForced((BYTE*)&size, sizeof(size));
  if(size != m_indexSize) {
    throwException(_T("Unexpected indexSize=%s read from stream. Expected %s"), format1000(size).cstr(), format1000(m_indexSize).cstr());
  }
  clear();
  m_statusArray.setCapacity(m_indexSize);
  m_statusArray.addZeroes(0, m_indexSize);
  
  EndGameResult *buffer = NULL;
#define BUFFERSIZE 30000
  try {
    buffer = new EndGameResult[BUFFERSIZE];
 
    for(int i = 0; i < m_indexSize;) {
      int n = min(BUFFERSIZE, m_indexSize-i);
      s.getBytesForced((BYTE*)buffer, n*sizeof(buffer[0]));
      if(m_getResultEnabled) {
        for(const EndGameResult *egrp = buffer; n--; egrp++) {
          if(!egrp->exists()) {
            m_statusArray.set(i++,EG_UNDEFINED);
          } else {
            m_statusArray.set(i++, statusConversion[egrp->getStatus()] | (egrp->getPliesToEnd() << 2));
          }
        }
      } else {
        for(const EndGameResult *egrp = buffer; n--; egrp++) {
          if(!egrp->exists()) {
            m_statusArray.set(i++,EG_UNDEFINED);
          } else {
            m_statusArray.set(i++, statusConversion[egrp->getStatus()]);
          }
        }
      }
    }
    delete[] buffer;
  } catch(Exception e) {
    delete[] buffer;
    throw e;
  } catch(...) {
    delete[] buffer;
    throw;
  }
}

int PackedIndexedMap::findBitsPerItem(UINT maxPlies) { // static
  for(int l = 2; maxPlies; maxPlies >>= 1, l++);
  return l;
}

EndGamePositionStatus PackedIndexedMap::getPositionStatus(const EndGameKey &key) const {
  return (EndGamePositionStatus)(GETELEMENT(key) & 3);
}

EndGameResult PackedIndexedMap::getPositionResult(const EndGameKey &key) const {
  if(!m_getResultEnabled) {
    throwException(_T("PackedIndexedMap::getPositionResult:Unsupported operation. Index initialized with enableGetResult=false"));
  }
  const UINT v = GETELEMENT(key);
  return EndGameResult((EndGamePositionStatus)(v&3), v >> 2);
}

#endif
