#pragma once

#include "CaptureDoc.h"

class CCaptureView : public CScrollView {
protected:
  CCaptureView();
  DECLARE_DYNCREATE(CCaptureView)

private:
  void setScrollRange();
  CPrintInfo  *m_printInfo;

public:
  CCaptureDoc *GetDocument();
  void repaint();
  void ajourMenuItems();

public:
  virtual void OnDraw(CDC* pDC);
  virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
  virtual void OnInitialUpdate();
protected:
  virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
  virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
  virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
  virtual ~CCaptureView();

#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnFilePrint();
  afx_msg void OnFilePrintPreview();
  afx_msg void OnFileSave();
  afx_msg void OnEditCopy();
  DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in CaptureView.cpp
inline CCaptureDoc* CCaptureView::GetDocument()
   { return (CCaptureDoc*)m_pDocument; }
#endif

