#include "stdafx.h"
#include "EndGameTablebase.h"

SubTablebaseKey::SubTablebaseKey() {
  m_movingPlayer     = WHITEPLAYER;
  m_moveType         = NOMOVE;
  m_capturedPieceKey = EMPTYPIECEKEY;
  m_promoteTo        = NoPiece;
}

SubTablebaseKey::SubTablebaseKey(const Move &m) {
  m_movingPlayer     = m.m_piece->getPlayer();
  m_moveType         = m.getType();
  m_capturedPieceKey = m.m_capturedPiece ? m.m_capturedPiece->getKey() : EMPTYPIECEKEY;
  m_promoteTo        = m.getPromoteTo();
}

unsigned long SubTablebaseKey::hashCode() const {
  return ((((((m_promoteTo << 4) | m_capturedPieceKey) << 3) | m_moveType) << 1) | m_movingPlayer);
}

bool SubTablebaseKey::operator==(const SubTablebaseKey &key) const {
  return m_movingPlayer     == key.m_movingPlayer
      && m_moveType         == key.m_moveType
      && m_capturedPieceKey == key.m_capturedPieceKey
      && m_promoteTo        == key.m_promoteTo;
}

SubTablebasePositionInfo::SubTablebasePositionInfo() {
  m_tablebase   = NULL;
  m_swapPlayers = false;
}

SubTablebasePositionInfo::SubTablebasePositionInfo(const Game &game) {
  static const TCHAR *drawSignatureSet[] = {
    _T("KK"), _T("KNK"), _T("KBK")

#ifdef ENDGAME_NOKING
   ,_T("KQK") , _T("KRK") , _T("KPK")
   ,_T("KQKQ"), _T("KQKR"), _T("KQKB"), _T("KQKN"), _T("KQKP")
   ,_T("KRKR"), _T("KRKB"), _T("KRKN"), _T("KRKP"), _T("KRBK"), _T("KRNK"), _T("KRPK")
   ,_T("KBKB"), _T("KBKN"), _T("KBKP"), _T("KBBK"), _T("KBNK"), _T("KBPK")
   ,_T("KNKN"), _T("KNKP"), _T("KNNK"), _T("KNPK")
#endif
  };

  const PositionSignature gps = game.getPositionSignature();

  m_tablebase = NULL;

  // First check if this one of the "certain draw games"
  const String gps1String = gps.toString();
  const String gps2String = gps.swapPlayers().toString();
  for(int i = 0; i < ARRAYSIZE(drawSignatureSet); i++) {
    if((drawSignatureSet[i] == gps1String) || (drawSignatureSet[i] == gps2String)) {
      return;
    }
  }

  m_tablebase = EndGameTablebase::getInstanceBySignature(gps, m_swapPlayers);

#ifdef TABLEBASE_BUILDER
#define LOADASSUBTABLEBASE() loadPacked()
#else
#define LOADASSUBTABLEBASE() load()
#endif

  if(m_tablebase != NULL) {
    bool error = false;
    try {
      m_tablebase->LOADASSUBTABLEBASE();
    } catch(Exception e) {
      error = true;
    }
#ifdef __AFXWIN_H__
    catch(CMemoryException *e) {
      error = true;
      e->Delete();
    }
#endif
    if(error) {
      m_tablebase->unload();
      m_tablebase = new RemoteEndGameSubTablebase(m_tablebase->getKeyDefinition());
      m_tablebase->LOADASSUBTABLEBASE();
    }
  } else { // Non found.
    throwException(_T("Endgame %s is not defined"), gps1String.cstr());
  }
}

EndGamePositionStatus SubTablebasePositionInfo::getPositionStatus(const Game &game) const {
  if(m_tablebase != NULL) {
    return m_tablebase->getPositionStatus(game, m_swapPlayers);
  } else {
    return EG_DRAW;
  }
}

EndGameResult SubTablebasePositionInfo::getPositionResult(const Game &game) const {
  if(m_tablebase != NULL) {
    return m_tablebase->getPositionResult(game, m_swapPlayers);
  } else {
    return EGR_DRAW;
  }
}

void SubTablebasePositionInfo::unload() {
  if(m_tablebase != NULL) {
    m_tablebase->unload();
    if(m_tablebase->isRemote()) {
      delete m_tablebase;
    }
    m_tablebase = NULL;
  }
}

#ifdef TABLEBASE_BUILDER
bool SubTablebasePositionInfo::allKeysFound() const {
  return (m_tablebase == NULL) ? true : m_tablebase->allKeysFound();
}
#endif
