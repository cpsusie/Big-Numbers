#include "stdafx.h"
#include <ByteFile.h>
#include <CountedByteStream.h>
#include <BitStream.h>
#include <TimeEstimator.h>
#include "IndexedMap.h"

#if !defined(NEWCOMPRESSION)

#define WRITEMOVEBITS(bitCount, shift)                                                                \
  { forEachPlayer(p) {                                                                                \
      for(const EndGameResult *ep = firstElement+(int)p; ep <= lastElement; ep+=2) {                  \
        if(ep->isWinner()) {                                                                          \
          bs.putBits(ep->getMovesToEnd()>>(shift), bitCount);                                         \
        }                                                                                             \
      }                                                                                               \
    }                                                                                                 \
  }

#define READMOVEBITS(op, bitCount, shift)                                                             \
  { forEachPlayer(p) {                                                                                \
      int arrayIndex = 0;                                                                             \
      for(Iterator<size_t> it = winnerPositionSet.getIterator(); it.hasNext(); arrayIndex++) {        \
        if((it.next()&1) == (int)p) {                                                                 \
          BYTE bits;                                                                                  \
          if(bits = (BYTE)bs.getBits(bitCount)) {                                                     \
            positionInfoArray.op(arrayIndex, bits<<(shift));                                          \
          }                                                                                           \
        }                                                                                             \
      }                                                                                               \
    }                                                                                                 \
  }

#if defined(TABLEBASE_BUILDER)

void IndexedMap::saveCompressed(ByteOutputStream      &s, const TablebaseInfo &info) const {
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
  const BYTE bitsPerMove = getBitCount(maxMoves);

  BitOutputStream bs(s);

  for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
    bs.putBit(ep->isWinner() ? 1 : 0);
  }

  if(canWinFlags == BOTHCANWIN) { // if(canWinflags != BOTHCANWIN) then no need for extra bit in each position to indicate winnner.
                                  // A winnerposition is the one indicated by canWinFlags
    for(const EndGameResult *ep = firstElement; ep <= lastElement; ep++) {
      switch(ep->getStatus()) {
      case EG_WHITEWIN:
        bs.putBit(0);
        break;
      case EG_BLACKWIN:
        bs.putBit(1);
        break;
      default         :
        break;
      }
    }
  }

  switch(bitsPerMove) {
  case 1:
    WRITEMOVEBITS(1,0);
    break;
  case 2:
    WRITEMOVEBITS(2,0);
    break;
  case 3:
    WRITEMOVEBITS(2,0);
    WRITEMOVEBITS(1,2);
    break;
  default: // 4, 5, 6, 7, 8
    WRITEMOVEBITS(4,0);
    break;
  }

  switch(bitsPerMove) {
  case 5:
    WRITEMOVEBITS(1,4);
    break;
  case 6:
    WRITEMOVEBITS(2,4);
    break;
  case 7:
    WRITEMOVEBITS(2,4);
    WRITEMOVEBITS(1,6);
    break;
  case 8:
    WRITEMOVEBITS(4,4);
    break;
  }
}

#else // !TABLEBASE_BUILDER ie chess-program

IndexedMap::IndexedMap(const EndGameKeyDefinition &keydef) : m_keydef(keydef) {
  m_positionIndex       = nullptr;
  m_infoArray           = nullptr;
}

IndexedMap::~IndexedMap() {
  clear();
}

void IndexedMap::clear() {
  SAFEDELETE(m_positionIndex);
  SAFEDELETE(m_infoArray    );
}

class DecompressedHeader : public TablebaseInfo {
public:
  BYTE m_bitsPerEntry;
  UINT m_bitSetIndexOffset;
  UINT m_arrayStartOffset;
  DecompressedHeader(const TablebaseInfo &info);
  DecompressedHeader(const String        &fileName);
  void save(ByteOutputStream &s);
  void load(ByteInputStream  &s);
};

DecompressedHeader::DecompressedHeader(const TablebaseInfo &info) : TablebaseInfo(info) {
  m_bitsPerEntry      = 0;
  m_bitSetIndexOffset = 0;
  m_arrayStartOffset  = 0;
}

DecompressedHeader::DecompressedHeader(const String &fileName) {
  load(ByteInputFile(fileName));
}

void DecompressedHeader::save(ByteOutputStream &s) {
  s.putBytes((BYTE*)this, sizeof(DecompressedHeader));
}

void DecompressedHeader::load(ByteInputStream &s) {
  s.getBytesForced((BYTE*)this, sizeof(DecompressedHeader));
}

// Read format written with IndexedMap::saveCompressed(ByteOutputStream &s);
void IndexedMap::decompress(ByteInputStream &s, const TablebaseInfo &info) const {
  const INT64 indexSize = m_keydef.getIndexSize();
  if(info.m_indexCapacity != indexSize) {
    throwInvalidArgumentException(__TFUNCTION__, _T("info.indexCapacity=%s, keydef.indexSize=%s")
                                 ,format1000(info.m_indexCapacity).cstr()
                                 ,format1000(indexSize).cstr());
  }

  const BYTE  canWinFlags  = info.getCanWinFlags();
  const UINT  maxPly       = info.m_maxPlies.getMax();
  const UINT  maxMoves     = PLIESTOMOVES(maxPly);
  const BYTE  bitsPerMove  = getBitCount(maxMoves);
  const BYTE  bitsPerEntry = bitsPerMove+((canWinFlags == BOTHCANWIN)?1:0);
  // maybe need a bit to white/black win, which will be the first bit for every entry

  BitInputStream bs(s);

  BitSet winnerPositionSet((size_t)indexSize);
  for(int index = 0; index < indexSize; index++) {
    if(bs.getBit()) {
      winnerPositionSet.add(index);
    }
  }
  const UINT64 winnerPositions = winnerPositionSet.size();

  if(winnerPositions != info.getWinnerPositionCount().getTotal()) {
    throwException(_T("Counted winnerPositions(=%s) != info.totalWinnerPosition(=%s)")
                  ,format1000(winnerPositions).cstr()
                  ,info.getWinnerPositionCount().toStringWithTotal().cstr()
                  );
  }

  BitSetIndex positionIndex(winnerPositionSet);

  PackedArray positionInfoArray(bitsPerEntry);
  positionInfoArray.setCapacity(winnerPositions);
  positionInfoArray.insertZeroes(0,winnerPositions);

  if(canWinFlags == BOTHCANWIN) {
    for(UINT64 i = 0; i < winnerPositions; i++) {
      if(bs.getBit()) {
        positionInfoArray.set(i, 1);
      }
    }
  }

  switch(bitsPerMove) {
  case 1 :
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 1, 1);
    } else {
      READMOVEBITS(set, 1, 0)
    }
    break;
  case 2:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 1);
    } else {
      READMOVEBITS(set, 2, 0)
    }
    break;
  case 3 :
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 1);
      READMOVEBITS(or , 1, 3)
    } else {
      READMOVEBITS(set, 2, 0);
      READMOVEBITS(or , 1, 2)
    }
    break;
  default: // 4, 5, 6, 7, 8
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 4, 1);
    } else {
      READMOVEBITS(set, 4, 0);
    }
    break;
  }

  switch(bitsPerMove) {
  case 5:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 1, 5);
    } else {
      READMOVEBITS(or , 1, 4);
    }
    break;
  case 6:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 5);
    } else {
      READMOVEBITS(or , 2, 4);
    }
    break;
  case 7:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 2, 5);
      READMOVEBITS(or , 1, 7);
    } else {
      READMOVEBITS(or , 2, 4);
      READMOVEBITS(or , 1, 6);
    }
    break;
  case 8:
    if(canWinFlags == BOTHCANWIN) {
      READMOVEBITS(or , 4, 5);
    } else {
      READMOVEBITS(or , 4, 4);
    }
    break;
  }

  // save decompressed format
  ByteOutputFile          file(m_keydef.getTbFileName(DECOMPRESSEDTABLEBASE));
  ByteCounter             outputCounter;
  CountedByteOutputStream out(outputCounter, file);

  DecompressedHeader header(info); // parts undefined. defined it below and rewrite it

  header.save(out);
  header.m_bitSetIndexOffset = (UINT)(outputCounter.getCount());
  positionIndex.save(out);
  header.m_arrayStartOffset  = (UINT)(outputCounter.getCount());
  positionInfoArray.save(out);
  header.m_bitsPerEntry      = bitsPerEntry;
  file.seek(0);
  header.save(file);

#if defined(__NEVER__)
  FILE *logFile = FOPEN(_T("c:\\temp\\decomp.txt"), _T("w"));
  int arrayIndex = 0;
  for(Iterator<UINT> it = winnerPositionSet.getIterator(); it.hasNext();) {
    UINT             index  = it.next();
    const EndGameKey key    = m_keydef.indexToKey(index);
    UINT             entry  = positionInfoArray.get(arrayIndex++);
    Player           winner;
    if(canWinFlags == BOTHCANWIN) {
      winner = (entry & 1) ? BLACKPLAYER : WHITEPLAYER;
      entry >>= 1;
    } else {
      winner = (canWinFlags&WHITECANWIN)?WHITEPLAYER:BLACKPLAYER;
    }

    EndGameResult eg((winner == WHITEPLAYER) ? EG_WHITEWIN : EG_BLACKWIN, (2*entry) + ((key.getPlayerInTurn() != winner ) ? 1 : 0));

    _ftprintf(logFile, _T("%s:%s\n"), key.toString(m_keydef).cstr(), eg.toString(true).cstr());
  }
  fclose(logFile);
#endif // __NEVER__

}

void IndexedMap::load() {
  clear();
  const String fileName = m_keydef.getTbFileName(DECOMPRESSEDTABLEBASE);
  const DecompressedHeader header(fileName);
  m_canWinFlags   = header.getCanWinFlags();
  m_positionIndex = new FileBitSetIndex(fileName, header.m_bitSetIndexOffset); TRACE_NEW(m_positionIndex);
  m_infoArray     = new PackedFileArray(fileName, header.m_arrayStartOffset ); TRACE_NEW(m_infoArray);
//  verbose(_T("%s"), m_positionIndex->getInfoString().cstr());
}

EndGameResult IndexedMap::get(EndGameKey key) const {
  if(!isAllocated()) {
    throwException(_T("%s:Index not loaded"), __TFUNCTION__);
  }
  const intptr_t index = m_positionIndex->getIndex((size_t)m_keydef.keyToIndex(key));

  if(index < 0) {
    return EGR_DRAW;
  } else if(m_canWinFlags == BOTHCANWIN) {
    const UINT         info         = m_infoArray->get(index);
    const Player       winner       = (info & 1) ? BLACKPLAYER : WHITEPLAYER;
    const Player       playerInTurn = key.getPlayerInTurn();
    const UINT         movesToEnd   = info >> 1;
    return EGR_WINNER(playerInTurn, winner, movesToEnd);
  } else {
    const UINT         info         = m_infoArray->get(index);
    const Player       winner       = (m_canWinFlags&WHITECANWIN)?WHITEPLAYER:BLACKPLAYER;
    const Player       playerInTurn = key.getPlayerInTurn();
    const UINT         movesToEnd   = info;
    return EGR_WINNER(playerInTurn, winner, movesToEnd);
  }
}

#endif // TABLEBASE_BUILDER

#endif // NEWCOMPRESSION
