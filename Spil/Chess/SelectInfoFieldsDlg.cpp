#include "stdafx.h"
#include "Chess.h"
#include "SelectInfoFieldsDlg.h"
#include "afxdialogex.h"

IMPLEMENT_DYNAMIC(CSelectInfoFieldsDlg, CDialog)

CSelectInfoFieldsDlg::CSelectInfoFieldsDlg(CWnd *pParent /*=nullptr*/) : CDialog(IDD, pParent)
{
  dataToWindow(getOptions().getengineVerboseFields());
}

CSelectInfoFieldsDlg::~CSelectInfoFieldsDlg() {
}

void CSelectInfoFieldsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Check(pDX, IDC_CHECK_DEPTH, m_depth);
  DDX_Check(pDX, IDC_CHECK_SELDEPTH, m_seldepth);
  DDX_Check(pDX, IDC_CHECK_SCORE, m_score);
  DDX_Check(pDX, IDC_CHECK_TIME, m_time);
  DDX_Check(pDX, IDC_CHECK_NODES, m_nodes);
  DDX_Check(pDX, IDC_CHECK_NODESPS, m_nodesps);
  DDX_Check(pDX, IDC_CHECK_PV, m_pv);
  DDX_Check(pDX, IDC_CHECK_STRING, m_string);
  DDX_Check(pDX, IDC_CHECK_HASHFULL, m_hashfull);
  DDX_Check(pDX, IDC_CHECK_MULTIPV, m_multipv);
  DDX_Check(pDX, IDC_CHECK_CPULOAD, m_cpuLoad);
  DDX_Text( pDX, IDC_EDIT_VARIANTLENGTH, m_variantLength);
  DDV_MinMaxUInt(pDX, m_variantLength, 1, 999);
}

BEGIN_MESSAGE_MAP(CSelectInfoFieldsDlg, CDialog)
  ON_BN_CLICKED(IDC_CHECK_PV, OnClickedCheckPv)
END_MESSAGE_MAP()

BOOL CSelectInfoFieldsDlg::OnInitDialog() {
  __super::OnInitDialog();
  setControlText(IDD, this);
  UpdateData(FALSE);
  enableFields();
  return TRUE;
}

void CSelectInfoFieldsDlg::OnOK() {
  UpdateData();
  EngineVerboseFields evf;
  windowToData(evf);
  getOptions().setEngineVerboseFields(evf);

  __super::OnOK();
}

void CSelectInfoFieldsDlg::OnClickedCheckPv() {
  enableFields();
}

void CSelectInfoFieldsDlg::enableFields() {
  GetDlgItem(IDC_EDIT_VARIANTLENGTH)->EnableWindow(IsDlgButtonChecked(IDC_CHECK_PV)==BST_CHECKED);
}

void CSelectInfoFieldsDlg::windowToData(EngineVerboseFields &evf) {
  evf.m_depth           = m_depth    ? true : false;
  evf.m_seldepth        = m_seldepth ? true : false;
  evf.m_score           = m_score    ? true : false;
  evf.m_time            = m_time     ? true : false;
  evf.m_nodes           = m_nodes    ? true : false;
  evf.m_nodesps         = m_nodesps  ? true : false;
  evf.m_pv              = m_pv       ? true : false;
  evf.m_string          = m_string   ? true : false;
  evf.m_hashfull        = m_hashfull ? true : false;
  evf.m_multipv         = m_multipv  ? true : false;
  evf.m_cpuLoad         = m_cpuLoad  ? true : false;
  evf.m_pvVariantLength = m_variantLength;
}

void CSelectInfoFieldsDlg::dataToWindow(const EngineVerboseFields &evf) {
  m_depth         = evf.m_depth;
  m_seldepth      = evf.m_seldepth;
  m_score         = evf.m_score;
  m_time          = evf.m_time;
  m_nodes         = evf.m_nodes;
  m_nodesps       = evf.m_nodesps;
  m_pv            = evf.m_pv;
  m_string        = evf.m_string;
  m_hashfull      = evf.m_hashfull;
  m_multipv       = evf.m_multipv;
  m_cpuLoad       = evf.m_cpuLoad;
  m_variantLength = evf.m_pvVariantLength;
}
