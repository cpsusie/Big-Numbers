#pragma once

#include <MatrixDimension.h>
#include <FlagTraits.h>

typedef enum {
  SPLIT_NONE
 ,SPLIT_VERTICAL
 ,SPLIT_HORIZONTAL
} SplitDirection;

#define SPLFMASK_CHILDCOUNT    0x03
#define SPLFMASK_SPLITPOSMOVED 0x04
#define SPLFMASK_RECALCACTIVE  0x08

class SplitViewSplitter : public CSplitterWnd {
private:
  const SplitDirection  m_splitDirection;
  const MatrixDimension m_dim;
  double                m_relativeSplitPos;
  FLAGTRAITS(SplitViewSplitter, BYTE, m_flags)
  static MatrixDimension createDimension(SplitDirection splitDirection);
  void   saveRelativeSplitPos(const CSize &size);
  inline void incrChildCount() {
    m_flags++;
  }
public:
  SplitViewSplitter(SplitDirection splitDirection, double relativeSplitPos);
  BOOL CreateView(int row, int col, CRuntimeClass* pViewClass,
    SIZE sizeInit, CCreateContext* pContext);
  void            setRelativeSplitPos(int cx, int cy);
  void            setSplit(const CSize &size);
  void            OnSize(int cx, int cy);
  void            RecalcLayout();
  void            OnInvertTracker(const CRect& rect);
  inline const MatrixDimension &getDimension() const {
    return m_dim;
  }
  inline SplitDirection getSplitDirection() const {
    return m_splitDirection;
  }
  int             getPanelCount() const {
    return GetRowCount()*GetColumnCount();
  }
  inline UINT getChildCount() const {
    return m_flags & SPLFMASK_CHILDCOUNT;
  }
  // Expect this contains 2 child-windows (either 2x1 or 1x2)
  // Return nullptr if wnd is not one of them
  CWnd *findNeighbor(CWnd *wnd) const;
};

class CD3FunctionDoc;

class CSplitView : public CView {
private:
  SplitViewSplitter m_splitter;

  static CRuntimeClass *s_childClass1, *s_childClass2;
  static SplitDirection s_splitDirection;
  static double         s_relativeSplitPos;
  inline bool isChildrenCreated() const {
    return m_splitter.getChildCount() == 2;
  }
protected: // create from serialization only
  DECLARE_DYNCREATE(CSplitView)
  DECLARE_MESSAGE_MAP()
public:
  static CSplitView *create(CWnd *parent, CRuntimeClass *child1, CRuntimeClass *child2, SplitDirection splitDirection, const CRect &rect, double initialRelativeSplitPos = 0.5);
  CSplitView();
  virtual ~CSplitView();
  CD3FunctionDoc *GetDocument();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
  CWnd        *getChild(UINT index) const;
  afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  void         OnShowWindow(BOOL bShow, UINT nStatus);
  virtual void OnDraw(CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
};

#if !defined(_DEBUG) // debug version in D3FunctionView.cpp
inline CD3FunctionDoc* CSplitView::GetDocument()
{
  return (CD3FunctionDoc*)m_pDocument;
}
#endif

MatrixIndex findPosition(const CSplitterWnd *splitter, const CWnd *wnd);
