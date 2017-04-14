#pragma once

#include <MFCUtil/ColormapDialog.h>
#include <MFCUtil/colormap.h>
#include <D3DGraphics/resource.h>
#include "D3Scene.h"

class CMaterialDlg : public CColormapDialog<MATERIAL> {
private:
  String   m_origName;

  void  resetControls();
  void  valueToWindow(const MATERIAL &v);
  void  showPower(double v);
  void  setSliderPower(double v);
  float getSliderPower() const;
  void  setSliderTransparency(double v);
  float getSliderTransparency() const;
  void  showTransparency(double v);
public:
  CMaterialDlg(PropertyChangeListener *listener, CWnd *pParent = NULL);
  enum { IDD = IDD_MATERIAL_DIALOG };
protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
    afx_msg void OnHideWindow();
    virtual void OnCancel();
    afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
private:
  CColormap m_colormapAmbient;
  CColormap m_colormapDiffuse;
  CColormap m_colormapSpecular;
  CColormap m_colormapEmissive;
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapAmbient();
  void OnColorchangedColormapDiffuse();
  void OnColorchangedColormapSpecular();
  void OnColorchangedColormapEmissive();
};
