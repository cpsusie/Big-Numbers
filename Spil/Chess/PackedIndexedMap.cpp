#include "stdafx.h"
#include <BitStream.h>
#include "IndexedMap.h"

#if defined(TABLEBASE_BUILDER)

#if defined(_DEBUG)

EndGamePosIndex PackedIndexedMap::getCheckedIndex(EndGameKey key) const {
  const EndGamePosIndex result = m_keydef.keyToIndex(key);
  if(result >= m_indexSize) {
    const EndGamePosIndex ii = m_keydef.keyToIndex(key);
    throwException(_T("Position [%s] has index %llu. Max=%llu")
                  ,key.toString(m_keydef).cstr()
                  ,result
                  ,m_indexSize-1);
  }
  return result;
}

#define GETELEMENT(key) m_statusArray.get(getCheckedIndex(key))

#else

#define GETELEMENT(key) m_statusArray.get(m_keydef.keyToIndex(key))

#endif // _DEBUG

void PackedIndexedMap::rethrowException(Exception &e, EndGameKey key) const {
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
  UINT64 size;
  s.getBytesForced((BYTE*)&size, sizeof(size));
  if(size != m_indexSize) {
    throwException(_T("Unexpected indexSize=%s read from stream. Expected %s")
                  ,format1000(size).cstr()
                  ,format1000(m_indexSize).cstr());
  }
  clear();
  m_statusArray.setCapacity(m_indexSize);
  m_statusArray.insertZeroes(0, m_indexSize);

  EndGameResult *buffer = NULL;
#define BUFFERSIZE 30000
  try {
    buffer = new EndGameResult[BUFFERSIZE]; TRACE_NEW(buffer);

    for(size_t i = 0; i < m_indexSize;) {
      UINT64 n = min(BUFFERSIZE, m_indexSize-i);
      s.getBytesForced((BYTE*)buffer, (size_t)n*sizeof(buffer[0]));
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
    SAFEDELETEARRAY(buffer);
  } catch(Exception e) {
    SAFEDELETEARRAY(buffer);
    throw e;
  } catch(...) {
    SAFEDELETEARRAY(buffer);
    throw;
  }
}

int PackedIndexedMap::findBitsPerItem(UINT maxPlies) { // static
  int l;
  for(l = 2; maxPlies; maxPlies >>= 1, l++);
  return l;
}

EndGamePositionStatus PackedIndexedMap::getPositionStatus(EndGameKey key) const {
  return (EndGamePositionStatus)(GETELEMENT(key) & 3);
}

EndGameResult PackedIndexedMap::getPositionResult(EndGameKey key) const {
  if(!m_getResultEnabled) {
    throwException(_T("%s:Unsupported operation. Index initialized with enableGetResult=false")
                  ,__TFUNCTION__);
  }
  const UINT v = GETELEMENT(key);
  return EndGameResult((EndGamePositionStatus)(v&3), v >> 2);
}

#endif // TABLEBASE_BUILDER
