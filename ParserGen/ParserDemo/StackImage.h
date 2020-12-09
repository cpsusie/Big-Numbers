#pragma once

#include "TestParser.h"
#include <CompactStack.h>

class StackImageElement {
  ParserStackElement m_elem;
  SyntaxNodep        m_node;
public:
  StackImageElement()
    : m_node(nullptr)
  {
  }
  StackImageElement(const ParserStackElement &elem, const SyntaxNodep node)
    : m_elem(elem)
    , m_node(node)
  {
  }
  const ParserStackElement &getElement() const {
    return m_elem;
  }
  String toString() const;
};

class StackImage {
private:
  CStatic                        *m_wnd;
  TestParser                     *m_parser;
  CDC                             m_workDC;
  CBitmap                         m_workBitmap;
  CFont                           m_font;
  CSize                           m_winSize, m_charSize;
  CSize                           m_maxChars; // size in chars, vertical and horizontal
  UINT                            m_maxTextLength;
  CompactStack<StackImageElement> m_stack;
  BYTE                            m_paintLevel;
  void startPaint() {
    m_paintLevel++;
  }
  void endPaint();
  void push(UINT i); // push i'te element from top of m_parser.stack and paint on m_workBitmap
  void pop();        // pop 1 element and paint blank
  void paintTopElement();
  void clearTopElement();
  void repaintAll();
  // return true, if size has chaged, else false
  void setAllWhite();
  bool createWorkBitmap();
  void destroyWorkBitmap();
  void flushImage();
public:
  StackImage();
  void init(CStatic *wnd, TestParser &parser);
  ~StackImage();
  void updateImage();
  void OnSize();
  // p relative to m_wnd
  // Return index fromstacktop of element at position p
  // or -1, if no element there
  int findStackIndexFromTop(const CPoint &p) const;
};
