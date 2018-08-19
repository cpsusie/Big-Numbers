#include "stdafx.h"
#include "ColorMapDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CColorMapDlg::CColorMapDlg(const ColorMapData &colorMapData, CWnd *pParent /*=NULL*/) : CDialog(CColorMapDlg::IDD, pParent) {
  m_maxCount     = colorMapData.getMaxCount();
	m_seed         = colorMapData.m_seed;
	m_randomSeed   = colorMapData.m_randomSeed;
}

void CColorMapDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_MAXCOUNT, m_maxCount);
  DDV_MinMaxUInt(pDX, m_maxCount, 10, 1000000);
	DDX_Text(pDX, IDC_EDIT_SEED, m_seed);
	DDX_Check(pDX, IDC_CHECK_RANDOMSEED, m_randomSeed);
}

BEGIN_MESSAGE_MAP(CColorMapDlg, CDialog)
  ON_COMMAND(ID_GOTO_MAXCOUNT   , OnGotoMaxCount    )
  ON_COMMAND(ID_GOTO_SEED           , OnGotoSeed        )
	ON_BN_CLICKED(IDC_CHECK_RANDOMSEED, OnCheckRandomSeed )
END_MESSAGE_MAP()


BOOL CColorMapDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_COLORMAP_ACCELERATOR));

  enableFields();
  OnGotoMaxCount();
  return false;
}

BOOL CColorMapDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CColorMapDlg::OnOK() {
  UpdateData();
  m_colorMapData.m_maxCount     = m_maxCount;
  m_colorMapData.m_randomSeed   = m_randomSeed;
  m_colorMapData.m_seed         = m_seed;
  __super::OnOK();
}

void CColorMapDlg::enableFields() {
  GetDlgItem(IDC_EDIT_SEED)->EnableWindow(!IsDlgButtonChecked(IDC_CHECK_RANDOMSEED));
}

void CColorMapDlg::OnGotoMaxCount() {
  gotoEditBox(this, IDC_EDIT_MAXCOUNT);
}

void CColorMapDlg::OnGotoSeed() {
  gotoEditBox(this, IDC_EDIT_SEED);
}

void CColorMapDlg::OnCheckRandomSeed() {
  enableFields();
}
