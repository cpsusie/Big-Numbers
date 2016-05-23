#include "pch.h"
#include <WinTools.h>
#include <ColorSpace.h>
#include <CompactHashMap.h>
#include <Math/Expression/ExpressionGraphics.h>
#include <Math/Expression/SumElement.h>
#include <Math/Expression/ExpressionFactor.h>

//#define SHOW_LINES

typedef CompactKeyType<unsigned int> FontSizeKey;

#define CUT_LEFT   0x01
#define CUT_RIGHT  0x02
#define CUT_TOP    0x04
#define CUT_BOTTOM 0x08

class FontCache : public CompactHashMap<FontSizeKey, CFont*> {
public:
  ~FontCache();
  CFont *getFont(bool text, unsigned int fontSize);
};

FontCache::~FontCache() {
  for(Iterator<Entry<FontSizeKey, CFont*> > it = getEntryIterator(); it.hasNext();) {
    CFont *font = it.next().getValue();
    font->DeleteObject();
    delete font;
  }
  clear();
}

CFont *FontCache::getFont(bool text, unsigned int fontSize) {
  const unsigned int key = (text ? (1<<31) : 0) | fontSize;
  CFont **font = get(key);
  if(font == NULL) {
    CFont *newFont = new CFont;

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

class SymbolStringMap : private CompactSymbolHashMap<SymbolString> {
private:
  DECLARECLASSNAME;
  void putString(ExpressionInputSymbol symbol, bool textFont, const TCHAR *text) {
    put(symbol, SymbolString(textFont, text));
  }
public:
  SymbolStringMap();
  const SymbolString &getString(ExpressionInputSymbol symbol) const;
};

DEFINECLASSNAME(SymbolStringMap);

const SymbolString &SymbolStringMap::getString(ExpressionInputSymbol symbol) const {
  DEFINEMETHODNAME(getString);
  const SymbolString *ss = get(symbol);
  if(ss == NULL) {
    throwMethodInvalidArgumentException(s_className, method, _T("Unknown symbol:%s"), ExpressionTables->getSymbolName(symbol));
  }
  return *ss;
}

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
  putString(MINUS          , true , _T("\x96" ));
  putString(PROD           , true , _T("\x95" ));
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
  int m_horizontalAlignment; // centerLine from top. Default is height/2
  int m_leftFiller;          // empty space filled when concatenating images. Can be negative. Default=0
  int m_fontSize;
public:
  AlignedImage(int fontSize, const CSize &sz);
  AlignedImage(int fontSize, const CSize &sz, D3DCOLOR backgroundColor);

  AlignedImage *setAlignment(int align) {
    m_horizontalAlignment = align;
    return this;
  }

  inline void setLeftFiller(int leftFiller) {
    m_leftFiller = leftFiller;
  }
  inline int getLeftFiller() const {
    return m_leftFiller;
  }
  inline int getAlignment() const {
    return m_horizontalAlignment;
  }
  void horizontalLine(int y, D3DCOLOR color = BLACK) {
    line(0,y,getWidth()-1,y, color);
  }
  virtual ImageType getImageType() const {
    return NORMAL_IMAGE;
  }
  int getFontSize() const {
    return m_fontSize;
  }
};

AlignedImage::AlignedImage(int fontSize, const CSize &sz) : m_fontSize(fontSize), PixRect(sz) {
  rop(0,0,sz.cx,sz.cy, WHITENESS, NULL, 0,0);
  setAlignment(sz.cy/2);
  m_leftFiller = 0;
}

AlignedImage::AlignedImage(int fontSize, const CSize &sz, D3DCOLOR backgroundColor) : m_fontSize(fontSize), PixRect(sz) {
  fillRect(getRect(), backgroundColor);
  setAlignment(sz.cy/2);
  m_leftFiller = 0;
}

class AlignedTextImage : public AlignedImage {
private:
  TEXTMETRIC   m_textMetric;
public:
  AlignedTextImage(FontCache &fontCache, const String &text, bool textFont, unsigned int fontSize, D3DCOLOR backgroundColor);
  AlignedTextImage(FontCache &fontCache, const CSize &size);
  AlignedTextImage(FontCache &fontCache, const CSize &size, D3DCOLOR backgroundColor);

  const TEXTMETRIC &getTextMetric() const {
    return m_textMetric;
  }
  ImageType getImageType() const {
    return TEXT_IMAGE;
  }
};

AlignedTextImage::AlignedTextImage(FontCache &fontCache, const String &text, bool textFont, unsigned int fontSize, D3DCOLOR backgroundColor) 
: AlignedImage(fontSize, CSize(10,10)) {

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
  if(backgroundColor != WHITE) {
    SetBkColor(hdc, D3DCOLOR2COLORREF(backgroundColor));
  }
//  if(!textFont) SetTextColor(hdc, RGB(0,0,128));
  TextOut(hdc, 0,0, text.cstr(), text.length());

  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
  setAlignment(size.cy/2);

//  horizontalLine(m_textMetric.tmHeight - m_textMetric.tmAscent + m_textMetric.tmInternalLeading , RGB_MAKE(255,255,255));
#ifdef SHOW_LINES
  horizontalLine(m_textMetric.tmHeight - m_textMetric.tmDescent, RGB_MAKE(255,  20,20));
#endif

}

AlignedTextImage::AlignedTextImage(FontCache &fontCache, const CSize &size) : AlignedImage(size.cy, size) {
  CFont         *font   = fontCache.getFont(true, size.cy);
  HDC            hdc    = getDC();
  HGDIOBJ        oldGDI = SelectObject(hdc, *font);
  GetTextMetrics(hdc, &m_textMetric);
  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
}

AlignedTextImage::AlignedTextImage(FontCache &fontCache, const CSize &size, D3DCOLOR backgroundColor) : AlignedImage(size.cy, size, backgroundColor) {
  CFont         *font   = fontCache.getFont(true, size.cy);
  HDC            hdc    = getDC();
  HGDIOBJ        oldGDI = SelectObject(hdc, *font);
  GetTextMetrics(hdc, &m_textMetric);
  SelectObject(hdc, oldGDI);
  releaseDC(hdc);
}

class ImageArray : public CompactArray<const AlignedImage*> {
public:
  ImageArray() {
  }
  ImageArray(const AlignedImage *img1, va_list argptr);
};

ImageArray::ImageArray(const AlignedImage *img1, va_list argptr) {
  add(img1);
  for(const AlignedImage *img = va_arg(argptr, const AlignedImage*); img; img = va_arg(argptr, const AlignedImage*)) {
    add(img);
  }
}

#define MARKED_COLOR RGB_MAKE(198,198,198)

class ExpressionPainter {
private:
  DECLARECLASSNAME;
  static FontCache            s_fontCache;
  static SymbolStringMap      s_stringMap;
  int                         m_maxWidth;
  CompactArray<AlignedImage*> m_imageTable;
  const Expression           &m_expression;
  D3DCOLOR                    m_backgroundColor;
  ExpressionRectangle         m_rectangle;

  static CFont *getFont(bool textFont, int fontSize);
  unsigned int createSubFontSize(unsigned int fontSize) const {
    return (fontSize >= 10) ? (fontSize * 3 / 5) : fontSize;
  }

  void clearImageTable();
  AlignedImage *createTextImage(const CSize &size);
  AlignedImage *createImage(const CSize &size);

  AlignedTextImage *getTextImage(const String &str, bool textFont, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *concatImages(const ImageArray &imageList, ExpressionRectangle &rect);
  AlignedImage *stackImages( const ImageArray &imageList, ExpressionRectangle &rect) {
    return stackImages(false, imageList, rect);
  }

  AlignedImage *stackImages(bool center, const ImageArray &imageList, ExpressionRectangle &rect);

  AlignedImage *concatImages(ExpressionRectangle &rect, const AlignedImage *img1, ...); // terminate arguments with NULL
  AlignedImage *stackImages( ExpressionRectangle &rect, bool center, const AlignedImage *img1, ...); // terminate arguments with NULL

  AlignedImage *cutImage(             AlignedImage *image, unsigned int percent, int flags);
  AlignedImage *getOpImage(           ExpressionInputSymbol symbol , unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getUnaryOpImage(      const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getBinaryOpImage(     const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(const ExpressionNode *n, const ExpressionNode *parent, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getParenthesizedImage(const ExpressionNode *n, bool textFont, const String &leftPar, const String &rightPar, unsigned int fontSize, ExpressionRectangle &rect, double alignPar = 0.5);
  AlignedImage *getImage(             const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getImage1(            const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getSummationImage(    const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getProductImage(      const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getProductImage(      const FactorArray          &factors, const ExpressionNode *parent, unsigned int fontSize, bool changeExponentSign, ExpressionRectangle &rect);
  AlignedImage *getFactorImage(       const ExpressionFactor     *f      , const ExpressionNode *parent, unsigned int fontSize, bool changeExponentSign, ExpressionRectangle &rect);
  AlignedImage *getQuotImage(         const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getQuotImage(         const AlignedImage         *p, const AlignedImage *q, ExpressionRectangle &rect);
  AlignedImage *getPowerImage(        const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getRootImage(         const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getFloorImage(        const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getCeilImage(         const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getAbsImage(          const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getBinomialImage(     const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getFunctionImage(     const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getIndexedImage(      const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getPolyImage(         const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getStatementImages(   const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
  AlignedImage *getIfImage(           const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect);
public:
  ExpressionPainter(const Expression &expr);
 ~ExpressionPainter();

  PixRect *paintExpression(unsigned int fontSize, int maxWidth);
  const ExpressionRectangle &getRectangleTree() const {
    return m_rectangle;
  }

};

DEFINECLASSNAME(ExpressionPainter);
FontCache       ExpressionPainter::s_fontCache;
SymbolStringMap ExpressionPainter::s_stringMap;

ExpressionPainter::ExpressionPainter(const Expression &expr) : m_expression(expr) {
  m_backgroundColor = WHITE;
}

ExpressionPainter::~ExpressionPainter() {
  clearImageTable();
}

PixRect *ExpressionPainter::paintExpression(unsigned int fontSize, int maxWidth) {
  clearImageTable();
  m_maxWidth = maxWidth;
  if(m_expression.getRoot() == NULL) {
    return getTextImage(_T("null"), true, fontSize, m_rectangle);
  }
  return getImage(m_expression.getRoot(), fontSize, m_rectangle);
}

AlignedImage *ExpressionPainter::createImage(const CSize &size) {
  AlignedImage *image = (m_backgroundColor != WHITE) ? new AlignedImage(0, size, m_backgroundColor) : new AlignedImage(0, size);
  m_imageTable.add(image);
  return image;
}

AlignedImage *ExpressionPainter::createTextImage(const CSize &size) {
  AlignedImage *image = (m_backgroundColor != WHITE) 
                      ? new AlignedTextImage(s_fontCache, size, m_backgroundColor) 
                      : new AlignedTextImage(s_fontCache, size);
  m_imageTable.add(image);
  return image;
}

AlignedImage *ExpressionPainter::cutImage(AlignedImage *image, unsigned int percent, int flags) {
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
  for(int i = 0; i < m_imageTable.size(); i++) {
    delete m_imageTable[i];
  }
  m_imageTable.clear();
}

CFont *ExpressionPainter::getFont(bool textFont, int fontSize) { // static
  return s_fontCache.getFont(textFont, fontSize);
}

AlignedImage *ExpressionPainter::getImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  const D3DCOLOR oldColor = m_backgroundColor;
  AlignedImage  *result;
  if(n->isMarked()) {
    m_backgroundColor = MARKED_COLOR;
  }
  result = getImage1(n, fontSize, rect);
  m_backgroundColor = oldColor;
  rect.m_node = n;
  rect.setSize(result);
  return result;
}

AlignedImage *ExpressionPainter::getImage1(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  DEFINEMETHODNAME(getImage);
  AlignedImage *result = NULL;
  switch(n->getSymbol()) {
  case NAME              :
    if(n->isPi()) {
      return getTextImage(_T("\x70"), false, fontSize, rect);
    } else {
      return getTextImage(n->getName(), true, fontSize, rect);
    }

  case NUMBER            :
    return getTextImage(n->getNumber().toString(), true, fontSize, rect);

  case SUM               :
    return getSummationImage(n, fontSize, rect);

  case PRODUCT           :
    return getProductImage(  n, fontSize, rect);

  case MINUS             :
    if(n->isUnaryMinus()) {
      return getUnaryOpImage( n, fontSize, rect);
    } else {
      return getBinaryOpImage(n, fontSize, rect);
    }

  case QUOT              :
    return getQuotImage(   n, fontSize, rect);

  case POW               :
    return getPowerImage(  n, fontSize, rect);

  case SQR               :
    return getPowerImage(m_expression.binaryExpression(POW, n->left(), m_expression.getTwo()), fontSize, rect);

  case ROOT              :
    return getRootImage(   n, fontSize, rect);

  case SQRT              :
    return getRootImage(m_expression.binaryExpression(ROOT, n->left(), m_expression.getTwo()), fontSize, rect);

  case NOT               :
    return getUnaryOpImage(n, fontSize, rect);

  case PLUS              :
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

  case RETURNREAL        :
    { ExpressionRectangle rect1(rect.TopLeft());
      result = getImage(n->left(), fontSize, rect1);
      rect.addChild(rect1);
    }
    break;

  case FAC               :
    { AlignedImage *parImage, *facImage;
      ExpressionRectangle parRect, facRect;
      parImage = getParenthesizedImage(n->left(), n, fontSize, parRect);
      facImage = getOpImage(FAC, fontSize, facRect);
      rect.addChild(parRect).addChild(facRect);
      return concatImages(rect, parImage, facImage, NULL);
    }

  case FLOOR             :
    return getFloorImage(n->left(), fontSize, rect);

  case CEIL              :
    return getCeilImage(n->left(), fontSize, rect);

  case ABS               :
    return getAbsImage(n->left(), fontSize, rect);

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
  case ATANH             :
  case COS               :
  case COSH              :
  case COT               :
  case CSC               :
  case ERF               :
  case EXP               :
  case GAMMA             :
  case GAUSS             :
  case INVERF            :
  case LN                :
  case LOG10             :
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

  case SEMI              :
    { ExpressionRectangle stmtRect, lastRect;
      AlignedImage *stmtListImage = getStatementImages(n->left(), fontSize, stmtRect);
      AlignedImage *lastImage     = getImage(n->right(), fontSize, lastRect);
      rect.addChild(stmtRect).addChild(lastRect);
      result = stackImages(rect, false, stmtListImage, lastImage, NULL);
    }
    break;
  default                                 :
    Expression::throwUnknownSymbolException(s_className, method, n);
    break;
  }
  return result;
}

AlignedImage *ExpressionPainter::getSummationImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle plusRect, minusRect;
  AlignedImage *plusImage  = getOpImage(PLUS , fontSize, plusRect );
  AlignedImage *minusImage = getOpImage(MINUS, fontSize, minusRect);
  ImageArray    imageList;

  bool first = true;
  const AddentArray &elements = n->getAddentArray();
  for(int i = 0; i < elements.size(); i++) {
    const SumElement *e = elements[i];
    ExpressionRectangle childRect;
    AlignedImage *img = getImage(e->getNode(), fontSize, childRect);
    if(first) {
      if(!e->isPositive()) {
        imageList.add(minusImage);
        rect.addChild(minusRect);
      }
      first = false;
    } else {
      if(e->isPositive()) {
        imageList.add(plusImage);
        rect.addChild(plusRect);
      } else {
        imageList.add(minusImage);
        rect.addChild(minusRect);
      }
    }
    imageList.add(img);
    rect.addChild(childRect);
  }
  return concatImages(imageList, rect);
}

AlignedImage *ExpressionPainter::getProductImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  const FactorArray &a = n->getFactorArray();
  FactorArray p = a.selectConstantPositiveExponentFactors();
  p.addAll(a.selectNonConstantExponentFactors());
  FactorArray q = a.selectConstantNegativeExponentFactors();

  ExpressionRectangle pRect(rect.TopLeft()), qRect;

  AlignedImage *pImage = p.size() > 0 ? getProductImage(p, n, fontSize, false, pRect) : getTextImage(_T("1"), false, fontSize, pRect);
  AlignedImage *qImage = q.size() > 0 ? getProductImage(q, n, fontSize, true , qRect) : NULL;
  if(qImage == NULL) {
    rect = pRect;
    return pImage;
  } else {
    rect.addChild(pRect).addChild(qRect);
    AlignedImage *result = getQuotImage(pImage, qImage, rect);
    return result;
  }
}

AlignedImage *ExpressionPainter::getProductImage(const FactorArray &factors, const ExpressionNode *parent, unsigned int fontSize, bool changeExponentSign, ExpressionRectangle &rect) {
  ExpressionRectangle prodRect;
  AlignedImage *multiplyImage = getOpImage(PROD, fontSize, prodRect);
  ImageArray imageList;
  AlignedImage *operatorImage = NULL;
  for(int i = 0; i < factors.size(); i++) {
    const ExpressionFactor *factor = factors[i];
    if(operatorImage == NULL) {
      operatorImage = multiplyImage;
    } else {
      imageList.add(operatorImage);
      rect.addChild(prodRect);
    }
    ExpressionRectangle childRect;
    imageList.add(getFactorImage(factor, parent, fontSize, changeExponentSign, childRect));
    rect.addChild(childRect);
  }
  return concatImages(imageList, rect);
}

AlignedImage *ExpressionPainter::getFactorImage(const ExpressionFactor *f, const ExpressionNode *parent, unsigned int fontSize, bool changeExponentSign, ExpressionRectangle &rect) {
  AlignedImage *result;
  if(changeExponentSign) {
    const ExpressionFactor *rf = m_expression.fetchFactorNode(f->base(), m_expression.minusC(f->exponent()));
    result = getParenthesizedImage(rf, parent, fontSize, rect);
  } else {
    result = getParenthesizedImage(f, parent, fontSize, rect);
  }
  rect.m_node = f;
  return result;
}

AlignedImage *ExpressionPainter::getQuotImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle leftRect, rightRect;
  AlignedImage *leftImage  = getImage(n->left() , fontSize, leftRect );
  AlignedImage *rightImage = getImage(n->right(), fontSize, rightRect);
  rect.addChild(leftRect).addChild(rightRect);
  return getQuotImage(leftImage, rightImage, rect);
}

AlignedImage *ExpressionPainter::getQuotImage(const AlignedImage *p, const AlignedImage *q, ExpressionRectangle &rect) {
  const int totalHeight = p->getHeight() + q->getHeight();
  const int lineWidth   = max(sqrt(totalHeight / 20), 1);

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

AlignedImage *ExpressionPainter::getPowerImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
//  m_backgroundColor = RGB_MAKE(0,128,0);
  ExpressionRectangle baseRect, expoRect;
  AlignedImage               *baseImage = getParenthesizedImage(n->left(),n,fontSize, baseRect);
  const int                   baseAlign = baseImage->getHeight() - baseImage->getAlignment();
  const ExpressionNode *exponent  = n->right();

  const int          baseHeight   = baseImage->getHeight();
  const unsigned int expoFontSize = min(createSubFontSize(baseHeight), fontSize);

//  m_backgroundColor       = RGB_MAKE(255,0,0);
  AlignedImage *expoImage = getParenthesizedImage(exponent, n, expoFontSize, expoRect);
//  m_backgroundColor       = WHITE;

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

AlignedImage *ExpressionPainter::getRootImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle radRect, rootSignRect;

  const ExpressionNode *radicand         = n->left();
  const ExpressionNode *root             = n->right();
  AlignedImage               *radImage         = getImage(radicand, fontSize, radRect);
  const int                   rootSignFontSize = max(radImage->getHeight(),fontSize);
  
//  m_backgroundColor = RGB_MAKE(255,0,0);
  AlignedImage         *rootSignImage = getOpImage(ROOT, rootSignFontSize, rootSignRect);
//  m_backgroundColor = WHITE;

  AlignedImage       *leftImage = rootSignImage;
  ExpressionRectangle leftRect  = rootSignRect;

  if(!root->isTwo()) { // only paint root-expression if not 2
    const unsigned int expoFontSize = createSubFontSize(max(createSubFontSize(rootSignFontSize), fontSize));
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
    const COLORREF c = result->getPixel(roofLeft, roofTop) & 0x00ffffff;
    if(c != m_backgroundColor) {
      break;
    }
  }
//  result->line(roofLeft, roofTop, 0, roofTop, RGB(255,0,0));

  for(;roofTop > 0;) { // search upwards for the top of the line
    const COLORREF c = result->getPixel(roofLeft, roofTop) & 0x00ffffff;
    if(c != m_backgroundColor) {
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

AlignedImage *ExpressionPainter::getFunctionImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  SymbolString ss;
  String tmpStr;
  try {
    ss = s_stringMap.getString(n->getSymbol());
  } catch(Exception) {
    tmpStr = toLowerCase(n->getSymbolName());
    ss = SymbolString(true, tmpStr.cstr());
  }

  ExpressionRectangle funcRect, lpRect,rpRect, commaRect;
  AlignedImage *functionImage = getTextImage(ss.m_text, ss.m_textFont, fontSize, funcRect);

  ImageArray imageList;
  imageList.add(functionImage); 
  imageList.add(getOpImage(LPAR, fontSize, lpRect));
  rect.addChild(funcRect).addChild(lpRect);

  AlignedImage *commaImage = NULL;
  const ExpressionNodeArray &a = n->getChildArray();
  for(int i = 0; i < a.size(); i++) {
    const ExpressionNode *child = a[i];
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

AlignedImage *ExpressionPainter::getBinomialImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle upperRect, lowerRect, lpRect, rpRect, innerRect;
  AlignedImage *upperImage = getImage(n->left() ,fontSize, upperRect);
  AlignedImage *lowerImage = getImage(n->right(),fontSize, lowerRect);
  innerRect.addChild(upperRect).addChild(lowerRect);
  AlignedImage *innerImage = stackImages(innerRect, true, upperImage, lowerImage, NULL);
  unsigned int  dfm        = upperImage->getHeight() + lowerImage->getHeight();

  AlignedImage *lpImage = getOpImage(LPAR, dfm, lpRect);
  AlignedImage *rpImage = getOpImage(RPAR, dfm, rpRect);

  rect.addChild(lpRect).addChild(innerRect).addChild(rpRect);
  return concatImages(rect, lpImage, innerImage, rpImage, NULL);
}

AlignedImage *ExpressionPainter::getPolyImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle polyRect, lbRect, rbRect, commaRect, lpRect, rpRect, argRect;
  ImageArray result;
  result.add(getTextImage("poly", true, fontSize, polyRect));
  result.add(getOpImage(LB, fontSize, lbRect));
  rect.addChild(polyRect).addChild(lbRect);

  AlignedImage *commaImage = NULL;
  const ExpressionNodeArray &coefficientArray = n->getCoefficientArray();
  for(int i = 0; i < coefficientArray.size(); i++) {
    const ExpressionNode *c = coefficientArray[i];
    if(commaImage == NULL) {
      commaImage = getOpImage(COMMA, fontSize, commaRect);
    } else {
      result.add(commaImage);
      rect.addChild(commaRect);
    }
    ExpressionRectangle cRect;
    result.add(getImage(c, fontSize, cRect));
    rect.addChild(cRect);
  }
  result.add(getOpImage(RB  , fontSize, rbRect));
  rect.addChild(rbRect);

  result.add(getOpImage(LPAR, fontSize, lpRect));
  result.add(getImage(n->getArgument(), fontSize, argRect));
  result.add(getOpImage(RPAR, fontSize, rpRect));
  rect.addChild(lpRect).addChild(argRect).addChild(rpRect);
  return concatImages(result, rect);
}

AlignedImage *ExpressionPainter::getIndexedImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  unsigned int fm                = createSubFontSize(fontSize);

  ExpressionRectangle opRect, startRect, endRect, leftRect, rightRect;

//  m_backgroundColor = RGB_MAKE(255,0,0);
  AlignedImage *operatorImage    = cutImage(getOpImage(n->getSymbol(), fontSize * 2, opRect), 85, CUT_TOP);
  opRect.setSize(operatorImage);

//  m_backgroundColor = RGB_MAKE(0,255,0);
  AlignedImage *startAssignImage = getImage(n->child(0), fm, startRect);
  AlignedImage *endExprImage     = getImage(n->child(1), fm, endRect);

  leftRect.addChild(endRect).addChild(opRect).addChild(startRect);
  AlignedImage *leftImage        = stackImages(leftRect, true, endExprImage, operatorImage, startAssignImage, NULL);
  leftImage->setAlignment(endExprImage->getHeight() + operatorImage->getHeight()/2);
//  m_backgroundColor = WHITE;
  AlignedImage *rightImage       = getImage(n->child(2), fontSize, rightRect);
  rect.addChild(leftRect).addChild(rightRect);
  return concatImages(rect, leftImage, rightImage, NULL);
}

AlignedImage *ExpressionPainter::getIfImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle ifRect, commaRect, lpRect, rpRect, condRect, trueRect,falseRect;

  ImageArray    result;
  AlignedImage *commaImage = getOpImage(COMMA, fontSize, commaRect);
  result.add(getOpImage(IIF      , fontSize, ifRect   ));
  result.add(getOpImage(LPAR     , fontSize, lpRect   ));
  result.add(getImage(n->child(0), fontSize, condRect ));
  result.add(commaImage);
  result.add(getImage(n->child(1), fontSize, trueRect ));
  result.add(commaImage);
  result.add(getImage(n->child(2), fontSize, falseRect));
  result.add(getOpImage(RPAR     , fontSize, rpRect   ));
  rect.addChild(ifRect).addChild(lpRect).addChild(condRect).addChild(commaRect)
      .addChild(trueRect).addChild(commaRect).addChild(falseRect).addChild(rpRect);

  return concatImages(result, rect);
}

AlignedImage *ExpressionPainter::getStatementImages(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle semiRect;
  AlignedImage       *semiImage = getOpImage(SEMI, fontSize, semiRect);
  ExpressionNodeArray list      = getStatementList(n);

  ImageArray result;
  for(int i = 0; i < list.size(); i++) {
    ExpressionRectangle assignRect, stmtRect;
    AlignedImage *assignImage = getImage(list[i], fontSize, assignRect);
    stmtRect.addChild(assignRect).addChild(semiRect);
    result.add(concatImages(stmtRect, assignImage, semiImage, NULL));
    rect.addChild(stmtRect);
  }
  return stackImages(result, rect);
}

AlignedImage *ExpressionPainter::getUnaryOpImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle opRect, parRect;
  AlignedImage *opImage  = getOpImage(n->getSymbol(), fontSize, opRect);
  AlignedImage *parImage = getParenthesizedImage(n->left(), n, fontSize, parRect);
  rect.addChild(opRect).addChild(parRect);
  return concatImages(rect, opImage, parImage, NULL);
}

AlignedImage *ExpressionPainter::getBinaryOpImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  ExpressionRectangle opRect, lRect, rRect;

  AlignedImage *leftImage  = getParenthesizedImage(n->left(), n, fontSize, lRect);
  AlignedImage *opImage    = getOpImage(n->getSymbol(), fontSize, opRect);
  AlignedImage *rightImage = getParenthesizedImage(n->right(), n, fontSize, rRect);

  rect.addChild(lRect).addChild(opRect).addChild(rRect);
  return concatImages(rect, leftImage, opImage, rightImage, NULL);
}

AlignedImage *ExpressionPainter::getOpImage(ExpressionInputSymbol symbol, unsigned int fontSize, ExpressionRectangle &rect) {
  const SymbolString &ss = s_stringMap.getString(symbol);
  return getTextImage(ss.m_text, ss.m_textFont, fontSize, rect);
}

AlignedImage *ExpressionPainter::getFloorImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("\xeb"), _T("\xfb"), fontSize, rect);
}

AlignedImage *ExpressionPainter::getCeilImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("\xe9"), _T("\xf9"), fontSize, rect);
}

AlignedImage *ExpressionPainter::getAbsImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("|"), _T("|"), fontSize, rect);
}

AlignedImage *ExpressionPainter::getParenthesizedImage(const ExpressionNode *n, unsigned int fontSize, ExpressionRectangle &rect) {
  return getParenthesizedImage(n, false, _T("("), _T(")"), fontSize, rect, 0.70);
}

AlignedImage *ExpressionPainter::getParenthesizedImage(const ExpressionNode *n, const ExpressionNode *parent, unsigned int fontSize, ExpressionRectangle &rect) {
  if(n->needParentheses(parent)) {
    return getParenthesizedImage(n, fontSize, rect);
  } else {
    return getImage(n, fontSize, rect);
  }
}

AlignedImage *ExpressionPainter::getParenthesizedImage(const ExpressionNode *n, bool textFont, const String &leftPar, const String &rightPar, unsigned int fontSize, ExpressionRectangle &rect, double alignPar) {
  ExpressionRectangle imgRect, lpRect, rpRect;
  AlignedImage *img     = getImage(n, fontSize, imgRect);

  const int     parSize = max(img->getSize().cy, fontSize);
  const int     parAlignMent = parSize * alignPar;
//  m_backgroundColor = RGB_MAKE(0,255,0);
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
  DEFINEMETHODNAME(concatImages);
  if(imageList.size() != rect.getChildCount()) {
    throwMethodInvalidArgumentException(s_className, method, _T("imageLIst.size()=%d != rect.children.size()=%d")
                                       ,imageList.size(), rect.getChildCount());
  }

  int maxAlignment   = 0;
  int textImageIndex = -1;
  for(int i = 0; i < imageList.size(); i++) {
    const AlignedImage *img = imageList[i];
    if(img->getAlignment() > maxAlignment) {
      maxAlignment = img->getAlignment();
    }
    if((img->getImageType() == TEXT_IMAGE) && (textImageIndex == -1)) {
      textImageIndex = i;
    }
  }
  CSize size(0,0);
  for(i = 0; i < imageList.size(); i++) {
    const AlignedImage *img = imageList[i];
    CSize               sz  = img->getSize();
    size.cx += sz.cx + img->getLeftFiller();
    size.cy = max(size.cy, maxAlignment + (sz.cy - img->getAlignment()));
  }

#ifdef SHOW_LINES
  for(i = 0; i < imageList.size(); i++) {
    AlignedImage *img = (AlignedImage*)imageList[i];
    D3DCOLOR color = (i&1)?RGB_MAKE(128,128,128):RGB_MAKE(247,25,0);
    img->horizontalLine(img->getAlignment(), RGB_MAKE(128,128,128));
  }
#endif

  AlignedImage *result = (textImageIndex < 0) ? createImage(size) : createTextImage(size);
  rect.setSize(size);

  int x = size.cx;
  for(i = imageList.size()-1; i >= 0; i--) {
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
  DEFINEMETHODNAME(stackImages);
  if(imageList.size() != rect.getChildCount()) {
    throwMethodInvalidArgumentException(s_className, method, _T("imageLIst.size()=%d != rect.children.size()=%d")
                                       ,imageList.size(), rect.getChildCount());
  }
  CSize size(0,0);

  for(int i = 0; i < imageList.size(); i++) {
    const CSize sz = imageList[i]->getSize();
    size.cx =  max(size.cx,sz.cx);
    size.cy += sz.cy;
  }
  AlignedImage *result = createImage(size);
  rect.setSize(size);

  int y = 0;
  for(i = 0; i < imageList.size(); i++) {
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

AlignedTextImage *ExpressionPainter::getTextImage(const String &str, bool textFont, unsigned int fontSize, ExpressionRectangle &rect) {
  AlignedTextImage *image = new AlignedTextImage(s_fontCache, str, textFont, fontSize, m_backgroundColor);
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
  for(int i = 0; i < m_children.size(); i++) {
    const ExpressionRectangle &r = m_children[i];
    if(!r.traverseTree(handler,this)) return false;
  }
  return true;
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
  String rStr = format(_T("Rect:(%d,%d,%d,%d)"), left, top, right, bottom);
  if(m_node == NULL) {
    return rStr;
  } else {
    return format(_T("%s Node:%s"), rStr.cstr(), m_node->toString().cstr());
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
  if(m_pr) {
    delete m_pr;
    m_pr = NULL;
  }
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


ExpressionImage expressionToImage(const Expression &expr, unsigned int fontSize, int maxWidth) {
  ExpressionPainter painter(expr);
  PixRect *pr = painter.paintExpression(fontSize, maxWidth);
  const ExpressionImage result(pr, painter.getRectangleTree());
  result.traverseRectangleTree(AdjustPosition());
  return result;
}
