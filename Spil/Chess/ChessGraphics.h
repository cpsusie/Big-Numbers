#pragma once

#include "AnimatedImage.h"

class OffboardPiece : public CRect {
private:
  PieceKey m_key;
public:
  inline OffboardPiece() : m_key(EMPTYPIECEKEY) {
  }
  inline OffboardPiece(const CPoint &p, const CSize &size, PieceKey key) : CRect(p,size), m_key(key) {
  }

  inline OffboardPiece(const CRect &rect, PieceKey key) : CRect(rect), m_key(key) {
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
  inline bool operator==(const OffboardPiece &p) const {
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

  inline ColoredText(const String &str, COLORREF backColor, COLORREF textColor)
  : String(str)
  , m_backColor(backColor)
  , m_textColor(textColor)
  {
  }
};

class ColoredTextFields : public Array<ColoredText> {
private:
  const Player m_player;
public:
  inline ColoredTextFields(Player player) : m_player(player) {
  }
  void add(bool error, const String &str);
  void add(COLORREF backColor, COLORREF textColor, const String &str);
  void print(HDC hdc, int x, int y     , TextAlignment align) const;
  inline void print(HDC hdc, const CPoint &pos, TextAlignment align) const {
    print(hdc, pos.x, pos.y, align);
  }
};

void dtextOut(HDC dc, int x, int y,                           const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
void dtextOut(HDC dc, const CPoint &pos,                      const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
int  dtextOut(HDC dc, int x, int y,      TextAlignment align, const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);
int  dtextOut(HDC dc, const CPoint &pos, TextAlignment align, const String &str, COLORREF backColor = WHITE, COLORREF textColor = BLACK);

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
  static int            s_instanceCount;
  static const Point2DP s_upperLeftCorner0;
  static const Size2DS  s_fieldSize0;
  Size2DS               m_imageSize0;
  Size2DS               m_boardSize0;
  Size2DS               m_selectionFrameSize0;
  Point2D               m_scale;
  CSize                 m_crSize; // current clientRect size

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

  void setClientRectSize(const CSize &size);
  inline const Point2D &getScale() const {
    return m_scale;
  }
  inline double getAvgScale() const {
    return (m_scale.x + m_scale.y)/2;
  }
  inline Point2DP     scalePoint(   const Point2DP &p) const {
    return Point2DP(p.x  * m_scale.x , p.y  * m_scale.y);
  }
  inline Point2DP     unscalePoint( const Point2DP &p) const {
    return Point2DP(p.x  / m_scale.x , p.y  / m_scale.y);
  }
  inline Size2DS      scaleSize(    const Size2DS  &s) const {
    return Size2DS( s.cx * m_scale.x , s.cy * m_scale.y);
  }
  inline Rectangle2DR scaleRect(    const Rectangle2DR &r) const {
    return Rectangle2DR(scalePoint(r.getTopLeft()), scalePoint(r.getBottomRight()));
  }
  inline CFont &getBoardFont() const {
    return s_boardTextFont;
  }
  inline CFont &getDebugFont() const {
    return s_debugInfoFont;
  }
  inline const Size2DS &getBoardSize0() const {
    return m_boardSize0;
  }
  inline const CSize &getBoardSize() const {
    return m_crSize;
  }
  inline const Point2DP &getUpperLeftCorner0() const {
    return s_upperLeftCorner0;
  }
  inline Point2DP getUpperLeftCorner() const {
    return scalePoint(getUpperLeftCorner0());
  }
  inline const Size2DS &getFieldSize0() const {
    return s_fieldSize0;
  }
  inline const Size2DS getFieldSize() const {
    return scaleSize(getFieldSize0());
  }
  inline const Size2DS &getImageSize0() const {
    return m_imageSize0;
  }
  inline const Size2DS getImageSize() const {
    return scaleSize(getImageSize0());
  }
  inline const Size2DS &getSelectionFrameSize0() const {
    return m_selectionFrameSize0;
  }
  inline Size2DS getSelectionFrameSize() const {
    return scaleSize(getSelectionFrameSize0());
  }
  inline const Image *getBoardImage() const {
    return s_boardImage;
  }
  inline const Image *getSelectionFrameImage() const {
    return s_selectionFrameImage;
  }
  inline const Image *getPlayerIndicatorImage() const {
    return s_playerIndicator;
  }
  inline const Image *getPieceImage(PieceKey pk) const {
    return s_pieceImage[GET_PLAYER_FROMKEY(pk)][GET_TYPE_FROMKEY(pk)];
  }
  inline AnimatedImage &getHourGlassAnimation() {
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

#define WHITECLOCK_VISIBLE 0x01
#define BLACKCLOCK_VISIBLE 0x02
#define BOTHCLOCKS_VISIBLE (WHITECLOCK_VISIBLE | BLACKCLOCK_VISIBLE)

class FieldAttackTextPosition {
public:
  TextAlignment m_align;
  CSize         m_offset;
  inline FieldAttackTextPosition() {
  }
  inline FieldAttackTextPosition(TextAlignment align, int x, int y)
  : m_align(align)
  , m_offset(x,y)
  {
  }
};

class SavedImageRect { // to save/restore a part of Graphics.m_bufferPr;
private:
  PixRect *m_pr;
  CRect    m_rect;
  static bool needResize(const CSize &oldSize, const CSize &newSize) {
    return (newSize.cx > oldSize.cx) || (newSize.cy > oldSize.cy);
  }
  PixRect *getPixRect(const CSize &size);
  SavedImageRect(           const SavedImageRect &src); // not implemented
  SavedImageRect &operator=(const SavedImageRect &src);
public:
  SavedImageRect() : m_pr(NULL), m_rect(0, 0, 0, 0) {
  }
  ~SavedImageRect() {
    cleanup();
  }
  void cleanup();
  void save(   const PixRect *src, const CRect &r);
  void restore(PixRect *dst) const;
  const CRect &getSavedRect() const {
    return m_rect;
  }
};

class PieceDragger;

class ChessGraphics {
  friend class ChessAnimation;
  friend class AbstractPieceMoveAnimation;
  friend class RotatePieceAnimation;

private:
  HWND                        m_hwnd;
  const Game                 *m_game;
  GameKey                     m_lastFlushedGameKey;
  BYTE                        m_paintLevel;
  ChessResources              m_resources;
  PixRect                    *m_bufferPr;
  SavedImageRect              m_selectedRect;
  int                         m_mouseField    , m_selectedPieceField;
  int                         m_computerFrom  , m_computerTo;
  PieceDragger               *m_pieceDragger;
  FieldSet                    m_legalMoveFields, m_matingPositions;
  BYTE                        m_kingFlags[2];
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
  inline void pushLevel() {
    if(m_paintLevel++ == 0) {
      m_resources.setClientRectSize(getClientRect(m_hwnd).Size());
    }
  }
  inline void popLevel() {
    if(--m_paintLevel == 0) flushImage();
  }
  void allocate();
  void deallocate();
  void paintSelectedPiece();
  void markLegalMoves();
  void unmarkLegalMoves();
  void unmarkMatingPositions();
  void paintMark(int pos, FieldMark mark);
  void paintComputerMoveMarks();
  void paintFieldNames();
  void paintFieldName(HDC hdc, const CPoint &p, const String &str);
  void unpaintFieldNames();
  void paintKing(Player player);
  void paintKings();
  void paintPlayerIndicator();
  void unpaintPlayerIndicator();
  void initPlayerIndicatorRect();

  void paintModeText();
  void unpaintModeText();
  void initModeTextRect();

  void paintClocks();
  void unpaintClocks();
  void paintClock(const CPoint &pos, int seconds);

  void setSelectedField(int pos);

  CRect              getSelectionFrameRect(int pos) const;
  const CPoint      &getFirstOffboardPiecePosition(Player player) const;
  OffboardPieceArray getOffboardPieces(Player player) const;
  void               clearOffboardPieces(Player player);
  const CPoint       getTimeTextPosition(int i) const; // upperText:i = 0  lowerText: i = 1
  const CRect       &getTimeTextRect(int i) const;
  const CSize       &getTimeTextSize() const;
  void               paintDebugInfo();
  void               paintFieldAttacks(     HDC dc, const Game &game1, const Game &game2);
  void               paintFieldAttacks(     HDC dc, const CPoint &p, UINT count1, UINT count2, Player player, AttackInfoField f, bool hasKing1=false, bool hasKing2=false);
  void               paintStateString(      HDC dc, int &line,                const Game &game1, const Game &game2);
  void               paintStateString(      HDC dc, int &line, Player player, const Game &game1, const Game &game2);
  void               paintLastMove(         HDC dc, int &line);
  void               initFieldTextOffsets(  HDC dc);
  const FieldAttackTextPosition *getLDAOffset(AttackInfoField f);
  const FieldAttackTextPosition *getSDAOffset(Player player);

  void markFields(            const FieldSet &fields, FieldMark mark);
  void unmarkFields(          const FieldSet &fields);
  void markMatingPositions();
  void paintEmptyField(       int pos);
  void paintField(            int pos);
  void paintOffboardPieces(   Player player);
  void restoreBackground(     const CRect  &r);
  void restoreBackground(     const CPoint &p, const CSize &size);
  inline void saveImageRect(SavedImageRect &sr, const CRect &r) const {
    sr.save(m_bufferPr, r);
  }
  inline void restoreImageRect(const SavedImageRect &sr) const {
    sr.restore(m_bufferPr);
  }
  void addFieldNameRectangle( const CPoint &corner1, const CPoint &corner2, const CSize &charSize);
  void updatePlayerIndicator();
  CSize flushImage();
  MoveBaseArray getLegalMoves() const;
public:
  ChessGraphics(CWnd *wnd);
 ~ChessGraphics();
  void  reopen();
  void  setGame(const Game &game);
  int   getBoardPosition(const CPoint &point) const;
  inline void beginPaint() {
    pushLevel();
  }
  inline void endPaint() {
    popLevel();
  }
  inline void render() {
    m_bufferPr->render();
  }
  const Piece         *getSelectedPiece() const;
  const OffboardPiece *getOffboardPieceByPosition(const CPoint &point) const;
  const OffboardPiece *getOffboardPieceByKey(PieceKey key) const;
  inline int getMouseField() const {
    return m_mouseField;
  }
  void  paintAll();
  void  paintGamePositions();
  int   unmarkMouse();
  void  markMouse(           int    pos);
  void  markSelectedPiece(   int    pos);
  void  unmarkSelectedPiece();
  void  unmarkAll();
  int   markField(           int pos,             FieldMark mark); // return pos
  void  unmarkField(         int pos);
  void  repaintOffboardPieces(Player player);
  void  markLastMoveAsComputerMove();
  void  unmarkLastMove();
  void  beginDragPiece(      const CPoint &point, const OffboardPiece *obp);
  void  beginDragPiece(      const CPoint &point, PieceKey key            );
  void  dragPiece(           const CPoint &point                          );
  void  endDragPiece(                                                     );
  void  animateMove(         const MoveBase &m                            );
  void  animateCheckMate();

  Point2DP     getFieldPosition(int r, int c, bool scaled) const;
  Point2DP     getFieldPosition(int pos     , bool scaled) const;
  Rectangle2DR getFieldRect(    int pos     , bool scaled) const;

  inline bool isDragging() const {
    return m_pieceDragger != NULL;
  }
  PieceKey getDraggedPiece() const;
  const Image *getPieceImage(int pos) const;

  void setComputerPlayer(Player computerPlayer);
  inline char getVisibleClocks() const {
    return m_visibleClocks;
  }
  void setVisibleClocks(    char visible); // visible = 0,1,2,3. if(bit[i] is set, clock[i] is visible. i=[WHITE/BLACK]
  void setShowFieldNames(   bool show );
  void setShowLegalMoves(   bool show );
  void setShowPlayerInTurn( bool show );
  void setModeText(const String &text);

  inline const CSize getBoardSize(bool scaled) const {
    return scaled ? m_resources.getBoardSize() : m_resources.getBoardSize0();
  }
  inline const CSize getFieldSize(bool scaled) const {
    return scaled ? m_resources.getFieldSize() : m_resources.getFieldSize0();
  }
  inline const CSize getImageSize(bool scaled) const {
    return scaled ? m_resources.getImageSize() : m_resources.getImageSize0();
  }
  inline const Game &getGame() const {
    return *m_game;
  }

  inline const ChessResources &getResources() const {
    return m_resources;
  }

  inline void setShowFieldAttacks(   bool show) { m_debugFlags.m_flags.m_showFieldAttacks    = show;  }
  inline void setShowMaterial(       bool show) { m_debugFlags.m_flags.m_showMaterial        = show;  }
  inline void setShowCheckingSDAPos( bool show) { m_debugFlags.m_flags.m_showCheckingSDAPos  = show;  }
  inline void setShowBishopFlags(    bool show) { m_debugFlags.m_flags.m_showBishopFlags     = show;  }
  inline void setShowPawnCount(      bool show) { m_debugFlags.m_flags.m_showPawnCount       = show;  }
  inline void setShowLastCapture(    bool show) { m_debugFlags.m_flags.m_showLastCapture     = show;  }
  inline void setShowPositionRepeats(bool show) { m_debugFlags.m_flags.m_showPositionRepeats = show;  }
  inline void setShowLastMoveInfo(   bool show) { m_debugFlags.m_flags.m_showLastMoveInfo    = show;  }
  inline void setShowSetupMode(      bool show) { m_debugFlags.m_flags.m_showSetupMode       = show;  }
  inline void setShowFEN(            bool show) { m_debugFlags.m_flags.m_showFEN             = show;  }
  inline void setShowBackMoves(      bool show);
  inline void resetAllDebugFlags() {
    m_debugFlags.m_anySet = 0;
  }

  void startHourGlassAnimation();
  void stopHourGlassAnimation();
  void showClocks(UINT whiteTime, UINT blackTime);
};

class FontSizeMenuManager {
public:
  static void setFontSize(CWnd *wnd, int percent);
};

void showMessage(  CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format,... );
void vshowMessage( CWnd *parent, int milliSeconds, const String &caption, const TCHAR *format, va_list argptr);
bool confirmCancel(CWnd *parent);
