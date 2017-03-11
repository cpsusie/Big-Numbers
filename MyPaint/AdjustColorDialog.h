#pragma once

#include "colormap.h"
#include "DrawTool.h"
#include <MFCUtil/ColorSpace.h>

class CAdjustColorDialog : public CDialog {
  PixRectContainer &m_container;
  PixRect          *m_origImage;
  CSize             m_size;
  LSHColor          m_origLSHColor;
  COLORREF          m_lastColor;
  bool              m_initDone;
  bool              m_firstChange;
public:
    CAdjustColorDialog(PixRectContainer &container, CWnd *pParent = NULL);
   ~CAdjustColorDialog();
    enum { IDD = IDD_ADJUSTLSHDIALOG };
    CColormap m_colorMap;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnColorChangedColorMapCtrl();
    virtual void OnOK();
    virtual void OnCancel();
    virtual BOOL OnInitDialog();
    DECLARE_EVENTSINK_MAP()
    DECLARE_MESSAGE_MAP()
};

