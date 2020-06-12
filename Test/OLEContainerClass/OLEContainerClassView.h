#pragma once

class COLEContainerClassView : public CView {
protected: // create from serialization only
    COLEContainerClassView();
    DECLARE_DYNCREATE(COLEContainerClassView)

public:
    COLEContainerClassDoc *GetDocument();

public:
    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
protected:

public:
    virtual ~COLEContainerClassView();
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    afx_msg void OnCancelEditSrvr();
    DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in OLEContainerClassView.cpp
inline COLEContainerClassDoc *COLEContainerClassView::GetDocument()
   { return (COLEContainerClassDoc*)m_pDocument; }
#endif

