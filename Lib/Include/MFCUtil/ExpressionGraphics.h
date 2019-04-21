#pragma once

#include "WinTools.h"
#include "PixRect.h"
#include <Math/Expression/Expression.h>

namespace Expr {

class ExpressionRectangle;


class ExpressionRectangleHandler {
public:
  virtual bool handleRectangle(const ExpressionRectangle &r, const ExpressionRectangle *parent) = 0;
};

class ExpressionRectangle : public CRect {
private:
  ExpressionNode             *m_node;
  Array<ExpressionRectangle>  m_children;
  const ExpressionRectangle  *m_parent;

  friend class ExpressionPainter;
  friend class ExpressionImage;
  friend class AdjustPosition;
  friend class ParentInitializer;

  inline void setSize(   const PixRect *pr) {
    setSize(pr->getSize());
  }
  void setTopLeft(const CPoint  &p );
  void setSize(   const CSize   &sz);
  inline ExpressionRectangle &addChild(const ExpressionRectangle &r) {
    m_children.add(r);
    return *this;
  }
  bool traverseTree(ExpressionRectangleHandler &handler, const ExpressionRectangle *parent) const;
public:
  ExpressionRectangle() : CRect(0,0,0,0), m_node(NULL), m_parent(NULL) {
  }
  ExpressionRectangle(const CPoint &topLeft) : CRect(topLeft.x,topLeft.y,0,0), m_node(NULL), m_parent(NULL) {
  }
  inline CPoint BottomLeft() const {
    return CPoint(left,bottom);
  }
  inline CPoint TopRight() const {
    return CPoint(right, top);
  }
  inline bool hasNode() const {
    return getNode() != NULL;
  }
  inline ExpressionNode *getNode() const {
    return m_node;
  }
  void clear();
  inline int getChildCount() const {
    return (int)m_children.size();
  }
  inline ExpressionRectangle &child(size_t index) {
    return m_children[index];
  }
  inline bool hasParent() const {
    return m_parent != NULL;
  }
  inline bool hasAncestorWithNode() const {
    return getFirstAncestorWithNode() != NULL;
  }
  const ExpressionRectangle *getFirstAncestorWithNode() const;

  inline const ExpressionRectangle *getParent() const {
    return m_parent;
  }
  bool isDescentantOf(const ExpressionRectangle &r) const;
  inline const Array<ExpressionRectangle> &getChildArray() const {
    return m_children;
  }
  String toString() const;
  void dump(int level=0) const; // recursive dump to debugLog
};

class ExpressionImage {
private:
  PixRect            *m_pr;
  ExpressionRectangle m_rectangle;
  void setParents();
public:
  ExpressionImage() : m_pr(NULL) {
  }
  ExpressionImage(const ExpressionImage &src);
  ExpressionImage(const PixRect *pr, const ExpressionRectangle &rectangle);

  ExpressionImage &operator=(const ExpressionImage &src);

  ~ExpressionImage();

  inline const PixRect *getImage() const {
    return m_pr;
  }
  void clear();
  CSize size() const {
    return isEmpty() ? CSize(0,0) : m_pr->getSize();
  }
  inline bool isEmpty() const {
    return m_pr == NULL;
  }
  inline const ExpressionRectangle &getRectangleTree() const {
    return m_rectangle;
  }
  void traverseRectangleTree(ExpressionRectangleHandler &handler) const;
  const ExpressionRectangle *findLeastRectangle(const CPoint &point, bool withNode = true) const;
};

typedef enum {
  SCIENTIFIC_NOTATION
 ,ENGINEERING_NOTATION
 ,E_NOTATION
} NumberFormat;

String       numberFormatToString(NumberFormat  nf );
NumberFormat getNumberFormat(     const String &str);

ExpressionImage expressionToImage(PixRectDevice &device, const Expression &expr, int fontSize, NumberFormat numberFormat = SCIENTIFIC_NOTATION, int decimals = 6, int maxWidth = -1);

}; // namespace Expr
