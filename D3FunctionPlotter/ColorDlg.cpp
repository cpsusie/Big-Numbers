#include "stdafx.h"
#include "d3functionplotter.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CColorDlg::CColorDlg(const String &caption, int propertyId, D3DCOLOR color, CWnd *pParent) 
: CColormapDialog<D3DCOLOR>(CColorDlg::IDD, propertyId, pParent)
, m_caption(caption)
{
  setStartValue(color);

}

void CColorDlg::DoDataExchange(CDataExchange *pDX) {
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAP_COLOR, m_color);
}

BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
END_MESSAGE_MAP()

BOOL CColorDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  setWindowText(this, m_caption);
  const D3DCOLOR color = getStartValue();
  setCurrentValue(color);
  setD3DCOLOR(IDC_COLORMAP_COLOR, color);
  putWindowBesideParent();
  setNotifyEnabled(true);
  return TRUE;
}

void CColorDlg::OnColorChangedColormapColor() {
  setCurrentValue(getD3DCOLOR(IDC_COLORMAP_COLOR));
}
BEGIN_EVENTSINK_MAP(CColorDlg, CColormapDialog)
  ON_EVENT(CColorDlg, IDC_COLORMAP_COLOR, 1, CColorDlg::OnColorchangedColormapColor, VTS_NONE)
END_EVENTSINK_MAP()


void CColorDlg::OnColorchangedColormapColor()
{
  // TODO: Add your message handler code here
}
