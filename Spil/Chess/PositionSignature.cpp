#include "stdafx.h"

void PlayerSignature::add(PieceType pt) {
  switch(pt) {
  case King  : assert(m_pieceCount.m_kingCount  ==  0); m_pieceCount.m_kingCount++;   break;
  case Queen : assert(m_pieceCount.m_queenCount  <  9); m_pieceCount.m_queenCount++;  break;
  case Rook  : assert(m_pieceCount.m_rookCount   < 10); m_pieceCount.m_rookCount++;   break;
  case Bishop: assert(m_pieceCount.m_bishopCount < 10); m_pieceCount.m_bishopCount++; break;
  case Knight: assert(m_pieceCount.m_knightCount < 10); m_pieceCount.m_knightCount++; break;
  case Pawn  : assert(m_pieceCount.m_pawnCount   <  8); m_pieceCount.m_pawnCount++;   break;
  default    : throwInvalidArgumentException(__TFUNCTION__, _T("pt=%d"), pt);
  }
}

int PlayerSignature::getPieceCount() const {
  return m_pieceCount.m_kingCount
       + m_pieceCount.m_queenCount  
       + m_pieceCount.m_rookCount   
       + m_pieceCount.m_bishopCount 
       + m_pieceCount.m_knightCount 
       + m_pieceCount.m_pawnCount;
}

int PlayerSignature::getPieceCount(PieceType pt) const {
  switch(pt) {
  case King  : return m_pieceCount.m_kingCount;
  case Queen : return m_pieceCount.m_queenCount;
  case Rook  : return m_pieceCount.m_rookCount;
  case Bishop: return m_pieceCount.m_bishopCount;
  case Knight: return m_pieceCount.m_knightCount;
  case Pawn  : return m_pieceCount.m_pawnCount;
  default    : throwInvalidArgumentException(__TFUNCTION__, _T("pt=%d"), pt);
               return 0;
  }
}

TCHAR PlayerSignature::getPieceShortName(PieceType type) { // static
  switch(type) {
  case Pawn: return _T('P');
  default  : return getPieceTypeShortNameEnglish(type)[0];
  }
}

#define ADDLONGNAME(f, type)                                                    \
{ if(m_pieceCount.f) {                                                          \
    const String name = firstLetterToUpperCase(getPieceTypeNameEnglish(type));  \
    for(int n = m_pieceCount.f; n--;) {                                         \
      if(delimiter) {                                                           \
        *(cp++) = delimiter;                                                    \
      } else {                                                                  \
        delimiter = _T('-');                                                    \
      }                                                                         \
      _tcscpy(cp, name.cstr());                                                 \
      cp += name.length();                                                      \
    }                                                                           \
  }                                                                             \
}

#define ADDSHORTNAME(f, type)                                                   \
{ if(m_pieceCount.f) {                                                          \
    const TCHAR name = getPieceShortName(type);                                 \
    for(int n = m_pieceCount.f; n--;) {                                         \
      *(cp++) = name;                                                           \
    }                                                                           \
  }                                                                             \
}

String PlayerSignature::toString(bool longNames) const {
  TCHAR result[1000], *cp = result;
  if(longNames) {
    TCHAR delimiter = 0;
    ADDLONGNAME( m_kingCount  , King   );
    ADDLONGNAME( m_queenCount , Queen  );
    ADDLONGNAME( m_rookCount  , Rook   );
    ADDLONGNAME( m_bishopCount, Bishop )
    ADDLONGNAME( m_knightCount, Knight );
    ADDLONGNAME( m_pawnCount  , Pawn   );
  } else {
    ADDSHORTNAME(m_kingCount  , King   );
    ADDSHORTNAME(m_queenCount , Queen  );
    ADDSHORTNAME(m_rookCount  , Rook   );
    ADDSHORTNAME(m_bishopCount, Bishop )
    ADDSHORTNAME(m_knightCount, Knight );
    ADDSHORTNAME(m_pawnCount  , Pawn   );
  }
  *cp = '\0';
  return result;
}

void PositionSignature::invalidArgument(const String &str) { // static 
  throwInvalidArgumentException(_T("PositionSignature"), _T("str=%s"), str.cstr());
}

PositionSignature::PositionSignature(const String &str) {
  PlayerSignature *ps = m_pieceTypes;
  const TCHAR *cp = str.cstr();
  if(*(cp++) != 'K') {
    invalidArgument(str);
  }
  ps->add(King);

  for(;*cp; cp++) {
    switch(*cp) {
    case 'K':
      if(ps == m_pieceTypes) {
        ps++;
      } else {
        invalidArgument(str);
      }
      ps->add(King  );
      break;
    case 'Q':
      ps->add(Queen );
      break;
    case 'R':
      ps->add(Rook  );
      break;
    case 'B':
      ps->add(Bishop);
      break;
    case 'N':
      ps->add(Knight);
      break;
    case 'P':
      ps->add(Pawn  );
      break;
    default:
      invalidArgument(str);
    }
  }
  if(isEmpty()) {
    invalidArgument(str);
  }
}

PositionSignature PositionSignature::swapPlayers() const {
  PositionSignature result;
  result.m_pieceTypes[BLACKPLAYER] = m_pieceTypes[WHITEPLAYER];
  result.m_pieceTypes[WHITEPLAYER] = m_pieceTypes[BLACKPLAYER];
  return result;
}

bool PositionSignature::operator==(const PositionSignature &ps) const {
  return (m_pieceTypes[WHITEPLAYER] == ps.m_pieceTypes[WHITEPLAYER])
      && (m_pieceTypes[BLACKPLAYER] == ps.m_pieceTypes[BLACKPLAYER]);
}

bool PositionSignature::operator!=(const PositionSignature &ps) const {
  return (m_pieceTypes[WHITEPLAYER] != ps.m_pieceTypes[WHITEPLAYER])
      || (m_pieceTypes[BLACKPLAYER] != ps.m_pieceTypes[BLACKPLAYER]);
}

bool PositionSignature::match(const PositionSignature &pt, bool &swap) const {
  if(*this == pt) {
    swap = false;
    return true;
  } else if(swapPlayers() == pt) {
    swap = true;
    return true;
  } else {
    return false;
  }
}

String PositionSignature::toString(bool longNames) const {
  if(longNames) {
    return m_pieceTypes[WHITEPLAYER].toString(true ) + _T("/") + m_pieceTypes[BLACKPLAYER].toString(true );
  } else {
    return m_pieceTypes[WHITEPLAYER].toString(false) +       m_pieceTypes[BLACKPLAYER].toString(false);
  }
}
