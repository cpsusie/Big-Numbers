#include "stdafx.h"
#include <Math/MathLib.h>
#include "AvlTree.h"

void AvlGraphics::message( COLORREF color, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  vmessage(color,format,argptr);
  va_end(argptr);
}

void AvlTree::vmessage(COLORREF color, const TCHAR *format, va_list argptr) {
  m_avlGraphics->vmessage(color,format,argptr);
}

void AvlTree::message( COLORREF color, const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr,format);
  m_avlGraphics->vmessage(color,format,argptr);
  va_end(argptr);
}

void AvlTree::traceTree(AvlNode *p, int markColor, const TCHAR *msg) {
  if(p) {
    p->m_color = markColor;
  }
  paint();
  message(markColor,_T("%s"),msg);
}

#define TRACETREE(node,color,msg) { if(isTracing()) traceTree(node,color,msg); }

static int intCompare(int key1, int key2) {
  return key1 - key2;
}

bool AvlTree::nodeInsert(AvlNode **pp, int key) {
  AvlNode *p1, *p2, *p = *pp;

  if(!p) {
    *pp = p = new AvlNode;

    if(!p) {
      _ftprintf(stderr, _T("AvlnodeInsert:out of memory\n"));
      exit(-1);
    }

    p->m_left    = p->m_right = NULL;
    p->m_balance = 0;
    p->m_color   = BLACK;
    p->m_key     = key;

    m_updateDone = true;

    TRACETREE(p,GREEN,format(_T("Key %d inserted"), key).cstr());

    return true;
  }

  const int c = intCompare(p->m_key, key);
  if(c > 0) {                    // node.key > key
    if(!nodeInsert(&p->m_left, key)) {
      return false;
    }

                                 // left brance has grown
    switch(p->m_balance) {
    case 1 :
      TRACETREE(p,TURKIS,format(_T("NodeInsert:(%d): Left brance has grown. Changing balance 1 -> 0"), p->m_key).cstr());
      p->m_balance =  0;
      return false;

    case 0 :
      TRACETREE(p,TURKIS,format(_T("NodeInsert(%d): Left brance has grown. Changing balance 0 -> -1"), p->m_key).cstr());
      p->m_balance = -1;
      return true;

    case -1:                     // rebalance
      p1 = p->m_left;
      if(p1->m_balance == -1) {  // single right rotation
        TRACETREE(p,CYAN,format(_T("NodeInsert: %d out of balance. Left brance has grown and balance = -1. Making single right rotation"), p->m_key).cstr());
        if(isTracing()) findNodePositions(false);
        p->m_left     = p1->m_right;
        p1->m_right   = p;
        p->m_balance  = 0;
        *pp           = p = p1;
      } else {                   // double left-right rotation
        TRACETREE(p,CYAN,format(_T("NodeInsert: %d out of balance: Left brance has grown and balance = -1. Making double left-right rotation"), p->m_key).cstr());
        if(isTracing()) findNodePositions(false);
        p2            = p1->m_right;
        p1->m_right   = p2->m_left;
        p2->m_left    = p1;
        p->m_left     = p2->m_right;
        p2->m_right   = p;
        p->m_balance  = (p2->m_balance == -1) ?  1 : 0;
        p1->m_balance = (p2->m_balance ==  1) ? -1 : 0;
        *pp           = p = p2;
      }
      p->m_balance = 0;
      animateTransition();
      break;
    }
    return false;
  }


  if(c < 0) {                    // node.key < key
    if(!nodeInsert(&p->m_right, key)) {
      return false;
    }

                                 // right brance has grown
    switch(p->m_balance) {
    case -1:
      TRACETREE(p,TURKIS,format(_T("NodeInsert(%d): Right brance has grown. Changing balance -1 -> 0"), p->m_key).cstr());
      p->m_balance = 0;
      return false;

    case  0:
      TRACETREE(p,TURKIS,format(_T("NodeInsert(%d): Right brance has grown. Changing balance 0 -> 1"), p->m_key).cstr());
      p->m_balance = 1;
      return true;

    case  1:                     // rebalance
      p1 = p->m_right;
      if(p1->m_balance == 1) {   // single left rotation
        TRACETREE(p,CYAN,format(_T("NodeInsert: %d out of balance. Right brance has grown and balance = 1. Making single left rotation"), p->m_key).cstr());
        if(isTracing()) findNodePositions(false);
        p->m_right    = p1->m_left;
        p1->m_left    = p;
        p->m_balance  = 0;
        *pp           = p = p1;
      } else {                   // double right-left rotation
        TRACETREE(p,CYAN,format(_T("NodeInsert: %d out of balance. Right brance has grown and balance = 1. Making double right-left rotation"), p->m_key).cstr());
        if(isTracing()) findNodePositions(false);
        p2            = p1->m_left;
        p1->m_left    = p2->m_right;
        p2->m_right   = p1;
        p->m_right    = p2->m_left;
        p2->m_left    = p;
        p->m_balance  = (p2->m_balance ==  1) ? -1 : 0;
        p1->m_balance = (p2->m_balance == -1) ?  1 : 0;
        *pp           = p = p2;
      }
      p->m_balance = 0;
      animateTransition();
      break;
    }
    return false;
  }
  return false;                  // duplicate key
}

bool AvlTree::balanceLeft(AvlNode **pp) {
  AvlNode *p1, *p2, *p = *pp;
  char b;

  switch(p->m_balance) {
  case -1:
    TRACETREE(p,TURKIS,format(_T("BalanceLeft(%d): Changing balance -1 -> 0"), p->m_key).cstr());
    p->m_balance =  0;
    return true;

  case  0:
    TRACETREE(p,TURKIS,format(_T("BalanceLeft(%d): Changing balance 0 -> 1"), p->m_key).cstr());
    p->m_balance =  1;
    return false;

  case  1:                     // Rebalance
    p1 = p->m_right;
    b  = p1->m_balance;
    if(b >= 0) {               // single left rotation
      TRACETREE(p,CYAN,format(_T("BalanceLeft: %d out of balance. Making single left rotation"), p->m_key).cstr());
      if(isTracing()) {
        findNodePositions(false);
      }
      p->m_right  = p1->m_left;
      p1->m_left  = p;
      if(b == 0) {
        p->m_balance  =  1;
        p1->m_balance = -1;
        *pp           = p1;
        animateTransition();
        return false;
      } else {
        p->m_balance  = 0;
        p1->m_balance = 0;
        *pp           = p1;
        animateTransition();
        return true;
      }
    } else {                   // double right-left rotation
      TRACETREE(p,CYAN,format(_T("BalanceLeft: %d out of balance. Making double right-left rotation"), p->m_key).cstr());
      if(isTracing()) {
        findNodePositions(false);
      }
      p2            = p1->m_left;
      b             = p2->m_balance;
      p1->m_left    = p2->m_right;
      p2->m_right   = p1;
      p->m_right    = p2->m_left;
      p2->m_left    = p;
      p->m_balance  = (b ==  1) ? -1 : 0;
      p1->m_balance = (b == -1) ?  1 : 0;
      p2->m_balance = 0;
      *pp           = p2;
      animateTransition();
      return true;
    }
  }
  _tprintf( _T("balanceLeft dropped to the end !!!\n") );
  exit(-1);
  return true;
}

bool AvlTree::balanceRight(AvlNode **pp) {
  AvlNode *p1, *p2, *p = *pp;
  char b;

  switch(p->m_balance) {
  case  1:
    TRACETREE(p,TURKIS,format(_T("BalanceRight(%d): Changing balance 1 -> 0"), p->m_key).cstr());
    p->m_balance =  0;
    return true;

  case  0:
    TRACETREE(p,TURKIS,format(_T("BalanceRight(%d): Changing balance 0 -> -1"), p->m_key).cstr());
    p->m_balance = -1;
    return false;

  case -1:                     // Rebalance
    p1 = p->m_left;
    b  = p1->m_balance;
    if(b <= 0) {               // single right rotation
      TRACETREE(p,CYAN,format(_T("BalanceRight: %d out of balance: Making single right rotation"), p->m_key).cstr());
      if(isTracing()) {
        findNodePositions(false);
      }
      p->m_left   = p1->m_right;
      p1->m_right = p;
      if(b == 0) {
        p->m_balance  = -1;
        p1->m_balance =  1;
        *pp           = p1;
        animateTransition();
        return false;
      } else {
        p->m_balance  = 0;
        p1->m_balance = 0;
        *pp           = p1;
        animateTransition();
        return true;
      }
    } else {                   // double left-right rotation
      TRACETREE(p,CYAN,format(_T("BalanceRight: %d out of balance. Making double left-right rotation"), p->m_key).cstr());
      if(isTracing()) {
        findNodePositions(false);
      }
      p2            = p1->m_right;
      b             = p2->m_balance;
      p1->m_right   = p2->m_left;
      p2->m_left    = p1;
      p->m_left     = p2->m_right;
      p2->m_right   = p;
      p->m_balance  = (b == -1) ?  1 : 0;
      p1->m_balance = (b ==  1) ? -1 : 0;
      p2->m_balance = 0;
      *pp           = p2;
      animateTransition();
      return true;
    }
  }
  _tprintf( _T("balanceRight dropped to the end !!!\n") );
  exit(-1);
  return true;
}


/* Replace m_deleteHelper with the rightmost node in this subtree */

bool AvlTree::nodeDel(AvlNode **r) {
  if((*r)->m_right) {
    if(nodeDel( &((*r)->m_right))) {
      return balanceRight(r);
    } else {
      return false;
    }
  }

  TRACETREE(*r,TURKIS,format(_T("Replacing key (%d -> %d)"), (*r)->m_key, m_deleteHelper->m_key).cstr());

  m_deleteHelper->m_key = (*r)->m_key;
  m_deleteHelper = *r;
  *r             = m_deleteHelper->m_left;

  TRACETREE(*r,TURKIS,_T("Replace done"));

  return true;
}

bool AvlTree::nodeDelete(AvlNode **pp, int key) {
  AvlNode *p = *pp;

  if(p == NULL) {
    return false; // key not found
  }

  const int c = intCompare(p->m_key, key);
  if(c > 0) {
    if(nodeDelete(&p->m_left, key)) {
      return balanceLeft(pp);
    } else {
      return false;
    }
  } else if(c < 0) {
    if(nodeDelete(&p->m_right, key)) {
      return balanceRight(pp);
    } else {
      return false;
    }
  }

  bool ret = false;
  if(p->m_right == NULL) {
    *pp = p->m_left;
    ret = true;
  } else if( p->m_left  == NULL ) {
    *pp = p->m_right;
    ret = true;
  } else {
    m_deleteHelper = p;
    if(nodeDel(&p->m_left)) {
      ret = balanceLeft(pp);
    }
    p = m_deleteHelper;
  }

  delete p;

  m_updateDone = true;

  return ret;
}

AvlTree::AvlTree() {
  m_root           = NULL;
  m_size           = 0;
  m_tracing        = false;
  m_balanceVisible = false;
}

void AvlNode::visitNode(AvlNodeHandler &handler) {
  if(m_left) {
    handler.m_level++;
    m_left->visitNode(handler);
    handler.m_level--;
  }

  handler.handleNode(this);

  if(m_right) {
    handler.m_level++;
    m_right->visitNode(handler);
    handler.m_level--;
  }
}

bool AvlNode::contains(const CPoint &p) const {
  CRect  rect   = getEllipseRect(this->m_newPos);
  CPoint p0 = rect.CenterPoint();
  UINT a = sqr(rect.Width()  / 2);
  UINT b = sqr(rect.Height() / 2);
  return b * sqr(p.x-p0.x) + a * sqr(p.y-p0.y) <= a*b;
}

CRect AvlNode::getEllipseRect(const CPoint &pos) const {
  return CRect(pos.x-6, pos.y-6, pos.x+m_textSize.cx+6, pos.y+m_textSize.cy+6);
}

void AvlTree::traverse(AvlNodeHandler &handler) {
  handler.m_level = 0;
  if(m_root) {
    m_root->visitNode(handler);
  }
}

static void deleteNodeRecurse(AvlNode *p) {
  if(p->m_left) {
    deleteNodeRecurse(p->m_left);
  }
  if(p->m_right) {
    deleteNodeRecurse(p->m_right);
  }
  delete p;
}

AvlTree::~AvlTree() {
  clear();
}

void AvlTree::clear() {
  if(m_root) {
    deleteNodeRecurse(m_root);
  }
  m_root  = NULL;
  m_size  = 0;
}

bool AvlTree::insert(int key) {
  m_updateDone  = false;

  nodeInsert(&m_root, key);

  if(m_updateDone) {
    m_size++;
  }

  setColor(BLACK);
  return m_updateDone;
}

bool AvlTree::remove(int key) {
  m_updateDone  = false;

  nodeDelete(&m_root, key);

  if(m_updateDone) {
    m_size--;
  }

  setColor(BLACK);
  return m_updateDone;
}

int *AvlTree::getFirst() {
  AvlNode *q = NULL;
  for(AvlNode *p = m_root; p; p = p->m_left) {
    q = p;
  }
  return q ? &q->m_key : NULL;
}

int *AvlTree::getLast() {
  AvlNode *q = NULL;
  for(AvlNode *p = m_root; p; p = p->m_right) {
    q = p;
  }
  return q ? &q->m_key : NULL ;
}

bool AvlTree::contains(int key) {
  AvlNode *p = m_root;
  while(p) {
    const int c = intCompare(p->m_key, key);
    if(c > 0) {
      p = p->m_left;
    } else if(c < 0) {
      p = p->m_right;
    } else {
      return true;
    }
  }
  return false;
}

class AvlNodeFinder : public AvlNodeHandler {
private:
  const CPoint m_point;
  AvlNode *m_result;
public:
  AvlNodeFinder(const CPoint &point) : m_point(point) {
    m_result = NULL;
  }
  AvlNode *getResult() {
    return m_result;
  }
  void handleNode(AvlNode *n);
};

void AvlNodeFinder::handleNode(AvlNode *n) {
  if(n->contains(m_point)) {
    m_result = n;
  }
}

AvlNode *AvlTree::findNodeByPosition(const CPoint &p) {
  CPoint np = p;
  getAvlGraphics()->getWindow()->ScreenToClient(&np);
  AvlNodeFinder anf(np);
  traverse(anf);
  return anf.getResult();
}

class TreeHeightFinder : public AvlNodeHandler {
private:
  int m_maxYPos;
public:
  void handleNode(AvlNode *p);
  TreeHeightFinder(AvlTree &tree);
  int getMaxYPos() const {
    return m_maxYPos;
  }
};

void TreeHeightFinder::handleNode(AvlNode *p) {
  if(p->m_newPos.y > m_maxYPos) {
    m_maxYPos = p->m_newPos.y;
  }
  if(p->m_oldPos.y > m_maxYPos) {
    m_maxYPos = p->m_oldPos.y;
  }
}

TreeHeightFinder::TreeHeightFinder(AvlTree &tree) {
  m_maxYPos = 0;
  tree.traverse(*this);
}

class TreePainter {
private:
  AvlTree   &m_tree;
  bool       m_balanceVisible;
  double     m_slideAmount; // [0..1]
  CRect      m_workRect;
  CDC        m_workDC;
  CBitmap    m_workBitmap;
  CSize      m_charSize;
  void initWorkFields();
  void paintNode(AvlNode *p, const CPoint *parentPos);
  void flush();
public:
  TreePainter(AvlTree &tree);
  ~TreePainter();
  void paint(double slideAmount);
};

TreePainter::TreePainter(AvlTree &tree) : m_tree(tree) {
  m_balanceVisible = m_tree.isBalanceVisible();
  initWorkFields();
}

void TreePainter::initWorkFields() {
  AvlGraphics *graphics = m_tree.getAvlGraphics();
  CClientDC screen(graphics->getWindow());
  m_workDC.CreateCompatibleDC(&screen);
  m_workRect = graphics->getTreeRect();
  const int w = m_workRect.Width();
  const int h = m_workRect.Height();
  m_charSize = graphics->getCharSize();
  m_workBitmap.CreateBitmap(w,h,screen.GetDeviceCaps(PLANES),screen.GetDeviceCaps(BITSPIXEL),NULL);
  m_workBitmap.SetBitmapDimension(w,h);
  m_workDC.SelectObject(m_workBitmap);
}

void TreePainter::paint(double slideAmount) {
  m_slideAmount = slideAmount;
  m_workDC.FillSolidRect(&m_workRect,RGB(255,255,255));
  AvlNode *root = m_tree.getRoot();
  if(root) {
    paintNode(root,NULL);
  }
  flush();
}

TreePainter::~TreePainter() {
  if(m_workDC.m_hDC != NULL) {
    m_workDC.DeleteDC();
  }
  if(m_workBitmap.m_hObject != NULL) {
    m_workBitmap.DeleteObject();
  }
}

void TreePainter::paintNode(AvlNode *p, const CPoint *parentPos) {
  const double s1 = m_slideAmount;
  const double s0 = 1.0 - s1;

  CPoint pos((int)(s0*p->m_oldPos.x + s1*p->m_newPos.x), (int)(s0*p->m_oldPos.y + s1*p->m_newPos.y));

  const String nodeText      = m_balanceVisible ? format(_T("%d,%d"), p->m_key,p->m_balance) : format(_T("%d"), p->m_key);
  p->m_textSize              = m_workDC.GetTextExtent(nodeText.cstr());
  const CRect  ellipseRect   = p->getEllipseRect(pos);
  const CPoint ellipseCenter = ellipseRect.CenterPoint();

  if(parentPos != NULL) {
    m_workDC.MoveTo(*parentPos);
    m_workDC.LineTo(ellipseCenter.x, ellipseRect.top);
  }

  CPen pen;
  pen.CreatePen(PS_SOLID,1,p->m_color);
  CPen *oldPen = m_workDC.SelectObject(&pen);
  m_workDC.Ellipse(&ellipseRect);
  m_workDC.SelectObject(oldPen);
  m_workDC.SetTextColor(p->m_color);
  m_workDC.TextOut(pos.x,pos.y, nodeText.cstr());

  if(p->m_left || p->m_right) {
    const int xOffset = ellipseRect.Width()/3;
//    const int yOffset = (int)(sqrt((double)1.0 - sqr((double)xOffset/(ellipseRect.Width()/2))) * ellipseRect.Height()/2);
    const int yOffset = (int)(0.3727 * ellipseRect.Height()); // 0.3767 = sqrt(1-sqr(2/3))/2
    if(p->m_left) {
      paintNode(p->m_left ,&CPoint(ellipseCenter.x-xOffset, ellipseCenter.y+yOffset));
    }
    if(p->m_right) {
      paintNode(p->m_right,&CPoint(ellipseCenter.x+xOffset, ellipseCenter.y+yOffset));
    }
  }
}

void TreePainter::flush() {
  AvlGraphics *graphics = m_tree.getAvlGraphics();
  CClientDC screen(graphics->getWindow());
  CRect r = graphics->getTreeRect();
  screen.BitBlt(0, 0, m_workRect.Width(), m_workRect.Height(), &m_workDC, 0, 0, SRCCOPY);
}

static void setNodePosition(AvlNode *p, int leftBound, int rightBound, int level, int charHeight, bool newPos) {
  const int middle = (leftBound+rightBound)/2;

  CPoint &pos = newPos ? p->m_newPos : p->m_oldPos;
  pos = CPoint(middle,charHeight*(6*level+1));

  if(p->m_left ) {
    setNodePosition(p->m_left, leftBound, middle - 1, level+1, charHeight, newPos);
  }
  if(p->m_right) {
    setNodePosition(p->m_right,middle+1 , rightBound, level+1, charHeight, newPos);
  }
}

void AvlTree::findNodePositions(bool newPos) {
  if(m_root) {
    CSize charSize = getAvlGraphics()->getCharSize();
    setNodePosition(m_root, 1, getAvlGraphics()->getTreeRect().Width()-charSize.cx*4, 0, charSize.cy, newPos);
  }
}

void AvlTree::paint() {
  findNodePositions(true);
  TreePainter(*this).paint(1);
}

void AvlTree::animateTransition() {
  if(isTracing()) {
    findNodePositions(true);
    TreePainter painter(*this);

#define MAXSIZE 20000
    double animationSteps = (double)MAXSIZE / (size() + 100);
    double paintTimePerImage = 1.86 + size() * 0.024; // msec

#define TOTALANIMATIONTIME 1500

  // The relation: TOTALANIMATIONTIME = animationSteps * (paintTimePerImage+sleeptime)
  // => sleepTime = TOTALANIMATIONTIME / animationSteps - paintTimePerImage
  // => animationSteps = TOTALANIMATIONTIME / (sleepTime + paintTimePerImage)

    double sleepTime = TOTALANIMATIONTIME / animationSteps - paintTimePerImage;
    if(sleepTime < 0) {
      sleepTime = 1;
      animationSteps = TOTALANIMATIONTIME / (sleepTime + paintTimePerImage);
      if(animationSteps < 10) {
        animationSteps = 10;
      }
    }

#ifdef __NEVER
    double startTime = getProcessTime();
#endif

    for(int i = 0; i <= animationSteps; i++) {
      painter.paint((double)i/animationSteps);
      Sleep((DWORD)sleepTime);
    }

#ifdef __NEVER
    const double loopTime = (getProcessTime() - startTime) / 1000000;
    m_avlGraphics->getWindow()->MessageBox(
               format(_T("size:%d, loopTime:%6.2lf sec, #loops:%d, loopTime/#loops = painttime pr. picture:%6.2lf msec")
                     ,size()
                     ,loopTime
                     ,animationSteps
                     ,loopTime / animationSteps * 1000
                     ).cstr()
               ,_T("Measure")
               );
#endif
  }
}

class SetColor : public AvlNodeHandler {
private:
  int m_color;
public:
  void handleNode(AvlNode *p) {
    p->m_color = m_color;
  }
  SetColor(int color) {
    m_color = color;
  }
};

void AvlTree::setColor(int color) {
  traverse(SetColor(color));
}

// Create a fibonacci-tree, where all non-leaves has balance -1
// Makes a cascade of single-rotations if root is deleted
AvlNode *AvlTree::makeFibonacciNodeType1(int height) {
  if(height <= 0)
    return NULL;

  AvlNode *p   = new AvlNode;
  p->m_left    = makeFibonacciNodeType1(height-1);
  p->m_key     = 10 * m_size++;
  p->m_right   = makeFibonacciNodeType1(height-2);
  p->m_balance = (height == 1) ? 0 : -1;
  p->m_color   = BLACK;
  return p;
}

// Create a fibonacci-tree, where all non-leaves has balance -1 or 1 alternating between
// consecutive levels.
// Makes a cascade of double-rotations if root is deleted,
AvlNode *AvlTree::makeFibonacciNodeType2(int height, int balance) {
  if(height <= 0)
    return NULL;

  AvlNode *p = new AvlNode;
  p->m_color = BLACK;
  if(balance == -1) {
    p->m_left    = makeFibonacciNodeType2(height-1,-balance);
    p->m_key     = 10 * m_size++;
    p->m_right   = makeFibonacciNodeType2(height-2,-balance);
    p->m_balance = (height == 1) ? 0 : -1;
  } else {
    p->m_left    = makeFibonacciNodeType2(height-2,-balance);
    p->m_key     = 10 * m_size++;
    p->m_right   = makeFibonacciNodeType2(height-1,-balance);
    p->m_balance = (height == 1) ? 0 : 1;
  }
  return p;
}

void AvlTree::makeFibonacciTree(int height, int type) { // type = 1/2
  switch(type) {
  case 1:
    clear();
    m_root = makeFibonacciNodeType1(height);
    break;
  case 2:
    clear();
    m_root = makeFibonacciNodeType2(height,1);
    break;
  }
}

static int checkAvlNode(const AvlNode *n) { // Return height of tree.
  int leftHeight = 0, rightHeight = 0;
  if(n == NULL)
    return 0;

  if(n->m_left) {
    if(n->m_left->m_key >= n->m_key) {
      throwException(_T("Node %d has left child with key = %d"), n->m_key, n->m_left->m_key);
    }
    leftHeight = checkAvlNode(n->m_left);
  }
  if(n->m_right) {
    if(n->m_right->m_key <= n->m_key) {
      throwException(_T("Node %d has right child with key = %d"), n->m_key, n->m_right->m_key);
    }
    rightHeight = checkAvlNode(n->m_right);
  }

  if((rightHeight - leftHeight) != n->m_balance) { // validate balance-field
    throwException(_T("Node %d has balance %d, but (height(right subtree)(=%d) - height(left subtree)=%d) = %d")
                  ,rightHeight,leftHeight,rightHeight-leftHeight);
  }

  return max(leftHeight,rightHeight) + 1;
}

void AvlTree::validate() {
  try {
    checkAvlNode(m_root);
  } catch(Exception e) {
    message(RED,_T("Tree invariant is broken:%s"),e.what());
  }
}

// ------------------------------- Iterator ----------------------------------------

AvlIterator::AvlIterator(AvlTree &tree) : m_tree(tree) {
  m_top  = 0;
  first();
}

void AvlIterator::push(AvlNode *avlNode, AvlIteratorStatus status) {
  AvlIteratorStackElem *sp = m_stack + m_top++;
  sp->m_avlNode = avlNode;
  sp->m_status  = status;
}

int *AvlIterator::next() {
  AvlIteratorStackElem *sp;
  if(!m_lastWasNext) {
    if((sp = top()) != NULL) {
      if(sp->m_status == LEFT_FOR) {
        sp->m_status = RIGHT_FOR;
      }
    }
    m_lastWasNext = true;
  }

  for(sp = top(); sp; sp = top()) {
    AvlNode *p = sp->m_avlNode;
    switch(sp->m_status) {
    case LEFT_DONE :
      sp->m_status = RIGHT_FOR;
      return &p->m_key;

    case LEFT_FOR  :
      if(p->m_left) {
        sp->m_status = LEFT_DONE;
        push(p->m_left, LEFT_FOR);
      } else {
        sp->m_status = RIGHT_FOR;
        return &p->m_key;
      }
      break;

    case RIGHT_FOR  :
      if(p->m_right) {
        sp->m_status = RIGHT_DONE;
        push(p->m_right, LEFT_FOR);
      } else
        pop();
      break;

    case RIGHT_DONE:
      pop();
      break;

    default:
      NODEFAULT;
    }
  }
  return NULL;
}

int *AvlIterator::prev() {
  AvlIteratorStackElem *sp;
  if(m_lastWasNext) {
    if((sp = top()) != NULL) {
      if(sp->m_status == RIGHT_FOR) {
        sp->m_status = LEFT_FOR;
      }
    }
    m_lastWasNext = false;
  }

  for(sp = top(); sp; sp = top()) {
    AvlNode *p = sp->m_avlNode;
    switch(sp->m_status) {
      case RIGHT_DONE :
      sp->m_status = LEFT_FOR;
      return &p->m_key;

    case RIGHT_FOR  :
      if(p->m_right) {
        sp->m_status = RIGHT_DONE;
        push(p->m_right, RIGHT_FOR);
      } else {
        sp->m_status = LEFT_FOR;
        return &p->m_key;
      }
      break;

    case LEFT_FOR  :
      if(p->m_left) {
        sp->m_status = LEFT_DONE;
        push(p->m_left, RIGHT_FOR);
      } else
        pop();
      break;

    case LEFT_DONE:
      pop();
      break;

    default:
      NODEFAULT;
    }
  }
  return NULL;
}

int *AvlIterator::first() {
  m_lastWasNext = true;
  m_top = 0;
  if(m_tree.m_root) {
    push(m_tree.m_root, LEFT_FOR);
  }
  return next();
}

int *AvlIterator::last() {
  m_lastWasNext = false;
  m_top = 0;
  if(m_tree.m_root) {
    push(m_tree.m_root, RIGHT_FOR);
  }
  return prev();
}

bool AvlIterator::find(int key) {
  AvlNode *p = m_tree.m_root;

  m_top = 0;
  m_lastWasNext = true;

  while(p) {
    const int c = intCompare(p->m_key, key);
    if(c > 0) {
      push(p, LEFT_DONE);
      p = p->m_left;
    } else if(c < 0) {
      push(p, RIGHT_DONE);
      p = p->m_right;
    } else {
      push(p, RIGHT_FOR);
      return true;
    }
  }
  return false;
}

static TCHAR *statusString[] = {
  _T("left_done")
 ,_T("left_for")
 ,_T("right_for")
 ,_T("right_done")
};
/*
void AvlIterator::paint() {
  const int n = m_top;
  Console::printf(50,20,_T("top:%2d"), n);
  for(int i = 0; i < 10; i++ ) {
    if(i < n) {
      const AvlIteratorStackElem &sp = m_stack[i];
      Console::printf(30,20-i,_T("%-10s  %3d"), statusString[sp.m_status], sp.m_avlNode->m_key);
    }
    else
      Console::printf(30,20-i,_T("                  ") );
  }
}
*/

static String makeCurrentKey(const int *x) {
  return x ? format(_T("%d"),*x) : _T("NULL");
}

