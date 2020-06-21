#include "pch.h"
#include <MFCUtil/resource.h>
#include <D3DGraphics/MaterialDlg.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CMaterialDlg::CMaterialDlg(PropertyChangeListener *listener, CWnd *pParent)
: CColormapDialog<D3Material>(IDD, SP_MATERIALPARAMETERS, pParent)
{
  if(listener) {
    addPropertyChangeListener(listener);
  }
}

void CMaterialDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAP_AMBIENT        , m_colormapAmbient   );
  DDX_Control(pDX, IDC_COLORMAP_DIFFUSE        , m_colormapDiffuse   );
  DDX_Control(pDX, IDC_COLORMAP_EMISSIVE       , m_colormapEmissive  );
  DDX_Control(pDX, IDC_COLORMAP_SPECULAR       , m_colormapSpecular  );
}

BEGIN_MESSAGE_MAP(CMaterialDlg, CDialog)
  ON_WM_CLOSE()
  ON_WM_SHOWWINDOW()
  ON_WM_HSCROLL()
  ON_WM_VSCROLL()
  ON_BN_CLICKED(ID_BUTTON_HIDEWINDOW        , OnHideWindow                      )
  ON_BN_CLICKED(IDC_CHECK_SPECULARHIGHLIGHTS, OnBnClickedCheckSpecularHighlights)
  ON_MESSAGE(_ID_MSG_RESETCONTROLS          , OnMsgResetControls                )
END_MESSAGE_MAP()

BOOL CMaterialDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_origName = getWindowText(this);
  const TabOrder tabOrder(this);
  initSlider(IDC_SLIDER_POWER  , 0.1, 400, 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_OPACITY, 1  ,   0, 200);
  tabOrder.restoreTabOrder();
  LoadDynamicLayoutResource(m_lpszTemplateName);
  return TRUE;
}

LRESULT CMaterialDlg::OnMsgResetControls(WPARAM wp, LPARAM lp) {
  resetControls();
  return 0;
}

void CMaterialDlg::resetControls() {
  const bool notifyEnable = setNotifyEnable(false);

  const D3Material &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);

  setNotifyEnable(notifyEnable);
}

void CMaterialDlg::valueToWindow(const D3Material &v) {
  setWindowText(this, format(_T("%s (Material %d)"), m_origName.cstr(), v.getId()));

  setSliderPower(v.Power);
  setSliderOpacity(v.getOpacity());
  setSpecularHighlights(v.m_specularHighlights);
  setD3DCOLORVALUE(IDC_COLORMAP_AMBIENT , v.Ambient );
  setD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE , v.Diffuse );
  setD3DCOLORVALUE(IDC_COLORMAP_SPECULAR, v.Specular);
  setD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE, v.Emissive);
}

void CMaterialDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  D3Material v = getCurrentValue();
  v.Power    = getSliderPower();
  setCurrentValue(v);
  showPower(v.Power);
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMaterialDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  D3Material v  = getCurrentValue();
  setCurrentValue(v.setOpacity(getSliderOpacity()));
  showOpacity(v.getOpacity());
  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CMaterialDlg::OnBnClickedCheckSpecularHighlights() {
  D3Material v = getCurrentValue();
  v.m_specularHighlights = getSpecularHighlights();
  setCurrentValue(v);
}

void CMaterialDlg::setSpecularHighlights(bool v) {
  CheckDlgButton(IDC_CHECK_SPECULARHIGHLIGHTS, v ? BST_CHECKED : BST_UNCHECKED);
}

bool CMaterialDlg::getSpecularHighlights() const {
  return IsDlgButtonChecked(IDC_CHECK_SPECULARHIGHLIGHTS) == BST_CHECKED;
}

void CMaterialDlg::setSliderPower(double v) {
  setSliderValue(IDC_SLIDER_POWER, v);
  showPower(v);
}

float CMaterialDlg::getSliderPower() const {
  return getSliderValue(IDC_SLIDER_POWER);
}

void CMaterialDlg::showPower(double v) {
  setWindowText(this, IDC_STATIC_POWERVALUE, format(_T("%.2f"), v));
}

void CMaterialDlg::setSliderOpacity(double v) {
  setSliderValue(IDC_SLIDER_OPACITY, v);
  showOpacity(v);
}

float CMaterialDlg::getSliderOpacity() const {
  return getSliderValue(IDC_SLIDER_OPACITY);
}

void  CMaterialDlg::showOpacity(double v) {
  setWindowText(this, IDC_STATIC_OPACITYVALUE, format(_T("%.1f%%"), v*100.0));
}

void CMaterialDlg::OnHideWindow() {
  ShowWindow(SW_HIDE);
}

void CMaterialDlg::OnClose() {
  OnHideWindow();
}

void CMaterialDlg::OnCancel() {
  const D3Material &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);
}

BEGIN_EVENTSINK_MAP(CMaterialDlg, CColormapDialog)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_AMBIENT , 1, OnColorchangedColormapAmbient , VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_DIFFUSE , 1, OnColorchangedColormapDiffuse , VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_SPECULAR, 1, OnColorchangedColormapSpecular, VTS_NONE)
  ON_EVENT(CMaterialDlg, IDC_COLORMAP_EMISSIVE, 1, OnColorchangedColormapEmissive, VTS_NONE)
END_EVENTSINK_MAP()


void CMaterialDlg::OnColorchangedColormapAmbient(){
  D3Material v = getCurrentValue();
  v.Ambient = getD3DCOLORVALUE(IDC_COLORMAP_AMBIENT);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapDiffuse() {
  D3Material v  = getCurrentValue();
  v.Diffuse   = getD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE);
  v.setOpacity(getSliderOpacity());
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapSpecular() {
  D3Material v = getCurrentValue();
  v.Specular = getD3DCOLORVALUE(IDC_COLORMAP_SPECULAR);
  setCurrentValue(v);
}

void CMaterialDlg::OnColorchangedColormapEmissive() {
  D3Material v = getCurrentValue();
  v.Emissive = getD3DCOLORVALUE(IDC_COLORMAP_EMISSIVE);
  setCurrentValue(v);
}
