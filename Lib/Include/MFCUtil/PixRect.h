#pragma once

#include "WinTools.h"
#include "D3DeviceFactory.h"
#include <vfw.h>
#include "Point2DP.h"
#include "PolygonCurve.h"
#include "ShapeFunctions.h"
#include "ColorSpace.h"

class PixRect;
class PixRectFont;
class GlyphCurveData;

class TextOperator : public CurveOperator {
public:
  virtual void beginGlyph(const Point2D &offset) = 0;
  virtual void endGlyph() {
  };
};

String get3DErrorMsg(HRESULT hr);

#ifdef _DEBUG
#define DECLARERESULTCHECKER void check3DResult(TCHAR *fileName, int line, HRESULT hr) const
#define CHECK3DRESULT(hr) check3DResult(_T(__FILE__),__LINE__,hr)
#else
#define DECLARERESULTCHECKER void check3DResult(HRESULT hr) const
#define CHECK3DRESULT(hr) check3DResult(hr)
#endif

class PixelAccessor {
protected:
  PixRect          &m_pixRect;
  D3DLOCKED_RECT    m_lockedRect;
  D3DSURFACE_DESC   m_desc;
#ifdef _DEBUG
  void inline checkPoint(const TCHAR *method, UINT x, UINT y) const {
    if (x >= m_desc.Width || y >= m_desc.Height) {
      throwException(_T("%s(%u,%u) outside pixRect. Size=(%u,%u)")
                    ,method
                    ,x, y
                    ,m_desc.Width, m_desc.Height);
    }
  }
#endif
public:
  PixelAccessor(PixRect *pixRect, DWORD flags = 0);
  virtual ~PixelAccessor();
  static PixelAccessor *createPixelAccessor(PixRect *pixRect, DWORD flags = 0);

  virtual void     setPixel(UINT x, UINT y, D3DCOLOR color)  = 0;
  virtual D3DCOLOR getPixel(UINT x, UINT y)                  = 0;
  virtual void     setPixel(const CPoint &p, D3DCOLOR color) = 0;
  virtual D3DCOLOR getPixel(const CPoint &p)                 = 0;

  const PixRect *getPixRect() const {
    return &m_pixRect;
  }

  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void fill(const CPoint &p, D3DCOLOR color);
};

class DWordPixelAccessor : public PixelAccessor {
private:
  DWORD *m_pixels;
  UINT   m_pixelsPerLine;
public:
  DWordPixelAccessor(PixRect *pixRect, DWORD flags) : PixelAccessor(pixRect, flags) {
    m_pixels        = (DWORD*)m_lockedRect.pBits;
    m_pixelsPerLine = m_lockedRect.Pitch / sizeof(m_pixels[0]);
  }
  void     setPixel(UINT x, UINT y, D3DCOLOR color);
  D3DCOLOR getPixel(UINT x, UINT y);
  void     setPixel(const CPoint &p, D3DCOLOR color);
  D3DCOLOR getPixel(const CPoint &p);
};

class PixRectOperator : public PointOperator {
private:
  void init();
protected:
  PixRect       *m_pixRect;
  PixelAccessor *m_pixelAccessor;
public:
  PixRectOperator(PixRect *pr = NULL);
  virtual ~PixRectOperator();
  virtual void setPixRect(PixRect *pixRect);
};

class PixRectFilter : public PixRectOperator {
private:
  void init();
protected:
  PixRect       *m_result;
  PixelAccessor *m_resultPixelAccessor;
  friend class PixRect;
public:
  PixRectFilter() { init(); }
  void setPixRect(PixRect *pixRect); // set m_result = m_pixRect = pixRect
  virtual CRect getRect() const; // returns default CRect(0, 0, m_pixRrect->getWidth(), m_pixRect->getHeight())
};

class SetColor : public PixRectOperator {
private:
  const D3DCOLOR m_color;
public:
  SetColor(D3DCOLOR color, PixRect *pr = NULL) : m_color(color), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SetAlpha : public PixRectOperator {
private:
  const D3DCOLOR m_alphaMask;
public:
  SetAlpha(unsigned char alpha, PixRect *pr = NULL) : m_alphaMask(D3DCOLOR_ARGB(alpha,0,0,0)), PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class InvertColor : public PixRectOperator {
public:
  InvertColor(PixRect *pr = NULL) : PixRectOperator(pr) {
  }
  void apply(const CPoint &p);
};

class SubstituteColor : public PixRectOperator {
private:
  const D3DCOLOR m_from, m_to;
public:
  SubstituteColor(D3DCOLOR from, D3DCOLOR to, PixRect *pr = NULL) : m_from(from), m_to(to), PixRectOperator(pr) {
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
  void move(const CPoint &dp);
  CRect getBoundsRect() const;
  int contains(const CPoint &p) const; // 1=inside, -1=outside, 0=edge
  void applyToEdge(PointOperator &f, bool closingEdge = true) const;
  bool add(const CPoint &p);
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

typedef enum {
  PIXRECT_TEXTURE
 ,PIXRECT_RENDERTARGET
 ,PIXRECT_PLAINSURFACE
 ,PiXRECT_FORCE_DWORD = 0xffffffff
} PixRectType;

class PixRectDevice {
private:
  LPDIRECT3DDEVICE   m_device;
  LPDIRECT3DSURFACE  m_renderTarget;
  CSize              m_renderTargetSize;
  D3DFORMAT          m_defaultPixelFormat; // same format as the screen
  float              m_appScaleX, m_appScaleY;
  mutable bool       m_exceptionInProgress;

  LPDIRECT3DSURFACE getRenderTarget();
  void releaseRenderTarget();
  void setRenderTargetSize(const CSize &size);
public:
  PixRectDevice();
  ~PixRectDevice();
  void attach(HWND hwnd, bool windowed = true, const CSize *size = NULL);
  void detach();
  void beginScene() {
    CHECK3DRESULT(m_device->BeginScene());
  }
  void endScene() {
    CHECK3DRESULT(m_device->EndScene());
  }
  void render(const PixRect *pr);
  LPDIRECT3DTEXTURE createTexture(              const CSize &size, D3DFORMAT format, D3DPOOL pool);
  LPDIRECT3DSURFACE createRenderTarget(         const CSize &size, D3DFORMAT format = D3DFMT_FORCE_DWORD, bool    lockable = false); // always in D3DPOOL_DEFAULT 
  LPDIRECT3DSURFACE createOffscreenPlainSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool);
  void releaseTexture(LPDIRECT3DTEXTURE texture);
  void releaseSurface(LPDIRECT3DSURFACE surface, PixRectType type);

  inline D3DFORMAT getDefaultPixelFormat() const {
    return m_defaultPixelFormat;
  }
  LPDIRECT3DDEVICE &getD3Device() {
    return m_device;
  }
  bool supportFormatConversion(D3DFORMAT srcFormat, D3DFORMAT dstFormat, UINT adapter = D3DADAPTER_DEFAULT) const;
  void setWorldMatrix(const D3DXMATRIX &m) {
    CHECK3DRESULT(m_device->SetTransform(D3DTS_WORLD, &m));
  }
  D3DXMATRIX &getWorldMatrix(D3DXMATRIX &m) const {
    CHECK3DRESULT(m_device->GetTransform(D3DTS_WORLD, &m));
    return m;
  }
  void set2DTransform(const CSize &size);
  static CompactArray<D3DDISPLAYMODE> getDisplayModes(UINT adapter = D3DADAPTER_DEFAULT);
  D3DCAPS getDeviceCaps() const;
  DECLARERESULTCHECKER;
  inline void resetException() {
    m_exceptionInProgress = false;
  }
  void alphaBlend(const PixRect *texture, const CRect &dstRect);
};

class PixRect {
private:
  DECLARERESULTCHECKER;
  friend class PixelAccessor;
  friend class PixRectOperator;
  friend class PixRectClipper;
  friend class PixRectDevice;

  static const TCHAR       *s_typeName[];
  PixRectDevice            &m_device;
  PixRectType               m_type;
  D3DSURFACE_DESC           m_desc;
  mutable LPDIRECT3DSURFACE m_DCSurface; // only valid when we have an outstanding DC

  union {
    LPDIRECT3DSURFACE m_surface;
    LPDIRECT3DTEXTURE m_texture;
  };

  inline void initSurfaces() {
    m_surface = m_DCSurface = NULL;
  }
  void create(PixRectType type, const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);
  void destroy();
  void createTexture(     const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);
  void createRenderTarget(const CSize &sz, D3DFORMAT pixelFormat, bool    lockable = false);         // always in D3DPOOL_DEFAULT 
  void createPlainSurface(const CSize &sz, D3DFORMAT pixelFormat, D3DPOOL pool);

  void destroyTexture();
  void destroySurface();
  void drawEllipsePart(const CPoint &start, const CPoint &end, CPoint &center, D3DCOLOR color, bool invert);
  void fill(const CPoint &p, D3DCOLOR color, ColorComparator &cmp);
  void init(HBITMAP src, D3DFORMAT pixelFormat, D3DPOOL pool);
  void checkHasAlphaChannel() const; // throw Exception if no alpha-channel
  LPDIRECT3DSURFACE getSurface() const;
  LPDIRECT3DSURFACE cloneSurface(D3DPOOL pool) const;
  D3DLOCKED_RECT    lockRect(  DWORD Flags, const CRect *rect = NULL);
  void              unlockRect();
  void              checkType(       const TCHAR *method, PixRectType expectedType) const;
  static void       unknownTypeError(const TCHAR *method, PixRectType type);
  void              unknownTypeError(const TCHAR *method) const;
public:
  static void reOpenDirectX();
  PixRect(PixRectDevice &device);         // create a PixRect to draw directly on the screen

  PixRect(const PixRect &src);            // not defined
  PixRect &operator=(const PixRect &src); // do

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
  inline PixelAccessor *getPixelAccessor(DWORD flags = 0) {
    return PixelAccessor::createPixelAccessor(this, flags);
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

  CSize getSizeInMillimeters(HDC hdc = NULL) const;

  inline CRect getRect() const {
    return CRect(0, 0, getWidth(), getHeight());
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
  void fillColor(D3DCOLOR color, const CRect *r = NULL); // if r == NULL, whole area will be colored
  void fillRect(const CRect &rect, D3DCOLOR color, bool invert=false);
  void fillRect(const CPoint &p0, const CSize &size, D3DCOLOR color, bool invert=false);
  void fillRect(int x1, int y1, int x2, int y2, D3DCOLOR color, bool invert=false);
  void fillPolygon(const MyPolygon &polygon, D3DCOLOR color, bool invert=false);
  void fillEllipse(const CRect &rect, D3DCOLOR color, bool invert=false);
  static PixRect *mirror(     const PixRect *src, bool vertical);
  static PixRect *rotateImage(const PixRect *src, double degree);
  static PixRect *scaleImage( const PixRect *src, const ScaleParameters &param);
  static CSize    getRotatedSize(const CSize &size, double degree);
  void            drawRotated(const PixRect *src, const CPoint &dst, double degree);
  void text(const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void drawGlyph(const CPoint &p, const GlyphCurveData &glyphCurve, D3DCOLOR color, bool invert=false);
  void drawText( const CPoint &p, const String &text, const PixRectFont &font, D3DCOLOR color, bool invert=false);
  void copy(VIDEOHDR &videoHeader);  
  void formatConversion(const PixRect &pr);

  inline bool contains(const CPoint &p) const {
    return (UINT)p.x < m_desc.Width && (UINT)p.y < m_desc.Height;
  }

  inline bool contains(UINT x, UINT y) const {
    return x < m_desc.Width && y < m_desc.Height;
  }
  bool contains(        const Point2D &p) const;
  bool containsExtended(const Point2D &p) const;

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
  void fillTransparent(const CPoint &p, unsigned char alpha=255); // alpha = 0 => transparent, 255 = opaque
  void approximateFill(const CPoint &p, D3DCOLOR color, double tolerance);
  void preMultiplyAlpha();
  PixRect &apply(PixRectOperator &op    );                                                // return *this
  PixRect &apply(PixRectFilter   &filter);                                                // return *this

  inline void render() {
    getDevice().render(this);
  }
  void setClipper(PixRectClipper *clipper);

  PixRect &operator=(HBITMAP src);
  operator HBITMAP() const;

  HDC getDC() const;
  void releaseDC(HDC dc) const;

  void fromBitmap( CBitmap &src);
  void toBitmap(   CBitmap &dst) const;

  void writeAsBMP( ByteOutputStream &out);
  void writeAsJPG( ByteOutputStream &out);
  void writeAsPNG( ByteOutputStream &out);
  void writeAsTIFF(ByteOutputStream &out);

  static PixRect *load(PixRectDevice &device, ByteInputStream &in);

  friend bool operator==(const PixRect &p1, const PixRect &p2);
  friend bool operator!=(const PixRect &p1, const PixRect &p2);
};

class GlyphPolygon {
public:
  Point2D             m_start;
  Array<PolygonCurve> m_polygonCurveArray;
  
  GlyphPolygon(const Point2DP &start) {
    m_start = start;
  }

  void addCurve(PolygonCurve &curve) {
    m_polygonCurveArray.add(curve);
  }
  
  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;
  void move(const Point2DP &dp);
  String toString() const;
  String toXML();
};

class GlyphCurveData {
private:
  Array<GlyphPolygon> m_glyphPolygonArray;
public:

  GlyphCurveData(HDC hdc, _TUCHAR ch, const MAT2 &m);
  GlyphCurveData();

  String toString() const;
  String toXML();
  Rectangle2D getBoundingBox() const;
  Point2DArray getAllPoints() const;
  Point2DArray getLinePoints() const;
  
  void addPolygon(const GlyphPolygon &polygon) {
    m_glyphPolygonArray.add(polygon);
  }
  
  void addLine(const Point2DP &p1, const Point2DP &p2);
  void move(const Point2DP &dp);
  const Array<GlyphPolygon> &getPolygonArray() const {
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
  PixRectDevice           &m_device;
protected:
  CFont                    m_font;
  TEXTMETRIC               m_textMetrics;
  CompactArray<GlyphData*> m_glyphData;
public:
  PixRectFont(PixRectDevice &device);
  PixRectFont(PixRectDevice &device, const LOGFONT &logfont, float orientation=0);
 ~PixRectFont();

  void getLogFont(LOGFONT &logfont);
  
  CFont &getFont() {
    return m_font;
  }
  
  const TEXTMETRIC getTextMetrics() const {
    return m_textMetrics;
  }
  
  const GlyphData *getGlyphData(_TUCHAR index) const;
};

void applyToGlyphPolygon(const GlyphPolygon   &glyphPolygon, CurveOperator &op);
void applyToGlyph(       const GlyphCurveData &glyphCurve  , CurveOperator &op);
void applyToText(        const String         &text        , const PixRectFont &font, TextOperator &op);
