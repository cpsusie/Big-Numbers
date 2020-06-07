#pragma once

class C3DSceneView;

class D3ViewArray : public CompactArray<C3DSceneView*> {
public:
  // return index of view with m_hwnd == hwnd, -1 if none found
  int findIndex(HWND hwnd) const;
  C3DSceneView *findViewByHwnd(HWND hwnd) const;
  void remove(C3DSceneView*);
};

class C3DSceneView : public CView {
private:
  D3Camera *m_camera;
  D3Scene  *getScene() const;
  D3Camera *findCamera() const;
  D3Camera *getCamera();
  static D3ViewArray s_3DViewArray;
protected: // create from serialization only
  DECLARE_DYNCREATE(C3DSceneView)

public:
  C3DSceneView();
  virtual ~C3DSceneView();
  CD3FunctionDoc  *GetDocument();
#if defined(_DEBUG)
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif
  static inline HWND get3DWindow(UINT index) {
    return *s_3DViewArray[index];
  }
  static inline UINT get3DWindowCount() {
    return (UINT)s_3DViewArray.size();
  }
  static inline C3DSceneView *findViewByHwnd(HWND hwnd) {
    return s_3DViewArray.findViewByHwnd(hwnd);
  }
  static inline bool is3DWindow(HWND hwnd) {
    return s_3DViewArray.findIndex(hwnd) >= 0;
  }
  virtual void OnDraw(           CDC *pDC);  // overridden to draw this view
  virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
  virtual void OnBeginPrinting(  CDC *pDC, CPrintInfo *pInfo);
  virtual void OnEndPrinting(    CDC *pDC, CPrintInfo *pInfo);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
};

#if !defined(_DEBUG) // debug version in D3FunctionView.cpp
inline CD3FunctionDoc* C3DSceneView::GetDocument()
   { return (CD3FunctionDoc*)m_pDocument; }
#endif
