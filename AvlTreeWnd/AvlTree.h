#pragma once

#define BLACK  RGB(  0,  0,  0)
#define YELLOW RGB(255,255,  0)
#define RED    RGB(255,  0,  0)
#define GREEN  RGB(  0,255,  0)
#define TURKIS RGB(  0,255,255)
#define CYAN   RGB(255,  0,255)

class AvlGraphics {
public:
  virtual void vmessage(COLORREF color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr) = 0;
  void    message( COLORREF color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  virtual CRect getTreeRect() = 0;
  virtual CWnd *getWindow()   = 0;
  virtual CSize getCharSize() = 0;
};

class AvlNodeHandler;

class AvlNode {
public:
  AvlNode *m_left, *m_right;
  char     m_balance;
  COLORREF m_color;
  int      m_key;
  CPoint   m_oldPos, m_newPos;
  CSize    m_textSize; // size of text in pixels
  void visitNode(AvlNodeHandler &handler);
  bool contains(const CPoint &p) const;
  CRect getEllipseRect(const CPoint &pos) const;
};

class AvlNodeHandler {
private:
  int m_level; // Root has level 0. if(p has lavel n then p->left and right has level n+1
protected:
  int getLevel() const {
    return m_level;
  }
public:
  virtual void handleNode(AvlNode *p) = 0;
  friend class AvlTree;
  friend class AvlNode;
};

class AvlTree {
private:
  AvlNode     *m_root;          // Root of the tree
  int          m_size;          // Number of nodes in tree
  AvlNode     *m_deleteHelper;
  bool         m_updateDone;    // Returnvalue of insert and remove
  bool         m_balanceVisible, m_tracing;
  CWnd        *m_wnd;
  AvlGraphics *m_avlGraphics;
  bool nodeInsert(  AvlNode **pp, int key);
  bool balanceLeft( AvlNode **pp );
  bool balanceRight(AvlNode **pp );
  bool nodeDel(     AvlNode **r  );
  bool nodeDelete(  AvlNode **pp, int key);
  AvlNode *makeFibonacciNodeType1(int height);
  AvlNode *makeFibonacciNodeType2(int height, int balance);
  void vmessage(COLORREF color, _In_z_ _Printf_format_string_ TCHAR const * const format, va_list argptr);
  void message( COLORREF color, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
  void traceTree(AvlNode *p, int markColor, const TCHAR *msg);
public:
  AvlTree();
  ~AvlTree();
  bool  insert(int key);
  bool  remove(int key);
  bool  contains(int key);
  AvlNode *findNodeByPosition(const CPoint &p);
  void  clear();
  int  *getFirst();
  int  *getLast();
  void traverse(AvlNodeHandler &handler);
  void setAvlGraphics(AvlGraphics *graphics) {
    m_avlGraphics = graphics;
  }

  AvlGraphics *getAvlGraphics() {
    return m_avlGraphics;
  }

  int size() const {
    return m_size;
  }

  AvlNode *getRoot() {
    return m_root;
  }

  friend class AvlIterator;
  void findNodePositions(bool newPos);
  void paint();
  void animateTransition();
  void setColor(int color);
  void makeFibonacciTree(int height, int type);
  void validate();

  void setBalanceVisible(bool visible) {
    m_balanceVisible = visible;
  }
  bool isBalanceVisible() const {
    return m_balanceVisible;
  }

  void setTracing(bool tracing) {
    m_tracing = tracing;
  }

  bool isTracing() const {
    return m_tracing;
  }
};

typedef enum {
  LEFT_DONE,
  LEFT_FOR,
  RIGHT_FOR,
  RIGHT_DONE
} AvlIteratorStatus;

typedef struct {
  AvlNode          *m_avlNode;
  AvlIteratorStatus m_status;
} AvlIteratorStackElem;

class AvlIterator {
private:
  AvlTree             &m_tree;
  unsigned char        m_top;
  AvlIteratorStackElem m_stack[40]; /* 40 should be enough ! */
  bool                 m_lastWasNext;

  void push(AvlNode *avlNode, AvlIteratorStatus status);

  void pop() {
    m_top--;
  }

  bool isEmpty() const {
    return m_top == 0;
  }

  AvlIteratorStackElem *top() {
    return isEmpty() ? NULL : m_stack + m_top - 1;
  }
public:
  AvlIterator(AvlTree &tree);
  int *next();
  int *prev();
  int *first();
  int *last();
  bool find(int key);
//  void paint();
};
