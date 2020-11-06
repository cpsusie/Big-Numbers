#include "stdafx.h"
#include <Math.h>
#include <WinGdi.h>
#include <Math/Point2D.h>
#include <MFCUtil/WinTools.h>
#include <Math/SigmoidIterator.h>
#include "MessageDlgThread.h"
#include "MessageDlg.h"
#include "ChessGraphicsAnimation.h"

#define UPPERLEFT m_resources.getUpperLeftCorner()
#define FIELDSIZE m_resources.getFieldSize()

#define KING_HAS_CHECKMARK 0x01
#define KING_UPSIDEDOWN    0x02
#define KING_HAS_MOVEMARK  0x04

ChessGraphics::ChessGraphics(CWnd *wnd) : m_hwnd(*wnd) {
  allocate();
  m_flushedGameKey.clear(WHITEPLAYER);

  m_game                   = nullptr;
  m_paintLevel             = 0;
#if defined(_DEBUG)
  m_maxPaintLevel          = 0;
#endif
  m_computerPlayer         = BLACKPLAYER;
  m_visibleClocks          = 0;
  m_mouseField             = m_selectedPieceField = -1;
  m_computerFrom           = m_computerTo         = -1;
  m_pieceDragger           = nullptr;
  m_remainingTime[0]       = m_remainingTime[1]       = 0;
  m_kingFlags[WHITEPLAYER] = m_kingFlags[BLACKPLAYER] = 0;
  m_lastDebugFieldSize     = CSize(0,0);
  initModeTextRect();
  initPlayerIndicatorRect();
}

ChessGraphics::~ChessGraphics() {
  deallocate();
}

void ChessGraphics::reopen() {
  deallocate();
  m_resources.unload();
  PixRect::reOpenDirectX();
  m_resources.load();
  allocate();
}

void ChessGraphics::allocate() {
  m_bufferPr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, BOARDSIZE0); TRACE_NEW(m_bufferPr);
  m_resources.setClientRectSize(getClientRect(m_hwnd).Size());
}

void ChessGraphics::deallocate() {
  SAFEDELETE(m_bufferPr);
  m_selectedRect.cleanup();
  m_bufferPr   = nullptr;
  m_paintLevel = 0;
}

// r,c might be an invalid position. See paintFieldNames
CPoint ChessGraphics::getFieldPosition0(int r, int c) const {
  const CSize fs = FIELDSIZE0;
  return UPPERLEFT0
        + ((m_computerPlayer == WHITEPLAYER)
        ? CSize( fs.cx*(7-c),fs.cy * r   )
        : CSize((fs.cx* c  ),fs.cy *(7-r)))
        ;
}

CPoint ChessGraphics::getFieldPosition0(int pos) const {
  return getFieldPosition0(GETROW(pos),GETCOL(pos));
}

CRect ChessGraphics::getFieldRect0(int pos) const {
  return CRect(getFieldPosition0(pos), FIELDSIZE0);
}

CPoint ChessGraphics::getFieldPosition(int r, int c) const {
  return (CPoint)m_resources.scalePoint(getFieldPosition0(r, c));
}
CPoint ChessGraphics::getFieldPosition(int pos     ) const {
  return (CPoint)m_resources.scalePoint(getFieldPosition0(pos));
}
CRect  ChessGraphics::getFieldRect(    int pos     ) const {
  return (CRect)m_resources.scaleRect(getFieldRect0(pos));
}

void ChessGraphics::setGame(const Game &game) {
  if(&game != m_game) {
    m_game = &game;
    pushLevel();
    unmarkAll();
    paintAll();
    popLevel();
  }
}

int ChessGraphics::getBoardPosition(const CPoint &point) const {
  const Size2D result = Point2D(point) - UPPERLEFT;
  const Size2D fs     = FIELDSIZE;
  int col = (int)floor(result.cx() / fs.cx());
  int row = (int)floor(result.cy() / fs.cy());

  if(!isValidPosition(row, col)) {
    return -1;
  }
  const int pos = MAKE_POSITION(row, col);
  return (m_computerPlayer == WHITEPLAYER) ? MIRRORCOLUMN(pos) : MIRRORROW(pos);
}

void ChessGraphics::setComputerPlayer(Player computerPlayer) {
  if(computerPlayer != m_computerPlayer) {
    pushLevel();
    unmarkAll();
    m_computerPlayer = computerPlayer;
    paintAll();
    popLevel();
  }
}

void ChessGraphics::setVisibleClocks(char visible) {
  if(visible != m_visibleClocks) {
    pushLevel();
    unpaintClocks();
    m_visibleClocks = visible;
    if(visible) {
      stopHourGlassAnimation();
      paintClocks();
    }
    popLevel();
  }
//  verbose(_T("Visible Clocks:[%02x]\n"), m_visibleClocks);
}

void ChessGraphics::setShowFieldNames(bool show) {
  if(show != m_showFieldNames) {
    pushLevel();
    m_showFieldNames = show;
    if(show) {
      paintFieldNames();
    } else {
      unpaintFieldNames();
    }
    popLevel();
  }
}

void ChessGraphics::setShowLegalMoves(bool show) {
  if(show != m_showLegalMoves) {
    pushLevel();
    m_showLegalMoves = show;
    if(show) {
      markLegalMoves();
    } else {
      unmarkLegalMoves();
    }
    popLevel();
  }
}

void ChessGraphics::setShowPlayerInTurn(bool show) {
  if(show != m_showPlayerInTurn) {
    m_showPlayerInTurn = show;
    pushLevel();
    if(show) {
      paintPlayerIndicator();
    } else {
      unpaintPlayerIndicator();
    }
    popLevel();
  }
}

void ChessGraphics::setModeText(const String &text) {
  if(text != m_modeText) {
    pushLevel();
    unpaintModeText();
    m_modeText = text;
    paintModeText();
    popLevel();
  }
}

void ChessGraphics::setShowBackMoves(bool show) {
  if(show != m_debugFlags.m_flags.m_showBackMoves) {
    pushLevel();
    m_debugFlags.m_flags.m_showBackMoves = show;
    markLegalMoves();
    popLevel();
  }
}

const Image *ChessGraphics::getPieceImage0(int pos) const {
  const Piece *piece = m_game ? m_game->getPieceAtPosition(pos) : nullptr;
  return piece ? m_resources.getPieceImage0(piece) : nullptr;
}

void ChessGraphics::paintAll() {
  pushLevel();
  m_bufferPr->rop(ORIGIN, BOARDSIZE0, SRCCOPY, m_resources.getBoardImage0(), ORIGIN);

  paintSelectedPiece();

  if(!m_game->isSetupMode() && m_game->isCheckmate()) {
    markMatingPositions();
  } else {
    unmarkMatingPositions();
  }
  if(m_showFieldNames) {
    paintFieldNames();
  }

  paintComputerMoveMarks();

  paintKings();

  if(m_showPlayerInTurn) {
    paintPlayerIndicator();
  }
  for(int pos = 0; pos < 64; pos++) {
    paintField(pos);
  }

  clearOffboardPieces(WHITEPLAYER);
  clearOffboardPieces(BLACKPLAYER);
  paintOffboardPieces(WHITEPLAYER);
  paintOffboardPieces(BLACKPLAYER);

  paintModeText();
  if(m_visibleClocks) {
    paintClocks();
  }

  m_mouseField = -1;

  popLevel();
}

void ChessGraphics::paintGamePositions() {
  pushLevel();

  const int oldMouse = unmarkMouse();

  const GameKey &gameKey = m_game->getKey();

  for(int pos = 0; pos < 64; pos++) {
    const PieceKey oldpk = m_flushedGameKey.m_pieceKey[pos];
    const PieceKey newpk = gameKey.m_pieceKey[pos];
    if(newpk != oldpk) {
      if(newpk == EMPTYPIECEKEY) {
        paintEmptyField(pos);
      } else {
        if(oldpk != EMPTYPIECEKEY) {
          paintEmptyField(pos);
        }
        paintField(pos);
      }
    }
  }
  updatePlayerIndicator();
  paintOffboardPieces(WHITEPLAYER);
  paintOffboardPieces(BLACKPLAYER);

  markMouse(oldMouse);
  popLevel();
}

#define LETTERCOLOR RGB(207,195,159)
#define WATCHCOLOR  RGB(  3, 27, 31)

void ChessGraphics::paintFieldNames() {
  pushLevel();

  HDC hdc = m_bufferPr->getDC();
  m_fieldNamesRectangles.clear();

  HGDIOBJ oldFont = SelectObject(hdc, m_resources.getBoardFont0());
  const  CSize charSize = getTextExtent(hdc, _T("8"));
  CSize        offset   = FIELDSIZE0 - charSize;
  offset.cx /= 2;
  offset.cy /= 2;

#define FN_POS(r,c) (getFieldPosition0(r,c) + offset)
#define FN_LEFTPOS( r) FN_POS( r,-1)
#define FN_RIGHTPOS(r) FN_POS( r, 8)
#define FN_UPPERPOS(c) FN_POS(-1, c)
#define FN_LOWERPOS(c) FN_POS( 8, c)

  for(int r = 0; r < 8; r++) {
    const String text = format(_T("%d"),r+1);
    paintFieldName(hdc, FN_LEFTPOS( r), text);
    paintFieldName(hdc, FN_RIGHTPOS(r), text);
  }
  for(int c = 0; c < 8; c++) {
    const String text = format(_T("%c"),'A'+c);
    paintFieldName(hdc, FN_UPPERPOS(c), text);
    paintFieldName(hdc, FN_LOWERPOS(c), text);
  }

  SelectObject(hdc, oldFont);
  m_bufferPr->releaseDC(hdc);

  if(m_fieldNamesRectangles.size() == 0) {
    addFieldNameRectangle(FN_UPPERPOS(0), FN_UPPERPOS(7), (CSize)charSize);
    addFieldNameRectangle(FN_LOWERPOS(0), FN_LOWERPOS(7), (CSize)charSize);
    addFieldNameRectangle(FN_LEFTPOS( 0), FN_LEFTPOS( 7), (CSize)charSize);
    addFieldNameRectangle(FN_RIGHTPOS(0), FN_RIGHTPOS(7), (CSize)charSize);
  }

  popLevel();
}

void ChessGraphics::addFieldNameRectangle(const CPoint &corner1, const CPoint &corner2, const CSize &charSize) {
  CRect tr = makePositiveRect(CRect(corner1, corner2));
  tr.right  += charSize.cx;
  tr.bottom += charSize.cy;
  m_fieldNamesRectangles.add(tr);
}

void ChessGraphics::paintFieldName(HDC dc, const CPoint &p, const String &str) {
  textOutTransparentBackground(dc, p, str, m_resources.getBoardFont0(), LETTERCOLOR);
}

void ChessGraphics::unpaintFieldNames() {
  pushLevel();
  for(size_t i = 0; i < m_fieldNamesRectangles.size(); i++) {
    restoreBackground(m_fieldNamesRectangles[i]);
  }
  popLevel();
}

void ChessGraphics::initPlayerIndicatorRect() {
  m_playerIndicatorRect = CRect(0,0,0,0);
}

void ChessGraphics::paintPlayerIndicator() {
  pushLevel();
  unpaintPlayerIndicator();
  const Image  *image    = m_resources.getPlayerIndicatorImage0();
  const CSize   markSize = image->getSize();
  const CSize   offset   = (FIELDSIZE0 - markSize)/2;

  const CPoint pos      = getFieldPosition0((m_game->getPlayerInTurn() == WHITEPLAYER)?-1:8
                                           ,(m_computerPlayer          == WHITEPLAYER)?8:-1) + offset;
  image->paintImage(*m_bufferPr, pos);
  m_playerIndicatorRect = CRect(pos, markSize);
  popLevel();
}

void ChessGraphics::unpaintPlayerIndicator() {
  pushLevel();
  if(m_playerIndicatorRect.Width()) {
    restoreBackground(m_playerIndicatorRect);
    initPlayerIndicatorRect();
  }
  popLevel();
}

void ChessGraphics::updatePlayerIndicator() {
  pushLevel();
  if(m_showPlayerInTurn) {
    paintPlayerIndicator();
  }
  popLevel();
}

void ChessGraphics::paintField(int pos) {
  pushLevel();
  const Piece *piece = m_game->getPieceAtPosition(pos);
  if(piece != nullptr) {
    const CRect r = getFieldRect0(pos);
    bool rotated = false;
    if(piece->getType() == King && !m_matingPositions.contains(pos)) { // king needs special attention
      const BYTE flags = m_kingFlags[piece->getPlayer()];
      if(flags & KING_HAS_CHECKMARK) {
        paintMark(pos, CHECKEDKING); // is never marked as from/to
        rotated = (flags & KING_UPSIDEDOWN) != 0;
      } else if(pos != m_selectedPieceField) {
        paintEmptyField(pos);
      }
    }
    m_resources.getPieceImage0(piece)->paintImage(*m_bufferPr, r.TopLeft(), 1.0, rotated ? 180 : 0);
  }
  popLevel();
}

void ChessGraphics::paintEmptyField(int pos) {
  pushLevel();
  const CRect r = getFieldRect0(pos);
  restoreBackground(r);
  if(pos == m_computerFrom) {
    paintMark(pos, MOVEFROMMARK);
  } else if(pos == m_computerTo) {
    paintMark(pos, MOVETOMARK);
  }
  popLevel();
}

void ChessGraphics::paintKings() {
  pushLevel();
  paintKing(WHITEPLAYER);
  paintKing(BLACKPLAYER);
  popLevel();
}

void ChessGraphics::paintKing(Player player) {
  if(m_game == nullptr) return;
  pushLevel();
  BYTE &flags = m_kingFlags[player];
  const Piece   *king  = m_game->getPlayerState(player).m_king;
  if(!king->isOnBoard()) {
    flags = 0;
  } else {
    const int  pos              = king->getPosition();
    const bool kingIsAttacked   = king->getState().m_kingAttackState != 0;
    const bool kingIsCheckMate  = kingIsAttacked && m_game->isCheckmate();
    const bool kingIsMoveMarked = (pos == m_computerFrom) || (pos == m_computerTo);
    const BYTE newFlags         = (kingIsAttacked    ? KING_HAS_CHECKMARK : 0)
                                 | (kingIsCheckMate  ? KING_UPSIDEDOWN    : 0)
                                 | (kingIsMoveMarked ? KING_HAS_MOVEMARK  : 0)
                                 ;


    if(newFlags != flags) {
      flags = newFlags;
      const int mousePos = getMouseField();
      const bool umm = Game::getWalkDistance(pos, mousePos) <= 1; // unmarkMouse-paint-markMouse
      if(umm) {
        unmarkMouse();
      }
      paintField(pos);
      if(umm) {
        markMouse(mousePos);
      }
    }
  }
  popLevel();
}

void ChessGraphics::animateMove(const MoveBase &m) {
  MoveBase am(m);

#if defined(TABLEBASE_BUILDER)
  if(m_debugFlags.m_flags.m_showBackMoves) {
    am.swapFromTo();
  }
#endif

  MoveAnimation animation(this);
  switch(m.getType()) {
  case SHORTCASTLING :
    { animation.addMovePiece(m.getFrom(), m.getTo());
      if(m.getFrom()==E1) {
        animation.addMovePiece(H1, F1);
      } else {
        animation.addMovePiece(H8, F8);
      }
    }
    break;

  case LONGCASTLING  :
    { animation.addMovePiece(m.getFrom(), m.getTo());
      if(m.getFrom()==E1) {
        animation.addMovePiece(A1, D1);
      } else {
        animation.addMovePiece(A8, D8);
      }
    }
    break;

  default:
    animation.addMovePiece(am.getFrom(), am.getTo());
    break;
  }
  animation.animate();
}

#define ISLEFTSIDE(player) ((player) != m_computerPlayer)

const CPoint &ChessGraphics::getFirstOffboardPiecePosition(Player player) const {
  static const CPoint pos[] = {
    (CPoint)Point2D(UPPERLEFT0.x - FIELDSIZE0.cx*2 -  80, UPPERLEFT0.y)
   ,(CPoint)Point2D(UPPERLEFT0.x + FIELDSIZE0.cx*8 + 160, UPPERLEFT0.y)
  };
  return pos[ISLEFTSIDE(player)?0:1];
}

OffboardPieceArray ChessGraphics::getOffboardPieces(Player player) const {
  OffboardPieceArray result;
  const CompactArray<PieceKey> capturedPieces = m_game->getPlayerState(player).getCapturedPieces();
  if(capturedPieces.size() == 0) {
    return result;
  }
  const CPoint p0 = getFirstOffboardPiecePosition(player);
  CPoint p = p0;
  for(size_t i = 0; i < capturedPieces.size(); i++) {
    const PieceKey key = capturedPieces[i];
    if(GET_TYPE_FROMKEY(key) == Pawn && (p.x == p0.x)) {
      p.x += ISLEFTSIDE(player) ? -FIELDSIZE0.cx : FIELDSIZE0.cx;
      p.y = p0.y;
    }
    result.add(OffboardPiece(p, FIELDSIZE0, key));
    p.y += FIELDSIZE0.cy;
  }
  return result;
}

void ChessGraphics::paintOffboardPieces(Player player) {
  const OffboardPieceArray offboardPieces = getOffboardPieces(player);
  if(offboardPieces == m_offboardPieces[player]) {
    return;
  }
  pushLevel();
  clearOffboardPieces(player);
  m_offboardPieces[player] = offboardPieces;
  for(size_t i = 0; i < offboardPieces.size(); i++) {
    const OffboardPiece &obp = offboardPieces[i];
    m_resources.getPieceImage0(obp.getKey())->paintImage(*m_bufferPr, obp.TopLeft());
  }
  popLevel();
}

void ChessGraphics::clearOffboardPieces(Player player) {
  OffboardPieceArray &a = m_offboardPieces[player];

  pushLevel();
  for(size_t i = 0; i < a.size(); i++) {
    restoreBackground(a[i]);
  }
  a.clear();
  popLevel();
}

void ChessGraphics::repaintOffboardPieces(Player player) {
  pushLevel();
  clearOffboardPieces(player);
  paintOffboardPieces(player);
  popLevel();
}

const OffboardPiece *ChessGraphics::getOffboardPieceByPosition(const CPoint &point) const {
  const CPoint up = (CPoint)m_resources.unscalePoint(point);
  forEachPlayer(p) {
    const OffboardPieceArray &a = m_offboardPieces[p];
    for(size_t i = 0; i < a.size(); i++) {
      const OffboardPiece &obp = a[i];
      if(obp.PtInRect(up)) {
        return &obp;
      }
    }
  }
  return nullptr;
}

const OffboardPiece *ChessGraphics::getOffboardPieceByKey(PieceKey key) const {
  const OffboardPieceArray &a = m_offboardPieces[GET_PLAYER_FROMKEY(key)];
  for(int i = (int)a.size() - 1; i >= 0; i--) {
    const OffboardPiece &obp = a[i];
    if(obp.getKey() == key) {
      return &obp;
    }
  }
  return nullptr;
}

void ChessGraphics::beginDragPiece(const CPoint &point, PieceKey key) {
  const OffboardPiece *obp = getOffboardPieceByKey(key);
  pushLevel();
  beginDragPiece((CPoint)m_resources.scalePoint(obp->CenterPoint()), obp);
  dragPiece(point);
  popLevel();
}

void ChessGraphics::beginDragPiece(const CPoint &point, const OffboardPiece *obp) {
  pushLevel();
  m_pieceDragger = new PieceDragger(this, point, obp); TRACE_NEW(m_pieceDragger);
  paintGamePositions();
  popLevel();
}

void ChessGraphics::dragPiece(const CPoint &point) {
  m_pieceDragger->drag(point);
}

void ChessGraphics::endDragPiece() {
  const Player p = GET_PLAYER_FROMKEY(m_pieceDragger->getPieceKey());
  m_pieceDragger->endDrag();
  SAFEDELETE(m_pieceDragger);

  pushLevel();
  repaintOffboardPieces(p);
  paintGamePositions();
  popLevel();
}

PieceKey ChessGraphics::getDraggedPiece() const {
  return m_pieceDragger ? m_pieceDragger->getPieceKey() : EMPTYPIECEKEY;
}

void ChessGraphics::paintModeText() {
  if(m_modeText.length() != 0) {
    pushLevel();

    HDC          dc        = m_bufferPr->getDC();
    HGDIOBJ      oldFont   = SelectObject(dc, m_resources.getBoardFont0());
    const CSize  textSize  = getTextExtent(dc, m_modeText);
    SelectObject(dc, oldFont);

    const CPoint p((BOARDSIZE0.cx - textSize.cx) / 2, 20);

    textOutTransparentBackground(dc, p, m_modeText, m_resources.getBoardFont0(), LETTERCOLOR);

    m_bufferPr->releaseDC(dc);
    m_modeTextRect = CRect(p, textSize);
    popLevel();
  }
}

void ChessGraphics::unpaintModeText() {
  if(m_modeTextRect.Width() > 0) {
    pushLevel();
    restoreBackground(m_modeTextRect);
    initModeTextRect();
    popLevel();
  }
}

void ChessGraphics::initModeTextRect() {
  m_modeTextRect.left = m_modeTextRect.right = m_modeTextRect.top = m_modeTextRect.bottom = 0;
}

static String formatSeconds(int sec) {
  if(sec < 3600) {
    return format(_T("%02d:%02d"), GET_MINUTES(sec), GET_SECONDS(sec));
  } else {
    return format(_T("%d:%02d:%02d"), GET_HOURS(sec), GET_MINUTES(sec), GET_SECONDS(sec));
  }
}

const CPoint ChessGraphics::getTimeTextPosition(int i) const {
  static const CPoint pos[] = {
    CPoint(UPPERLEFT0.x + 8 * FIELDSIZE0.cx + 180, UPPERLEFT0.y - 80)
   ,CPoint(UPPERLEFT0.x + 8 * FIELDSIZE0.cx + 180, UPPERLEFT0.y + 8 * FIELDSIZE0.cy + 40)
  };
  return pos[i];
}

const CSize &ChessGraphics::getTimeTextSize() const {
  static CSize size(0,0);
  if(size.cx == 0) {
    HDC     tmpDC   = CreateCompatibleDC(nullptr);
    HGDIOBJ oldFont = SelectObject(tmpDC, m_resources.getBoardFont0());
    size = getTextExtent(tmpDC, _T("00:00:00"));
    SelectObject(tmpDC, oldFont);
    DeleteDC(tmpDC);
  }
  return size;
}

const CRect &ChessGraphics::getTimeTextRect(int i) const {
  static const CRect rect[] = {
    CRect(getTimeTextPosition(0),getTimeTextSize())
   ,CRect(getTimeTextPosition(1),getTimeTextSize())
  };
  return rect[i];
}

void ChessGraphics::paintClocks() {
  int posIndex = m_computerPlayer;
  forEachPlayer(player) {
    if(m_visibleClocks & (1 << player)) {
      paintClock(getTimeTextPosition(posIndex), m_remainingTime[player]);
    }
    posIndex ^= 1;
  }
}

void ChessGraphics::unpaintClocks() {
  for(int i = 0; i < 2; i++) {
    restoreBackground(getTimeTextRect(i));
  }
}

void ChessGraphics::paintClock(const CPoint &pos, int seconds) {
  HDC dc = m_bufferPr->getDC();
  textOutTransparentBackground(dc, pos, formatSeconds(seconds), m_resources.getBoardFont0(), LETTERCOLOR);
  m_bufferPr->releaseDC(dc);
}

CSize ChessGraphics::flushImage() {
  if(m_paintLevel != 0) {
    showWarning(_T("%s:called with paintLevel=%d"), __TFUNCTION__, m_paintLevel);
  }
  pushLevel();
  paintKings();
  switch(m_game->findGameResult()) {
  case WHITE_CHECKMATE:
  case BLACK_CHECKMATE:
    markMatingPositions();
    break;
  default             :
    unmarkMatingPositions();
    break;
  }
  m_paintLevel=0; // dont call popLevel here. cause infinte recursion

  render();

  if(m_debugFlags.m_anySet) {
    paintDebugInfo();
  }
  setProperty(FLUSHEDGAMEKEY , m_flushedGameKey , m_game->getKey());
  setProperty(FLUSHEDMODETEXT, m_flushedModeText, m_modeText);
  return m_resources.getBoardSize();
}

void ChessGraphics::startHourGlassAnimation() {
#if defined(__NEVER__)
  if(!isWatchVisible()) {
    m_resources.getHourGlassAnimation().startAnimation(wnd,CPoint(UPPERLEFTCORNER.x + 8 * FIELDSIZE.cx + 40,UPPERLEFTCORNER.y - 80),true,ANIMATE_INFINITE);
  }
#endif
}

void ChessGraphics::stopHourGlassAnimation() {
  m_resources.getHourGlassAnimation0().stopAnimation();
}

void ChessGraphics::showClocks(UINT whiteTime, UINT blackTime) {
  m_remainingTime[WHITEPLAYER] = whiteTime;
  m_remainingTime[BLACKPLAYER] = blackTime;

  int posIndex = m_computerPlayer;

  pushLevel();
  const CSize &textSize = getTimeTextSize();
  forEachPlayer(p) {
    const CPoint &textPos = getTimeTextPosition(posIndex);
    restoreBackground(textPos, textSize);
    posIndex ^= 1;
  }
  paintClocks();
  popLevel();
}

void ChessGraphics::animateCheckMate() {
  const Player player  = m_game->getPlayerInTurn();
  const int    kingPos = m_game->getKingPosition(player);
  MateAnimation(this, kingPos).animate();
  m_kingFlags[player] = KING_HAS_CHECKMARK | KING_UPSIDEDOWN;
  paintField(kingPos);
}

void ChessGraphics::unmarkAll() {
  pushLevel();
  unmarkMouse();
  unmarkSelectedPiece();
  unmarkLegalMoves();
  unmarkMatingPositions();
  paintKings();
  unmarkLastMove();

  popLevel();
}

void ChessGraphics::markLegalMoves() {
  pushLevel();
  unmarkFields(m_legalMoveFields);
  if(!m_showLegalMoves) {
    popLevel();
    return;
  }

  const MoveBaseArray legalMoves = getLegalMoves();
  for(const MoveBase move : legalMoves) {
    switch(move.m_type) {
    case NOMOVE       :
      break;
    case NORMALMOVE   :
    case PROMOTION    :
      if(m_debugFlags.m_flags.m_showBackMoves) {
        m_legalMoveFields += markField(move.getFrom(), GREENMARK);
      } else {
        m_legalMoveFields += markField(move.getTo(), m_game->isPositionEmpty(move.getTo()) ? BLUEMARK : PINKMARK);
      }
      break;

    case ENPASSANT    :
      m_legalMoveFields += markField(move.getTo(), BLUEMARK);
      m_legalMoveFields += markField(move.getCaptureEPSquare(), PINKMARK);
      break;

    case SHORTCASTLING:
    case LONGCASTLING :
      m_legalMoveFields += markField(move.getTo(), PURPLEMARK);
      break;
    }
  }
  popLevel();
}

void ChessGraphics::unmarkLegalMoves() {
  pushLevel();
  unmarkFields(m_legalMoveFields);
  m_legalMoveFields.clear();
  popLevel();
}

void ChessGraphics::markLastMoveAsComputerMove() {
  pushLevel();
  unmarkLastMove();
  if(m_game->getPlyCount() > 0) {
    const MoveBase m = m_game->getLastMove();
    m_computerFrom   = m.getFrom();
    m_computerTo     = m.getTo();
    paintComputerMoveMarks();
  }
  popLevel();
}

void ChessGraphics::paintComputerMoveMarks() {
  if(isValidPosition(m_computerFrom)) {
    pushLevel();
    markField(m_computerFrom, MOVEFROMMARK);
    markField(m_computerTo  , MOVETOMARK  );
    popLevel();
  }
}

void ChessGraphics::unmarkLastMove() {
  if(isValidPosition(m_computerFrom)) {
    pushLevel();
    unmarkField(m_computerFrom);
    unmarkField(m_computerTo  );
    m_computerFrom = m_computerTo = -1;
    popLevel();
  }
}

void ChessGraphics::markMatingPositions() {
  pushLevel();
  int mouse = unmarkMouse();
  unmarkMatingPositions();
  m_matingPositions = m_game->getMatingPositions();
  for(auto it = m_matingPositions.getIterator(); it.hasNext();) {
    markField(it.next(), PINKMARK);
  }
  markMouse(mouse);
  popLevel();
}

void ChessGraphics::unmarkMatingPositions() {
  pushLevel();
  unmarkFields(m_matingPositions);
  m_matingPositions.clear();
  popLevel();
}

void ChessGraphics::markFields(const FieldSet &fields, FieldMark mark) {
  pushLevel();
  for(auto it = fields.getIterator(); it.hasNext();) {
    markField(it.next(), mark);
  }
  popLevel();
}

void ChessGraphics::unmarkFields(const FieldSet &fields) {
  pushLevel();
  for(auto it = fields.getIterator(); it.hasNext();) {
    unmarkField(it.next());
  }
  popLevel();
}

int ChessGraphics::markField(int pos, FieldMark mark) {
  pushLevel();
  paintMark(pos, mark);
  paintField(pos);
  popLevel();

  return pos;
}

void ChessGraphics::unmarkField(int pos) {
  pushLevel();
  restoreBackground(getFieldRect0(pos));
  paintField(pos);
  popLevel();
}

void ChessGraphics::paintMark(int pos, FieldMark mark) {
  pushLevel();
  m_resources.getFieldMarkImage0(mark)->paintImage(*m_bufferPr, getFieldPosition0(pos));
  if((pos == m_computerFrom) && (mark != MOVEFROMMARK)) {
    paintMark(pos, MOVEFROMMARK);
  } else if((pos == m_computerTo) && (mark != MOVETOMARK)) {
    paintMark(pos, MOVETOMARK);
  }
  popLevel();
}

void ChessGraphics::markMouse(int pos) {
  if(pos == m_mouseField) return;

  pushLevel();

  unmarkMouse();
  m_mouseField = pos;
  if(!isValidPosition(pos)) {
    popLevel();
    return;
  }

  const CRect r = getSelectionFrameRect0(pos);

  saveImageRect(m_selectedRect, r);
  m_resources.getSelectionFrameImage0()->paintImage(*m_bufferPr, r.TopLeft());
  popLevel();
}

int ChessGraphics::unmarkMouse() {
  int result = m_mouseField;
  if(isValidPosition(m_mouseField)) {
    pushLevel();
    const CRect r = getSelectionFrameRect0(m_mouseField);
    restoreImageRect(m_selectedRect);
    m_mouseField = -1;
    popLevel();
  }
  return result;
}

void ChessGraphics::restoreBackground(const CRect &r) {
  pushLevel();
  m_bufferPr->rop(r, SRCCOPY, m_resources.getBoardImage0(),r.TopLeft());
  popLevel();
}

void ChessGraphics::restoreBackground(const CPoint &p, const CSize &size) {
  pushLevel();
  m_bufferPr->rop(p, size, SRCCOPY, m_resources.getBoardImage0(),p);
  popLevel();
}

CRect ChessGraphics::getSelectionFrameRect0(int pos) const {
  const CSize &selectFrameSize0 = m_resources.getSelectionFrameSize0();
  const CSize offset0((selectFrameSize0 - FIELDSIZE0)/2);
  return CRect(getFieldPosition0(pos) - offset0, selectFrameSize0);
}

MoveBaseArray ChessGraphics::getLegalMoves() const {
  MoveBaseArray result;
  if(!isValidPosition(m_selectedPieceField)) {
    return result;
  }

#if defined(TABLEBASE_BUILDER)
  if(m_debugFlags.m_flags.m_showBackMoves) {
    const Piece *piece = m_game->getPieceAtPosition(m_selectedPieceField);
    if(piece == nullptr || piece->getPlayer() == m_game->getPlayerInTurn()) {
      return result;
    }
    return m_game->getLegalBackMoves( m_selectedPieceField);
  }
#endif

  const Piece *piece = m_game->getPieceAtPosition(m_selectedPieceField);
  if((piece == nullptr) || (piece->getPlayer() != m_game->getPlayerInTurn())) {
    return result;
  }
  return m_game->getLegalMoves(m_selectedPieceField);
}

const Piece *ChessGraphics::getSelectedPiece() const {
  return (m_selectedPieceField < 0) ? nullptr : m_game->getPieceAtPosition(m_selectedPieceField);
}

void ChessGraphics::setSelectedField(int pos) {
  m_selectedPieceField = -1;
  if(!isValidPosition(pos)) {
    return;
  }
  const Piece *piece = m_game->getPieceAtPosition(pos);
  if(piece == nullptr) {
    return;
  }
#if defined(TABLEBASE_BUILDER)
  if(m_debugFlags.m_flags.m_showBackMoves) {
    if(piece->getPlayer() != m_game->getPlayerInTurn()) {
      m_selectedPieceField = pos;
      return;
    }
  }
#endif
  if(piece->getPlayer() == m_game->getPlayerInTurn()) {
    m_selectedPieceField = pos;
  }
}

void ChessGraphics::markSelectedPiece(int pos) {
  if(pos == m_selectedPieceField) {
    return;
  }
  pushLevel();
  const int oldMouse = unmarkMouse();
  unmarkSelectedPiece();

  setSelectedField(pos);

  paintSelectedPiece();
  markMouse(oldMouse);
  popLevel();
}

void ChessGraphics::paintSelectedPiece() {
  if(m_selectedPieceField >= 0) {
    pushLevel();
    markLegalMoves();
    markField(m_selectedPieceField, YELLOWMARK);
    popLevel();
  }
}

void ChessGraphics::unmarkSelectedPiece() {
  if(m_selectedPieceField >= 0) {
    pushLevel();
    unmarkField(m_selectedPieceField);
    m_selectedPieceField = -1;
    unmarkLegalMoves();
    popLevel();
  }
}

PixRect *SavedImageRect::getPixRect(const CSize &size) {
  if((m_pr == nullptr) || needResize(m_pr->getSize(), size)) {
    cleanup();
    m_pr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, size); TRACE_NEW(m_pr);
  }
  return m_pr;
}

void SavedImageRect::save(const PixRect *src, const CRect &r) {
  PixRect *dst = getPixRect(r.Size());
  dst->rop(CRect(ORIGIN, r.Size()), SRCCOPY, src, r);
  m_rect = r;
}

void SavedImageRect::restore(PixRect *dst) const {
  if(m_pr == nullptr) return;
  dst->rop(m_rect, SRCCOPY, m_pr, CRect(ORIGIN, m_rect.Size()));
}

void SavedImageRect::cleanup() {
  SAFEDELETE(m_pr);
}

bool confirmCancel(CWnd *parent) {
  return parent->MessageBox(loadString(IDS_CANCELQUESTION).cstr(), loadString(IDS_CANCELLABEL).cstr(), MB_ICONQUESTION|MB_YESNO) == IDYES;
}

void showMessage(CWnd *parent, int milliSeconds, const String &caption, _In_z_ _Printf_format_string_ TCHAR const * const format,...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessage(parent, milliSeconds, caption, format, argptr);
  va_end(argptr);
}

void vshowMessage(CWnd *parent, int milliSeconds, const String &caption, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) {
  const String message = vformat(format,argptr);
  CMessageDlg dlg(milliSeconds, caption, message);
  dlg.DoModal();
//  CMessageDlgThread *thread = (CMessageDlgThread*)AfxBeginThread(RUNTIME_CLASS(CMessageDlgThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
//  thread->setParameters(milliSeconds, caption, message);
}
