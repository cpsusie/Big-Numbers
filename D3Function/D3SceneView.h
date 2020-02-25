#pragma once

class C3DSceneView : public CView {
private:
  D3Camera *m_camera;
  D3Scene  *getScene() const;
  D3Camera *findCamera() const;
  D3Camera *getCamera();

protected: // create from serialization only
  DECLARE_DYNCREATE(C3DSceneView)

public:
  C3DSceneView();
  virtual ~C3DSceneView();
  CD3FunctionDoc  *GetDocument();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

  virtual void OnDraw(           CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in D3FunctionView.cpp
inline CD3FunctionDoc* C3DSceneView::GetDocument()
   { return (CD3FunctionDoc*)m_pDocument; }
#endif
