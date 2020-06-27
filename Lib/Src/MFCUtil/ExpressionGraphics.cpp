#include "pch.h"
#include <CompactHashMap.h>
#include <Math/Expression/ParserTree.h>
#include <Math/Expression/ExpressionNode.h>
#include <MFCUtil/ExpressionGraphics.h>

//#define SHOW_LINES

namespace Expr {

using namespace std;

String numberFormatToString(NumberFormat nf) {
  switch(nf) {
  case SCIENTIFIC_NOTATION : return _T("scientific" );
  case ENGINEERING_NOTATION: return _T("engineering");
  case E_NOTATION          : return _T("enotation"  );
  default                  : return format(_T("Unknwn numberformat:%d"), nf);
  }
}

NumberFormat getNumberFormat(const String &str) {
  if(str.equalsIgnoreCase(_T("scientific"))) {
    return SCIENTIFIC_NOTATION;
  } else if(str.equalsIgnoreCase(_T("engineering"))) {
    return ENGINEERING_NOTATION;
  } else if(str.equalsIgnoreCase(_T("enotation"))) {
    return E_NOTATION;
  } else {
    return SCIENTIFIC_NOTATION;
  }
}
typedef CompactKeyType<UINT> FontSizeKey;

#define CUT_LEFT   0x01
#define CUT_RIGHT  0x02
#define CUT_TOP    0x04
#define CUT_BOTTOM 0x08

class FontCache : public CompactHashMap<FontSizeKey, CFont*> {
public:
  ~FontCache() override;
  CFont *getFont(bool text, int fontSize);
};

FontCache::~FontCache() {
  for(Iterator<Entry<FontSizeKey, CFont*> > it = getEntryIterator(); it.hasNext();) {
    CFont *font = it.next().getValue();
    font->DeleteObject();
    SAFEDELETE(font);
  }
  clear();
}

CFont *FontCache::getFont(bool text, int fontSize) {
  const UINT key = (text ? (1<<31) : 0) | fontSize;
  CFont **font = get(key);
  if(font == NULL) {
    CFont *newFont = new CFont; TRACE_NEW(newFont);

//#define TEXTFONT _T(""DejaVu Sans Mono")
#define TEXTFONT   _T("DejaVu Serif")
#define SYMBOLFONT _T("Symbol")

    const TCHAR *fontName = text ? TEXTFONT : SYMBOLFONT;

    newFont->CreateFont(-fontSize, 0, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                       ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                       ,DEFAULT_PITCH | FF_MODERN
                       ,fontName
                       );
    put(key, newFont);
    font = get(key);
  }
  return *font;
}

class SymbolString {
public:
  bool         m_textFont;
  const TCHAR *m_text;
  SymbolString() {
  }
  SymbolString(bool textFont, const TCHAR *text) : m_textFont(textFont), m_text(text) {
  }
};

class SymbolStringMap : public CompactSymbolHashMap<SymbolString> {
private:
  void putString(ExpressionInputSymbol symbol, bool textFont, const TCHAR *text) {
    put(symbol, SymbolString(textFont, text));
  }
public:
  SymbolStringMap();
};

SymbolStringMap::SymbolStringMap() {
  putString(EQ             , true , _T("="    ));
  putString(ASSIGN         , true , _T("="    ));
  putString(NE             , false, _T("\xb9" ));
  putString(GT             , true , _T(">"    ));
  putString(GE             , false, _T("\xb3" ));
  putString(LT             , true , _T("<"    ));
  putString(LE             , false, _T("\xa3" ));
  putString(AND            , false, _T("\xd9" ));
  putString(OR             , false, _T("\xda" ));
  putString(NOT            , true , _T("\xac" ));
  putString(INDEXEDSUM     , false, _T("\xe5" ));
  putString(INDEXEDPRODUCT , false, _T("\xd5" ));
  putString(GAMMA          , false, _T("\x47" ));
  putString(NORM           , false, _T("\x46" ));
  putString(ROOT           , false, _T("\xd6" ));
  putString(FAC            , true , _T("!"    ));
  putString(IIF            , true , _T("if"   ));
  putString(MOD            , true , _T(" mod "));
  putString(PLUS           , true , _T("+"    ));
  putString(MINUS          , true , _T("-"    ));
  putString(UNARYMINUS     , true , _T("-"    ));
  putString(PROD           , true , _T("\xB7" ));
  putString(COMMA          , true , _T(","    ));
  putString(SEMI           , true , _T(";"    ));
  putString(LPAR           , true , _T("("    ));
  putString(RPAR           , true , _T(")"    ));
  putString(LB             , true , _T("["    ));
  putString(RB             , true , _T("]"    ));
}

typedef enum {
  NORMAL_IMAGE
 ,TEXT_IMAGE
} ImageType;

class AlignedImage : public PixRect {
private:
  int m_horizontalAlignment; // CenterLine from top. Default is height/2
  int m_leftFiller;          // Empty space (in pixels) filled when concatenating images. Can be negative. Default=0
  int m_fontSize;
public:
  AlignedImage(PixRectDevice &device, int fontSize, const CSize &sz);
  AlignedImage(PixRectDevice &device, int fontSize, const CSize &sz, D3DCOLOR backgroundColor);

  // Set centerLine (in pixels) from top. Default is height/2
  inline AlignedImage *setAlignment(int align) {
    m_horizontalAlignment = align;
    return this;
  }

  // Set empty space (in pixels) filled when concatenating images. Can be negative. Default=0
  inline void setLeftFiller(int leftFiller) {
    m_leftFiller = leftFiller;
  }
  inline int getLeftFiller() const {
    return m_leftFiller;
  }
  inline int getAlignment() const {
    return m_horizontalAlignment;
  }
  inline void horizontalLine(int y, D3DCOLOR color = BLACK) {
    line(0, y, getWidth()-1, y, color);
  }
  virtual ImageType getImageType() const {
    return NORMAL_IMAGE;
  }
  inline int getFontSize() const {
    return m_fontSize;
  }
};

AlignedImage::AlignedImage(PixRectDevice &device, int fontSize, const CSize &sz) : m_fontSize(fontSize), PixRect(device, PIXRECT_PLAINSURFACE, sz) {
  rop(0,0,sz.cx,sz.cy, WHITENESS, NULL, 0,0);
  setAlignment(sz.cy/2);
  m_leftFiller = 0;
}

AlignedImage::AlignedImage(PixRectDevice &device, int fontSize, const CSize &sz, D3DCOLOR backgroundColor) : m_fontSize(fontSize), PixRect(device, PIXRECT_PLAINSURFACE, sz) {
  fillRect(getRect(), backgroundColor);
  setAlignment(sz.cy/2);
  m_leftFiller = 0;
}

class AlignedTextImage : public AlignedImage {
private:
  TEXTMETRIC   m_textMetric;
public:
  AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const String &text, bool textFont, int fontSize, D3DCOLOR backgroundColor);
  AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const CSize &size);
  AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const CSize &size, D3DCOLOR backgroundColor);

  inline const TEXTMETRIC &getTextMetric() const {
    return m_textMetric;
  }
  inline ImageType getImageType() const {
    return TEXT_IMAGE;
  }
};

AlignedTextImage::AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const String &text, bool textFont, int fontSize, D3DCOLOR backgroundColor)
: AlignedImage(device, fontSize, CSize(10,10)) {

  CFont         *font   = fontCache.getFont(textFont, fontSize);
  HDC            hdc    = getDC();
  HGDIOBJ        oldGDI = SelectObject(hdc, *font);
  GetTextMetrics(hdc, &m_textMetric);
  const CSize    size   = getTextExtent(hdc, text);
  SelectObject(hdc, oldGDI);

  releaseDC(hdc);

  setSize(size);
  hdc    = getDC();
  oldGDI = SelectObject(hdc, *font);
  if(backgroundColor != D3D_WHITE) {
    SetBkColor(hdc, D3DCOLOR2COLORREF(backgroundColor));
  }
//  if(!textFont) SetTextColor(hdc, RGB(0,0,128));
  textOut(hdc, 0,0, text);

  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
  setAlignment(size.cy/2);

//  horizontalLine(m_textMetric.tmHeight - m_textMetric.tmAscent + m_textMetric.tmInternalLeading , D3DCOLOR_XRGB(255,255,255));
#if defined(SHOW_LINES)
  horizontalLine(m_textMetric.tmHeight - m_textMetric.tmDescent, D3DCOLOR_XRGB(255,  20,20));
#endif

}

AlignedTextImage::AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const CSize &size) : AlignedImage(device, size.cy, size) {
  CFont         *font   = fontCache.getFont(true, size.cy);
  HDC            hdc    = getDC();
  HGDIOBJ        oldGDI = SelectObject(hdc, *font);
  GetTextMetrics(hdc, &m_textMetric);
  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
}

AlignedTextImage::AlignedTextImage(PixRectDevice &device, FontCache &fontCache, const CSize &size, D3DCOLOR backgroundColor) : AlignedImage(device, size.cy, size, backgroundColor) {
  CFont         *font   = fontCache.getFont(true, size.cy);
  HDC            hdc    = getDC();
  HGDIOBJ        oldGDI = SelectObject(hdc, *font);
  GetTextMetrics(hdc, &m_textMetric);
  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
}

class ImageArray : public CompactArray<const AlignedImage*> {
  inline static int getArgCount(const AlignedImage *img1, va_list argptr) {
    int count = 1;
    for(const AlignedImage *img = va_arg(argptr, const AlignedImage*); img; img = va_arg(argptr, const AlignedImage*)) {
      count++;
    }
    return count;
  }
public:
  inline ImageArray() {
  }
  inline explicit ImageArray(size_t capacity) : CompactArray<const AlignedImage*>(capacity) {
  }
  ImageArray(const AlignedImage *img1, va_list argptr);
};

ImageArray::ImageArray(const AlignedImage *img1, va_list argptr) {
  setCapacity(getArgCount(img1, argptr));
  add(img1);
  for(const AlignedImage *img = va_arg(argptr, const AlignedImage*); img; img = va_arg(argptr, const AlignedImage*)) {
    add(img);
  }
}

#define MARKED_COLOR D3DCOLOR_XRGB(198,198,198)

class ExpressionPainter {
private:
  DECLARECLASSNAME;
  static FontCache             s_fontCache;
  static const SymbolStringMap s_stringMap;
  const NumberFormat           m_numberFormat;
  const int                    m_decimals;
  const int                    m_maxWidth;
  bool                         m_getNumberActive;
  CompactArray<AlignedImage*>  m_imageTable;
  PixRectDevice               &m_device;
  ParserTree                  *m_tree;
  D3DCOLOR                     m_backgroundColor;
  ExpressionRectangle          m_rectangle;

 static CFont *getFont(bool textFont, int fontSize);
  inline int createSubFontSize(int fontSize) const {
    return (fontSize >= 10) ? (fontSize * 3 / 5) : fontSize;
  }

  void clearImageTable();
  AlignedImage *createTextImage(const CSize &size);
  AlignedImage *createImage(const CSize &size);

  AlignedTextImage    *getTextImage(     const String     &str, bool textFont, int fontSize, ExpressionRectangle &rect);
  AlignedImage        *getNumberImage(   SNode             n  ,                int fontSize, ExpressionRectangle &rect);
  void                 splitReal(        const Real       &v  , Real &significant, int &exponent) const;
  bool                 mustConvertNumber(const Number     &n) const;
  AlignedImage        *concatImages(     const ImageArray &imageList,                        ExpressionRectangle &rect);
  inline AlignedImage *stackImages(      const ImageArray &imageList,                        ExpressionRectangle &rect) {
    return stackImages(false, imageList, rect);
  }

  AlignedImage *stackImages(bool center, const ImageArray &imageList, ExpressionRectangle &rect);

  AlignedImage *concatImages(ExpressionRectangle &rect, const AlignedImage *img1, ...); // terminate arguments with NULL
  AlignedImage *stackImages( ExpressionRectangle &rect, bool center, const AlignedImage *img1, ...); // terminate arguments with NULL

  AlignedImage *cutImage(             AlignedImage *image, UINT percent, int flags);
  AlignedImage *getOpImage(           ExpressionInputSymbol symbol , int fontSize, ExpressionRectangle &rect);
  AlignedImage *getUnaryOpImage(      SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getBinaryOpImage(     SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(SNode n, SNode parent, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(SNode n, bool textFont, const String &leftPar, const String &rightPar, int fontSize, ExpressionRectangle &rect, double alignPar = 0.5);
  AlignedImage *getImage(             SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getImage1(            SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getSumImage(          SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getSumElementImage(   SNode e, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getProductImage(      SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getFactorImage(       SNode factor , int fontSize, ExpressionRectangle &rect);
  AlignedImage *getQuotImage(         SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getQuotImage(         const AlignedImage *p, const AlignedImage *q, ExpressionRectangle &rect);
  AlignedImage *getPowerImage(        SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getRootImage(         SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getFloorImage(        SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getCeilImage(         SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getAbsImage(          SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getBinomialImage(     SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getFunctionImage(     SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getIndexedImage(      SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getPolyImage(         SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getStdPolyImage(      SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getStatementImages(   SNode n, int fontSize, ExpressionRectangle &rect);
  AlignedImage *getIfImage(           SNode n, int fontSize, ExpressionRectangle &rect);
public:
  ExpressionPainter(PixRectDevice &device, const Expression &expr, NumberFormat numberFormat, int decimals, int maxWidth);
 ~ExpressionPainter();

  PixRect *paintExpression(int fontSize);
  inline const ExpressionRectangle &getRectangleTree() const {
    return m_rectangle;
  }
  void clear();
};

DEFINECLASSNAME(ExpressionPainter);
FontCache             ExpressionPainter::s_fontCache;
const SymbolStringMap ExpressionPainter::s_stringMap;

ExpressionPainter::ExpressionPainter(PixRectDevice &device, const Expression &expr, NumberFormat numberFormat, int decimals, int maxWidth)
: m_device(      device         )
, m_tree(        expr.getTree() )
, m_numberFormat(numberFormat   )
, m_decimals(    decimals       )
, m_maxWidth(    maxWidth       )
{
  m_backgroundColor = D3D_WHITE;
  m_getNumberActive = false;
}

ExpressionPainter::~ExpressionPainter() {
  clear();
}

void ExpressionPainter::clear() {
  clearImageTable();
}

PixRect *ExpressionPainter::paintExpression(int fontSize) {
  clearImageTable();
  if(m_tree->getRoot() == NULL) {
    return getTextImage(_T("null"), true, fontSize, m_rectangle);
  }
  PixRect *pr = getImage(m_tree->getRoot(), fontSize, m_rectangle);
//  m_tree->pruneUnusedNodes(); Dont call this here. cause reduction-debugger in TestExpressionGraphics to crash
  return pr;
}

AlignedImage *ExpressionPainter::createImage(const CSize &size) {
  AlignedImage *image = (m_backgroundColor != D3D_WHITE) ? new AlignedImage(m_device, 0, size, m_backgroundColor) : new AlignedImage(m_device, 0, size); TRACE_NEW(image);
  m_imageTable.add(image);
  return image;
}

AlignedImage *ExpressionPainter::createTextImage(const CSize &size) {
  AlignedImage *image = (m_backgroundColor != D3D_WHITE)
                      ? new AlignedTextImage(m_device, s_fontCache, size, m_backgroundColor)
                      : new AlignedTextImage(m_device, s_fontCache, size);
  TRACE_NEW(image);
  m_imageTable.add(image);
  return image;
}

AlignedImage *ExpressionPainter::cutImage(AlignedImage *image, UINT percent, int flags) {
  const int srcW = image->getWidth();
  const int srcH = image->getHeight();

  const int dstW = (flags & (CUT_LEFT | CUT_RIGHT )) ? (srcW * percent / 100) : srcW;
  const int dstH = (flags & (CUT_TOP  | CUT_BOTTOM)) ? (srcH * percent / 100) : srcH;

  int srcX = 0, srcY = 0;
  if(dstW != srcW) {
    switch(flags & (CUT_LEFT | CUT_RIGHT)) {
    case CUT_LEFT           : srcX = srcW - dstW; break;
    case CUT_RIGHT          : srcX = 0;
    case CUT_LEFT|CUT_RIGHT : srcX = (srcW-dstW)/2; break;
    }
  }
  if(dstH != srcH) {
    switch(flags & (CUT_TOP  | CUT_BOTTOM)) {
    case CUT_TOP            : srcY = srcH - dstH; break;
    case CUT_BOTTOM         : srcY = 0;
    case CUT_TOP|CUT_BOTTOM : srcY = (srcH-dstH)/2; break;
    }
  }

  AlignedImage *result = createImage(CSize(dstW, dstH));
  result->rop(0,0,srcW,dstH, SRCCOPY, image, srcX, srcY);
  return result->setAlignment(image->getAlignment() * dstH / srcH);
}

void ExpressionPainter::clearImageTable() {
  for(size_t i = 0; i < m_imageTable.size(); i++) {
    SAFEDELETE(m_imageTable[i]);
  }
  m_imageTable.clear();
}

CFont *ExpressionPainter::getFont(bool textFont, int fontSize) { // static
  return s_fontCache.getFont(textFont, fontSize);
}

AlignedImage *ExpressionPainter::getImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  const D3DCOLOR oldColor = m_backgroundColor;
  AlignedImage  *result;
  if(n.isMarked()) {
    m_backgroundColor = MARKED_COLOR;
  }
  result = getImage1(n, fontSize, rect);
  m_backgroundColor = oldColor;
  rect.m_node = n.node();
  rect.setSize(result);
  return result;
}

AlignedImage *ExpressionPainter::getImage1(SNode n, int fontSize, ExpressionRectangle &rect) {
  DEFINEMETHODNAME;
  AlignedImage *result = NULL;
  switch(n.getSymbol()) {
  case NAME              :
    if(n.isPi()) {
      return getTextImage(_T("\x70"), false, fontSize, rect);
    } else {
      return getTextImage(n.getName(), true, fontSize, rect);
    }

  case NUMBER            :
    return getNumberImage(n, fontSize, rect);

  case BOOLCONST         :
    return getTextImage(boolToStr(n.getBool()), true, fontSize, rect);

  case SUM               :
    return getSumImage(n, fontSize, rect);

  case PRODUCT           :
    return getProductImage(n, fontSize, rect);

  case QUOT              :
    return getQuotImage(   n, fontSize, rect);

  case POW               :
    return getPowerImage(  n, fontSize, rect);

  case SQR               :
    return getPowerImage(binExp(POW, n.left(), n._2()), fontSize, rect);

  case ROOT              :
    return getRootImage(   n, fontSize, rect);

  case SQRT              :
    return getRootImage(binExp(ROOT, n.left(), n._2()), fontSize, rect);

  case NOT               :
    return getUnaryOpImage(n, fontSize, rect);

  case PLUS              :
  case MINUS             :
  case PROD              :
  case MOD               :
  case AND               :
  case OR                :
  case EQ                :
  case NE                :
  case GE                :
  case GT                :
  case LE                :
  case LT                :
  case ASSIGN            :
    return getBinaryOpImage(n, fontSize, rect);

  case UNARYMINUS:
    return getUnaryOpImage(n, fontSize, rect);

  case FAC               :
    { AlignedImage *parImage, *facImage;
      ExpressionRectangle parRect, facRect;
      parImage = getParenthesizedImage(n.left(), n, fontSize, parRect);
      facImage = getOpImage(FAC, fontSize, facRect);
      rect.addChild(parRect).addChild(facRect);
      return concatImages(rect, parImage, facImage, NULL);
    }

  case FLOOR             :
    return getFloorImage(n.left(), fontSize, rect);

  case CEIL              :
    return getCeilImage(n.left(), fontSize, rect);

  case ABS               :
    return getAbsImage(n.left(), fontSize, rect);

  case BINOMIAL          :
    return getBinomialImage(n, fontSize, rect);

  case POLY              :
    return getPolyImage(n, fontSize, rect);

  case MAX               :
  case MIN               :
  case RAND              :
  case ACOS              :
  case ACOSH             :
  case ACOT              :
  case ACSC              :
  case ASEC              :
  case ASIN              :
  case ASINH             :
  case ATAN              :
  case ATAN2             :
  case ATANH             :
  case COS               :
  case COSH              :
  case COT               :
  case CSC               :
  case ERF               :
  case EXP               :
  case EXP10             :
  case EXP2              :
  case HYPOT             :
  case GAMMA             :
  case GAUSS             :
  case INVERF            :
  case LN                :
  case LOG10             :
  case LOG2              :
  case NORM              :
  case PROBIT            :
  case SEC               :
  case SIGN              :
  case SIN               :
  case SINH              :
  case TAN               :
  case TANH              :
    return getFunctionImage(n, fontSize, rect);

  case INDEXEDSUM        :
  case INDEXEDPRODUCT:
    return getIndexedImage(n, fontSize, rect);

  case IIF                :
    return getIfImage(n, fontSize, rect);

  case STMTLIST           :
    return getStatementImages(n, fontSize, rect);
  default                                 :
    n.throwUnknownSymbolException(method);
    break;
  }
  return result;
}

AlignedImage *ExpressionPainter::getSumImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle sumRect, commaRect, lpRect, rpRect;
  ImageArray result;
  result.add(getTextImage(_T("sum"), true, fontSize, sumRect));
  result.add(getOpImage(LPAR, fontSize, lpRect));
  rect.addChild(sumRect).addChild(lpRect);

  AlignedImage *commaImage = NULL;
  const SNodeArray &addentArray = n.getChildArray();
  for(size_t i = 0; i < addentArray.size(); i++) {
    const SNode &e = addentArray[i];
    if(commaImage == NULL) {
      commaImage = getOpImage(COMMA, fontSize, commaRect);
    } else {
      result.add(commaImage);
      rect.addChild(commaRect);
    }
    ExpressionRectangle eRect;
    result.add(getSumElementImage(e, fontSize, eRect));
    rect.addChild(eRect);
  }
  result.add(getOpImage(RPAR, fontSize, rpRect));
  rect.addChild(rpRect);
  return concatImages(result, rect);
}

AlignedImage *ExpressionPainter::getSumElementImage(SNode e, int fontSize, ExpressionRectangle &rect) {
  if(e.isPositive()) {
    return getImage(e.left(), fontSize, rect);
  } else {
    ExpressionRectangle minusRect, lpRect, rpRect, childRect;
    ImageArray result;
    AlignedImage *minusImage = getOpImage(MINUS, fontSize, minusRect);
    result.add(minusImage);
    result.add(getOpImage(LPAR  , fontSize, lpRect   ));
    result.add(getImage(e.left(), fontSize, childRect));
    result.add(getOpImage(RPAR  , fontSize, rpRect   ));
    rect.addChild(minusRect).addChild(lpRect).addChild(childRect).addChild(rpRect);
    return concatImages(result, rect);
  }
}

AlignedImage *ExpressionPainter::getProductImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle productRect, commaRect, lpRect, rpRect;
  ImageArray result;
  result.add(getTextImage(_T("prod"), true, fontSize, productRect));
  result.add(getOpImage(LPAR, fontSize, lpRect));
  rect.addChild(productRect).addChild(lpRect);

  AlignedImage *commaImage = NULL;
  const FactorArray &factorArray = n.getFactorArray();
  for(size_t i = 0; i < factorArray.size(); i++) {
    SNode factor = factorArray[i];
    if(commaImage == NULL) {
      commaImage = getOpImage(COMMA, fontSize, commaRect);
    } else {
      result.add(commaImage);
      rect.addChild(commaRect);
    }
    ExpressionRectangle fRect;
    result.add(getFactorImage(factor, fontSize, fRect));
    rect.addChild(fRect);
  }
  result.add(getOpImage(RPAR, fontSize, rpRect));
  rect.addChild(rpRect);
  return concatImages(result, rect);
}

AlignedImage *ExpressionPainter::getFactorImage(SNode factor, int fontSize, ExpressionRectangle &rect) {
  return getPowerImage(factor, fontSize, rect);
}

AlignedImage *ExpressionPainter::getQuotImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle leftRect, rightRect;
  AlignedImage *leftImage  = getImage(n.left() , fontSize, leftRect );
  AlignedImage *rightImage = getImage(n.right(), fontSize, rightRect);
  rect.addChild(leftRect).addChild(rightRect);
  return getQuotImage(leftImage, rightImage, rect);
}

AlignedImage *ExpressionPainter::getQuotImage(const AlignedImage *p, const AlignedImage *q, ExpressionRectangle &rect) {
  const int totalHeight = p->getHeight() + q->getHeight();
  const int lineWidth   = max((int)::sqrt(totalHeight / 20), 1);

  const CSize   size(max(p->getWidth(), q->getWidth()), lineWidth);
  AlignedImage *lineImage = createImage(size);
  PixRect::bitBlt(lineImage, 0,0,size.cx, size.cy, BLACKNESS, NULL, 0,0);
  ExpressionRectangle lineRect;
  lineRect.setSize(lineImage);
  rect.m_children.add(1, lineRect);
  AlignedImage *result = stackImages(rect, true, p, lineImage, q, NULL);
  result->setLeftFiller(2);
//  result->setAlignment(p->getHeight() + lineWidth/2);
  return result;
}

AlignedImage *ExpressionPainter::getPowerImage(SNode n, int fontSize, ExpressionRectangle &rect) {
//  m_backgroundColor = D3DCOLOR_XRGB(0,128,0);
  ExpressionRectangle baseRect, expoRect;
  AlignedImage               *baseImage = getParenthesizedImage(n.left(),n,fontSize, baseRect);
  const int                   baseAlign = baseImage->getHeight() - baseImage->getAlignment();
  SNode exponent  = n.right();

  const int          baseHeight   = baseImage->getHeight();
  const UINT         expoFontSize = min(createSubFontSize(baseHeight), fontSize);

//  m_backgroundColor       = D3DCOLOR_XRGB(255,0,0);
  AlignedImage *expoImage = getParenthesizedImage(exponent, n, expoFontSize, expoRect);
//  m_backgroundColor       = D3D_WHITE;

  if(baseImage->getImageType() == NORMAL_IMAGE) {
    baseImage->setAlignment(0);
  } else {
    const TEXTMETRIC &metric = ((AlignedTextImage*)baseImage)->getTextMetric();
    baseImage->setAlignment(metric.tmHeight - metric.tmAscent + metric.tmInternalLeading);
  }

  if(expoImage->getImageType() == NORMAL_IMAGE) {
    const int expoHeight = expoImage->getHeight();
    expoImage->setAlignment(expoHeight);
  } else {
    const TEXTMETRIC &metric = ((AlignedTextImage*)expoImage)->getTextMetric();
    expoImage->setAlignment(metric.tmHeight - metric.tmDescent);
  }
  rect.addChild(baseRect).addChild(expoRect);
  AlignedImage *result = concatImages(rect, baseImage, expoImage, NULL);
  return result->setAlignment(result->getHeight()-baseAlign);
}

AlignedImage *ExpressionPainter::getRootImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle radRect, rootSignRect;

  SNode         radicand         = n.left();
  SNode         root             = n.right();
  AlignedImage *radImage         = getImage(radicand, fontSize, radRect);
  const int     rootSignFontSize = max(radImage->getHeight(),fontSize);

//  m_backgroundColor = D3DCOLOR_XRGB(255,0,0);
  AlignedImage         *rootSignImage = getOpImage(ROOT, rootSignFontSize, rootSignRect);
//  m_backgroundColor = D3D_WHITE;

//  PixRect *rootCopy = rootSignImage->clone(true);
//  PixRect::showPixRect(rootCopy);

  AlignedImage       *leftImage = rootSignImage;
  ExpressionRectangle leftRect  = rootSignRect;

  if(!root.isTwo()) { // only paint root-expression if not 2
    const UINT         expoFontSize = createSubFontSize(max(createSubFontSize(rootSignFontSize), fontSize));
    ExpressionRectangle rootRect;
    AlignedImage       *rootImage    = getImage(root, expoFontSize, rootRect);
    rootImage->setAlignment(rootImage->getHeight());
    const int filler = rootSignImage->getWidth()*4/5;
    rootSignImage->setLeftFiller(-filler);

    int rl = rootSignImage->getHeight()*2/5;
    rootSignImage->setAlignment(rl);
//    rootSignImage->line(0,rl, rootSignImage->getWidth(),rl, RGB(255,0,0));

    rootImage->setLeftFiller(max(0, filler - rootImage->getWidth()));
    leftRect.clear();
    leftRect.addChild(rootRect).addChild(rootSignRect);
    leftImage = concatImages(leftRect, rootImage, rootSignImage, NULL);
  }
  leftImage->setAlignment(leftImage->getHeight() - rootSignImage->getHeight()/2);
  radImage->setAlignment(radImage->getHeight()/2);
  rect.addChild(leftRect).addChild(radRect);
  AlignedImage *result = concatImages(rect, leftImage, radImage, NULL);

  // draw the roof of the root-sign

  const int roofWidth = max(rootSignFontSize / 30,1);
  const int roofRight = result->getWidth() - 1;
        int roofTop   = result->getHeight() - (rootSignFontSize * 33) / 30 - (roofWidth+1)/2;
  int       roofLeft  = result->getWidth() - radImage->getWidth() - roofWidth*2/3 - max(roofWidth/2,1);
  for(;roofLeft > 0; roofLeft--) {
    const D3DCOLOR c = ARGB_TORGB(result->getPixel(roofLeft, roofTop));
    if(c != ARGB_TORGB(m_backgroundColor)) {
      break;
    }
  }
//  rootCopy->line(roofLeft, roofTop, 0, roofTop, RGB(255,0,0));
//  PixRect::showPixRect(rootCopy);

  for(;roofTop > 0;) { // search upwards for the top of the line
    const D3DCOLOR c = ARGB_TORGB(result->getPixel(roofLeft, roofTop));
    if(c != ARGB_TORGB(m_backgroundColor)) {
      roofTop--;
    } else {
      roofTop+=roofWidth/2+1; // roofTop is now where to put the roof of the rootsign
      break;
    }
  }

  CPen pen;
  pen.CreatePen(PS_SOLID, roofWidth, RGB(0,0,0));

  HDC hdc = result->getDC();
  CDC *dc = CDC::FromHandle(hdc);
  CPen *oldPen = dc->SelectObject(&pen);
  dc->MoveTo(roofLeft ,roofTop);
  dc->LineTo(roofRight,roofTop);
  dc->SelectObject(oldPen);
  result->releaseDC(hdc);
  return result;
}

AlignedImage *ExpressionPainter::getFunctionImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  SymbolString ssBuf;
  String tmpStr;
  const SymbolString *ssp = s_stringMap.get(n.getSymbol());
  if(ssp == NULL) {
    tmpStr = toLowerCase(n.getSymbolName());
    ssBuf = SymbolString(true, tmpStr.cstr());
  }
  const SymbolString &ss = ssp ? *ssp : ssBuf;

  ExpressionRectangle funcRect, lpRect,rpRect, commaRect;
  AlignedImage *functionImage = getTextImage(ss.m_text, ss.m_textFont, fontSize, funcRect);

  ImageArray imageList;
  imageList.add(functionImage);
  imageList.add(getOpImage(LPAR, fontSize, lpRect));
  rect.addChild(funcRect).addChild(lpRect);

  AlignedImage *commaImage = NULL;
  const SNodeArray &a = n.getChildArray();
  for(size_t i = 0; i < a.size(); i++) {
    const SNode &child = a[i];
    if(commaImage == NULL) {
      commaImage = getOpImage(COMMA, fontSize, commaRect);
    } else {
      imageList.add(commaImage);
      rect.addChild(commaRect);
    }

    ExpressionRectangle argRect;
    imageList.add(getImage(child, fontSize, argRect));
    rect.addChild(argRect);
  }
  imageList.add(getOpImage(RPAR, fontSize, rpRect));
  rect.addChild(rpRect);
  return concatImages(imageList, rect);
}

AlignedImage *ExpressionPainter::getBinomialImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle upperRect, lowerRect, lpRect, rpRect, innerRect;
  AlignedImage *upperImage = getImage(n.left() ,fontSize, upperRect);
  AlignedImage *lowerImage = getImage(n.right(),fontSize, lowerRect);
  innerRect.addChild(upperRect).addChild(lowerRect);
  AlignedImage *innerImage = stackImages(innerRect, true, upperImage, lowerImage, NULL);
  UINT          dfm        = upperImage->getHeight() + lowerImage->getHeight();

  AlignedImage *lpImage = getOpImage(LPAR, dfm, lpRect);
  AlignedImage *rpImage = getOpImage(RPAR, dfm, rpRect);

  rect.addChild(lpRect).addChild(innerRect).addChild(rpRect);
  return concatImages(rect, lpImage, innerImage, rpImage, NULL);
}

AlignedImage *ExpressionPainter::getPolyImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  assert(n.getSymbol() == POLY);
  if(n.getArgument().isNameOrNumber() && n.isCoefArrayConstant()) {
    return getStdPolyImage(n, fontSize, rect);
  }
  ExpressionRectangle polyRect, lbRect, rbRect, commaRect, lpRect, rpRect, argRect;
  ImageArray result;
  result.add(getTextImage(_T("poly"), true, fontSize, polyRect));
  result.add(getOpImage(LB, fontSize, lbRect));
  rect.addChild(polyRect).addChild(lbRect);

  AlignedImage *commaImage = NULL;
  const SNodeArray &coefArray = n.getCoefArray();
  for(size_t i = 0; i < coefArray.size(); i++) {
    const SNode &coef = coefArray[i];
    if(commaImage == NULL) {
      commaImage = getOpImage(COMMA, fontSize, commaRect);
    } else {
      result.add(commaImage);
      rect.addChild(commaRect);
    }
    ExpressionRectangle cRect;
    result.add(getImage(coef, fontSize, cRect));
    rect.addChild(cRect);
  }
  result.add(getOpImage(RB  , fontSize, rbRect));
  rect.addChild(rbRect);

  result.add(getOpImage(LPAR, fontSize, lpRect));
  result.add(getImage(n.getArgument(), fontSize, argRect));
  result.add(getOpImage(RPAR, fontSize, rpRect));
  rect.addChild(lpRect).addChild(argRect).addChild(rpRect);
  return concatImages(result, rect);
}

// assume argument is name of number, and coefficientarray is constant
AlignedImage *ExpressionPainter::getStdPolyImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle plusRect, minusRect;
  AlignedImage *plusImage  = getOpImage(PLUS , fontSize, plusRect );
  AlignedImage *minusImage = getOpImage(MINUS, fontSize, minusRect);
  ImageArray result;
  const SNodeArray &coefArray = n.getCoefArray();
  int pow = (int)coefArray.size() - 1;
  for(size_t i = 0; i < coefArray.size(); i++, pow--) {
    SNode coef = coefArray[i];
    Number c = coef.isNumber() ? coef.getNumber() : coef.evaluateReal();
    if(c == 0) continue;
    if(c > 0) {
      if(result.size() > 0) {
        result.add(plusImage);
        rect.addChild(plusRect);
      }
    } else { // c < 0
      result.add(minusImage);
      rect.addChild(minusRect);
      c = -c;
    }
    if(c != 1) {
      ExpressionRectangle coefRect;
      result.add(getNumberImage(coef, fontSize, coefRect));
      rect.addChild(coefRect);
    }
    switch(pow) {
    case 0 : // dont paint
      break;
    case 1 :
      { ExpressionRectangle argRect;
        result.add(getImage(n.getArgument(), fontSize, argRect));
        rect.addChild(argRect);
      }
      break;
    default:
      { ExpressionRectangle powerRect;
        result.add(getPowerImage(binExp(POW, n.getArgument(), SNode(n.getTree(),pow)), fontSize, powerRect));
        rect.addChild(powerRect);
      }
      break;
    }
  }
  return concatImages(result, rect);
}

AlignedImage *ExpressionPainter::getIndexedImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  UINT fm                = createSubFontSize(fontSize);

  ExpressionRectangle opRect, startRect, endRect, leftRect, rightRect;

//  m_backgroundColor = D3DCOLOR_XRGB(255,0,0);
  AlignedImage *operatorImage    = cutImage(getOpImage(n.getSymbol(), fontSize * 2, opRect), 85, CUT_TOP);
  opRect.setSize(operatorImage);

//  m_backgroundColor = D3DCOLOR_XRGB(0,255,0);
  AlignedImage *startAssignImage = getImage(n.child(0), fm, startRect);
  AlignedImage *endExprImage     = getImage(n.child(1), fm, endRect);

  leftRect.addChild(endRect).addChild(opRect).addChild(startRect);
  AlignedImage *leftImage        = stackImages(leftRect, true, endExprImage, operatorImage, startAssignImage, NULL);
  leftImage->setAlignment(endExprImage->getHeight() + operatorImage->getHeight()/2);
//  m_backgroundColor = D3D_WHITE;
  AlignedImage *rightImage       = getImage(n.child(2), fontSize, rightRect);
  rect.addChild(leftRect).addChild(rightRect);
  return concatImages(rect, leftImage, rightImage, NULL);
}

class ConditionExpression {
public:
  SNode m_cond;
  SNode m_expr;
  inline ConditionExpression() {
  }
  inline ConditionExpression(SNode cond, SNode expr) : m_cond(cond), m_expr(expr) {
  }
};

class CondExpressionArray : public CompactArray<ConditionExpression> {
private:
  void addCondExpr(SNode cond, SNode expr);
public:
  CondExpressionArray(SNode n);
};

CondExpressionArray::CondExpressionArray(SNode n) {
  addCondExpr( n.child(0), n.child(1));
  addCondExpr(!n.child(0), n.child(2));
  last().m_cond = SNode();
  const size_t count = size();
  for(size_t i = 0; i < count; i++) {
    ConditionExpression &ce = (*this)[i];
    ce.m_cond = SNode::beautify(ce.m_cond);
  }
}

void CondExpressionArray::addCondExpr(SNode cond, SNode expr) {
  if(expr.getSymbol() != IIF) {
    add(ConditionExpression( cond, expr));
  } else {
    SNode exprCond = expr.child(0);
    addCondExpr(cond &&  exprCond, expr.child(1));
    addCondExpr(cond && !exprCond, expr.child(2));
  }
}

AlignedImage *ExpressionPainter::getIfImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  CondExpressionArray a(n);
  const size_t        rowCount = a.size();
  Array<ImageArray>   imageMatrix(rowCount);
  ExpressionRectangle rectMatrix;
  int                 maxExprWidth = 0;
  for(size_t i = 0; i < rowCount; i++) {
    const ConditionExpression &ce = a[i];
    ImageArray    line;
    ExpressionRectangle exprRect, commaRect, condRect, lineRect;
    AlignedImage *commaImage = getOpImage(COMMA, fontSize, commaRect);
    line.add(getImage(ce.m_expr, fontSize, exprRect ));
    line.add(commaImage);
    if(ce.m_cond.isEmpty()) {
      line.add(getTextImage(_T("else"), true, fontSize, condRect));
    } else {
      line.add(getImage(ce.m_cond, fontSize, condRect));
    }
    lineRect.addChild(exprRect).addChild(commaRect).addChild(condRect);
    maxExprWidth = max(maxExprWidth, exprRect.Width());
    imageMatrix.add(line);
    rectMatrix.addChild(lineRect);
  }
  assert(imageMatrix.size() == rowCount);
  assert(rectMatrix.getChildCount() == rowCount);
  ImageArray lineArray(rowCount);
  for(size_t i = 0; i < rowCount; i++) {
    ImageArray          &line     = imageMatrix[i];
    ExpressionRectangle &lineRect = rectMatrix.child(i);
    const int missing = maxExprWidth - line[0]->getWidth();
    if(missing > 0) {
      ((AlignedImage*)line[1])->setLeftFiller(missing);
    }
    lineArray.add(concatImages(line, lineRect));
  }
  AlignedImage *condStack = stackImages(lineArray, rectMatrix);

  ExpressionRectangle tuborgRect;
  AlignedImage *tuborgImage = getTextImage(_T("{"), true, fontSize*((UINT)rowCount+1), tuborgRect);
  rect.addChild(tuborgRect).addChild(rectMatrix);
  return concatImages(rect, tuborgImage, condStack, NULL);
}

AlignedImage *ExpressionPainter::getStatementImages(SNode n, int fontSize, ExpressionRectangle &rect) {
  const SNodeArray &list       = n.getChildArray();
  const size_t      childCount = list.size();

  ExpressionRectangle semiRect;
  AlignedImage       *semiImage = NULL;
  ImageArray          imageArray;
  for(size_t i = 0; i < childCount; i++) {
    ExpressionRectangle mathRect;
    AlignedImage       *mathImage = getImage(list[i], fontSize, mathRect);
    if(i < childCount-1) {
      ExpressionRectangle stmtRect;
      stmtRect.addChild(mathRect).addChild(semiRect);
      if(semiImage == NULL) {
        semiImage = getOpImage(SEMI, fontSize, semiRect);
      }
      imageArray.add(concatImages(stmtRect, mathImage, semiImage, NULL));
      rect.addChild(stmtRect);
    } else {
      imageArray.add(mathImage); // no ';' after the last expression
      rect.addChild(mathRect);
    }
  }
  return stackImages(imageArray, rect);
}

AlignedImage *ExpressionPainter::getUnaryOpImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle opRect, parRect;
  AlignedImage *opImage  = getOpImage(n.getSymbol(), fontSize, opRect);
  AlignedImage *parImage = getParenthesizedImage(n.left(), n, fontSize, parRect);
  rect.addChild(opRect).addChild(parRect);
  return concatImages(rect, opImage, parImage, NULL);
}

AlignedImage *ExpressionPainter::getBinaryOpImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle opRect, lRect, rRect;

  AlignedImage *leftImage  = getParenthesizedImage(n.left(), n, fontSize, lRect);
  AlignedImage *opImage    = getOpImage(n.getSymbol(), fontSize, opRect);
  AlignedImage *rightImage = getParenthesizedImage(n.right(), n, fontSize, rRect);

  rect.addChild(lRect).addChild(opRect).addChild(rRect);
  return concatImages(rect, leftImage, opImage, rightImage, NULL);
}

AlignedImage *ExpressionPainter::getOpImage(ExpressionInputSymbol symbol, int fontSize, ExpressionRectangle &rect) {
  const SymbolString *ss = s_stringMap.get(symbol);
  if(ss == NULL) {
    throwInvalidArgumentException(__TFUNCTION__, _T("symbol=%d (=%s)"), symbol, ExpressionNode::getSymbolName(symbol).cstr());
  }
  return getTextImage(ss->m_text, ss->m_textFont, fontSize, rect);
}

AlignedImage *ExpressionPainter::getFloorImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("\xeb"), _T("\xfb"), fontSize, rect);
}

AlignedImage *ExpressionPainter::getCeilImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("\xe9"), _T("\xf9"), fontSize, rect);
}

AlignedImage *ExpressionPainter::getAbsImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("|"), _T("|"), fontSize, rect,0.7);
}

AlignedImage *ExpressionPainter::getParenthesizedImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("("), _T(")"), fontSize, rect, 0.70);
}

AlignedImage *ExpressionPainter::getParenthesizedImage(SNode n, SNode parent, int fontSize, ExpressionRectangle &rect) {
  if(n.needParentheses(parent)) {
    return getParenthesizedImage(n, fontSize, rect);
  } else {
    return getImage(n, fontSize, rect);
  }
}

AlignedImage *ExpressionPainter::getParenthesizedImage(SNode n, bool textFont, const String &leftPar, const String &rightPar, int fontSize, ExpressionRectangle &rect, double alignPar) {
  ExpressionRectangle imgRect, lpRect, rpRect;
  AlignedImage       *img = getImage(n, fontSize, imgRect);

  const int     parSize = max(img->getSize().cy, fontSize);
  const int     parAlignMent = (int)(parSize * alignPar);
//  m_backgroundColor = D3DCOLOR_XRGB(0,255,0);
//  AlignedImage *rpImage = getTextImage(rightPar, textFont, parsize);
//  m_backgroundColor = WHITE;
  AlignedImage *lpImage = getTextImage(leftPar , textFont, parSize, lpRect)->setAlignment(parAlignMent);
  AlignedImage *rpImage = getTextImage(rightPar, textFont, parSize, rpRect)->setAlignment(parAlignMent);

  rect.addChild(lpRect).addChild(imgRect).addChild(rpRect);
  AlignedImage *result = concatImages(rect, lpImage, img, rpImage, NULL);
  return result; //->setAlignment(result->getHeight()/2);
}

AlignedImage *ExpressionPainter::concatImages(ExpressionRectangle &rect, const AlignedImage *img1, ...) {
  va_list argptr;
  va_start(argptr, img1);
  ImageArray tmpArray(img1, argptr);
  va_end(argptr);
  return concatImages(tmpArray, rect);
}

AlignedImage *ExpressionPainter::stackImages(ExpressionRectangle &rect, bool center, const AlignedImage *img1, ...) {
  va_list argptr;
  va_start(argptr, img1);
  ImageArray tmpArray(img1, argptr);
  va_end(argptr);
  return stackImages(center, tmpArray, rect);
}

AlignedImage *ExpressionPainter::concatImages(const ImageArray &imageList, ExpressionRectangle &rect) {
  if(imageList.size() != rect.getChildCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("imageLIst.size()=%zu != rect.children.size()=%d")
                                 ,imageList.size(), rect.getChildCount());
  }

  int maxAlignment   = 0;
  int textImageIndex = -1;
  for(size_t i = 0; i < imageList.size(); i++) {
    const AlignedImage *img = imageList[i];
    if(img->getAlignment() > maxAlignment) {
      maxAlignment = img->getAlignment();
    }
    if((img->getImageType() == TEXT_IMAGE) && (textImageIndex == -1)) {
      textImageIndex = (int)i;
    }
  }
  CSize size(0,0);
  for(size_t i = 0; i < imageList.size(); i++) {
    const AlignedImage *img = imageList[i];
    CSize               sz  = img->getSize();
    size.cx += sz.cx + img->getLeftFiller();
    size.cy = max(size.cy, maxAlignment + (sz.cy - img->getAlignment()));
  }

#if defined(SHOW_LINES)
  for(i = 0; i < imageList.size(); i++) {
    AlignedImage *img = (AlignedImage*)imageList[i];
    D3DCOLOR color = (i&1)?D3DCOLOR_XRGB(128,128,128):D3DCOLOR_XRGB(247,25,0);
    img->horizontalLine(img->getAlignment(), D3DCOLOR_XRGB(128,128,128));
  }
#endif

  AlignedImage *result = (textImageIndex < 0) ? createImage(size) : createTextImage(size);
  rect.setSize(size);

  int x = size.cx;
  for(int i = (int)imageList.size()-1; i >= 0; i--) {
    const AlignedImage *img = imageList[i];
    ExpressionRectangle &childRect = rect.child(i);
    CSize               sz  = img->getSize();
    x -= sz.cx;
    const int           y   = maxAlignment - img->getAlignment();
    const int           leftFiller = img->getLeftFiller();
    result->rop(x,y,sz.cx,sz.cy,SRCCOPY, img, 0,0);

    childRect.setTopLeft(CPoint(x,y));

    x -= leftFiller;
  }
  return result->setAlignment(maxAlignment);
}

AlignedImage *ExpressionPainter::stackImages(bool center, const ImageArray &imageList, ExpressionRectangle &rect) {
  if(imageList.size() != rect.getChildCount()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("imageLIst.size()=%zu != rect.children.size()=%d")
                                 ,imageList.size(), rect.getChildCount());
  }
  CSize size(0,0);
  const size_t listSize = imageList.size();
  for(size_t i = 0; i < listSize; i++) {
    const CSize sz = imageList[i]->getSize();
    size.cx =  max(size.cx,sz.cx);
    size.cy += sz.cy;
  }
  AlignedImage *result = createImage(size);
  rect.setSize(size);

  int y = 0;
  for(size_t i = 0; i < listSize; i++) {
    ExpressionRectangle &childRect = rect.child(i);
    const AlignedImage *img = imageList[i];
    const CSize         sz  = img->getSize();
    const int           x   = center ? (size.cx - sz.cx)/2 : 0;
    result->rop(x,y,sz.cx,sz.cy,SRCCOPY, img, 0,0);
    childRect.setTopLeft(CPoint(x,y));
    y += sz.cy;
  }
  return result;
}

AlignedImage *ExpressionPainter::getNumberImage(SNode n, int fontSize, ExpressionRectangle &rect) {
  const Number &num = n.getNumber();
  if(m_getNumberActive || !mustConvertNumber(num)) {
    return getTextImage(toString(num), true, fontSize, rect);
  } else {
    m_getNumberActive = true; // to prevent infinte recursion
    Real significand;
    int  exponent;
    splitReal(getReal(num), significand, exponent);
    if(m_numberFormat == ENGINEERING_NOTATION) {
      const int r = ::abs(exponent % 3);
      if(r) {
        exponent    -= r;
        significand *= (r==1) ? 10 : 100;
      }
    }

    ExpressionNode *en = m_tree->fetchTreeNode(PROD
                                              ,m_tree->numberExpr(significand)
                                              ,m_tree->fetchTreeNode(POW
                                                                    ,m_tree->numberExpr(10)
                                                                    ,m_tree->numberExpr(exponent)
                                                                    ,NULL
                                                                    )
                                              ,NULL
                                              );

    AlignedImage *result = getImage(en, fontSize, rect);
    rect.m_children.clear();
    rect.m_node = n.node();
    m_getNumberActive = false;
    return result;
  }
}

void ExpressionPainter::splitReal(const Real &v, Real &significant, int &exponent) const {
  const String   s    = toString(v, m_decimals, 0, ios::scientific);
  const intptr_t ePos = s.find('e');
  if(ePos < 0) {
    exponent    = 0;
    significant = v;
  } else {
    _stscanf(s.cstr() + ePos + 1 , _T("%d" ), &exponent   );
    _stscanf(left(s, ePos).cstr(), _T("%lf"), &significant);
  }
}

bool ExpressionPainter::mustConvertNumber(const Number &n) const {
  switch(m_numberFormat) {
  case SCIENTIFIC_NOTATION :
  case ENGINEERING_NOTATION:
    if(n.isRational()) return false;
    { Real significant;
      int  exponent;
      splitReal(getReal(n), significant, exponent);
      return (exponent != 0);
    }
    break;
  default                  :
    return false;
  }
}

AlignedTextImage *ExpressionPainter::getTextImage(const String &str, bool textFont, int fontSize, ExpressionRectangle &rect) {
  AlignedTextImage *image = new AlignedTextImage(m_device, s_fontCache, str, textFont, fontSize, m_backgroundColor); TRACE_NEW(image);
  m_imageTable.add(image);
  rect.setSize(image);
  return image;
}

// ------------------------------------------------- ExpressionRectangle -----------------------------------------------------

void ExpressionRectangle::clear() {
  m_children.clear();
  m_node = NULL;
  (*(CRect*)this) = CRect(0,0,0,0);
}

void ExpressionRectangle::setSize(const CSize &sz) {
  right  = left + sz.cx;
  bottom = top  + sz.cy;
}

void ExpressionRectangle::setTopLeft(const CPoint &p) {
  const CSize sz = Size();
  left   = p.x;
  top    = p.y;
  setSize(sz);
}

bool ExpressionRectangle::traverseTree(ExpressionRectangleHandler &handler, const ExpressionRectangle *parent) const {
  if(!handler.handleRectangle(*this, parent)) {
    return false;
  }
  for(size_t i = 0; i < m_children.size(); i++) {
    const ExpressionRectangle &r = m_children[i];
    if(!r.traverseTree(handler,this)) return false;
  }
  return true;
}

const ExpressionRectangle *ExpressionRectangle::getFirstAncestorWithNode() const {
  for(const ExpressionRectangle *p = getParent(); p; p = p->getParent()) {
    if(p->m_node != NULL) {
      return p;
    }
  }
  return NULL;
}

bool ExpressionRectangle::isDescentantOf(const ExpressionRectangle &r) const {
  for(const ExpressionRectangle *p = getParent(); p; p = p->getParent()) {
    if(p == &r) {
      return true;
    }
  }
  return false;
}

String ExpressionRectangle::toString() const {
  const String rStr = format(_T("Rect:(%3d,%3d,%3d,%3d)"), left, top, right, bottom);
  if(m_node == NULL) {
    return format(_T("%s Node:%p"), rStr.cstr(), m_node);
  } else {
    try {
      return format(_T("%s Node:%p (%s)"), rStr.cstr(), m_node, m_node->toString().cstr());
    } catch(...) {
      return format(_T("%s Node:%p (Illegal pointer)"), rStr.cstr(), m_node);
    }
  }
}

void ExpressionRectangle::dump(int level) const { // recursive dump to debugLog
  debugLog(_T("%*.*s%s\n"), level,level,EMPTYSTRING, toString().cstr());
  for(size_t i = 0; i < m_children.size(); i++) {
    m_children[i].dump(level+2);
  }
}

// ------------------------------------------------- Helpers -----------------------------------------------------

class AdjustPosition : public ExpressionRectangleHandler {
public:
  bool handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent);
};

bool AdjustPosition::handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent) {
  if(parent) {
    CPoint p = r.TopLeft();
    const CPoint pp = parent->TopLeft();
    ((ExpressionRectangle&)r).setTopLeft(CPoint(p.x+pp.x,p.y+pp.y));
  }
  return true;
}

class ParentInitializer : public ExpressionRectangleHandler {
public:
  bool handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent) {
    ((ExpressionRectangle&)r).m_parent = parent;
    return true;
  }
};

class RectFinder : public ExpressionRectangleHandler {
private:
  const CPoint               m_point;
  const bool                 m_withNode;
  const ExpressionRectangle *m_bestRectangle;
public:
  RectFinder(const CPoint &point, bool withNode);
  bool handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent);
  const ExpressionRectangle *getFoundRectangle() const {
    return m_bestRectangle;
  }
};

RectFinder::RectFinder(const CPoint &point, bool withNode) : m_point(point), m_withNode(withNode) {
  m_bestRectangle = NULL;
}

bool RectFinder::handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent) {
  if(m_withNode && (r.getNode() == NULL)) {
    return true;
  }
  if(r.PtInRect(m_point)) {
    if((m_bestRectangle == NULL) || r.isDescentantOf(*m_bestRectangle)) {
      m_bestRectangle = &r;
    }
  }
  return true;
}

// ------------------------------------------------- ExpressionImage -----------------------------------------------------

ExpressionImage::ExpressionImage(const ExpressionImage &src) {
  if(!src.isEmpty()) {
    m_pr        = src.m_pr->clone(true);
    m_rectangle = src.m_rectangle;
    setParents();
  }
}

ExpressionImage::ExpressionImage(const PixRect *pr, const ExpressionRectangle &rectangle) {
  m_pr        = pr->clone(true);
  m_rectangle = rectangle;
  setParents();
}

ExpressionImage &ExpressionImage::operator=(const ExpressionImage &src) {
  if(&src == this) {
    return *this;
  }
  if(!isEmpty()) {
    clear();
  }
  if(!src.isEmpty()) {
    m_pr        = src.m_pr->clone(true);
    m_rectangle = src.m_rectangle;
    setParents();
  }
  return *this;
}

void ExpressionImage::setParents() {
  traverseRectangleTree(ParentInitializer());
}

ExpressionImage::~ExpressionImage() {
  clear();
}

void ExpressionImage::clear() {
  SAFEDELETE(m_pr);
  m_rectangle.clear();
}

void ExpressionImage::traverseRectangleTree(ExpressionRectangleHandler &handler) const {
  m_rectangle.traverseTree(handler, NULL);
}

const ExpressionRectangle *ExpressionImage::findLeastRectangle(const CPoint &point, bool withNode) const {
  RectFinder rectFinder(point, withNode);
  traverseRectangleTree(rectFinder);
  return rectFinder.getFoundRectangle();
}

ExpressionImage expressionToImage(PixRectDevice &device, const Expression &expr, int fontSize, NumberFormat numberFormat, int decimals, int maxWidth) {
  ExpressionPainter     painter(device, expr, numberFormat, decimals, maxWidth);
  PixRect              *pr = painter.paintExpression(fontSize);
  const ExpressionImage result(pr, painter.getRectangleTree());
  result.traverseRectangleTree(AdjustPosition());
//  result.getRectangleTree().dump();
  return result;
}

}; // amespace Expr
