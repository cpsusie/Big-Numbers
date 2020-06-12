#include "stdafx.h"
#include <ByteFile.h>
#include <CountedByteStream.h>
#include <BitStream.h>
#include <TimeEstimator.h>
#include "IndexedMap.h"

#if defined(NEWCOMPRESSION)

#if defined(TABLEBASE_BUILDER)

#if defined(DEBUG_NEWCOMPRESSION)
void IndexedMap::listCompressedContent(const TablebaseInfo &info, String fileName) const {
  FILE *f = NULL;
  if(fileName.length() == 0) {
    fileName = m_keydef.getTbFileName(COMPRESSEDDUMP);
  }
  try {
    f = MKFOPEN(fileName, _T("w"));
    const EndGameResult *firstElement = &first();
    const EndGameResult *lastElement  = getLastElement();
    UINT64 winnerPositions = 0;
    UINT64 nzCount         = 0;
    const BYTE canWinFlags = info.getCanWinFlags();
    _ftprintf(f, _T("%s, %s\n"), m_keydef.toString().cstr(), EndGameKeyDefinition::getMetricName().cstr());
    _ftprintf(f, _T("%s"  ), info.toString(TBIFORMAT_PRINT_ALL, true).cstr());
    const UINT  maxMoves       = PLIESTOMOVES(info.m_maxPlies.getMax());
    const BYTE  ignoreBitCount = 2;
    const UINT  ignoreBitMask  = (1<<ignoreBitCount)-1;
    const BYTE  bitsPerEntry   = getBitCount(maxMoves) - ignoreBitCount + ((canWinFlags == BOTHCANWIN) ? 1 : 0);
    const UINT  blackWinBit    = (canWinFlags == BOTHCANWIN) ? (1 << (bitsPerEntry-1)) : 0;
    const TCHAR winnerChar     = blackWinBit ? 0 : ((canWinFlags&WHITECANWIN) ? 'W' : 'B');

    for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
      if(ep->isWinner()) {
        const EndGamePosIndex index      = ep - firstElement;
        const EndGameKey      key        = m_keydef.indexToKey(index);
        const UINT            movesToEnd = ep->getMovesToEnd();
        String                nzStr      = EMPTYSTRING;
        if((movesToEnd&ignoreBitMask)==0) {
          const UINT code = (movesToEnd >> ignoreBitCount) | ((ep->getStatus()==EG_WHITEWIN) ? 0 : blackWinBit);
          if(code) {
            nzStr = format(_T(" %2d [%s]"), code, format1000(nzCount++).cstr());
          }
        }
        _ftprintf(f,_T("%11s(%s)->%11s:(%c) %3u plies%s\n")
                   ,format1000(index).cstr()
                   ,key.toString(m_keydef).cstr()
                   ,format1000(winnerPositions).cstr()
                   ,winnerChar ? winnerChar : (ep->getStatus()==EG_WHITEWIN)? 'W' : 'B'
                   ,ep->getPliesToEnd()
                   ,nzStr.cstr()
                 );
        winnerPositions++;
      }
    }
    fclose(f); f = NULL;
  } catch(...) {
    if(f != NULL) {
      fclose(f);
      throw;
    }
  }
}
#endif // DEBUG_NEWCOMPRESSION


void IndexedMap::saveCompressed(BigEndianOutputStream &s, const TablebaseInfo &info) const {
// Saved in format used by IndexedMap::load(ByteInputStream &s) defined last in this file

  DEFINEMETHODNAME;
  const EndGamePosIndex indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(method, _T("info.indexCapacity=%I64u, keydef.indexSize=%I64u"), info.m_indexCapacity, indexSize);
  }
  const EndGameResult *firstElement = &first();
  const EndGameResult *lastElement  = getLastElement();
  BYTE                 canWinFlags  = 0;
  for(const EndGameResult *ep = firstElement; ep <= lastElement && (canWinFlags != BOTHCANWIN); ep++) {
    switch(ep->getStatus()) {
    case EG_WHITEWIN: canWinFlags |= WHITECANWIN; break;
    case EG_BLACKWIN: canWinFlags |= BLACKCANWIN; break;
    default         :;
    }
  }

  if((info.getCanWinFlags() != canWinFlags) || (canWinFlags == 0)) {
    throwInvalidArgumentException(method, _T("CanWinFlags inconsistent. info.canWinFlags:%d, counted:%d")
                                 ,info.getCanWinFlags()
                                 ,canWinFlags
                                 );
  }

  UINT   maxPly          = 0;
  UINT64 winnerPositions = 0;
  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) {
      winnerPositions++;
      if(ep->getPliesToEnd() > maxPly) {
        maxPly = ep->getPliesToEnd();
      }
    }
  }

  if(winnerPositions != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions(=%s) != info.totalWinnerPosition(=%s)")
                  ,format1000(winnerPositions).cstr()
                  ,info.getWinnerPositionCount().toStringWithTotal().cstr()
                  );
  }

  if(maxPly != info.m_maxPlies.getMax()) {
    throwInvalidArgumentException(method
                                 ,_T("Calculated maxPly=%u != info.maxPlies:%s")
                                 ,maxPly
                                 ,info.m_maxPlies.toString().cstr());
  }

  const UINT maxMoves    = PLIESTOMOVES(maxPly);

#if defined(DEBUG_NEWCOMPRESSION)
  listCompressedContent(info);
#endif

  BitSet wpSet(indexSize);
  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) {
      wpSet.add(ep-firstElement);
    }
  }

  const BYTE  ignoreBitCount = 2;  // drop 2 least significand bits of movesToEnd
  const UINT  ignoreBitMask  = (1<<ignoreBitCount)-1;
  const BYTE  bitsPerEntry   = getBitCount(maxMoves) - ignoreBitCount + ((canWinFlags == BOTHCANWIN) ? 1 : 0);
  size_t      dstIndex       = 0;
  PackedArray infoArray(bitsPerEntry);
  infoArray.setCapacity(winnerPositions);
  infoArray.addZeroes(0, winnerPositions);
  if(canWinFlags == BOTHCANWIN) { // if(canWinflags != BOTHCANWIN) then no need for extra bit in each position to indicate winnner.
                                  // A winnerposition is the one indicated by canWinFlags
    const UINT blackWinBit = (1 << (bitsPerEntry-1));
    for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
      if(ep->isWinner()) {
        const UINT m = ep->getMovesToEnd();
        if((m & ignoreBitMask) == 0) {
          const UINT code = (m >> ignoreBitCount) | ((ep->getStatus()==EG_WHITEWIN) ? 0 : blackWinBit);
          infoArray.set(dstIndex, code);
        }
        dstIndex++;
      }
    }
  } else {
    for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
      if(ep->isWinner()) {
        const UINT m = ep->getMovesToEnd();
        if((m & ignoreBitMask) == 0) {
          const UINT code = m >> ignoreBitCount;
          infoArray.set(dstIndex, code);
        }
        dstIndex++;
      }
    }
  }
  assert(dstIndex == winnerPositions);
  wpSet.save(s);
  infoArray.save(s);
}

#else // !TABLEBASE_BUILDER ie chess-program

class DecompressedHeader : public TablebaseInfo {
public:
  UINT64 m_wpIndexAddress;
  UINT64 m_nzIndexAddress;
  UINT64 m_infoArrayAddress;

  DecompressedHeader(const TablebaseInfo &info);
  DecompressedHeader(const String &fileName) {
    load(ByteInputFile(fileName));
  }
  void save(ByteOutputStream &s) {
    s.putBytes((BYTE*)this, sizeof(DecompressedHeader));
  }
  void load(ByteInputStream  &s) {
    s.getBytesForced((BYTE*)this, sizeof(DecompressedHeader));
  }
};

DecompressedHeader::DecompressedHeader(const TablebaseInfo &info) : TablebaseInfo(info) {
  m_wpIndexAddress   = 0;
  m_nzIndexAddress   = 0;
  m_infoArrayAddress = 0;
}

IndexedMap::IndexedMap(const EndGameKeyDefinition &keydef) : m_keydef(keydef) {
  init();
}

void IndexedMap::init() {
  m_wpIndex   = NULL;
  m_nzIndex   = NULL;
  m_infoArray = NULL;
}

IndexedMap::~IndexedMap() {
  clear();
}

void IndexedMap::clear() {
  SAFEDELETE(m_wpIndex  );
  SAFEDELETE(m_nzIndex  );
  SAFEDELETE(m_infoArray);
}

static UINT64 getNonZeroCount(const PackedArray &a) {
  const UINT64 n     = a.size();
  UINT64       count = 0;
  for(UINT64 i = 0; i < n;) {
    if(a.get(i++)) count++;
  }
  return count;
}

// Read format written with IndexedMap::saveCompressed(BigEndianOutputStream &s);
void IndexedMap::decompress(BigEndianInputStream &in, const TablebaseInfo &info) const {
  const INT64   indexSize       = info.m_indexCapacity;
  const BYTE    canWinFlags     = info.getCanWinFlags();
  const UINT64  winnerPositions = info.getWinnerPositionCount().getTotal();
  const UINT    maxPly          = info.m_maxPlies.getMax();
  const UINT    maxMoves        = PLIESTOMOVES(maxPly);

  if(indexSize != m_keydef.getIndexSize()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("info.indexCapacity=%s, keydef.indexSize=%s")
                                 ,format1000(indexSize).cstr()
                                 ,format1000(m_keydef.getIndexSize()).cstr());
  }

  BitSet      wpSet((size_t)indexSize);
  PackedArray infoArray(1);
  wpSet.load(in);
  infoArray.load(in);

  const UINT64 wpSetSize = wpSet.size();
  if(wpSetSize != winnerPositions) {
    throwException(_T("Counted winnerPositions(=%s) != info.winnerPosition(=%s)")
                  ,format1000(wpSetSize).cstr()
                  ,info.getWinnerPositionCount().toStringWithTotal().cstr());
  }
  if(infoArray.size() != winnerPositions) {
    throwException(_T("infoArray.size(=%s) != info.WinnerPosition(=%s)")
                  ,format1000(infoArray.size()).cstr()
                  ,info.getWinnerPositionCount().toStringWithTotal().cstr());
  }

  const UINT64 nzCount   = getNonZeroCount(infoArray);
  BitSet       nzSet((size_t)winnerPositions);
  PackedArray  nzInfoArray(infoArray.getBitsPerItem());
  nzInfoArray.setCapacity(nzCount);
  nzInfoArray.addZeroes(0,nzCount);
  UINT64 dstIndex = 0;
  for(UINT64 i = 0; i < winnerPositions; i++) {
    const UINT m = infoArray.get(i);
    if(m) {
      nzSet.add(i);
      nzInfoArray.set(dstIndex++, m);
    }
  }
  assert(dstIndex == nzCount);
  if(nzSet.getCapacity() != winnerPositions) {
    throwException(_T("nzSet.capacity(=%s) != winnerPosition(=%s)")
                  ,format1000(nzSet.getCapacity()).cstr()
                  ,info.getWinnerPositionCount().toStringWithTotal().cstr());
  }
  if(nzSet.size() != nzInfoArray.size()) {
    throwException(_T("nzSet.size(=%s) != nzInfoArray.size(=%s)")
                  ,format1000(nzSet.size()).cstr()
                  ,format1000(nzInfoArray.size()).cstr());
  }

  // save decompressed format
  ByteOutputFile          file(m_keydef.getTbFileName(DECOMPRESSEDTABLEBASE));
  ByteCounter             outputCounter;
  CountedByteOutputStream out(outputCounter, file);

  DecompressedHeader      header(info); // some parts undefined. define it below and rewrite it
  BitSetIndex             wpIndex(wpSet);
  BitSetIndex             nzIndex(nzSet);

  header.save(out);
  header.m_wpIndexAddress   = outputCounter.getCount();
  wpIndex.save(out);
  header.m_nzIndexAddress   = outputCounter.getCount();
  nzIndex.save(out);
  header.m_infoArrayAddress = outputCounter.getCount();
  nzInfoArray.save(out);

  file.seek(0);
  header.save(file);
}

void IndexedMap::load() {
  clear();
  const String fileName = m_keydef.getTbFileName(DECOMPRESSEDTABLEBASE);
  DecompressedHeader header(fileName);
  m_canWinFlags    = header.getCanWinFlags();
  m_wpIndex        = new FileBitSetIndex(fileName, header.m_wpIndexAddress  ); TRACE_NEW(m_wpIndex);
  m_nzIndex        = new FileBitSetIndex(fileName, header.m_nzIndexAddress  ); TRACE_NEW(m_nzIndex);
  m_infoArray      = new PackedFileArray(fileName, header.m_infoArrayAddress); TRACE_NEW(m_infoArray);
  if(m_wpIndex->getCapacity() != m_keydef.getIndexSize()) {
    throwException(_T("wpIndex.capacity(=%s) != keydef.indexSize(=%s)")
                  ,format1000(m_wpIndex->getCapacity()).cstr()
                  ,format1000(m_keydef.getIndexSize()).cstr());
  }
  if(m_wpIndex->size() != m_nzIndex->getCapacity()) {
    throwException(_T("wpIndex.size(=%s) != nzIndez.capacity(=%s)")
                  ,format1000(m_wpIndex->size()).cstr()
                  ,format1000(m_nzIndex->getCapacity()).cstr());
  }
  if(m_nzIndex->size() != m_infoArray->size()) {
    throwException(_T("nzIndex.size(=%s) != infoArray.size(=%s)")
                  ,format1000(m_nzIndex->size()).cstr()
                  ,format1000(m_infoArray->size()).cstr());
  }
}

EndGameResult IndexedMap::get(EndGameKey key) const {
  if(!isAllocated()) {
    throwException(_T("%s:Index %s not loaded"), __TFUNCTION__, m_keydef.getName().cstr());
  }
  const intptr_t wpIndex = m_wpIndex->getIndex((size_t)m_keydef.keyToIndex(key));
  if(wpIndex < 0) {
    return EGR_DRAW;
  } else {
    Player winner;
    if(m_canWinFlags != BOTHCANWIN) {
      winner = (m_canWinFlags | WHITECANWIN) ? WHITEPLAYER : BLACKPLAYER;
    }
    const intptr_t nzIndex = m_nzIndex->getIndex(wpIndex);
    if(nzIndex < 0) {
      return EndGameResult::getPrunedWinnerResult();
    } else {
      const UINT info = m_infoArray->get(nzIndex);
      UINT       movesToEnd;
      const BYTE ignoreBitCount = 2;
      if(m_canWinFlags != BOTHCANWIN) {
        movesToEnd = info << ignoreBitCount;
      } else {
        const BYTE bitsPerEntry = m_infoArray->getBitsPerItem();
        const UINT blackWinBit  = (1 << (bitsPerEntry-1));
        movesToEnd = (info & ~blackWinBit) << ignoreBitCount;
        winner     = (info & blackWinBit) ? BLACKPLAYER : WHITEPLAYER;
      }
      return EGR_WINNER(key.getPlayerInTurn(), winner, movesToEnd);
    }
  }
}
#endif // !TABLEBASE_BUILDER

#endif // NEWCOMPRESSION
