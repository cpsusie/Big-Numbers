#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include "ChessGraphics.h"

#define RED   RGB(255,0,0)
#define BLUE  RGB(0,0,255)

#define PLAYERTEXTCOLOR(player) ((player == WHITEPLAYER) ? WHITE : BLACK)
#define PLAYERBKCOLOR(  player) ((player == WHITEPLAYER) ? BLACK : WHITE)

#define FIELDSIZE m_resources.getFieldSize()

bool DebugFlags::showState() const {
  return m_flags.m_showMaterial      
      || m_flags.m_showCheckingSDAPos 
      || m_flags.m_showBishopFlags     
      || m_flags.m_showPawnCount      
      || m_flags.m_showLastCapture 
      || m_flags.m_showPositionRepeats
      || m_flags.m_showSetupMode
      || m_flags.m_showFEN
      ;
}

DebugFlags::DebugFlags() {
  m_anySet = 0;
}

void ChessGraphics::paintDebugInfo(HDC dc) {
  int line = 10;

  HGDIOBJ oldFont = SelectObject(dc, m_resources.getDebugFont());
  try {
    if(m_debugFlags.showState() || m_debugFlags.m_flags.m_showFieldAttacks) {
      Game game1 = *m_game;
      game1.initState(false, &m_game->getKey());
      if(m_debugFlags.m_flags.m_showFieldAttacks) {
        paintFieldAttacks(dc, *m_game, game1);
      }
      if(m_debugFlags.showState()) {
        paintStateString(dc, line, *m_game, game1);
      }
    }

    if(m_debugFlags.m_flags.m_showLastMoveInfo) {
      paintLastMove(dc, line);
    }

    SelectObject(dc, oldFont);

  } catch(...) {
    SelectObject(dc, oldFont);
    throw;
  }
}

void ChessGraphics::paintFieldAttacks(HDC dc, const Game &game1, const Game &game2) {
  if(FIELDSIZE != m_lastDebugFieldSize) {
    initFieldTextOffsets(dc);
    m_lastDebugFieldSize = FIELDSIZE;
  }
  for(int pos = 0; pos < 64; pos++) {
    const CPoint p = getFieldPosition(pos, true);
#ifdef _DEBUG
    const TCHAR *fieldName = getFieldName(pos);
#endif
    const FieldAttacks &whiteAttacks1 = FIELDATTACKS(  game1.getPlayerState(WHITEPLAYER), pos);
    const FieldAttacks &blackAttacks1 = FIELDATTACKS(  game1.getPlayerState(BLACKPLAYER), pos);
    const FieldAttacks &whiteAttacks2 = FIELDATTACKS(  game2.getPlayerState(WHITEPLAYER), pos);
    const FieldAttacks &blackAttacks2 = FIELDATTACKS(  game2.getPlayerState(BLACKPLAYER), pos);
    const MoveDirection wk1           = KING_DIRECTION(game1.getPlayerState(WHITEPLAYER), pos);
    const MoveDirection bk1           = KING_DIRECTION(game1.getPlayerState(BLACKPLAYER), pos);
    const MoveDirection wk2           = KING_DIRECTION(game2.getPlayerState(WHITEPLAYER), pos);
    const MoveDirection bk2           = KING_DIRECTION(game2.getPlayerState(BLACKPLAYER), pos);


    if( whiteAttacks1.m_isAttacked || blackAttacks1.m_isAttacked || whiteAttacks2.m_isAttacked || blackAttacks2.m_isAttacked
     || wk1 || bk1 || wk2 || bk2) {
      const AttackInfo &wa1 = whiteAttacks1.m_attackInfo;
      const AttackInfo &ba1 = blackAttacks1.m_attackInfo;
      const AttackInfo &wa2 = whiteAttacks2.m_attackInfo;
      const AttackInfo &ba2 = blackAttacks2.m_attackInfo;

      paintFieldAttacks(dc, p, ATT_RIGHT(    wa1)   , ATT_RIGHT(    wa2)  , WHITEPLAYER, FROM_LEFT         ,wk1==MD_LEFT     ,wk2==MD_LEFT     );
      paintFieldAttacks(dc, p, ATT_LEFT(     wa1)   , ATT_LEFT(     wa2)  , WHITEPLAYER, FROM_RIGHT        ,wk1==MD_RIGHT    ,wk2==MD_RIGHT    );
      paintFieldAttacks(dc, p, ATT_UP(       wa1)   , ATT_UP(       wa2)  , WHITEPLAYER, FROM_BELOVE       ,wk1==MD_DOWN     ,wk2==MD_DOWN     );
      paintFieldAttacks(dc, p, ATT_DOWN(     wa1)   , ATT_DOWN(     wa2)  , WHITEPLAYER, FROM_ABOVE        ,wk1==MD_UP       ,wk2==MD_UP       );
      paintFieldAttacks(dc, p, ATT_UPDIAG1(  wa1)   , ATT_UPDIAG1(  wa2)  , WHITEPLAYER, FROM_LOWERDIAG1   ,wk1==MD_DOWNDIAG1,wk2==MD_DOWNDIAG1);
      paintFieldAttacks(dc, p, ATT_DOWNDIAG1(wa1)   , ATT_DOWNDIAG1(wa2)  , WHITEPLAYER, FROM_UPPERDIAG1   ,wk1==MD_UPDIAG1  ,wk2==MD_UPDIAG1  );
      paintFieldAttacks(dc, p, ATT_UPDIAG2(  wa1)   , ATT_UPDIAG2(  wa2)  , WHITEPLAYER, FROM_LOWERDIAG2   ,wk1==MD_DOWNDIAG2,wk2==MD_DOWNDIAG2);
      paintFieldAttacks(dc, p, ATT_DOWNDIAG2(wa1)   , ATT_DOWNDIAG2(wa2)  , WHITEPLAYER, FROM_UPPERDIAG2   ,wk1==MD_UPDIAG2  ,wk2==MD_UPDIAG2  );
      paintFieldAttacks(dc, p, wa1.m_sdAttacks      , wa2.m_sdAttacks     , WHITEPLAYER, FROM_SHORTDISTANCE);

      paintFieldAttacks(dc, p, ATT_RIGHT(    ba1)   , ATT_RIGHT(    ba2)  , BLACKPLAYER, FROM_LEFT         ,bk1==MD_LEFT     ,bk2==MD_LEFT     );
      paintFieldAttacks(dc, p, ATT_LEFT(     ba1)   , ATT_LEFT(     ba2)  , BLACKPLAYER, FROM_RIGHT        ,bk1==MD_RIGHT    ,bk2==MD_RIGHT    );
      paintFieldAttacks(dc, p, ATT_UP(       ba1)   , ATT_UP(       ba2)  , BLACKPLAYER, FROM_BELOVE       ,bk1==MD_DOWN     ,bk2==MD_DOWN     );
      paintFieldAttacks(dc, p, ATT_DOWN(     ba1)   , ATT_DOWN(     ba2)  , BLACKPLAYER, FROM_ABOVE        ,bk1==MD_UP       ,bk2==MD_UP       );
      paintFieldAttacks(dc, p, ATT_UPDIAG1(  ba1)   , ATT_UPDIAG1(  ba2)  , BLACKPLAYER, FROM_LOWERDIAG1   ,bk1==MD_DOWNDIAG1,bk2==MD_DOWNDIAG1);
      paintFieldAttacks(dc, p, ATT_DOWNDIAG1(ba1)   , ATT_DOWNDIAG1(ba2)  , BLACKPLAYER, FROM_UPPERDIAG1   ,bk1==MD_UPDIAG1  ,bk2==MD_UPDIAG1  );
      paintFieldAttacks(dc, p, ATT_UPDIAG2(  ba1)   , ATT_UPDIAG2(  ba2)  , BLACKPLAYER, FROM_LOWERDIAG2   ,bk1==MD_DOWNDIAG2,bk2==MD_DOWNDIAG2);
      paintFieldAttacks(dc, p, ATT_DOWNDIAG2(ba1)   , ATT_DOWNDIAG2(ba2)  , BLACKPLAYER, FROM_UPPERDIAG2   ,bk1==MD_UPDIAG2  ,bk2==MD_UPDIAG2  );
      paintFieldAttacks(dc, p, ba1.m_sdAttacks      , ba2.m_sdAttacks     , BLACKPLAYER, FROM_SHORTDISTANCE);
    }

    const Piece *piece1 = game1.getPieceAtPosition(pos);
    if(piece1) {
      const Player      player = piece1->getPlayer();
      const PinnedState ps1    = piece1->getPinnedState();
      const PinnedState ps2    = game2.getPieceAtPosition(pos)->getPinnedState();

      if(ps1 != NOT_PINNED || ps2 != NOT_PINNED) {
        String str;
        const COLORREF textColor = (player == WHITEPLAYER) ? BLACK : WHITE;
        COLORREF bkColor;
        if(ps1 != ps2) {
          str = format(_T("%s(%s)"), getPinnedStateToString(ps1).cstr(), getPinnedStateToString(ps2).cstr());
          bkColor = RED;
        } else {
          str = getPinnedStateToString(ps1);
          bkColor = (player == WHITEPLAYER) ? WHITE : BLACK;
        }
        const CSize textSize = getTextExtent(dc, str);
        dtextOut(dc,p.x+FIELDSIZE.cx,p.y+textSize.cy,ALIGN_RIGHT,str,bkColor,textColor);
      }

      if(piece1->getType() == King) {
        const KingAttackState as1  = piece1->getState().m_kingAttackState;
        const KingAttackState as2 = game2.getPieceAtPosition(pos)->getState().m_kingAttackState;
        if(as1 || as2) {
          String str;
          COLORREF bkColor;
          if(as1 != as2) {
            str = format(_T("%s(%s)"), getKingAttackStateToString(as1).cstr(), getKingAttackStateToString(as2).cstr());
            bkColor = RED;
          } else {
            str = getKingAttackStateToString(as1);
            bkColor = WHITE;
          }
          const CSize textSize = getTextExtent(dc, str);
          dtextOut(dc,p.x+FIELDSIZE.cx-2-textSize.cx,p.y,str,bkColor);
        }
      }
    }
  }
}

static FieldAttackTextPosition offsetsComputerPlayBlack[8], offsetsComputerPlayWhite[8], offsetSDA[2];

void ChessGraphics::initFieldTextOffsets(HDC dc) {
  const CSize charSize = getTextExtent(dc, _T("/"));

  FieldAttackTextPosition *tp = offsetsComputerPlayBlack;
  *tp++ = FieldAttackTextPosition( ALIGN_LEFT  ,  0               ,(FIELDSIZE.cy-charSize.cy)/2); // FROM_LEFT
  *tp++ = FieldAttackTextPosition( ALIGN_RIGHT ,  FIELDSIZE.cx    ,(FIELDSIZE.cy-charSize.cy)/2); // FROM_RIGHT
  *tp++ = FieldAttackTextPosition( ALIGN_CENTER,  FIELDSIZE.cx/2  , FIELDSIZE.cy-charSize.cy   ); // FROM_BELOVE
  *tp++ = FieldAttackTextPosition( ALIGN_CENTER,  FIELDSIZE.cx/2  , 0                          ); // FROM_ABOVE
  *tp++ = FieldAttackTextPosition( ALIGN_LEFT  ,  0               , FIELDSIZE.cy-charSize.cy   ); // FROM_LOWERDIAG1
  *tp++ = FieldAttackTextPosition( ALIGN_RIGHT ,  FIELDSIZE.cx    , 0                          ); // FROM_UPPERDIAG1
  *tp++ = FieldAttackTextPosition( ALIGN_RIGHT ,  FIELDSIZE.cx    , FIELDSIZE.cy-charSize.cy   ); // FROM_LOWERDIAG2
  *tp++ = FieldAttackTextPosition( ALIGN_LEFT  ,  0               , 0                          ); // FROM_UPPERDIAG2


  tp = offsetsComputerPlayWhite;
  *tp++ = offsetsComputerPlayBlack[FROM_RIGHT     ];                                              // FROM_LEFT
  *tp++ = offsetsComputerPlayBlack[FROM_LEFT      ];                                              // FROM_RIGHT
  *tp++ = offsetsComputerPlayBlack[FROM_ABOVE     ];                                              // FROM_BELOVE
  *tp++ = offsetsComputerPlayBlack[FROM_BELOVE    ];                                              // FROM_ABOVE
  *tp++ = offsetsComputerPlayBlack[FROM_UPPERDIAG1];                                              // FROM_LOWERDIAG1
  *tp++ = offsetsComputerPlayBlack[FROM_LOWERDIAG1];                                              // FROM_UPPERDIAG1
  *tp++ = offsetsComputerPlayBlack[FROM_UPPERDIAG2];                                              // FROM_LOWERDIAG2
  *tp++ = offsetsComputerPlayBlack[FROM_LOWERDIAG2];                                              // FROM_UPPERDIAG2

  tp = offsetSDA;
  *tp++ = FieldAttackTextPosition( ALIGN_LEFT, 2*charSize.cx, max(FIELDSIZE.cy - 2*charSize.cy, 2*charSize.cy));
  *tp++ = FieldAttackTextPosition( ALIGN_LEFT, 2*charSize.cx, charSize.cy                                     );
}

const FieldAttackTextPosition *ChessGraphics::getLDAOffset(AttackInfoField f) {
  return (m_computerPlayer == WHITEPLAYER) ? &offsetsComputerPlayWhite[f] : &offsetsComputerPlayBlack[f];
}

const FieldAttackTextPosition *ChessGraphics::getSDAOffset(Player player) {
  return (player == m_computerPlayer) ? &offsetSDA[1]  : &offsetSDA[0];
}

void ChessGraphics::paintFieldAttacks(HDC dc, const CPoint &p, UINT count1, UINT count2, Player player, AttackInfoField f, bool hasKing1, bool hasKing2) {
  if(count1 == 0 && count2 == 0 && !hasKing1 && !hasKing2) {
    return;
  }

  const bool isError     = (count1 != count2);
  const bool isKingError = (hasKing1 != hasKing2);
  String attackStr, kingStr;
  const FieldAttackTextPosition *offset;
  
  switch(f) {
  case FROM_SHORTDISTANCE:
    offset    = getSDAOffset(player);
    attackStr = isError ? format(_T("%d(%d)"),count1,count2) : format(_T("%d"),count1);
    break;
  default                :
    offset    = getLDAOffset(f);
    attackStr = isError ? format(_T("%d(%d)"),count1,count2) : (count1?format(_T("%d"),count1):_T(""));
    kingStr   = isKingError ? format(_T("%c(%c)"),(hasKing1?'K':' '),(hasKing2?'K':' ')) : (hasKing1?_T("K"):_T(""));
    break;
  }

  Array<ColoredText> fieldText;

  if(attackStr.length()) {
    const COLORREF backColor = isError     ? RED : PLAYERBKCOLOR(player);
    fieldText.add(ColoredText(attackStr, backColor, PLAYERTEXTCOLOR(player)));
  }

  if(kingStr.length()) {
    const COLORREF backColor = isKingError ? RED : PLAYERBKCOLOR(player);
    fieldText.add(ColoredText(kingStr, backColor, PLAYERTEXTCOLOR(player)));
  }

  dtextOut(dc,p+offset->m_offset,offset->m_align,fieldText);
}

// game1 is the currrent game. game2 is a referencegame which is initialized,
// so we can detect errors in the incremental updates done on game1
void ChessGraphics::paintStateString(HDC dc, int &line, const Game &game1, const Game &game2) {

  paintStateString(dc,line,WHITEPLAYER,game1,game2);
  paintStateString(dc,line,BLACKPLAYER,game1,game2);

  if(m_debugFlags.m_flags.m_showSetupMode) {
    dtextOut(dc, 0, line, format(_T("SetupMode:%s"), boolToStr(m_game->isSetupMode())));
    line += 16;
  }
  if(m_debugFlags.m_flags.m_showFEN) {
    dtextOut(dc, 0, line, format(_T("FEN:%s"), m_game->toFENString().cstr()).cstr());
    line += 16;
  }
  if(m_debugFlags.m_flags.m_showLastCapture) {
#ifdef TABLEBASE_BUILDER
    const String str = _T(" No last capture");
#else
    const String str = format(_T("Plies:%3d, #plies without capt/pawn:%2d (max:%d)")
                             ,m_game->getPlyCount()
                             ,m_game->getPlyCountWithoutCaptureOrPawnMove()
                             ,m_game->getMaxPlyCountWithoutCaptureOrPawnMove()
                             );
#endif
    dtextOut(dc, 0, line, str);
    line += 16;
  }

  if(m_debugFlags.m_flags.m_showPositionRepeats) {
#ifdef TABLEBASE_BUILDER
    const String str = _T(" No repetition count");
#else
    const int repeats = m_game->getPositionRepeats();
    const String str = format(_T("Position occurred %d %s"), repeats, (repeats==1)?_T("time"):_T("times"));
#endif
    dtextOut(dc, 0, line, str);
    line += 16;
  }
}

void ChessGraphics::paintStateString(HDC dc, int &line, Player player, const Game &game1, const Game &game2) {
  bool showBWState = false;

  Array<ColoredText> fieldText;

  fieldText.add(ColoredText(format(_T("%s:"), getPlayerNameEnglish(player)),WHITE,BLACK));

  if(m_debugFlags.m_flags.m_showMaterial) {
#ifdef TABLEBASE_BUILDER
    fieldText.add(ColoredText(_T(" No material"), RED, BLACK)); 
#else
    const int mat1 = game1.getMaterial(player);
    const int mat2 = game2.getMaterial(player);
    if(mat1 != mat2) {
      fieldText.add(ColoredText(format(_T("Material:%4d(%4d)"),mat1,mat2), RED, BLACK));
    } else {
      fieldText.add(ColoredText(format(_T("Material:%4d"),mat1), WHITE, BLACK));
    }
#endif
    showBWState = true;
  }

  if(m_debugFlags.m_flags.m_showCheckingSDAPos) {
    int pos = game1.getCheckingSDAPosition(player);
    fieldText.add(ColoredText(format(_T(" Checking SDA:[%s]"), (pos < 0) ? _T("-1") : getFieldName(pos)), WHITE, BLACK));
    showBWState = true;
  }

  if(m_debugFlags.m_flags.m_showBishopFlags) {
#ifdef TABLEBASE_BUILDER
    fieldText.add(ColoredText(_T(" No bishopflags"), RED, BLACK)); 
#else
    const BishopFlags bf1 = game1.getBishopFlags(player);
    const BishopFlags bf2 = game2.getBishopFlags(player);
    if(bf1 != bf2) {
      fieldText.add(ColoredText(format(_T(" Bishops:[%s(%s)]"),getBishopFlagsToString(bf1).cstr(),getBishopFlagsToString(bf2).cstr()), RED, BLACK));
    } else {
      fieldText.add(ColoredText(format(_T(" Bishops:[%s]"),getBishopFlagsToString(bf1).cstr()), WHITE, BLACK));
    }
#endif
    showBWState = true;
  }

  if(m_debugFlags.m_flags.m_showPawnCount) {
#ifdef TABLEBASE_BUILDER
    fieldText.add(ColoredText(_T(" No pawnCount"), RED, BLACK)); 
#else
    const String s1 = game1.getPawnCountToString(player);
    const String s2 = game2.getPawnCountToString(player);
    if(s1 != s2) {
      fieldText.add(ColoredText(format(_T(" PawnCount:[%s(%s)]"),s1.cstr(),s2.cstr()), RED, BLACK));
    } else {
      fieldText.add(ColoredText(format(_T(" PawnCount:[%s]"),s1.cstr()), WHITE, BLACK));
    }
#endif
    showBWState = true;
  }

  if(showBWState) {
    dtextOut(dc,0,line,ALIGN_LEFT,fieldText);
    line += 12;
  }
}

void ChessGraphics::paintLastMove(HDC dc, int &line) {
  if(m_game->getPlyCount() > 0) {
    const String str = format(_T("Last move:%s"), m_game->getLastMove().toString().cstr());
    dtextOut(dc, 0, line, str);
    line += 12;
  }
}
