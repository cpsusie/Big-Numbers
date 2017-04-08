#include "pch.h"
#include <D3DGraphics/MaterialDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMaterialDlg::CMaterialDlg(D3Scene &scene, PropertyChangeListener *listener) 
: CColormapDialog<D3DMATERIAL>(CMaterialDlg::IDD, SP_MATERIALPARAMETERS, NULL)
, m_scene(scene)
{
  if(listener) {
    addPropertyChangeListener(listener);
  }
}

void CMaterialDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAP_AMBIENT, m_colormapAmbient);
  DDX_Control(pDX, IDC_COLORMAP_DIFFUSE, m_colormapDiffuse);
  DDX_Control(pDX, IDC_COLORMAP_EMISSIVE, m_colormapEmissive);
  DDX_Control(pDX, IDC_COLORMAP_SPECULAR, m_colormapSpecular);
}

BEGIN_MESSAGE_MAP(CMaterialDlg, CDialog)
    ON_WM_HSCROLL()
	  ON_WM_SHOWWINDOW()
    ON_MESSAGE(ID_MSG_RESETCONTROLS, OnMsgResetControls)
	  ON_BN_CLICKED(ID_HIDEWINDOW    , OnHideWindow      )
	  ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CMaterialDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  m_origName = getWindowText(this);
  initSlider(IDC_SLIDER_POWER, 0.1, 200, 200, true);
  return TRUE;
}

LRESULT CMaterialDlg::OnMsgResetControls(WPARAM wp, LPARAM lp) {
  resetControls();
  return 0;
}

void CMaterialDlg::resetControls() {
  setNotifyEnabled(false);

  const D3DMATERIAL &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);

  setNotifyEnabled(true);
}

void CMaterialDlg::valueToWindow(const D3DMATERIAL &v) {
  setSliderPower(v.Power);
  setD3DCOLORVALUE(IDC_COLORMAP_AMBIENT , v.Ambient );
  setD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE , v.Diffuse );
  setD3DCOLORVALUE(IDC_COLORMAP_SPECULAR, v.Specular);
  setD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE, v.Emissive);
}

void CMaterialDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  D3DMATERIAL v = getCurrentValue();
  v.Power = getSliderPower();
  setCurrentValue(v);
  showPower(v.Power);
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMaterialDlg::setSliderPower(double v) {
  setSliderValue(IDC_SLIDER_POWER, v);
  showPower(v);
}

float CMaterialDlg::getSliderPower() const {
  return getSliderValue(IDC_SLIDER_POWER);
}

void CMaterialDlg::showPower(double v) {
  GetDlgItem(IDC_STATIC_POWER)->SetWindowText(format(_T("%.2f"), v).cstr());
}


void CMaterialDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  PropertyDialog::OnShowWindow(bShow, nStatus);
}

void CMaterialDlg::OnHideWindow() {
  ShowWindow(SW_HIDE);
}

void CMaterialDlg::OnClose() {
  OnHideWindow();
}

void CMaterialDlg::OnCancel() {
  const D3DMATERIAL &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);
}

BEGIN_EVENTSINK_MAP(CMaterialDlg, CColormapDialog)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_AMBIENT, 1, CMaterialDlg::OnColorchangedColormapAmbient, VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_DIFFUSE, 1, CMaterialDlg::OnColorchangedColormapDiffuse, VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_SPECULAR, 1, CMaterialDlg::OnColorchangedColormapSpecular, VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_EMISSIVE, 1, CMaterialDlg::OnColorchangedColormapEmissive, VTS_NONE)
END_EVENTSINK_MAP()


void CMaterialDlg::OnColorchangedColormapAmbient(){
  D3DMATERIAL v = getCurrentValue();
  v.Ambient = getD3DCOLORVALUE(IDC_COLORMAP_AMBIENT);
  setCurrentValue(v);
}


void CMaterialDlg::OnColorchangedColormapDiffuse() {
  D3DMATERIAL v = getCurrentValue();
  v.Diffuse = getD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapSpecular() {
  D3DMATERIAL v = getCurrentValue();
  v.Specular = getD3DCOLORVALUE(IDC_COLORMAP_SPECULAR);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapEmissive() {
  D3DMATERIAL v = getCurrentValue();
  v.Emissive = getD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE);
  setCurrentValue(v);
}
