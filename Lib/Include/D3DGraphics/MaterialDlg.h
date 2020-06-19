#pragma once

#include <MFCUtil/ColormapDialog.h>
#include <MFCUtil/colormap.h>
#include <D3DGraphics/resource.h>
#include "D3Scene.h"

class CMaterialDlg : public CColormapDialog<D3Material> {
private:
  String   m_origName;

  void  resetControls();
  void  valueToWindow(const D3Material &v);
  void  showPower(       double v);
  void  setSliderPower(  double v);
  float getSliderPower() const;
  void  setSpecularHighlights(bool v);
  bool  getSpecularHighlights() const;
  void  setSliderOpacity(double v);
  float getSliderOpacity() const;
  void  showOpacity(     double v);
public:
  CMaterialDlg(PropertyChangeListener *listener = NULL, CWnd *pParent = NULL);
  enum { IDD = IDD_MATERIAL_DIALOG };
  String getTypeName() const {
    return _T("D3Material");
  }
protected:
  virtual void    DoDataExchange(CDataExchange *pDX);
  virtual BOOL    OnInitDialog();
  virtual void    OnCancel();
  afx_msg void    OnClose();
  afx_msg void    OnHideWindow();
  afx_msg void    OnBnClickedCheckSpecularHighlights();
  afx_msg void    OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg void    OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar);
  afx_msg LRESULT OnMsgResetControls(WPARAM wp, LPARAM lp);
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
