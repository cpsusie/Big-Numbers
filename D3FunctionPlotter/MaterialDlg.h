#pragma once

#include "ColormapDialog.h"
#include "colormap.h"

class CMaterialDlg : public CColormapDialog<MATERIAL> {
private:
  D3Scene &m_scene;
  String   m_origName;

  void   resetControls();
  void   valueToWindow(const MATERIAL &v);
  void   showPower(double v);
  void   setSliderPower(double v);
  float  getSliderPower() const;
public:
    CMaterialDlg(D3Scene &scene, PropertyChangeListener *listener);

	enum { IDD = IDD_MATERIAL_DIALOG };

public:
protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
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
  CColormap m_colormapDiffuse;
  CColormap m_colormapSpecular;
  CColormap m_colormapEmissive;
  DECLARE_EVENTSINK_MAP()
  void OnColorchangedColormapAmbient();
  void OnColorchangedColormapDiffuse();
  void OnColorchangedColormapSpecular();
  void OnColorchangedColormapEmissive();
};

