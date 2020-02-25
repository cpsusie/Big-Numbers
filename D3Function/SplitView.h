#pragma once

#include <MatrixDimension.h>

typedef enum {
  SPLIT_NONE
 ,SPLIT_VERTICAL
 ,SPLIT_HORIZONTAL
} SplitDirection;

class SplitViewSplitter : public CSplitterWnd {
private:
  const SplitDirection m_splitDirection;
  bool                 m_splitPointMoved;
  BYTE                 m_childCount;
public:
  SplitViewSplitter();
  BOOL CreateView(int row, int col, CRuntimeClass* pViewClass,
    SIZE sizeInit, CCreateContext* pContext);
  void            RecalcLayout();
  void            OnInvertTracker(const CRect& rect);
  MatrixDimension getDimension() const;
  inline SplitDirection getSplitDirection() const {
    return m_splitDirection;
  }
  int             getPanelCount() const {
    return GetRowCount()*GetColumnCount();
  }
  inline UINT getChildCount() const {
    return m_childCount;
  }
};

class CSplitView : public CView {
  friend class SplitViewSplitter;
private:
  SplitViewSplitter m_splitter;
  double            m_relativeSplitPoint;
  void saveRelativeSplitPoint();

  static CRuntimeClass *s_childClass1, *s_childClass2;
  static SplitDirection s_splitDirection;

protected: // create from serialization only
  DECLARE_DYNCREATE(CSplitView)
  DECLARE_MESSAGE_MAP()
public:
  static CSplitView *create(CWnd *parent, CRuntimeClass *child1, CRuntimeClass *child2, SplitDirection splitDirection, const CRect &rect);
  CSplitView();
  virtual ~CSplitView();
  CD3FunctionDoc *GetDocument();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
  void                   setRelativeSplitPoint(int cx, int cy);
  inline MatrixDimension getDimension() const {
    return m_splitter.getDimension();
  }
  CWnd                  *getChild(UINT index) const;
  inline UINT            getChildCount() const {
    return (UINT)getDimension().getElementCount();
  }
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  void         OnShowWindow(BOOL bShow, UINT nStatus);
  virtual void OnDraw(CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
};

#ifndef _DEBUG  // debug version in D3FunctionView.cpp
inline CD3FunctionDoc* CSplitView::GetDocument()
{
  return (CD3FunctionDoc*)m_pDocument;
}
#endif
