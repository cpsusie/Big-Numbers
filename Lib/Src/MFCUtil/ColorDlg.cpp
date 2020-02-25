#include "pch.h"
#include <MFCUtil/ColorDlg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CColorDlg::CColorDlg(const String &caption, int propertyId, D3DCOLOR color, CWnd *pParent)
: CColormapDialog<D3DCOLOR>(IDD, propertyId, pParent)
, m_caption(caption)
{
  setStartValue(color);

}

void CColorDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, _IDC_COLORMAP_COLOR, m_color);
}

BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
END_MESSAGE_MAP()

BOOL CColorDlg::OnInitDialog() {
  __super::OnInitDialog();
  setWindowText(this, m_caption);
  const D3DCOLOR color = getStartValue();
  setCurrentValue(color);
  setD3DCOLOR(_IDC_COLORMAP_COLOR, color);
  putWindowBesideParent();
  setNotifyEnable(true);
  return TRUE;
}

BEGIN_EVENTSINK_MAP(CColorDlg, CColormapDialog)
  ON_EVENT(CColorDlg, _IDC_COLORMAP_COLOR, 1, CColorDlg::OnColorchangedColormapColor, VTS_NONE)
END_EVENTSINK_MAP()


void CColorDlg::OnColorchangedColormapColor() {
  setCurrentValue(getD3DCOLOR(_IDC_COLORMAP_COLOR));
}
