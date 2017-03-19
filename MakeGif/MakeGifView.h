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
  GIFCTRL_STATUS
 ,SHOW_MODE
 ,CONTROL_UPDATE_TIME
} ViewProperty;

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
    void loadGifFileFromDoc();
    void showPanelSize();
    void drawImagePanel(const PixRectArray &prArray);
    ShowFormat getShowFormat()  const;
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

public:
    virtual void OnDraw(CDC *pDC);
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnInitialUpdate();

    virtual ~CMakeGifView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in MakeGifView.cpp
inline CMakeGifDoc *CMakeGifView::GetDocument()
   { return (CMakeGifDoc*)m_pDocument; }
#endif

