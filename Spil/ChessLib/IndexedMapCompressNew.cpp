#include "stdafx.h"
#include <Date.h>
#include <AdaptiveHuffmanTree.h>
#include <ArithmeticCoder.h>
#include <ZlibCompressor.h>
#include "BitSetCompression.h"
#include "PackedArrayCompression.h"
#include "IndexedMap.h"

String packedArrayToString(const PackedArray &a) {
  String result = format(_T("Packed Array:Bits/Item:%d, Size:%s\n")
                        ,a.getBitsPerItem()
                        ,format1000(a.size()).cstr());
#define AW 4
  const size_t n = a.size();
  result += _T("        ");
  for(size_t i = 0; i < 10; i++) {
    result += format(_T("%*d"), AW, i);
  }
  result += _T("\n");
  for(size_t i = 0; i < n; i++) {
    if(i % 10 == 0) {
      result += format(_T("%7d "), i);
    }
    result += format(_T("%*d"), AW, a.get(i));
    if(((i % 10) == 9) || (i == n-1)) result += _T("\n");
  }
  return result;
}

#ifdef __NEVER__

#ifndef NEWCOMPRESSION
void IndexedMap::saveCompressedNew(ByteOutputStream &out, const TablebaseInfo &info) const {
#else
void IndexedMap::saveCompressedNew(BigEndianOutputStream &out, const TablebaseInfo &info) {
#endif
  throwException(_T("%s:%s:Not supported"), m_keydef.getName().cstr(), __TFUNCTION__);
  DEFINEMETHODNAME;
  const EndGamePosIndex indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(method, _T("info.indexCapacity=%I64u, keydef.indexSize=%I64u"), info.m_indexCapacity, indexSize);
  }
  const EndGameResult *firstElement = &first();
  const EndGameResult *lastElement  = getLastElement();
  const EndGameResult *ep           = firstElement;
  unsigned char        canWinFlags  = 0;
  for(INT64 i = indexSize; (canWinFlags != BOTHCANWIN) && i--; ep++) {
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

  UINT  maxPly          = 0;
  INT64 winnerPositions = 0;
  for(ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) {
      winnerPositions++;
      if(ep->getPliesToEnd() > maxPly) {
        maxPly = ep->getPliesToEnd();
      }
    }
  }

  if(winnerPositions != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions=%I64d != info.totalWinnerPosition:%s (=%I64u)")
                  ,winnerPositions
                  ,info.getWinnerPositionCount().toString().cstr()
                  ,info.getWinnerPositionCount().getTotal());
  }

  if(maxPly != info.m_maxPlies.getMax()) {
    throwInvalidArgumentException(method
                                 ,_T("Calculated maxPly=%u != info.maxPlies:%s")
                                 ,maxPly
                                 ,info.m_maxPlies.toString().cstr());
  }

  ZLibCompressor       zcompressor;
//  AdaptivHuffmanTree compressor;
  ArithmeticCoder      acompressor;

  const int  maxMoves    = PLIESTOMOVES(maxPly);
  const BYTE bitsPerMove = getBitCount(maxMoves);

  BitSet winnerSet(indexSize);
  for(ep = firstElement; ep <= lastElement; ep++) {
    if(ep->isWinner()) winnerSet.add(ep - firstElement);
  }
  _tprintf(_T("winnerSet:\n%s"), BitSetCompressor(acompressor).compress(winnerSet, out).cstr());

  BitSet blackWinSet(10);
  if(canWinFlags == BOTHCANWIN) { // No need for indicating who wins. A winposition is the one indicated by canWinFlags
    blackWinSet.setCapacity(winnerPositions);
    size_t counter = 0;
    for(ep = firstElement; ep <= lastElement; ep++) {
      switch(ep->getStatus()) {
      case EG_WHITEWIN:
        counter++;
        break;
      case EG_BLACKWIN:
        blackWinSet.add(counter++);
        break;
      }
    }
    _tprintf(_T("blackWinComp:\n%s"), BitSetCompressor(acompressor).compress(blackWinSet, out).cstr());
  }
  PackedArray movesArray(bitsPerMove);
  movesArray.setCapacity(winnerPositions);
  for(int i = 0; i < 2; i++) {
    for(ep = firstElement+i; ep <= lastElement; ep+=2) {
      if(ep->isWinner()) movesArray.add(ep->getMovesToEnd());
    }
  }
  const String dumpFileName = format(_T("c:\\temp\\moveArrays\\moveArray%s_%s.txt"), getKeyDefinition().getName().cstr(), EndGameKeyDefinition::getMetricName());
  FILE *f = MKFOPEN(dumpFileName, _T("w"));
  _ftprintf(f, _T("%s"), packedArrayToString(movesArray).cstr());
  fclose(f);

  const EndGamePosIndex n = movesArray.size();
  BitSet      evenMap(n);
  UINT        evenCount = 0;
  for(EndGamePosIndex i = 0; i < n; i++) {
    if((movesArray.get(i)&2)==0) {
      evenMap.add(i);
      evenCount++;
    }
  }
  BitSetCompressor(acompressor).compress(evenMap, out);
  evenMap.clear();

  PackedArray evenMovesArray(bitsPerMove-1);
  PackedArray oddMovesArray( bitsPerMove-1);
  evenMovesArray.setCapacity(evenCount  );
  oddMovesArray.setCapacity( n-evenCount);
  for(EndGamePosIndex i = 0; i < n; i++) {
    const UINT m = movesArray.get(i);
    if((m&1)==0) {
      evenMovesArray.add(m/2);
    } else {
      oddMovesArray.add(m/2);
    }
  }
  PackedArrayCompressor(zcompressor).compress(evenMovesArray, out);
  PackedArrayCompressor(zcompressor).compress(oddMovesArray , out);
}

#endif // __NEVER__
