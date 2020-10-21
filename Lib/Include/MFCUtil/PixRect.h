#pragma once

#include "WinTools.h"
#include "DirectXDeviceFactory.h"
#include "PolygonCurve2D.h"
#include "ShapeFunctions.h"
#include "ColorSpace.h"
#include "PixRectType.h"

class PixRectFont;
class GlyphCurveData;
class VideoHeader;

class TextOperator : public CurveOperator {
public:
  virtual void beginGlyph(const Point2D &offset) = 0;
  virtual void endGlyph() {
  };
};

class PixelAccessor {
  friend class PixRect;
protected:
  PixRect          &m_pixRect;
  D3DLOCKED_RECT    m_lockedRect;
  D3DSURFACE_DESC   m_desc;
#if defined(_DEBUG)
  void inline checkPoint(const TCHAR *method, UINT x, UINT y) const {
    if(x >= m_desc.Width || y >= m_desc.Height) {
      throwException(_T("%s(%u,%u) outside pixRect. Size=(%u,%u)")
                    ,method
                    ,x, y
                    ,m_desc.Width, m_desc.Height);
    }
  }
#endif
  PixelAccessor(PixRect *pixRect, DWORD flags);
  virtual ~PixelAccessor();
public:
  virtual void     setPixel(UINT x, UINT y, D3DCOLOR color)  = 0;
  virtual D3DCOLOR getPixel(UINT x, UINT y) const            = 0;
  inline  void     setPixel(const CPoint &p, D3DCOLOR color) {
    setPixel(p.x,p.y,color);
  }
  inline D3DCOLOR  getPixel(const CPoint &p) const {
    return getPixel(p.x,p.y);
  }

  const PixRect *getPixRect() const {
    return &m_pixRect;
  }

  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void fill(const CPoint &p, D3DCOLOR color);
  virtual void fillRect(D3DCOLOR color, const CRect *r = nullptr) = 0; // if r == nullptr, entire surface will be filled
};

class DWordPixelAccessor : public PixelAccessor {
private:
  DWORD *m_pixels;
  UINT   m_pixelsPerLine;
  inline DWORD *getPixelAddr(UINT x, UINT y) const {
    return m_pixels + m_pixelsPerLine * y + x;
  }
public:
  DWordPixelAccessor(PixRect *pixRect, DWORD flags) : PixelAccessor(pixRect, flags) {
    m_pixels        = (DWORD*)m_lockedRect.pBits;
    m_pixelsPerLine = m_lockedRect.Pitch / sizeof(m_pixels[0]);
  }
  void     setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR getPixel(UINT x, UINT y) const;
  // if r == nullptr, entire surface will be filled
  void fillRect(D3DCOLOR color, const CRect *r);
};

class PixRectOperator : public PointOperator {
private:
  void init();
protected:
  PixRect       *m_pixRect;
  PixelAccessor *m_pixelAccessor;
  void releasePixelAccessor();
public:
  PixRectOperator(PixRect *pr = nullptr);
  virtual ~PixRectOperator();
  virtual void setPixRect(PixRect *pixRect);
};

class PixRectFilter : public PixRectOperator {
private:
  void init();
protected:
  PixRect       *m_result;
  PixelAccessor *m_resultPixelAccessor;
  void releasePixelAccessor();
public:
  PixRectFilter() { init(); }
  // if(pr == nullptr && m_result!=m_pixrect) => releasePixelAcc(); rop(m_pixRect,m_result); delete m_result;
  // After that call __super::setPixRect(pr); m_result = m_pixRect, m_resultPA = m_pa;
  void setPixRect(PixRect *pr);
  // returns default CRect(0, 0, m_pixRrect->getWidth(), m_pixRect->getHeight())
  virtual CRect getRect() const;
};

class SetColor : public PixRectOperator {
private:
  const D3DCOLOR m_color;
public:
  SetColor(D3DCOLOR color, PixRect *pr = nullptr) : m_color(color), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SetAlpha : public PixRectOperator {
private:
  const D3DCOLOR m_alphaMask;
public:
  SetAlpha(BYTE alpha, PixRect *pr = nullptr) : m_alphaMask(D3DCOLOR_ARGB(alpha,0,0,0)), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class InvertColor : public PixRectOperator {
public:
  InvertColor(PixRect *pr = nullptr) : PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SubstituteColor : public PixRectOperator {
private:
  const D3DCOLOR m_from, m_to;
public:
  SubstituteColor(D3DCOLOR from, D3DCOLOR to, PixRect *pr = nullptr) : m_from(from), m_to(to), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class GrayScaleFilter : public PixRectFilter {
public:
  void apply(const CPoint &p);
};

class SobelFilter : public PixRectFilter {
protected:
  int Gx[3][3], Gy[3][3];
public:
  SobelFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class LaplaceFilter : public PixRectFilter {
private:
  int MASK[5][5];
public:
  LaplaceFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class GaussFilter : public PixRectFilter {
private:
  int MASK[5][5];
public:
  GaussFilter();
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class EdgeDirectionFilter : public SobelFilter {
public:
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class CannyEdgeFilter : public PixRectFilter {
public:
  void setPixRect(PixRect *src);
  CRect getRect() const;
  void apply(const CPoint &p);
};

class PixRectTextMaker : public TextOperator {
private:
  PixRect *m_pixRect;
  D3DCOLOR m_color;
  bool     m_invert;
  Point2D  m_textPos, m_glyphPos;
public:
  void line(const Point2D &from, const Point2D &to);
  void beginGlyph(const Point2D &offset);
  PixRectTextMaker(PixRect *pixRect, const Point2D &textPos, D3DCOLOR color, bool invert);
};

class ScaleParameters {
public:
  ScaleParameters();
  ScaleParameters(bool toSize, const Point2D &scale);
  ScaleParameters(bool toSize, double x, double y); // if(toSize) newImage.size = CSize(x, y);
                                                    // else       newImage.size = CSize(oldImage.size.cx*x, oldImage.size.cy*y);
  bool    m_toSize;
  Point2D m_scale;
};

class MyPolygon : public PointArray {
public:
  void  move(const CPoint &dp);
  CRect getBoundsRect() const;
  int   contains(const CPoint &p) const; // 1=inside, -1=outside, 0=edge
  void  applyToEdge(PointOperator &f, bool closingEdge = true) const;
  bool  add(const CPoint &p);
};

/*
class PixRectClipper {
private:
  LPDIRECTDRAWCLIPPER m_clipper;
  friend class PixRect;
public:
  PixRectClipper(HWND hwnd);
 ~PixRectClipper();
};
*/

class PixRect {
private:
  friend class PixelAccessor;
  friend class PixRectOperator;
  friend class PixRectClipper;
  friend class PixRectDevice;

  static const TCHAR       *s_typeName[];
  PixRectDevice            &m_device;
  PixRectType               m_type;
  D3DSURFACE_DESC           m_desc;
  mutable PixelAccessor    *m_pixelAccessor;
  mutable int               m_paRefCount;
  mutable LPDIRECT3DSURFACE m_DCSurface; // only valid when we have an outstanding DC

  union {
    LPDIRECT3DSURFACE m_surface;
    LPDIRECT3DTEXTURE m_texture;
  };

  PixRect(const PixRect &src);            // Not defined. Class not cloneable
  PixRect &operator=(const PixRect &src); // Not defined. Class not cloneable

  inline void initSurfaces() {
    m_surface = m_DCSurface = nullptr;
    m_pixelAccessor = nullptr;
    m_paRefCount = 0;
  }
  void create(PixRectType type, const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);
  void destroy();
  void createTexture(     const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);
  void createRenderTarget(const CSize &sz, D3DFORMAT pixelFormat, bool    lockable = false);         // always in D3DPOOL_DEFAULT
  void createPlainSurface(const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);

  void destroyTexture();
  void destroySurface();
  PixelAccessor *createPixelAccessor(DWORD flags = 0) const;
  void destroyPixelAccessor() const;
  void drawEllipsePart(const CPoint &start, const CPoint &end, CPoint &center, D3DCOLOR color, bool invert);
  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void init(HBITMAP src, D3DFORMAT pixelFormat, D3DPOOL pool);
  void checkHasAlphaChannel() const; // throw Exception if no alpha-channel
  bool canUseColorFill() const;
  LPDIRECT3DSURFACE getSurface() const;
  LPDIRECT3DSURFACE cloneSurface(D3DPOOL pool) const;
  D3DLOCKED_RECT    lockRect(  DWORD Flags, const CRect *rect = nullptr);
  void              unlockRect();
  void              checkType(       const TCHAR *method, PixRectType expectedType) const;
  static void       unknownTypeError(const TCHAR *method, PixRectType type);
  void              unknownTypeError(const TCHAR *method) const;
public:
  static void reOpenDirectX();
  PixRect(PixRectDevice &device);         // create a PixRect to draw directly on the screen

  PixRect(PixRectDevice &device, PixRectType type, UINT width, UINT height, D3DPOOL pool = D3DPOOL_FORCE_DWORD, D3DFORMAT pixelFormat = D3DFMT_FORCE_DWORD);
  PixRect(PixRectDevice &device, PixRectType type, const CSize &size,       D3DPOOL pool = D3DPOOL_FORCE_DWORD, D3DFORMAT pixelFormat = D3DFMT_FORCE_DWORD);
  PixRect(PixRectDevice &device, HBITMAP src,                               D3DPOOL pool = D3DPOOL_FORCE_DWORD, D3DFORMAT pixelFormat = D3DFMT_FORCE_DWORD);

  virtual ~PixRect();
  PixRect *clone(bool cloneImage = false, PixRectType type = PiXRECT_FORCE_DWORD, D3DPOOL pool = D3DPOOL_FORCE_DWORD) const;
  void moveToPool(D3DPOOL pool);
  static void showPixRect(const PixRect *pr);

  LPDIRECT3DTEXTURE &getTexture();
  LPDIRECT3DSURFACE &getRenderTarget();
  LPDIRECT3DSURFACE &getPlainSurface();

  //  static DDCAPS getEmulatorCaps();
  inline PixelAccessor *getPixelAccessor(DWORD flags = 0) const {
    if(m_paRefCount++ == 0) {
      m_pixelAccessor = createPixelAccessor(flags);
    }
    return m_pixelAccessor;
  }
  inline void releasePixelAccessor() const {
    assert(m_paRefCount > 0);
    if(--m_paRefCount==0) destroyPixelAccessor();
  }
  inline PixRectDevice &getDevice() const {
    return m_device;
  }
  inline PixRectType getType() const {
    return m_type;
  }
  inline const TCHAR *getTypeName() const {
    return s_typeName[m_type];
  }
  inline int getWidth()  const {
    return m_desc.Width;
  }
  inline int getHeight() const {
    return m_desc.Height;
  }
  inline CSize getSize() const {
    return CSize(getWidth(), getHeight());
  }

  void setSize(const CSize &size); // in pixels

  inline bool hasAlphaChannel() const {
    return hasAlphaChannel(getPixelFormat());
  }

  bool PixRect::isValidGDIFormat() const {
    return isValidGDIFormat(getPixelFormat());
  }

  inline D3DFORMAT getPixelFormat() const {
    return m_desc.Format;
  }
  inline D3DPOOL getPool() const {
    return m_desc.Pool;
  }

  static bool hasAlphaChannel( D3DFORMAT format);
  static bool isValidGDIFormat(D3DFORMAT format);
  static D3DFORMAT getPixelFormat(HBITMAP bm);
  static const TCHAR *getFormatName(D3DFORMAT format);

  CSize getSizeInMillimeters(HDC hdc = nullptr) const;

  inline CRect getRect() const {
    return CRect(ORIGIN, getSize());
  }

  void     setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR getPixel(UINT x, UINT y) const;
  void     setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p) const;
  D3DCOLOR getAverageColor(const Rectangle2D &rect) const;
  D3DCOLOR getAverageColor() const;

  void line(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void line(const CPoint &p1, const CPoint &p2, D3DCOLOR color, bool invert=false);
  void rectangle(const CRect &rect, D3DCOLOR color, bool invert=false);
  void rectangle(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert=false);
  void rectangle(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void drawDragRect(CRect *newRect, const CSize &newSize, CRect *lastRect, const CSize &lastSize);

  void polygon(const MyPolygon &polygon, D3DCOLOR color, bool invert=false, bool closingEdge=true);
  void ellipse(const CRect &rect, D3DCOLOR color, bool invert=false);
  void bezier(const Point2D &start, const Point2D &cp1, const Point2D &cp2, const Point2D &end, D3DCOLOR color, bool invert=false);
  void fillColor(D3DCOLOR color, const CRect *r = nullptr); // if r == nullptr, whole area will be colored
  void fillRect(const CRect &rect, D3DCOLOR color, bool invert=false);
  void fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert=false);
  void fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert=false);
  void fillEllipse(const CRect &rect, D3DCOLOR color, bool invert=false);
  static PixRect *mirror(     const PixRect *src, bool vertical);
  static PixRect *rotateImage(const PixRect *src, double degree, D3DCOLOR background);
  static PixRect *scaleImage( const PixRect *src, const ScaleParameters &param);
  static CSize    getRotatedSize(const CSize &size, double degree);
  // rotationCenter is the point in src pixRect about which the rotation occurs
  // dst is the point in this, where the rotationCenter is placed
  void            drawRotated(const PixRect *src, const CPoint &dst, double degree, const Point2D &rotationCenter);
  void text(const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void drawGlyph(const CPoint &p, const GlyphCurveData &glyphCurve, D3DCOLOR color, bool invert=false);
  void drawText( const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void copy(const VideoHeader &videoHeader);
  void formatConversion(const PixRect &pr);

  inline bool contains(const CPoint &p) const {
    return ((UINT)p.x < m_desc.Width) && ((UINT)p.y < m_desc.Height);
  }

  inline bool contains(UINT x, UINT y) const {
    return (x < m_desc.Width) && (y < m_desc.Height);
  }
  inline bool contains(const Point2D &p) const {
    return p.x >= 0 && p.x < m_desc.Width && p.y >= 0 && p.y < m_desc.Height;
  }

  void rop( const CRect  &dr                 , ULONG op, const PixRect *src, const CPoint &sp);
  void rop( const CRect  &dr                 , ULONG op, const PixRect *src, const CRect  &sr);
  void rop( const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp);
  void rop( int x, int y, int w, int h       , ULONG op, const PixRect *src, int sx, int sy);
  void mask(const CRect  &dr                 , ULONG op, const PixRect *src, const CPoint &sp, const PixRect *prMask);
  void mask(const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp, const PixRect *prMask);
  void mask(int x, int y, int w, int h       , ULONG op, const PixRect *src, int sx, int sy,   const PixRect *prMask);

  static void bitBlt(    PixRect *dst, int x, int y, int w, int h       , ULONG op,       HDC      src, int sx, int sy);
  static void bitBlt(    PixRect *dst, const CPoint &dp, const CSize &ds, ULONG op,       HDC      src, const CPoint &sp);
  static void bitBlt(    PixRect *dst, const CRect  &dr                 , ULONG op,       HDC      src, const CPoint &sp);
  static void bitBlt(    HDC      dst, int x, int y, int w, int h       , ULONG op, const PixRect *src, int sx, int sy);
  static void bitBlt(    HDC      dst, const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp);
  static void bitBlt(    HDC      dst, const CRect  &dr                 , ULONG op, const PixRect *src, const CPoint &sp);
  static void stretchBlt(PixRect *dst, int x, int y, int w, int h       , ULONG op, const HDC      src, int sx, int sy, int sw, int sh);
  static void stretchBlt(PixRect *dst, const CPoint &dp, const CSize &ds, ULONG op, const HDC      src, const CPoint &sp, const CSize &ss);
  static void stretchBlt(PixRect *dst, const CRect  &dr                 , ULONG op, const HDC      src, const CRect  &sr);
  static void stretchBlt(HDC      dst, int x, int y, int w, int h       , ULONG op, const PixRect *src, int sx, int sy, int sw, int sh);
  static void stretchBlt(HDC      dst, const CPoint &dp, const CSize &ds, ULONG op, const PixRect *src, const CPoint &sp, const CSize &ssize);
  static void stretchBlt(HDC      dst, const CRect  &dr                 , ULONG op, const PixRect *src, const CRect  &sr);
  static void alphaBlend(PixRect &dst, int x, int y, int w, int h                 , const PixRect &src, int sx, int sy, int sw, int sh   , int srcConstAlpha);
  static void alphaBlend(PixRect &dst, const CPoint &dp, const CSize &ds          , const PixRect &src, const CPoint &sp, const CSize &ss, int srcConstAlpha);
  static void alphaBlend(PixRect &dst, const CRect  &dr                           , const PixRect &src, const CRect  &sr                 , int srcConstAlpha);
  static void alphaBlend(HDC      dst, int x, int y, int w, int h                 , const PixRect &src, int sx, int sy, int sw, int sh   , int srcConstAlpha);
  static void alphaBlend(HDC      dst, const CPoint &dp, const CSize &dS          , const PixRect &src, const CPoint &sp, const CSize &ss, int srcConstAlpha);
  static void alphaBlend(HDC      dst, const CRect  &dr                           , const PixRect &src, const CRect  &sr                 , int srcConstAlpha);

  void replicate(int x, int y, int w, int h, const PixRect *src);
  void fill(const CPoint &p, D3DCOLOR color);
  void fillTransparent(const CPoint &p, BYTE alpha=255); // alpha = 0 => transparent, 255 = opaque
  void approximateFill(const CPoint &p, D3DCOLOR color, double tolerance);
  void preMultiplyAlpha();
  PixRect &apply(PixRectOperator &op    );                                                // return *this
  PixRect &apply(PixRectFilter   &filter);                                                // return *this

  void render();
  void setClipper(PixRectClipper *clipper);

  PixRect &operator=(HBITMAP src);
  operator HBITMAP() const;

  HDC getDC() const;
  void releaseDC(HDC dc) const;

  void fromBitmap( CBitmap &src);
  void toBitmap(   CBitmap &dst) const;

  void writeAsBMP( ByteOutputStream &out) const;
  void writeAsJPG( ByteOutputStream &out) const;
  void writeAsPNG( ByteOutputStream &out) const;
  void writeAsTIFF(ByteOutputStream &out) const;

  static PixRect *load(PixRectDevice &device, ByteInputStream &in);

  friend bool operator==(const PixRect &p1, const PixRect &p2);
  friend bool operator!=(const PixRect &p1, const PixRect &p2);
};

class GlyphPolygon {
private:
  Point2D               m_start;
  Array<PolygonCurve2D> m_polygonCurveArray;
public:
  inline GlyphPolygon() {
  }

  inline GlyphPolygon(const Point2D &start) {
    m_start             = start;
  }

  GlyphPolygon(const Point2D &start, const Array<PolygonCurve2D> &curveArray) {
    m_start             = start;
    m_polygonCurveArray = curveArray;
  }

  inline void addCurve(PolygonCurve2D &curve) {
    m_polygonCurveArray.add(curve);
  }

  Rectangle2D    getBoundingBox() const;
  Point2DArray   getAllPoints() const;

  inline const Point2D &getStart() const {
    return m_start;
  }
  inline const Array<PolygonCurve2D> &getCurveArray() const {
    return m_polygonCurveArray;
  }
};

class GlyphCurveData {
private:
  Array<GlyphPolygon> m_glyphPolygonArray;
public:

  inline GlyphCurveData() {
  }
  inline GlyphCurveData(const Array<GlyphPolygon> &polygonArray) : m_glyphPolygonArray(polygonArray) {
  }
  GlyphCurveData(HDC hdc, _TUCHAR ch, const MAT2 &m);

  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;

  inline void addPolygon(const GlyphPolygon &polygon) {
    m_glyphPolygonArray.add(polygon);
  }

  inline const Array<GlyphPolygon> &getPolygonArray() const {
    return m_glyphPolygonArray;
  }
};

class GlyphData {
public:
  PixRect         *m_pixRect;
  GLYPHMETRICS     m_metrics;
  GlyphCurveData   m_glyphCurveData;
  _TUCHAR          m_ch;

  GlyphData(PixRectDevice &device, HDC hdc, _TUCHAR ch, const MAT2 &m);
  ~GlyphData();
};

class PixRectFont {
private:
  void initGlyphData(float orientation);
  PixRectDevice                 &m_device;
  CompactArray<GlyphData*>       m_glyphData;
protected:
  CFont                          m_font;
  TEXTMETRIC                     m_textMetrics;
public:
  PixRectFont(PixRectDevice &device);
  PixRectFont(PixRectDevice &device, const LOGFONT &logfont, float orientation=0);
 ~PixRectFont();

  inline void getLogFont(LOGFONT &logfont) {
    m_font.GetLogFont(&logfont);
  }

  inline CFont &getFont() {
    return m_font;
  }
  inline const TEXTMETRIC &getTextMetrics() const {
    return m_textMetrics;
  }
  inline const GlyphData *getGlyphData(_TUCHAR index) const {
    return m_glyphData[index];
  }
};

void applyToGlyphPolygon(const GlyphPolygon   &glyphPolygon, CurveOperator &op);
void applyToGlyph(       const GlyphCurveData &glyphCurve  , CurveOperator &op);
void applyToText(        const String         &text        , const PixRectFont &font, TextOperator &op);
