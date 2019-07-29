// ColormapPropPage.cpp : Implementation of the CColormapPropPage property page class.

#include "stdafx.h"
#include <BasicIncludes.h>
#include "Colormap.h"
#include "ColormapPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CColormapPropPage, COlePropertyPage)

// Message map

BEGIN_MESSAGE_MAP(CColormapPropPage, COlePropertyPage)
  ON_BN_CLICKED(IDC_CHECKSUNKEN    , OnCheckSunken      )
  ON_BN_CLICKED(IDC_CHECKBORDER    , OnCheckBorder      )
  ON_BN_CLICKED(IDC_CHECKCLIENTEDGE, OnCheckClientEdge  )
  ON_BN_CLICKED(IDC_CHECKSTATICEDGE, OnCheckStaticEdge  )
  ON_BN_CLICKED(IDC_CHECKMODALFRAME, OnCheckModalFrame  )
  ON_EN_CHANGE( IDC_EDITCAPTION    , OnChangeEditCaption)
  ON_CBN_EDITCHANGE(IDC_COMBOALIGNMENT, OnEditChangeComboAlignment)
  ON_COMMAND(   ID_GOTO_CAPTION    , OnGotoCaption      )
  ON_COMMAND(   ID_GOTO_ALIGNMENT  , OnGotoAlignment    )
END_MESSAGE_MAP()

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CColormapPropPage, "COLORMAP.ColormapPropPage.1",
    0x5ae8d2b0, 0xe7c0, 0x4328, 0xbe, 0x67, 0x8b, 0x43, 0x5d, 0xdc, 0xfc, 0x68)

// CColormapPropPage::CColormapPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CColormapPropPage

BOOL CColormapPropPage::CColormapPropPageFactory::UpdateRegistry(BOOL bRegister)
{
    if (bRegister)
        return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
            m_clsid, IDS_COLORMAP_PPG);
    else
        return AfxOleUnregisterClass(m_clsid, NULL);
}

// CColormapPropPage::CColormapPropPage - Constructor

CColormapPropPage::CColormapPropPage() :
    COlePropertyPage(IDD, IDS_COLORMAP_PPG_CAPTION)
{
  m_sunken     = FALSE;
  m_hasBorder  = FALSE;
  m_caption    = EMPTYSTRING;
  m_clientEdge = FALSE;
  m_staticEdge = FALSE;
  m_modalFrame = FALSE;
  m_textAlignment = -1;
}

// CColormapPropPage::DoDataExchange - Moves data between page and properties

void CColormapPropPage::DoDataExchange(CDataExchange *pDX)
{
  DDP_Check(pDX, IDC_CHECKSUNKEN    , m_sunken, _T("Sunken") );
  DDX_Check(pDX, IDC_CHECKSUNKEN    , m_sunken);
  DDP_Check(pDX, IDC_CHECKBORDER    , m_hasBorder, _T("HasBorder") );
  DDX_Check(pDX, IDC_CHECKBORDER    , m_hasBorder);
  DDP_Text( pDX, IDC_EDITCAPTION    , m_caption, _T("Caption") );
  DDX_Text( pDX, IDC_EDITCAPTION    , m_caption);
  DDP_Check(pDX, IDC_CHECKCLIENTEDGE, m_clientEdge, _T("ClientEdge") );
  DDX_Check(pDX, IDC_CHECKCLIENTEDGE, m_clientEdge);
  DDP_Check(pDX, IDC_CHECKSTATICEDGE, m_staticEdge, _T("StaticEdge") );
  DDX_Check(pDX, IDC_CHECKSTATICEDGE, m_staticEdge);
  DDP_Check(pDX, IDC_CHECKMODALFRAME, m_modalFrame, _T("ModalFrame") );
  DDX_Check(pDX, IDC_CHECKMODALFRAME, m_modalFrame);
  DDX_CBIndex(pDX, IDC_COMBOALIGNMENT, m_textAlignment);
  DDP_CBIndex(pDX, IDC_COMBOALIGNMENT, m_textAlignment, _T("TextAlignment") );

  DDP_PostProcessing(pDX);
}

// CColormapPropPage message handlers
BOOL CColormapPropPage::OnInitDialog() {
  COlePropertyPage::OnInitDialog();

  m_accelTable = LoadAccelerators(AfxGetApp()->m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR));
  UpdateData(FALSE);
  adjustFieldStates();
  GetDlgItem(IDC_CHECKBORDER)->SetFocus();

  return FALSE;
}

BOOL CColormapPropPage::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return COlePropertyPage::PreTranslateMessage(pMsg);
}

void CColormapPropPage::OnCheckSunken() {
  UpdateData();
  adjustFieldStates();
}

void CColormapPropPage::OnCheckBorder() {
  UpdateData();
  adjustFieldStates();
}

void CColormapPropPage::OnCheckClientEdge() {
  UpdateData();
  adjustFieldStates();
}

void CColormapPropPage::OnCheckStaticEdge() {
  UpdateData();
  adjustFieldStates();
}

void CColormapPropPage::OnCheckModalFrame() {
  UpdateData();
  adjustFieldStates();
}

void CColormapPropPage::OnChangeEditCaption() {
  UpdateData();
}

void CColormapPropPage::OnEditChangeComboAlignment() {
  UpdateData();
}

void CColormapPropPage::adjustFieldStates() {
  const BOOL enabled = m_sunken | m_hasBorder | m_clientEdge | m_staticEdge | m_modalFrame;
  //IsDlgButtonChecked(IDC_CHECKBORDER);
  GetDlgItem(IDC_EDITCAPTION  )->EnableWindow(enabled);
  GetDlgItem(IDC_STATICCAPTION)->EnableWindow(enabled);
}

void CColormapPropPage::OnGotoCaption() {
  GetDlgItem(IDC_EDITCAPTION)->SetFocus();
}

void CColormapPropPage::OnGotoAlignment() {
  GetDlgItem(IDC_COMBOALIGNMENT)->SetFocus();
}

