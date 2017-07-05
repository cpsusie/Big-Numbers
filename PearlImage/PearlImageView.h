#pragma once

#include "PearlImageDoc.h"
#include "DrawTool.h"

class CMainFrame;

typedef enum {
  RIGHTMARK
 ,BOTTOMMARK
 ,RIGHTBOTTOMMARK
} EdgeMarkType;

class EdgeMark {
private:
  const EdgeMarkType m_type;
  CRect              m_activeRect;
  CRect              m_visibleRect;
  bool               m_visible;

  CRect createRect(const CPoint &p, int size) const;
  CRect createActiveRect( const CPoint &p) const;
  CRect createVisibleRect(const CPoint &p) const;
public:
  EdgeMark(EdgeMarkType type) : m_type(type) {
  }

  int getCursorId() const;

  EdgeMarkType getType() const {
    return m_type;
  }
  void setPosition(const CPoint &p);
  void setVisible(bool visible) {
    m_visible = visible;
  }
  bool isVisible()  const {
    return m_visible;
  }

  const CRect &getActiveRect() const {
    return m_activeRect;
  }

  const CRect &getVisibleRect() const {
    return m_visibleRect;
  }
};

class CPearlImageView : public CScrollView, public PixRectContainer {
private:
  HACCEL           m_accelTable;
  bool             m_initialized;
  int              m_currentZoomFactor;
  CPoint           m_lastPoint;
  CPrintInfo      *m_printInfo;
  Array<EdgeMark>  m_edgeMark;
  const EdgeMark  *m_currentEdgeMark;
  CRect            m_dragRect,*m_lastDragRect;

  CMainFrame *getMainFrame() {
    return (CMainFrame*)GetParent();
  }

  const CMainFrame *getMainFrame() const {
    return (const CMainFrame*)GetParent();
  }

  int  getCurrentToolCursor();
  void restoreOldTool();
  void paintBackgroundAndEdge(CDC &dc);
  void resizeDocument(const CPoint &p);
  void paintResizingFrame(const CPoint &p);
  CRect getDocumentRect() const;
  CRect getViewRect() const;
protected:
  CPearlImageView();
  DECLARE_DYNCREATE(CPearlImageView)
public:
    CPearlImageDoc *GetDocument();
    const CPearlImageDoc *GetDocument() const {
       return (const CPearlImageDoc*)m_pDocument;
    }
    PixRect     *getImage();
    D3DCOLOR     getColor();
    int          getApproximateFillTolerance() const;

    void         repaint();
    void         enableCut(bool enabled);
    void         saveDocState();
    void         setScrollRange();
    void         refreshDoc();
    void         clear();
    void         setCurrentZoomFactor(int factor);
    int          getCurrentZoomFactor() const {
      return m_currentZoomFactor;
    }

    CPoint       getCurrentMousePoint() const {
      return m_lastPoint;
    }

    bool         isMouseOnDocument() const;
    CPoint       getDocPoint( const CPoint &viewPoint) const;
    CPoint       getViewPoint(const CPoint &docPoint)  const;
    EdgeMark    *findEdgeMark(const CPoint &point);

    CPoint       getTopLeft() const { // coordinates of upper-left corner of visible part image
      return GetScrollPosition();
    }

    virtual ~CPearlImageView();
    virtual void OnDraw(CDC *pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT &cs);
    virtual void OnInitialUpdate();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual BOOL OnPreparePrinting(CPrintInfo *pInfo);
    virtual void OnBeginPrinting(CDC *pDC, CPrintInfo *pInfo);
    virtual void OnEndPrinting(CDC *pDC, CPrintInfo *pInfo);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PearlImageView.cpp
inline CPearlImageDoc* CPearlImageView::GetDocument()
   { return (CPearlImageDoc*)m_pDocument; }
#endif
