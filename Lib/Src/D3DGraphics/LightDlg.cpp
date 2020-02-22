#include "pch.h"
#include <MFCUtil/resource.h>
#include <D3DGraphics/LightDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CLightDlg::CLightDlg(PropertyChangeListener *listener, CWnd *pParent)
: CColormapDialog<D3Light>(IDD, SP_LIGHTPARAMETERS, pParent)
{
  if(listener) {
    addPropertyChangeListener(listener);
  }
}

void CLightDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAP_AMBIENT , m_colormapAmbient );
  DDX_Control(pDX, IDC_COLORMAP_SPECULAR, m_colormapSpecular);
  DDX_Control(pDX, IDC_COLORMAP_DIFFUSE , m_colormapDiffuse );
}

BEGIN_MESSAGE_MAP(CLightDlg, CDialog)
  ON_WM_HSCROLL()
  ON_WM_SHOWWINDOW()
  ON_MESSAGE(_ID_MSG_RESETCONTROLS   , OnMsgResetControls)
  ON_BN_CLICKED(ID_BUTTON_HIDEWINDOW , OnHideWindow      )
  ON_WM_CLOSE()
END_MESSAGE_MAP()

BOOL CLightDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_origName = getWindowText(this);

  initSlider(IDC_SLIDER_RANGE               , 0.001 , 100, 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_CONSTANTATTENUATION , 0.001 , 10 , 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_LINEARATTENUATION   , 0.0001, 10 , 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_QUADRATICATTENUATION, 0.0001, 10 , 200, LOGARITHMIC);
  initSlider(IDC_SLIDER_FALLOFF             , 0, 300);
  return TRUE;
}

LRESULT CLightDlg::OnMsgResetControls(WPARAM wp, LPARAM lp) {
  resetControls();
  return 0;
}

void CLightDlg::resetControls() {
  setNotifyEnable(false);

  const D3Light &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);
  setNotifyEnable(true);
}

void CLightDlg::setCurrentValue(const D3Light &v) {
  CPropertyDialog<D3Light>::setCurrentValue(v);
  ajourSliders(v);
}

D3Light &CLightDlg::copyModifiableValues(D3Light &dst, const D3Light &src) { // static
  dst.Diffuse      = src.Diffuse;
  dst.Specular     = src.Specular;
  dst.Ambient      = src.Ambient;
  dst.Range        = src.Range;
  dst.Attenuation0 = src.Attenuation0;
  dst.Attenuation1 = src.Attenuation1;
  dst.Attenuation2 = src.Attenuation2;
  dst.Falloff      = src.Falloff;
  return dst;
}

void CLightDlg::valueToWindow(const D3Light &v) {
  const TCHAR *lightTypeStr = EMPTYSTRING;
  switch(v.Type) {
  case D3DLIGHT_DIRECTIONAL    : lightTypeStr = _T("Directional"); break;
  case D3DLIGHT_POINT          : lightTypeStr = _T("Point")      ; break;
  case D3DLIGHT_SPOT           : lightTypeStr = _T("Spot")       ; break;
  }
  setWindowText(this, format(_T("%s (%s light[%d]"), m_origName.cstr(), lightTypeStr, v.getIndex()));

  setSliderValue(IDC_SLIDER_RANGE               , v.Range        );
  setSliderValue(IDC_SLIDER_CONSTANTATTENUATION , v.Attenuation0 );
  setSliderValue(IDC_SLIDER_LINEARATTENUATION   , v.Attenuation1 );
  setSliderValue(IDC_SLIDER_QUADRATICATTENUATION, v.Attenuation2 );
  setSliderValue(IDC_SLIDER_FALLOFF             , v.Falloff      );

  setD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE , v.Diffuse );
  setD3DCOLORVALUE(IDC_COLORMAP_SPECULAR, v.Specular);
  setD3DCOLORVALUE(IDC_COLORMAP_AMBIENT , v.Ambient );

  ajourSliders(v);
}

void CLightDlg::ajourSliders(const D3Light &v) {
  enableSliders(v);
  showSliderValues(v);
}

void CLightDlg::enableSliders(const D3Light &v) {
  const bool isSpot        = v.Type == D3DLIGHT_SPOT;
  const bool isDirectional = v.Type == D3DLIGHT_DIRECTIONAL;

  GetDlgItem(IDC_STATIC_RANGE               )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_SLIDER_RANGE               )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_STATIC_CONSTANTATTENUATION )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_SLIDER_CONSTANTATTENUATION )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_STATIC_LINEARATTENUATION   )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_SLIDER_LINEARATTENUATION   )->EnableWindow(!isDirectional);
  GetDlgItem(IDC_STATIC_QUADRATICATTENUATION)->EnableWindow(!isDirectional);
  GetDlgItem(IDC_SLIDER_QUADRATICATTENUATION)->EnableWindow(!isDirectional);

  GetDlgItem(IDC_STATIC_FALLOFF             )->EnableWindow(isSpot);
  GetDlgItem(IDC_SLIDER_FALLOFF             )->EnableWindow(isSpot);
}

void CLightDlg::showSliderValues(const D3Light &v) {
  setWindowText(this, IDC_STATIC_RANGEVALUE         ,format(_T("%.3f"), v.Range        ));
  setWindowText(this, IDC_STATIC_CONSTANTATTVALUE   ,format(_T("%.3f"), v.Attenuation0 ));
  setWindowText(this, IDC_STATIC_LINEARATTVALUE     ,format(_T("%.3f"), v.Attenuation1 ));
  setWindowText(this, IDC_STATIC_QUADRATICATTVALUE  ,format(_T("%.3f"), v.Attenuation2 ));
  setWindowText(this, IDC_STATIC_FALLOFFVALUE       ,format(_T("%.3f"), v.Falloff      ));
}

void CLightDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  const int   ctrlId = pScrollBar->GetDlgCtrlID();
  const float pos    = getSliderValue(ctrlId);
  D3Light     v      = getCurrentValue();
  switch(ctrlId) {
  case IDC_SLIDER_RANGE                 :
    v.Range        = pos;
    break;
  case IDC_SLIDER_CONSTANTATTENUATION   :
    v.Attenuation0 = pos;
    break;
  case IDC_SLIDER_LINEARATTENUATION     :
    v.Attenuation1 = pos;
    break;
  case IDC_SLIDER_QUADRATICATTENUATION  :
    v.Attenuation2 = pos;
    break;
  case IDC_SLIDER_FALLOFF               :
    v.Falloff      = pos;
    break;
  }
  setCurrentValue(v);

  __super::OnHScroll(nSBCode, nPos, pScrollBar);
}

BEGIN_EVENTSINK_MAP(CLightDlg, CDialog)
  ON_EVENT(CLightDlg, IDC_COLORMAP_AMBIENT , 1, CLightDlg::OnColorchangedColormapAmbient, VTS_NONE)
  ON_EVENT(CLightDlg, IDC_COLORMAP_DIFFUSE , 1, CLightDlg::OnColorchangedColormapDiffuse, VTS_NONE)
  ON_EVENT(CLightDlg, IDC_COLORMAP_SPECULAR, 1, CLightDlg::OnColorchangedColormapSpecular, VTS_NONE)
END_EVENTSINK_MAP()


void CLightDlg::OnShowWindow(BOOL bShow, UINT nStatus) {
  PropertyDialog::OnShowWindow(bShow, nStatus);
}

void CLightDlg::OnHideWindow() {
  ShowWindow(SW_HIDE);
}

void CLightDlg::OnClose() {
  OnHideWindow();
}

void CLightDlg::OnCancel() {
  const D3Light &v = getStartValue();
  setCurrentValue(v);
  valueToWindow(v);
}


void CLightDlg::OnColorchangedColormapAmbient() {
  D3Light v = getCurrentValue();
  v.Ambient  = getD3DCOLORVALUE(IDC_COLORMAP_AMBIENT);
  setCurrentValue(v);
}

void CLightDlg::OnColorchangedColormapDiffuse(){
  D3Light v = getCurrentValue();
  v.Diffuse  = getD3DCOLORVALUE(IDC_COLORMAP_DIFFUSE);
  setCurrentValue(v);
}

void CLightDlg::OnColorchangedColormapSpecular() {
  D3Light v = getCurrentValue();
  v.Specular = getD3DCOLORVALUE(IDC_COLORMAP_SPECULAR);
  setCurrentValue(v);
}
