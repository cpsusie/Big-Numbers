#pragma once

#include <PropertyContainer.h>
#include "AnimatedImage.h"

class OffboardPiece : public CRect {
private:
  PieceKey m_key;
public:
  OffboardPiece() : m_key(EMPTYPIECEKEY) {
  }
  OffboardPiece(const CPoint &p, const CSize &size, PieceKey key) : CRect(p,size), m_key(key) {
  }

  OffboardPiece(const CRect &rect, PieceKey key) : CRect(rect), m_key(key) {
  }

  inline Player getPlayer() const {
    return GET_PLAYER_FROMKEY(m_key);
  }

  inline PieceType getType() const {
    return GET_TYPE_FROMKEY(m_key);
  }
  inline PieceKey getKey() const {
    return m_key;
  }
  bool operator==(const OffboardPiece &p) const {
    return (((CRect&)(*this)) == p) && (m_key == p.m_key);
  }
};

typedef CompactArray<OffboardPiece> OffboardPieceArray;

typedef enum {
  FROM_LEFT
 ,FROM_RIGHT
 ,FROM_BELOVE
 ,FROM_ABOVE
 ,FROM_LOWERDIAG1
 ,FROM_UPPERDIAG1
 ,FROM_LOWERDIAG2
 ,FROM_UPPERDIAG2
 ,FROM_SHORTDISTANCE
} AttackInfoField;

typedef enum {
  ALIGN_LEFT
 ,ALIGN_RIGHT
 ,ALIGN_CENTER
} TextAlignment;

class ColoredText : public String {
public:
  COLORREF m_backColor, m_textColor;

  ColoredText(const String &str, COLORREF backColor, COLORREF textColor)
  : String(str)
  , m_backColor(backColor)
  , m_textColor(textColor)
  {
  }
};

void dtextOut(HDC dc, int x, int y,                           const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
void dtextOut(HDC dc, const CPoint &pos,                      const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
int  dtextOut(HDC dc, int x, int y,      TextAlignment align, const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
int  dtextOut(HDC dc, const CPoint &pos, TextAlignment align, const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
void dtextOut(HDC dc, int x, int y,      TextAlignment align, const Array<ColoredText> &strings);
void dtextOut(HDC dc, const CPoint &pos, TextAlignment align, const Array<ColoredText> &strings);

CPoint operator*(const CPoint &pt, double factor);
CSize  operator*(const CSize  &sz, double factor);

typedef enum {
  BLUEMARK
 ,GREENMARK
 ,PINKMARK
 ,PURPLEMARK
 ,YELLOWMARK
 ,CHECKEDKING
 ,MOVEFROMMARK
 ,MOVETOMARK
} FieldMark;

class ChessResources {
private:
  static int          s_instanceCount;
  static CSize        s_screenSize;
  static const CPoint s_upperLeftCorner0;
  static const CSize  s_fieldSize0;
  double              m_maxScale;
  double              m_scale;
  CSize               m_boardSize0;
  CSize               m_selectionFrameSize0;
  CSize               m_boardSize;
  CSize               m_selectionFrameSize;
  CPoint              m_upperLeftCorner;
  CSize               m_fieldSize;

  static Image       *s_boardImage;
  static ImageArray   s_pieceImage[2];
  static ImageArray   s_markImage;
  static Image       *s_selectionFrameImage, *s_playerIndicator;
  static CFont        s_boardTextFont, s_debugInfoFont;
  AnimatedImage       m_hourGlassImage;
public:
  ChessResources();
  ~ChessResources();
  void load();
  void unload();
  void reload();
  ChessResources(const ChessResources &src);            // not defined
  ChessResources &operator=(const ChessResources &src); // not defined

  const CSize &setScale(double scale);
  inline double getScale() const {
    return m_scale;
  }
  CPoint scalePoint(  const CPoint &p) const;
  CPoint unscalePoint(const CPoint &p) const;
  CSize  scaleSize(   const CSize  &s) const;
  CRect  scaleRect(   const CRect  &r) const;
  CFont &getBoardFont()                const {
    return s_boardTextFont;
  }
  CFont &getDebugFont()                const {
    return s_debugInfoFont;
  }
  inline const CSize &getScreenSize() const {
    return s_screenSize;
  }
  inline const CSize &getBoardSize0() const {
    return m_boardSize0;
  }
  inline const CSize &getBoardSize() const {
    return m_boardSize;
  }
  inline const CPoint &getUpperLeftCorner0() const {
    return s_upperLeftCorner0;
  }
  inline const CPoint &getUpperLeftCorner() const {
    return m_upperLeftCorner;
  }
  inline const CSize &getFieldSize0() const {
    return s_fieldSize0;
  }
  inline const CSize &getFieldSize() const {
    return m_fieldSize;
  }
  inline const CSize &getSelectionFrameSize0() const {
    return m_selectionFrameSize0;
  }
  inline const CSize &getSelectionFrameSize() const {
    return m_selectionFrameSize;
  }
  const Image *getBoardImage() const {
    return s_boardImage;
  }
  const Image *getSelectionFrameImage() const {
    return s_selectionFrameImage;
  }
  const Image *getPlayerIndicatorImage() const {
    return s_playerIndicator;
  }
  inline const Image *getPieceImage(PieceKey pk) const {
    return s_pieceImage[GET_PLAYER_FROMKEY(pk)][GET_TYPE_FROMKEY(pk)];
  }
  AnimatedImage &getHourGlassAnimation() {
    return m_hourGlassImage;
  }

  inline const Image *getPieceImage(const Piece *piece) const {
    return getPieceImage(piece->getKey());
  }

  CBitmap &getSmallPieceBitmap(CBitmap &dst, PieceKey pk) const; // return dst
  const Image *getFieldMarkImage(FieldMark m) const;
  static void loadBitmap(CBitmap &dst, int resId, ImageType type);
};

class DebugFlags {
public:
  union {
    struct {
      bool m_showFieldAttacks    : 1;
      bool m_showMaterial        : 1;
      bool m_showCheckingSDAPos  : 1;
      bool m_showBishopFlags     : 1;
      bool m_showPawnCount       : 1;
      bool m_showLastCapture     : 1;
      bool m_showPositionRepeats : 1;
      bool m_showLastMoveInfo    : 1;
      bool m_showBackMoves       : 1;
      bool m_showSetupMode       : 1;
      bool m_showFEN             : 1;
    } m_flags;
    UINT m_anySet;
  };
  bool showState() const;
  DebugFlags();
};

typedef enum {
  GRAPHICS_PLAYERINTURN
 ,GRAPHICS_COMPUTERPLAYER
} GraphicsProperty;

#define WHITECLOCK_VISIBLE 0x01
#define BLACKCLOCK_VISIBLE 0x02
#define BOTHCLOCKS_VISIBLE (WHITECLOCK_VISIBLE | BLACKCLOCK_VISIBLE)

class FieldAttackTextPosition {
public:
  TextAlignment m_align;
  CSize         m_offset;
  FieldAttackTextPosition() {
  }
  FieldAttackTextPosition(TextAlignment align, int x, int y)
  : m_align(align)
  , m_offset(x,y)
  {
  }
};

class ChessGraphics : public PropertyContainer {
  friend class PieceDragger;
  friend class MovePieceAnimation;

private:
  const Game                 *m_game;
  GameKey                     m_lastFlushedGameKey;
  ChessResources              m_resources;
  PixRect                    *m_bufferPr, *m_selectedOldPr;
  HDC                         m_imageDC;
  HBITMAP                     m_imageBitmap;
  int                         m_mouseField    , m_selectedPieceField;
  int                         m_computerFrom  , m_computerTo;
  PieceDragger               *m_pieceDragger;
  FieldSet                    m_legalMoveFields, m_matingPositions;
  unsigned char               m_kingFlags[2];
  OffboardPieceArray          m_offboardPieces[2];
  CompactArray<CRect>         m_fieldNamesRectangles;
  Player                      m_computerPlayer;
  char                        m_visibleClocks;
  bool                        m_showFieldNames;
  bool                        m_showLegalMoves;
  bool                        m_showPlayerInTurn;
  CRect                       m_playerIndicatorRect;
  String                      m_modeText;
  CRect                       m_modeTextRect;
  UINT                        m_remainingTime[2];
  DebugFlags                  m_debugFlags;
  CSize                       m_lastDebugFieldSize;
  void allocate();
  void deallocate();
  void paintSelectedPiece();
  void markLegalMoves();
  void unmarkLegalMoves();
  void unmarkMatingPositions();
  void paintMark(int pos, FieldMark mark, bool flush, HDC dc = NULL);
  void paintComputerMoveMarks(HDC dc = NULL);
  void paintFieldNames();
  void paintFieldName(HDC dc, const CPoint &p, const String &str);
  void unpaintFieldNames();
  void flushFieldNames();
  void ajourKing(Player player, bool flush, HDC dc = NULL);
  void ajourKings(bool flush, HDC dc = NULL);
  void paintPlayerIndicator(  bool flush, HDC dc = NULL);
  void unpaintPlayerIndicator(HDC dc = NULL);
  void initPlayerIndicatorRect();

  void paintModeText(bool flush);
  void unpaintModeText();
  void initModeTextRect();
  void flushModeText();

  void paintClocks();
  void unpaintClocks();
  void paintClock(HDC dc, const CPoint &pos, int seconds);
  void flushClocks();

  void flushPr();                                                                      // flush m_bufferPr to m_imageDC (include scaling)
  void flushPr(const CRect  &srcRect, bool highQuality=false);                         // flush part of m_bufferPr to m_imageDC
  void flushPr(const CPoint &srcPoint, const CSize &srcSize, bool highQuality=false);  // do
  void setSelectedField(int pos);

  CRect              getSelectionFrameRect(int pos) const;
  const CPoint      &getFirstOffboardPiecePosition(Player player) const;
  OffboardPieceArray getOffboardPieces(Player player) const;
  void               clearOffboardPieces(Player player, bool flush);
  const CPoint &getTimeTextPosition(int i) const; // upperText:i = 0  lowerText: i = 1
  const CRect  &getTimeTextRect(int i) const;
  const CSize  &getTimeTextSize() const;
  void paintDebugInfo(        HDC dc); // this is BOARDDC
  void paintFieldAttacks(     HDC dc, const Game &game1, const Game &game2);
  void paintFieldAttacks(     HDC dc, const CPoint &p, UINT count1, UINT count2, Player player, AttackInfoField f, bool hasKing1=false, bool hasKing2=false);
  void paintStateString(      HDC dc, int &line,                const Game &game1, const Game &game2);
  void paintStateString(      HDC dc, int &line, Player player, const Game &game1, const Game &game2);
  void paintLastMove(         HDC dc, int &line);
  void  initFieldTextOffsets( HDC dc);
  const FieldAttackTextPosition *getLDAOffset(AttackInfoField f);
  const FieldAttackTextPosition *getSDAOffset(Player player);

  void markFields(            const FieldSet &fields,   FieldMark mark,             HDC dc = NULL);
  void unmarkFields(          const FieldSet &fields,                               HDC dc = NULL);
  void markMatingPositions(                                                         HDC dc = NULL);
  void paintEmptyField(       int pos,                                  bool flush, HDC dc = NULL);
  void paintField(            int pos,                                  bool flush, HDC dc = NULL);
  void paintOffboardPieces(   Player player,                            bool flush, HDC dc = NULL);
  void restoreBackground(     const CRect  &r,                          bool flush, HDC dc = NULL);
  void restoreBackground(     const CPoint &p, const CSize &size,       bool flush, HDC dc = NULL);
  void addFieldNameRectangle(const CPoint &corner1, const CPoint &corner2, const CSize &charSize);
  void updatePlayerIndicator( HDC dc = NULL);
  MoveBaseArray getLegalMoves() const;
public:
  ChessGraphics();
 ~ChessGraphics();
  void reopen();
  void  setGame(const Game &game);
  int   getBoardPosition(const CPoint &point) const;

  const CSize         &getScreenSize() const {
    return m_resources.getScreenSize();
  }
  const Piece         *getSelectedPiece() const;
  const OffboardPiece *getOffboardPieceByPosition(const CPoint &point) const;
  const OffboardPiece *getOffboardPieceByKey(PieceKey key) const;
  CSize paintBoard(                                                           HDC dc = NULL);
  void  paintGamePosition(                                                    HDC dc = NULL);
  int   getMouseField() const {
    return m_mouseField;
  }
  int   unmarkMouse(                                                          HDC dc = NULL);
  void  markMouse(           int    pos,                                      HDC dc = NULL);
  void  markSelectedPiece(   int    pos,                                      HDC dc = NULL);
  void  unmarkSelectedPiece(                                                  HDC dc = NULL);
  void  unmarkAll(                                                            HDC dc = NULL);
  void  animateMove(         const MoveBase &m,                               HDC dc);
  void  animateCheckMate(                                                     HDC dc);
  void  repaintOffboardPieces(Player player,                                  HDC dc = NULL);
  void  markLastMoveAsComputerMove(                                           HDC dc = NULL);
  void  unmarkLastMove(                                                       HDC dc = NULL);
  void  startDragPiece(      const CPoint &point, const OffboardPiece *obp,   HDC dc);
  void  startDragPiece(      const CPoint &point, PieceKey key,               HDC dc);
  void  dragPiece(           const CPoint &point,                             HDC dc);
  void  endDragPiece(                                                         HDC dc = NULL);
  int   markField(           int pos,             FieldMark mark,             HDC dc = NULL); // return pos
  void  unmarkField(         int pos            ,                             HDC dc = NULL);

  CPoint getFieldPosition(int r, int c, bool scaled) const;
  CPoint getFieldPosition(int pos     , bool scaled) const;
  CRect  getFieldRect(    int pos     , bool scaled) const;

  bool isDragging() const {
    return m_pieceDragger != NULL;
  }
  PieceKey getDraggedPiece() const;
  const Image *getPieceImage(int pos) const;

  void setComputerPlayer(Player computerPlayer, HDC dc = NULL);
  inline char getVisibleClocks() const {
    return m_visibleClocks;
  }
  void setVisibleClocks(    char visible, HDC dc = NULL); // visible = 0,1,2,3. if(bit[i] is set, clock[i] is visible. i=[WHITE/BLACK]
  void setShowFieldNames(   bool show , HDC dc = NULL);
  void setShowLegalMoves(   bool show , HDC dc = NULL);
  void setShowPlayerInTurn( bool show , HDC dc = NULL);
  void setModeText(const String &text, HDC dc = NULL);

  const CSize &getBoardSize(bool scaled) const {
    return scaled ? m_resources.getBoardSize() : m_resources.getBoardSize0();
  }
  const CSize &getFieldSize(bool scaled) const {
    return scaled ? m_resources.getFieldSize() : m_resources.getFieldSize0();
  }
  const Game &getGame() const {
    return *m_game;
  }

  const ChessResources &getResources() const {
    return m_resources;
  }

  CSize setBoardSize(const CSize &size);

  CSize flushImage(HDC dc, bool resize);

  void setShowFieldAttacks(   bool show) { m_debugFlags.m_flags.m_showFieldAttacks    = show;  }
  void setShowMaterial(       bool show) { m_debugFlags.m_flags.m_showMaterial        = show;  }
  void setShowCheckingSDAPos( bool show) { m_debugFlags.m_flags.m_showCheckingSDAPos  = show;  }
  void setShowBishopFlags(    bool show) { m_debugFlags.m_flags.m_showBishopFlags     = show;  }
  void setShowPawnCount(      bool show) { m_debugFlags.m_flags.m_showPawnCount       = show;  }
  void setShowLastCapture(    bool show) { m_debugFlags.m_flags.m_showLastCapture     = show;  }
  void setShowPositionRepeats(bool show) { m_debugFlags.m_flags.m_showPositionRepeats = show;  }
  void setShowLastMoveInfo(   bool show) { m_debugFlags.m_flags.m_showLastMoveInfo    = show;  }
  void setShowSetupMode(      bool show) { m_debugFlags.m_flags.m_showSetupMode       = show;  }
  void setShowFEN(            bool show) { m_debugFlags.m_flags.m_showFEN             = show;  }
  void setShowBackMoves(      bool show);
  void resetAllDebugFlags();

  void startHourGlassAnimation(CWnd *wnd);
  void stopHourGlassAnimation();
  void showClocks(UINT whiteTime, UINT blackTime, HDC dc);
};

class FontSizeMenuManager {
public:
  static void setFontSize(CWnd *wnd, int percent);
};

void showMessage( CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format,... );
void vshowMessage(CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format, va_list argptr);
bool confirmCancel(CWnd *parent);
