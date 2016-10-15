#include "stdafx.h"
#include <Math.h>
#include <WinGdi.h>
#include <Math/Point2D.h>
#include <MFCUtil/WinTools.h>
#include <Math/SigmoidIterator.h>
#include "MessageDlgThread.h"
#include "MessageDlg.h"
#include "ChessGraphics.h"
#include "ChessGraphicsAnimation.h"

#define UPPERLEFTCORNER(scaled) ((scaled) ? m_resources.getUpperLeftCorner() : m_resources.getUpperLeftCorner0())
#define FIELDSIZE(      scaled) ((scaled) ? m_resources.getFieldSize()       : m_resources.getFieldSize0())
#define FIELDSIZE0              FIELDSIZE(false)
#define UPPERLEFT0              UPPERLEFTCORNER(false)

#define KING_HAS_CHECKMARK 0x01
#define KING_UPSIDEDOWN    0x02
#define KING_HAS_MOVEMARK  0x04

#define FLUSHIMAGE(dc) if(dc) { flushImage(dc, false); }

ChessGraphics::ChessGraphics() {
  allocate();
  m_lastFlushedGameKey.clear(WHITEPLAYER);

  HDC screenDC            = getScreenDC();
  m_imageDC               = CreateCompatibleDC(screenDC);
  const CSize &screenSize = getScreenSize();
  m_imageBitmap           = CreateCompatibleBitmap(screenDC, screenSize.cx, screenSize.cy);
  DeleteDC(screenDC);
  SelectObject(m_imageDC, m_imageBitmap);

  m_game                   = NULL;
  m_computerPlayer         = BLACKPLAYER;
  m_visibleClocks          = 0;
  m_mouseField             = m_selectedPieceField = -1;
  m_computerFrom           = m_computerTo         = -1;
  m_pieceDragger           = NULL;
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
  m_bufferPr      = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_resources.getBoardSize0()         );
  m_selectedOldPr = new PixRect(theApp.m_device, PIXRECT_PLAINSURFACE, m_resources.getSelectionFrameSize0());
}

void ChessGraphics::deallocate() {
  delete m_bufferPr;
  m_bufferPr = NULL;
  delete m_selectedOldPr;
  m_selectedOldPr = NULL;
}

CSize ChessGraphics::setBoardSize(const CSize &size) {
  const CSize &oldSize = m_resources.getBoardSize();

  if(size == oldSize) {
    return size;
  }
  const CSize size0 = m_resources.getBoardSize0();
  double scale;
  if(size.cx == oldSize.cx) {
    scale = (double)size.cy / size0.cy;
  } else if(size.cy == oldSize.cy) {
    scale = (double)size.cx / size0.cx;
  } else {
    const double asr0   = (double)size0.cx / size0.cy;
    const double newasr = (double)size.cx  / size.cy;
    scale = (newasr <= asr0) ? ((double)size.cx / size0.cx) : ((double)size.cy / size0.cy);
  }
  const CSize &newSize = m_resources.setScale(scale);

  return newSize;
}

CPoint ChessGraphics::getFieldPosition(int r, int c, bool scaled) const {
  double fs = FIELDSIZE0.cx;
  if(scaled) {
    fs *= m_resources.getScale();
  }
  if(m_computerPlayer == WHITEPLAYER) {
    return UPPERLEFTCORNER(scaled) + CSize((int)(fs*(7-c)),(int)(fs*r));
  } else {
    return UPPERLEFTCORNER(scaled) + CSize((int)(fs*c)    ,(int)(fs*(7-r)));
  }
}

CPoint ChessGraphics::getFieldPosition(int pos, bool scaled) const {
  return getFieldPosition(GETROW(pos),GETCOL(pos), scaled);
}

CRect ChessGraphics::getFieldRect(int pos, bool scaled) const {
  CPoint p = getFieldPosition(pos, scaled);
  return CRect(p, FIELDSIZE(scaled));
}

void ChessGraphics::setGame(const Game &game) {
  if(&game != m_game) {
    m_game = &game;
    unmarkAll(NULL);
  }
}

int ChessGraphics::getBoardPosition(const CPoint &point) const {
  CPoint result = point - UPPERLEFTCORNER(true);
  if(result.x < 0 || result.y < 0) {
    return -1;
  }
  const CSize &fs = FIELDSIZE(true);
  result.x /= fs.cx;
  result.y /= fs.cy;
  if(m_computerPlayer == WHITEPLAYER) {
    result.x = 7-result.x;
  } else {
    result.y = 7-result.y;
  }
  if(result.x >= 0 && result.x <= 7 && result.y >= 0 && result.y <= 7) {
    return MAKE_POSITION(result.y,result.x);
  } else {
    return -1;
  }
}

void ChessGraphics::setComputerPlayer(Player computerPlayer, HDC dc) {
  const Player oldComputerPlayer = m_computerPlayer;
  if(computerPlayer != oldComputerPlayer) {
    unmarkAll();
    m_computerPlayer = computerPlayer;
    paintBoard(dc);
    notifyPropertyChanged(GRAPHICS_COMPUTERPLAYER, &oldComputerPlayer, &m_computerPlayer);
  }
}

void ChessGraphics::setVisibleClocks(char visible, HDC dc) {
  if(visible != m_visibleClocks) {
    unpaintClocks();
    m_visibleClocks = visible;
    if(visible) {
      stopHourGlassAnimation();
      paintClocks();
    }
    FLUSHIMAGE(dc);
  }
//  verbose(_T("Visible Clocks:[%02x]\n"), m_visibleClocks);
}

void ChessGraphics::setShowFieldNames(bool show, HDC dc) {
  if(show != m_showFieldNames) {
    m_showFieldNames = show;
    if(show) {
      paintFieldNames();
    } else {
      unpaintFieldNames();
    }
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::setShowLegalMoves(bool show, HDC dc) {
  if(show != m_showLegalMoves) {
    m_showLegalMoves = show;
    markLegalMoves();
  } else {
    unmarkLegalMoves();
  }
  FLUSHIMAGE(dc);
}

void ChessGraphics::setShowPlayerInTurn(bool show, HDC dc) {
  if(show != m_showPlayerInTurn) {
    m_showPlayerInTurn = show;
    if(show) {
      paintPlayerIndicator(true, dc);
    } else {
      unpaintPlayerIndicator(dc);
    }
  }
}

void ChessGraphics::setModeText(const String &text, HDC dc) {
  if(text != m_modeText) {
    unpaintModeText();
    m_modeText = text;
    paintModeText(true);
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::setShowBackMoves(bool show) {
  if(show != m_debugFlags.m_flags.m_showBackMoves) {
    m_debugFlags.m_flags.m_showBackMoves = show;
    markLegalMoves();
  }
}

void ChessGraphics::resetAllDebugFlags() {
  m_debugFlags.m_anySet = 0;
}

const Image *ChessGraphics::getPieceImage(int pos) const {
  const Piece *piece = m_game->getPieceAtPosition(pos);
  return piece ? m_resources.getPieceImage(piece) : NULL;
}

CSize ChessGraphics::paintBoard(HDC dc) {
  const CSize &bs = m_resources.getBoardSize0();
  m_bufferPr->rop(0,0,bs.cx,bs.cy, SRCCOPY, m_resources.getBoardImage(), 0,0);

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

  ajourKings(false);

  if(m_showPlayerInTurn) {
    paintPlayerIndicator(false);
  }
  for(int pos = 0; pos < 64; pos++) {
    paintField(pos, false);
  }

  repaintOffboardPieces(WHITEPLAYER);
  repaintOffboardPieces(BLACKPLAYER);

  paintModeText(false);
  if(m_visibleClocks) {
    paintClocks();
  }

  m_mouseField = -1;
  return flushImage(dc, true);
}

void ChessGraphics::paintGamePosition(HDC dc) {
  const int oldMouse = unmarkMouse();

  const GameKey &gameKey = m_game->getKey();

//  String keyStr      = gameKey.toString();
//  String lastFlushed = m_lastFlushedGameKey.toString();

  for(int pos = 0; pos < 64; pos++) {
    const PieceKey oldpk = m_lastFlushedGameKey.m_pieceKey[pos];
    const PieceKey newpk = gameKey.m_pieceKey[pos];
    if(newpk != oldpk) {
      if(newpk == EMPTYPIECEKEY) {
        paintEmptyField(pos, true);
      } else {
        if(oldpk != EMPTYPIECEKEY) {
          paintEmptyField(pos, true);
        }
        paintField(pos, true);
      }
    }
  }
  updatePlayerIndicator();
  paintOffboardPieces(WHITEPLAYER, true);
  paintOffboardPieces(BLACKPLAYER, true);

  markMouse(oldMouse);
  FLUSHIMAGE(dc);
}

#define LETTERCOLOR RGB(207,195,159)
#define WATCHCOLOR  RGB(  3, 27, 31)

void ChessGraphics::paintFieldNames() {
  HDC dc = m_bufferPr->getDC();
  m_fieldNamesRectangles.clear();

  HGDIOBJ oldFont = SelectObject(dc, m_resources.getBoardFont());
  const CSize charSize = getTextExtent(dc, _T("8"));
  CSize offset = FIELDSIZE0 - charSize;
  offset.cx /= 2;
  offset.cy /= 2;

#define FN_POS(r,c) (getFieldPosition(r,c, false) + offset)
#define FN_LEFTPOS( r) FN_POS(r,-1)
#define FN_RIGHTPOS(r) FN_POS(r, 8)
#define FN_UPPERPOS(c) FN_POS(-1,c)
#define FN_LOWERPOS(c) FN_POS( 8,c)

  for(int r = 0; r < 8; r++) {
    const String text = format(_T("%d"),r+1);
    paintFieldName(dc, FN_LEFTPOS( r), text);
    paintFieldName(dc, FN_RIGHTPOS(r), text);
  }
  for(int c = 0; c < 8; c++) {
    const String text = format(_T("%c"),'A'+c);
    paintFieldName(dc, FN_UPPERPOS(c), text);
    paintFieldName(dc, FN_LOWERPOS(c), text);
  }

  SelectObject(dc, oldFont);
  m_bufferPr->releaseDC(dc);

  if(m_fieldNamesRectangles.size() == 0) {
    addFieldNameRectangle(FN_UPPERPOS(0), FN_UPPERPOS(7), charSize);
    addFieldNameRectangle(FN_LOWERPOS(0), FN_LOWERPOS(7), charSize);
    addFieldNameRectangle(FN_LEFTPOS( 0), FN_LEFTPOS( 7), charSize);
    addFieldNameRectangle(FN_RIGHTPOS(0), FN_RIGHTPOS(7), charSize);
  }

  flushFieldNames();
}

void ChessGraphics::flushFieldNames() {
  for(size_t i = 0; i < m_fieldNamesRectangles.size(); i++) {
    const CRect &r = m_fieldNamesRectangles[i];
    flushPr(m_fieldNamesRectangles[i],true);
  }
}

void ChessGraphics::addFieldNameRectangle(const CPoint &corner1, const CPoint &corner2, const CSize &charSize) {
  CRect tr = makePositiveRect(CRect(corner1, corner2));
  tr.right  += charSize.cx;
  tr.bottom += charSize.cy;
  m_fieldNamesRectangles.add(tr);
}

void ChessGraphics::paintFieldName(HDC dc, const CPoint &p, const String &str) {
  textOutTransparentBackground(dc, p, str, m_resources.getBoardFont(), LETTERCOLOR);
}

void ChessGraphics::unpaintFieldNames() {
  for(size_t i = 0; i < m_fieldNamesRectangles.size(); i++) {
    restoreBackground(m_fieldNamesRectangles[i], true);
  }
}

void ChessGraphics::initPlayerIndicatorRect() {
  m_playerIndicatorRect = CRect(0,0,0,0);
}

void ChessGraphics::paintPlayerIndicator(bool flush, HDC dc) {
  unpaintPlayerIndicator();
  const Image *image    = m_resources.getPlayerIndicatorImage();
  const CSize  markSize = image->getSize();
  CSize offset = FIELDSIZE0 - markSize;
  offset.cx /= 2;
  offset.cy /= 2;

  const CPoint pos = getFieldPosition((m_game->getPlayerInTurn() == WHITEPLAYER)?-1:8
                                     ,(m_computerPlayer          == WHITEPLAYER)?8:-1, false) + offset;
  image->paintImage(*m_bufferPr, pos);
  m_playerIndicatorRect = CRect(pos, markSize);
  if(flush) {
    flushPr(m_playerIndicatorRect);
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::unpaintPlayerIndicator(HDC dc) {
  if(m_playerIndicatorRect.Width()) {
    restoreBackground(m_playerIndicatorRect, true);
    initPlayerIndicatorRect();
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::updatePlayerIndicator(HDC dc) {
  if(m_showPlayerInTurn) {
    paintPlayerIndicator(true, dc);
  }
}

void ChessGraphics::paintField(int pos, bool flush, HDC dc) {
  const Piece *piece = m_game->getPieceAtPosition(pos);
  if(piece != NULL) {
    const CRect r = getFieldRect(pos, false);
    bool rotated = false;
    if(piece->getType() == King && !m_matingPositions.contains(pos)) { // king needs special attention
      const unsigned char flags = m_kingFlags[piece->getPlayer()];
      if(flags & KING_HAS_CHECKMARK) {
        paintMark(pos, CHECKEDKING, false); // is never marked as from/to
        rotated = (flags & KING_UPSIDEDOWN) != 0;
      } else if(pos != m_selectedPieceField) {
        paintEmptyField(pos, false);
      }
    }
    m_resources.getPieceImage(piece)->paintImage(*m_bufferPr, r.TopLeft(), 1.0, rotated ? 180 : 0);
    if(flush) {
      flushPr(r);
      FLUSHIMAGE(dc);
    }
  }
}

void ChessGraphics::paintEmptyField(int pos, bool flush, HDC dc) {
  const CRect r = getFieldRect(pos, false);
  restoreBackground(r, false);
  if(pos == m_computerFrom) {
    paintMark(pos, MOVEFROMMARK, false);
  } else if(pos == m_computerTo) {
    paintMark(pos, MOVETOMARK, false);
  }
  if(flush) {
    flushPr(r);
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::ajourKings(bool flush, HDC dc) {
  ajourKing(WHITEPLAYER, flush);
  ajourKing(BLACKPLAYER, flush);
  FLUSHIMAGE(dc);
}

void ChessGraphics::ajourKing(Player player, bool flush, HDC dc) {
  unsigned char &flags = m_kingFlags[player];
  const Piece   *king  = m_game->getPlayerState(player).m_king;
  if(!king->isOnBoard()) {
    flags = 0;
  } else {
    const int  pos              = king->getPosition();
    const bool kingIsAttacked   = king->getState().m_kingAttackState != 0;
    const bool kingIsCheckMate  = kingIsAttacked && m_game->isCheckmate();
    const bool kingIsMoveMarked = (pos == m_computerFrom) || (pos == m_computerTo);
    const unsigned char newFlags = (kingIsAttacked   ? KING_HAS_CHECKMARK : 0)
                                 | (kingIsCheckMate  ? KING_UPSIDEDOWN    : 0)
                                 | (kingIsMoveMarked ? KING_HAS_MOVEMARK  : 0)
                                 ;


    if(newFlags != flags) {
      flags = newFlags;
      const int mousePos = getMouseField();
      const bool umm = Game::getKingDistance(pos, mousePos) <= 1; // unmarkMouse-paint-markMouse
      if(umm) {
        unmarkMouse();
      }
      paintField(pos, flush);
      if(umm) {
        markMouse(mousePos);
      }
      FLUSHIMAGE(dc);
    }
  }
}

void ChessGraphics::animateMove(const MoveBase &m, HDC dc) {
  MoveBase am(m);

#ifdef TABLEBASE_BUILDER
  if(m_debugFlags.m_flags.m_showBackMoves) {
    am.swapFromTo();
  }
#endif

  CompactArray<MovePieceAnimation*> m_animations;
  switch(m.getType()) {
  case SHORTCASTLING :
    { MovePieceAnimation *kingAnimation = new MovePieceAnimation(this, dc, m.getFrom(), m.getTo());
      m_animations.add(kingAnimation);
      if(m.getFrom()==E1) {
        m_animations.add(new MovePieceAnimation(this, dc, H1, F1, kingAnimation->getSteps()));
      } else {
        m_animations.add(new MovePieceAnimation(this, dc, H8, F8, kingAnimation->getSteps()));
      }
    }
    break;

  case LONGCASTLING  :
    { MovePieceAnimation *kingAnimation = new MovePieceAnimation(this, dc, m.getFrom(), m.getTo());
      m_animations.add(kingAnimation);
      if(m.getFrom()==E1) {
        m_animations.add(new MovePieceAnimation(this, dc, A1, D1, kingAnimation->getSteps()));
      } else {
        m_animations.add(new MovePieceAnimation(this, dc, A8, D8, kingAnimation->getSteps()));
      }
    }
    break;

  default:
    m_animations.add(new MovePieceAnimation(this, dc, am.getFrom(), am.getTo()));
    break;
  }

  const int sleepTime = (m_animations.size() == 1) ? 15 : 10;
  const int n = m_animations.size();
  for(bool cont = true; cont;) {
    for(int i = n; i--;) {
      m_animations[i]->paint();
    }
    for(int i = n; i--;) {
      m_animations[i]->flush();
    }
    if(sleepTime) {
      Sleep(sleepTime);
    }
    cont = false;
    for(int i = 0; i < n; i++) {
      m_animations[i]->unpaint();
      cont |= m_animations[i]->step();
    }
  }
  for(int i = n; i--;) {
    delete m_animations[i];
  }
}

#define ISLEFTSIDE(player) ((player) != m_computerPlayer)

const CPoint &ChessGraphics::getFirstOffboardPiecePosition(Player player) const {
  static const CPoint pos[] = {
    CPoint(UPPERLEFTCORNER(false).x - FIELDSIZE0.cx*2 -  80, UPPERLEFTCORNER(false).y)
   ,CPoint(UPPERLEFTCORNER(false).x + FIELDSIZE0.cx*8 + 160, UPPERLEFTCORNER(false).y)
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

void ChessGraphics::paintOffboardPieces(Player player, bool flush, HDC dc) {
  const OffboardPieceArray offboardPieces = getOffboardPieces(player);
  if(offboardPieces == m_offboardPieces[player]) {
    return;
  }
  clearOffboardPieces(player, flush);
  m_offboardPieces[player] = offboardPieces;
  for(size_t i = 0; i < offboardPieces.size(); i++) {
    const OffboardPiece &obp = offboardPieces[i];
    m_resources.getPieceImage(obp.getKey())->paintImage(*m_bufferPr, obp.TopLeft());
    if(flush) {
      flushPr(obp);
    }
  }
  FLUSHIMAGE(dc);
}

void ChessGraphics::clearOffboardPieces(Player player, bool flush) {
  OffboardPieceArray &a = m_offboardPieces[player];
  for(size_t i = 0; i < a.size(); i++) {
    restoreBackground(a[i], flush);
  }
  a.clear();
}

void ChessGraphics::repaintOffboardPieces(Player player, HDC dc) {
  clearOffboardPieces(player, true);
  paintOffboardPieces(player, true);
  FLUSHIMAGE(dc);
}

const OffboardPiece *ChessGraphics::getOffboardPieceByPosition(const CPoint &point) const {
  const CPoint up = m_resources.unscalePoint(point);
  forEachPlayer(p) {
    const OffboardPieceArray &a = m_offboardPieces[p];
    for(size_t i = 0; i < a.size(); i++) {
      const OffboardPiece &obp = a[i];
      if(obp.PtInRect(up)) {
        return &obp;
      }
    }
  }
  return NULL;
}

const OffboardPiece *ChessGraphics::getOffboardPieceByKey(PieceKey key) const {
  const OffboardPieceArray &a = m_offboardPieces[GET_PLAYER_FROMKEY(key)];
  for(int i = a.size() - 1; i >= 0; i--) {
    const OffboardPiece &obp = a[i];
    if(obp.getKey() == key) {
      return &obp;
    }
  }
  return NULL;
}

void ChessGraphics::startDragPiece(const CPoint &point, PieceKey key, HDC dc) {
  const OffboardPiece *obp = getOffboardPieceByKey(key);
  startDragPiece(m_resources.scalePoint(obp->CenterPoint()), obp, dc);
  dragPiece(point, dc);
}

void ChessGraphics::startDragPiece(const CPoint &point, const OffboardPiece *obp, HDC dc) {
  m_pieceDragger = new PieceDragger(this, point, obp, dc);
}

void ChessGraphics::dragPiece(const CPoint &point, HDC dc) {
  m_pieceDragger->drag(point, dc);
}

void ChessGraphics::endDragPiece(HDC dc) {
  const Player p = GET_PLAYER_FROMKEY(m_pieceDragger->getPieceKey());
  if(dc) {
    m_pieceDragger->endDrag(dc);
  }
  delete m_pieceDragger;
  m_pieceDragger = NULL;

  repaintOffboardPieces(p, dc);
}

PieceKey ChessGraphics::getDraggedPiece() const {
  return m_pieceDragger ? m_pieceDragger->getPieceKey() : EMPTYPIECEKEY;
}

void ChessGraphics::paintModeText(bool flush) {
  if(m_modeText.length() != 0) {
    HDC dc = m_bufferPr->getDC();
    HGDIOBJ      oldFont   = SelectObject(dc, m_resources.getBoardFont());
    const CSize  textSize  = getTextExtent(dc, m_modeText);
    SelectObject(dc, oldFont);

    const CPoint p         = CPoint((m_resources.getBoardSize0().cx - textSize.cx) / 2,20);

    textOutTransparentBackground(dc, p, m_modeText, m_resources.getBoardFont(), LETTERCOLOR);

    m_bufferPr->releaseDC(dc);
    m_modeTextRect = CRect(p, textSize);
    if(flush) {
      flushModeText();
    }
  }
}

void ChessGraphics::unpaintModeText() {
  if(m_modeTextRect.Width() > 0) {
    restoreBackground(m_modeTextRect, true);
    initModeTextRect();
  }
}

void ChessGraphics::initModeTextRect() {
  m_modeTextRect.left = m_modeTextRect.right = m_modeTextRect.top = m_modeTextRect.bottom = 0;
}

void ChessGraphics::flushModeText() {
  if(m_modeTextRect.Width() > 0) {
    flushPr(m_modeTextRect, true);
  }
}

static TCHAR *formatTime(TCHAR *dst, int sec) {
  if(sec < 3600) {
    _stprintf(dst, _T("%02d:%02d"), GET_MINUTES(sec), GET_SECONDS(sec));
  } else {
    _stprintf(dst, _T("%d:%02d:%02d"), GET_HOURS(sec), GET_MINUTES(sec), GET_SECONDS(sec));
  }
  return dst;
}

const CPoint &ChessGraphics::getTimeTextPosition(int i) const {
  static const CPoint pos[] = { CPoint(UPPERLEFT0.x + 8 * FIELDSIZE0.cx + 180, UPPERLEFT0.y - 80)
                               ,CPoint(UPPERLEFT0.x + 8 * FIELDSIZE0.cx + 180, UPPERLEFT0.y + 8 * FIELDSIZE0.cy + 40)
                              };
  return pos[i];
}

const CSize &ChessGraphics::getTimeTextSize() const {
  static CSize size(0,0);
  if(size.cx == 0) {
    HDC tmpDC = CreateCompatibleDC(NULL);
    HGDIOBJ oldFont = SelectObject(tmpDC, m_resources.getBoardFont());
    size = getTextExtent(tmpDC, _T("00:00:00"));
    SelectObject(tmpDC, oldFont);
    DeleteDC(tmpDC);
  }
  return size;
}

const CRect &ChessGraphics::getTimeTextRect(int i) const {
  static const CRect rect[] = { CRect(getTimeTextPosition(0),getTimeTextSize())
                               ,CRect(getTimeTextPosition(1),getTimeTextSize())
                              };
  return rect[i];
}

void ChessGraphics::paintClocks() {
  HDC dc = m_bufferPr->getDC();

  int posIndex = m_computerPlayer;

  for(Player player = WHITEPLAYER; player <= BLACKPLAYER; NEXTPLAYER(player), posIndex ^= 1) {
    if(m_visibleClocks & (1 << player)) {
      paintClock(dc, getTimeTextPosition(posIndex), m_remainingTime[player]);
    }
  }
  m_bufferPr->releaseDC(dc);
  flushClocks();
}

void ChessGraphics::flushClocks() {
  for(int i = 0; i < 2; i++) {
    flushPr(getTimeTextRect(i), true);
  }
}

void ChessGraphics::unpaintClocks() {
  for(int i = 0; i < 2; i++) {
    const CRect &r = getTimeTextRect(i);
    restoreBackground(r, true);
  }
}

void ChessGraphics::paintClock(HDC dc, const CPoint &pos, int seconds) {
  TCHAR str[40];
  formatTime(str, seconds);
  textOutTransparentBackground(dc, pos, str, m_resources.getBoardFont(), LETTERCOLOR);
}

CSize ChessGraphics::flushImage(HDC dc, bool resize) {
  if(resize) {
    flushPr();
  } else {
    ajourKings(true);
    switch(m_game->findGameResult()) {
    case WHITE_CHECKMATE                   :
    case BLACK_CHECKMATE                   :
      markMatingPositions();
      break;
    default:
      unmarkMatingPositions();
      break;
    }
  }

  const CSize &size = m_resources.getBoardSize();
  if(dc) {
    BitBlt(dc, 0,0,size.cx,size.cy,  m_imageDC, 0,0, SRCCOPY);
  /*
    const String ss = format( _T("scale:%.3lf"),m_resources.getScale());
    TextOut(dst, 10,10, ss.cstr(), ss.length());
  */
    if(m_game->getPlayerInTurn() != m_lastFlushedGameKey.getPlayerInTurn()) {
      const Player oldValue = m_lastFlushedGameKey.getPlayerInTurn();
      const Player newValue = m_game->getPlayerInTurn();
      notifyPropertyChanged(GRAPHICS_PLAYERINTURN, &oldValue, &newValue);
    }
    m_lastFlushedGameKey = m_game->getKey();
    if(m_debugFlags.m_anySet) {
      paintDebugInfo(dc);
    }
  }
  return size;
}

void ChessGraphics::flushPr() {
  flushPr(m_bufferPr->getRect());
  if(m_resources.getScale() != 1) {
    if(m_showFieldNames) {
      flushFieldNames();
    }
    if(m_visibleClocks) {
      flushClocks();
    }
    flushModeText();
  }
}

void ChessGraphics::flushPr(const CRect &srcRect, bool highQuality) {
  flushPr(srcRect.TopLeft(), srcRect.Size(), highQuality);
}

void ChessGraphics::flushPr(const CPoint &srcPoint, const CSize &srcSize, bool highQuality) {
  if(m_resources.getScale() == 1) {
    PixRect::bitBlt(m_imageDC, srcPoint,srcSize,SRCCOPY, m_bufferPr, srcPoint);
  } else {
    const CRect r = m_resources.scaleRect(CRect(srcPoint, srcSize));
    SetStretchBltMode(m_imageDC, highQuality?HALFTONE:COLORONCOLOR);
    PixRect::stretchBlt(m_imageDC, r.left,r.top,r.Width(),r.Height(), SRCCOPY, m_bufferPr, srcPoint.x,srcPoint.y,srcSize.cx,srcSize.cy);
  }
}

void ChessGraphics::startHourGlassAnimation(CWnd *wnd) {
#ifdef __NEVER__
  if(!isWatchVisible()) {
    m_resources.getHourGlassAnimation().startAnimation(wnd,CPoint(UPPERLEFTCORNER.x + 8 * FIELDSIZE.cx + 40,UPPERLEFTCORNER.y - 80),true,ANIMATE_INFINITE);
  }
#endif
}

void ChessGraphics::stopHourGlassAnimation() {
  m_resources.getHourGlassAnimation().stopAnimation();
}

void ChessGraphics::showClocks(UINT whiteTime, UINT blackTime, HDC dc) {
  m_remainingTime[WHITEPLAYER] = whiteTime;
  m_remainingTime[BLACKPLAYER] = blackTime;

  int posIndex = m_computerPlayer;

  const CSize &textSize = getTimeTextSize();
  for(Player player = WHITEPLAYER; player <= BLACKPLAYER; NEXTPLAYER(player), posIndex ^= 1) {
    const CPoint &textPos = getTimeTextPosition(posIndex);
    restoreBackground(textPos, textSize, false);
  }
  paintClocks();

  FLUSHIMAGE(dc);
}

void ChessGraphics::animateCheckMate(HDC dc) {
  const Player player  = m_game->getPlayerInTurn();
  const int    kingPos = m_game->getKingPosition(player);
  RotatePieceAnimation(this, dc, kingPos).animate();
  m_kingFlags[player] = KING_HAS_CHECKMARK | KING_UPSIDEDOWN;
  paintField(kingPos, true);
}

void ChessGraphics::unmarkAll(HDC dc) {
  unmarkMouse();
  unmarkSelectedPiece();
  unmarkLegalMoves();
  unmarkMatingPositions();
  ajourKings(true);
  unmarkLastMove();

  FLUSHIMAGE(dc);
}

void ChessGraphics::markLegalMoves() {
  unmarkFields(m_legalMoveFields);
  if(!m_showLegalMoves) {
    return;
  }

  const MoveBaseArray legalMoves = getLegalMoves();
  for(size_t i = 0; i < legalMoves.size(); i++) {
    const MoveBase &move = legalMoves[i];
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
}

void ChessGraphics::unmarkLegalMoves() {
  unmarkFields(m_legalMoveFields);
  m_legalMoveFields.clear();
}

void ChessGraphics::markLastMoveAsComputerMove(HDC dc) {
  unmarkLastMove();
  if(m_game->getPlyCount() > 0) {
    const MoveBase m = m_game->getLastMove();
    m_computerFrom          = m.getFrom();
    m_computerTo            = m.getTo();
    paintComputerMoveMarks();
  }
  FLUSHIMAGE(dc);
}

void ChessGraphics::paintComputerMoveMarks(HDC dc) {
  if(isValidPosition(m_computerFrom)) {
    markField(m_computerFrom, MOVEFROMMARK);
    markField(m_computerTo  , MOVETOMARK  );
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::unmarkLastMove(HDC dc) {
  if(isValidPosition(m_computerFrom)) {
    unmarkField(m_computerFrom);
    unmarkField(m_computerTo  );
    m_computerFrom = m_computerTo = -1;
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::markMatingPositions(HDC dc) {
  int mouse = unmarkMouse();
  unmarkMatingPositions();
  m_matingPositions = m_game->getMatingPositions();
  for(Iterator<int> it = m_matingPositions.getIterator(); it.hasNext();) {
    markField(it.next(), PINKMARK);
  }
  markMouse(mouse);
  FLUSHIMAGE(dc);
}

void ChessGraphics::unmarkMatingPositions() {
  unmarkFields(m_matingPositions);
  m_matingPositions.clear();
}

void ChessGraphics::markFields(const FieldSet &fields, FieldMark mark, HDC dc) {
  for(Iterator<int> it = fields.getIterator(); it.hasNext();) {
    markField(it.next(), mark);
  }
  FLUSHIMAGE(dc);
}

void ChessGraphics::unmarkFields(const FieldSet &fields, HDC dc) {
  for(Iterator<int> it = fields.getIterator(); it.hasNext();) {
    unmarkField(it.next());
  }
  FLUSHIMAGE(dc);
}

int ChessGraphics::markField(int pos, FieldMark mark, HDC dc) {
  paintMark(pos, mark, false);
  paintField(pos, false);
  flushPr(getFieldRect(pos, false));
  FLUSHIMAGE(dc);

  return pos;
}

void ChessGraphics::unmarkField(int pos, HDC dc) {
  const CRect r = getFieldRect(pos, false);
  restoreBackground(r, false);
  paintField(pos, false);
  flushPr(r);
  FLUSHIMAGE(dc);
}

void ChessGraphics::paintMark(int pos, FieldMark mark, bool flush, HDC dc) {
  m_resources.getFieldMarkImage(mark)->paintImage(*m_bufferPr, getFieldPosition(pos, false));
  if((pos == m_computerFrom) && (mark != MOVEFROMMARK)) {
    paintMark(pos, MOVEFROMMARK, false);
  } else if((pos == m_computerTo) && (mark != MOVETOMARK)) {
    paintMark(pos, MOVETOMARK, false);
  }
  if(flush) {
    flushPr(getFieldRect(pos, false));
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::markMouse(int pos, HDC dc) {
  if(pos == m_mouseField) {
    return;
  }

  unmarkMouse();

  m_mouseField = pos;
  if(!isValidPosition(pos)) {
    FLUSHIMAGE(dc);
    return;
  }

  const CRect r = getSelectionFrameRect(pos);

  m_selectedOldPr->rop(m_selectedOldPr->getRect(), SRCCOPY, m_bufferPr, r.TopLeft());
  m_resources.getSelectionFrameImage()->paintImage(*m_bufferPr, r.TopLeft());
  flushPr(r);
  FLUSHIMAGE(dc);
}

int ChessGraphics::unmarkMouse(HDC dc) {
  int result = m_mouseField;
  if(isValidPosition(m_mouseField)) {
    const CRect r = getSelectionFrameRect(m_mouseField);
    m_bufferPr->rop(r, SRCCOPY, m_selectedOldPr, ORIGIN);
    flushPr(r);
    FLUSHIMAGE(dc);
    m_mouseField = -1;
  }
  return result;
}

void ChessGraphics::restoreBackground(const CRect &r, bool flush, HDC dc) {
  m_bufferPr->rop(r, SRCCOPY, m_resources.getBoardImage(),r.TopLeft());
  if(flush) {
    flushPr(r);
    FLUSHIMAGE(dc);
  }
}

void ChessGraphics::restoreBackground(const CPoint &p, const CSize &size, bool flush, HDC dc) {
  m_bufferPr->rop(p, size, SRCCOPY, m_resources.getBoardImage(),p);
  if(flush) {
    flushPr(p, size);
    FLUSHIMAGE(dc);
  }
}

CRect ChessGraphics::getSelectionFrameRect(int pos) const {
  const CSize &selectFrameSize = m_resources.getSelectionFrameSize0();
  const CSize offset((selectFrameSize.cx - FIELDSIZE0.cx)/2,(selectFrameSize.cy - FIELDSIZE0.cy)/2);
  return CRect(getFieldPosition(pos,false) - offset, selectFrameSize);
}

MoveBaseArray ChessGraphics::getLegalMoves() const {
  MoveBaseArray result;
  if(!isValidPosition(m_selectedPieceField)) {
    return result;
  }

#ifdef TABLEBASE_BUILDER
  if(m_debugFlags.m_flags.m_showBackMoves) {
    const Piece *piece = m_game->getPieceAtPosition(m_selectedPieceField);
    if(piece == NULL || piece->getPlayer() == m_game->getPlayerInTurn()) {
      return result;
    }
    return m_game->getLegalBackMoves( m_selectedPieceField);
  }
#endif

  const Piece *piece = m_game->getPieceAtPosition(m_selectedPieceField);
  if(piece == NULL || piece->getPlayer() != m_game->getPlayerInTurn()) {
    return result;
  }
  return m_game->getLegalMoves(m_selectedPieceField);
}

const Piece *ChessGraphics::getSelectedPiece() const {
  return (m_selectedPieceField < 0) ? NULL : m_game->getPieceAtPosition(m_selectedPieceField);
}

void ChessGraphics::setSelectedField(int pos) {
  m_selectedPieceField = -1;
  if(!isValidPosition(pos)) {
    return;
  }
  const Piece *piece = m_game->getPieceAtPosition(pos);
  if(piece == NULL) {
    return;
  }
#ifdef TABLEBASE_BUILDER
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

void ChessGraphics::markSelectedPiece(int pos, HDC dc) {
  if(pos == m_selectedPieceField) {
    return;
  }
  const int oldMouse = unmarkMouse();
  unmarkSelectedPiece();

  setSelectedField(pos);

  paintSelectedPiece();
  markMouse(oldMouse);
  FLUSHIMAGE(dc);
}

void ChessGraphics::paintSelectedPiece() {
  if(m_selectedPieceField >= 0) {
    markLegalMoves();
    markField(m_selectedPieceField, YELLOWMARK);
  }
}

void ChessGraphics::unmarkSelectedPiece(HDC dc) {
  if(m_selectedPieceField >= 0) {
    unmarkField(m_selectedPieceField);
    m_selectedPieceField = -1;
    unmarkLegalMoves();
    FLUSHIMAGE(dc);
  }
}

bool confirmCancel(CWnd *parent) {
  return parent->MessageBox(loadString(IDS_CANCELQUESTION).cstr(), loadString(IDS_CANCELLABEL).cstr(), MB_ICONQUESTION|MB_YESNO) == IDYES;
}

void showMessage(CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format,...) {
  va_list argptr;
  va_start(argptr, format);
  vshowMessage(parent, milliSeconds, caption, format, argptr);
  va_end(argptr);
}

void vshowMessage(CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format, va_list argptr) {
  const String message = vformat(format,argptr);
  CMessageDlg dlg(milliSeconds, caption, message);
  dlg.DoModal();
//  CMessageDlgThread *thread = (CMessageDlgThread*)AfxBeginThread(RUNTIME_CLASS(CMessageDlgThread),THREAD_PRIORITY_NORMAL,0,CREATE_SUSPENDED);
//  thread->setParameters(milliSeconds, caption, message);
}
