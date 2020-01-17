#pragma once

class CD3SceneView : public CView {
private:

protected: // create from serialization only
  CD3SceneView();
  DECLARE_DYNCREATE(CD3SceneView)

public:
  CD3FunctionDoc  *GetDocument();
  virtual ~CD3SceneView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  virtual void OnDraw(           CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in D3FunctionView.cpp
inline CD3FunctionDoc* CD3SceneView::GetDocument()
   { return (CD3FunctionDoc*)m_pDocument; }
#endif
