#pragma once

#include <CompactHashMap.h>
#include <TinyBitSet.h>
#include <Math/Point2D.h>

class FontKey {
public:
  bool m_symbol;
  int  m_degree;
  FontKey() {}
  FontKey(bool symbol, int degree) : m_symbol(symbol), m_degree(degree) {
  }
  inline bool operator==(const FontKey &k) const {
    return (m_symbol == k.m_symbol) && (m_degree == k.m_degree);
  }
  ULONG hashCode() const {
    return (m_symbol ? 180 : 0) + m_degree;
  }
};

class FontCache : public CompactHashMap<FontKey, CFont*> {
public:
  FontCache() : CompactHashMap<FontKey, CFont*>(361) {
  }
  ~FontCache() override;
};

#define CIRCLE_RADIUS  12
#define CIRCLE_SIZE    (CIRCLE_RADIUS*2)

typedef enum { // dont swap order
  DIR_N
 ,DIR_NE
 ,DIR_E
 ,DIR_SE
 ,DIR_S
 ,DIR_SW
 ,DIR_W
 ,DIR_NW
} ArrowDirection;

const TCHAR *directionToStr(ArrowDirection dir);

class DirectionPair {
public:
  const ArrowDirection m_startDir, m_endDir;
  inline DirectionPair(ArrowDirection start, ArrowDirection end) : m_startDir(start), m_endDir(end) {
  }
  String toString() const {
    return format(_T("Start:%s, end:%s"), directionToStr(m_startDir), directionToStr(m_endDir));
  }
};

class ArrowPoints {
public:
  CPoint m_start, m_end;
  ArrowPoints() {
  }
  ArrowPoints(const CPoint &start, const CPoint &end) : m_start(start), m_end(end) {
  }
};

typedef CompactArray<CPoint> TransitionPath;

class AutomatePainter {
private:
  static CPen                 s_whitePen, s_blackPen;
  static CBrush               s_blackBrush, s_whiteBrush, s_redBrush, s_orangeBrush, s_pinkBrush, s_greenBrush, s_lightGreenBrush;
  static FontCache            s_fontCache;
  static const Point2D        s_dirVector[8];
  static const ArrowDirection s_oppositeDir[8];

  CBitmap                     m_bm;
  static void initStaticGDIObjects();
  static inline const Point2D &getDirectionVector(ArrowDirection dir) {
    return s_dirVector[dir];
  }

protected:
  const CSize   m_size;
  CDC           m_dc;

  static CFont &getFont(bool symbol, const Point2D &vector);
  void          paintCircle(         const CPoint &p, int radius, CPen *pen, CBrush *brush                    , HDC hdc = NULL);
  void          paintRing(           const CPoint &p, int radius, CPen *pen, CBrush *brush                    , HDC hdc = NULL);
  DirectionPair paintLineArrow(      const CPoint &from, const CPoint &to                                     , HDC hdc = NULL);
  DirectionPair paintBezierArrow(    const CPoint &from, const CPoint &to                                     , HDC hdc = NULL);
  void          paintLoopArrow(      const CPoint &from, int radius, DirectionPair dir, const String &text    , HDC hdc = NULL);
  void          paintPathArrow(      const TransitionPath &path                                               , HDC hdc = NULL);
  // assume pen if selected.
  void          paintArrowEnd(       const CPoint &from, const CPoint &to                                     , HDC hdc);
  CPoint        findBestTextPosition(const CPoint pFrom, ArrowDirection direction, const CSize &textSize) const;
  CSize         getTextSize(CFont &font, const String &text);
  void          setToWhite(HDC hdc = NULL);
  void          textOut(             const CPoint &p, CFont &font, const String &text, HDC hdc = NULL);
  inline HDC    getDC(HDC hdc) { return hdc ? hdc : m_dc; }
  void          flushImage(HDC hdc);
  static        ArrowDirection getVectorDirection(const Point2D &vector);
  static inline ArrowDirection getOppositeDirection(ArrowDirection dir) {
    return s_oppositeDir[dir];
  }
  AutomatePainter(const CSize &size);
 ~AutomatePainter();
public:
  static void paintCircle(    HDC hdc, const CPoint &p, int radius, CPen *pen, CBrush *brush);
  static void paintRing(      HDC hdc, const CPoint &p, int radius, CPen *pen, CBrush *brush);
  static void textOut(        HDC hdc, const CPoint &p, CFont &font, const String &text);
  void        markStartState( HDC hdc, const CPoint &p);

  static CPen *getWhitePen() {
    return &s_whitePen;
  }
  static CPen *getBlackPen() {
    return &s_blackPen;
  }
  static CBrush *getBlackBrush() {
    return &s_blackBrush;
  }
  static CBrush *getWhiteBrush() {
    return &s_whiteBrush;
  }
  static CBrush *getRedBrush() {
    return &s_redBrush;
  }
  static CBrush *getOrangeBrush() {
    return &s_orangeBrush;
  }
  static CBrush *getPinkBrush() {
    return &s_pinkBrush;
  }
  static CBrush *getGreenBrush() {
    return &s_greenBrush;
  }
  static CBrush *getLightGreenBrush() {
    return &s_lightGreenBrush;
  }
  static CFont &getFont();
};

// bit 0-2 used for acceptAttribute
#define ATTR_ISACCEPTSTATE     0x07
#define ATTR_ISSTARTSTATE      0x08
#define ATTR_HASPREDECESSOR    0x10
#define ATTR_HASSUCCESSSORS    0x20


#define DEFINE_ATTRIBUTE_TRAITS                     \
inline bool isAcceptingState() const {              \
  return (m_attributes & ATTR_ISACCEPTSTATE ) != 0; \
}                                                   \
inline bool isStartState() const {                  \
  return (m_attributes & ATTR_ISSTARTSTATE  ) != 0; \
}                                                   \
inline bool hasPredecessor() const {                \
  return (m_attributes & ATTR_HASPREDECESSOR) != 0; \
}                                                   \
inline bool hasSuccessor() const {                  \
  return (m_attributes & ATTR_HASSUCCESSSORS) != 0; \
}
