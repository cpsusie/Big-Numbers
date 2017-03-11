#include "stdafx.h"
#include "AdjustColorDialog.h"
#include "DrawTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CAdjustColorDialog::CAdjustColorDialog(PixRectContainer &container, CWnd *pParent)
: m_container(container)
, CDialog(CAdjustColorDialog::IDD, pParent) {

  m_origImage     = m_container.getImage()->clone(true);
  m_size          = m_origImage->getSize();
  m_initDone      = false;
  m_container.saveDocState();
}

CAdjustColorDialog::~CAdjustColorDialog() {
  delete m_origImage;
}

void CAdjustColorDialog::DoDataExchange(CDataExchange *pDX) {
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COLORMAPCTRL, m_colorMap);
}

BEGIN_MESSAGE_MAP(CAdjustColorDialog, CDialog)
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CAdjustColorDialog, CDialog)
    ON_EVENT(CAdjustColorDialog, IDC_COLORMAPCTRL, 1 /* ColorChanged */, OnColorChangedColorMapCtrl, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CAdjustColorDialog::OnInitDialog() {
  CDialog::OnInitDialog();
  CColormap *map = (CColormap*)GetDlgItem(IDC_COLORMAPCTRL);
  const D3DCOLOR avgColor = m_origImage->getAverageColor();
  map->SetColor(D3DCOLOR2COLORREF(avgColor));
  m_lastColor    = (COLORREF)map->GetColor();
  m_origLSHColor = getLSHColor(COLORREF2D3DCOLOR(m_lastColor));
  m_initDone     = true;
  m_firstChange  = true;
  return TRUE;
}

void CAdjustColorDialog::OnColorChangedColorMapCtrl() {
  if(!m_initDone) {
    return;
  }
  CColormap *map = (CColormap*)GetDlgItem(IDC_COLORMAPCTRL);
  const COLORREF color = (COLORREF)map->GetColor();
  if(color == m_lastColor) {
    return;
  }
  if(m_firstChange) {
    m_firstChange = false;
    m_container.saveDocState();
  }
  const LSHColor lsh = getLSHColor(COLORREF2D3DCOLOR(color));
  const float deltaHue = lsh.m_hue        - m_origLSHColor.m_hue;
  const float deltaSat = lsh.m_saturation - m_origLSHColor.m_saturation;
  const float deltaLum = lsh.m_lumination - m_origLSHColor.m_lumination;
  PixRect *pr = m_origImage->clone(true);
  pr->apply(ChangeLSHOperator(pr,deltaHue,deltaSat,deltaLum));
  m_container.getImage()->rop(ORIGIN,m_size,SRCCOPY,pr,ORIGIN);
  delete pr;
  m_container.repaint();
  m_lastColor = color;
}

void CAdjustColorDialog::OnOK() {
  CDialog::OnOK();
}

void CAdjustColorDialog::OnCancel() {
  m_container.getImage()->rop(ORIGIN, m_size, SRCCOPY, m_origImage, ORIGIN);
  m_container.repaint();
  CDialog::OnCancel();
}
