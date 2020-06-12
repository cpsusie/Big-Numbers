#pragma once

#include "ImageDoc.h"

class CMainFrame;

class CImageView : public CScrollView {
private:
  double      m_currentZoomFactor;
  CPrintInfo *m_printInfo;

  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetParent();
  }
protected:
    CImageView();
    DECLARE_DYNCREATE(CImageView)

public:
    CImageDoc *GetDocument();
    PixRect   *getPixRect();
    void       setScrollRange();
    void       repaint();

    double getCurrentZoomFactor() const {
      return m_currentZoomFactor;
    }

    void zoom(bool in);

    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnInitialUpdate();
    protected:
    virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
    virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
    virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
    virtual ~CImageView();
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in ImageView.cpp
inline CImageDoc* CImageView::GetDocument()
   { return (CImageDoc*)m_pDocument; }
#endif

