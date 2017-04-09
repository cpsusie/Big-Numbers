#pragma once

#include "D3Scene.h"
#include "ColormapDialog.h"
#include "colormap.h"

class CLightDlg : public CColormapDialog<LIGHT> {
private:
    String   m_origName;
    LIGHT    m_origLight;

    void  resetControls();
    void  valueToWindow(   const LIGHT &v);
    void  ajourSliders(    const LIGHT &v);
    void  enableSliders(   const LIGHT &v);
    void  showSliderValues(const LIGHT &v);
    void  setCurrentValue( const LIGHT &v);
    const LIGHT getCurrentValue() const;
public:
    CLightDlg(PropertyChangeListener *listener);
	  enum { IDD = IDD_LIGHT_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
	  afx_msg void OnHideWindow();
	  virtual void OnCancel();
	  afx_msg void OnClose();
    DECLARE_MESSAGE_MAP()
private:
  CColormap m_colormapAmbient;
  CColormap m_colormapSpecular;
  CColormap m_colormapDiffuse;
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapAmbient();
  void OnColorchangedColormapDiffuse();
  void OnColorchangedColormapSpecular();
};
