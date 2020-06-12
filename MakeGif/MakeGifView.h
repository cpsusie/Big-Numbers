#pragma once

#include <Date.h>
#include <MFCUtil/LayoutManager.h>
#include "GifCtrlWithProperties.h"

typedef enum {
  SHOW_RAW
 ,SHOW_SCALED
 ,SHOW_QUANTIZED
} ShowFormat;

typedef enum {
  GIFCTRL_STATUS      // bool
 ,CONTROL_UPDATE_TIME // Timestamp
} ViewProperty;

class PanelPoint {
public:
  const int    m_panelId;
  const CPoint m_point;
  inline PanelPoint(int id, const CPoint &point) : m_panelId(id), m_point(point) {
  }
};

class CMakeGifView : public CFormView, public PropertyContainer, public PropertyChangeListener {
protected:
    CMakeGifView();
    DECLARE_DYNCREATE(CMakeGifView)
public:
  enum { IDD = IDD_GIFVIEW };
private:
    CGifCtrlWithProperties m_gifCtrl;
    Timestamp              m_lastCtrlUpdate;
    SimpleLayoutManager    m_layoutManager;
    CPoint                 m_currentMouse;
    void                loadGifFileFromDoc();
    void                showPanelSize();
    void                drawImgPanel(const PixRectArray *prArray);
    ShowFormat          getShowFormat() const;
    const PixRectArray *getVisiblePrArray();
    PanelPoint          getPanelPoint(const CPoint &viewPoint) const;
    void OnRButtonDownGifPanel(UINT nFlags, CPoint point);
    void OnRButtonDownImgPanel(UINT nFlags, CPoint point);
public:
    CMakeGifDoc *GetDocument();

    inline bool isPlaying() const {
      return m_gifCtrl.isPlaying();
    }

    inline bool isShowingAll() const {
      return m_gifCtrl.isPainted();
    }
    void startPlay();
    void stopPlay();
    void showAll();
    void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);

    virtual void OnDraw(CDC *pDC);
    virtual void OnInitialUpdate();
    virtual void OnFinalRelease();
    virtual ~CMakeGifView();
#if defined(_DEBUG)
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext &dc) const;
#endif

protected:
    afx_msg void OnSize(       UINT nType, int cx, int cy);
    afx_msg void OnMouseMove(  UINT nFlags, CPoint point);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg BOOL OnTtnNeedText(UINT id, NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};

#if !defined(_DEBUG  )// debug version in MakeGifView.cpp
inline CMakeGifDoc *CMakeGifView::GetDocument()
   { return (CMakeGifDoc*)m_pDocument; }
#endif
