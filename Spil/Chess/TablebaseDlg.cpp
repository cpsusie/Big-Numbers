#include "stdafx.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/SelectDirDlg.h>
#include "Chess.h"
#include "TablebaseDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTablebaseDlg::CTablebaseDlg(CWnd* pParent /*=NULL*/) : CDialog(CTablebaseDlg::IDD, pParent) {
  const Options &options = getOptions();
  m_moveCount       = options.getMaxMovesWithoutCaptureOrPawnMove();
	m_defendStrength  = options.getEndGameDefendStrength();
  m_tablebaseMetric = (options.getEndGameTablebaseMetric() == DEPTH_TO_MATE) ? 0 : 1;
	m_depthFormat     = options.getDepthInPlies() ? 1 : 0;
  m_tablebasePath   = options.getEndGameTablebasePath().cstr();
}

void CTablebaseDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX , IDC_EDIT_MOVECOUNT     , m_moveCount      );
    DDX_Text(pDX , IDC_EDIT_DEFENDSTRENGTH, m_defendStrength );
    DDX_Radio(pDX, IDC_RADIO_METRIC_DTM   , m_tablebaseMetric);
    DDX_Radio(pDX, IDC_RADIO_DEPTHINMOVES , m_depthFormat    );
    DDX_Text(pDX , IDC_EDIT_TABLEBASEPATH , m_tablebasePath  );
}

BEGIN_MESSAGE_MAP(CTablebaseDlg, CDialog)
  ON_COMMAND(ID_GOTO_MOVECOUNT                   , OnGotoMoveCount             )
  ON_COMMAND(ID_GOTO_DIR                         , OnGotoPath                  )
  ON_COMMAND(ID_GOTO_DEFENDSTRENGTH              , OnGotoDefendStrength        )
  ON_BN_CLICKED(IDC_BUTTON_PATH                  , OnButtonPath                )
  ON_BN_CLICKED(IDC_BUTTON_DECOMPRESSALL         , OnButtonDecompressAll       )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_MOVECOUNT     , OnDeltaposSpinMoveCount     )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_DEFENDSTRENGTH, OnDeltaposSpinDefendStrength)
END_MESSAGE_MAP()

BOOL CTablebaseDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_TABLEBASE_ACCELERATOR));
  setControlText(IDD, this);

  return FALSE;
}

BOOL CTablebaseDlg::PreTranslateMessage(MSG* pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CTablebaseDlg::OnOK() {
  UpdateData();
  if(m_moveCount <= 0) {
    OnGotoMoveCount();
    MessageBox(loadString(IDS_MSG_0_MOVES_NOT_ALLOWED).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
    return;
  }
  if(m_defendStrength < 0 || m_defendStrength > 100) {
    OnGotoDefendStrength();
    MessageBox(loadString(IDS_MSG_DEFENDSTRENGTH_0_100).cstr(), loadString(IDS_ERRORLABEL).cstr(), MB_OK | MB_ICONWARNING);
    return;
  }

  Options &options = getOptions();
  options.setEndGameDefendStrength(m_defendStrength);
  options.setMaxMovesWithoutCaptureOrPawnMove(m_moveCount);
  options.setEndGameTablebaseMetric(IsDlgButtonChecked(IDC_RADIO_METRIC_DTM) ? DEPTH_TO_MATE : DEPTH_TO_CONVERSION);
  options.setDepthInPlies(IsDlgButtonChecked(IDC_RADIO_DEPTHINPLIES) ? true : false);
  options.setEndGameTablebasePath((LPCTSTR)m_tablebasePath);
  __super::OnOK();
}

void CTablebaseDlg::OnGotoMoveCount() {
  gotoEditBox(this, IDC_EDIT_MOVECOUNT);
}

void CTablebaseDlg::OnGotoDefendStrength() {
  gotoEditBox(this, IDC_EDIT_DEFENDSTRENGTH);
}

void CTablebaseDlg::OnGotoPath() {
  gotoEditBox(this, IDC_EDIT_TABLEBASEPATH);
}

void CTablebaseDlg::OnButtonPath() {
  UpdateData();
  CSelectDirDlg dlg((LPCTSTR)m_tablebasePath);
  if(dlg.DoModal() == IDOK) {
    m_tablebasePath = dlg.getSelectedDir().cstr();
    UpdateData(FALSE);
  }
}

void CTablebaseDlg::OnButtonDecompressAll() {
#ifndef TABLEBASE_BUILDER
  UpdateData();
  const String oldDbPath = EndGameKeyDefinition::getDbPath();
  EndGameKeyDefinition::setDbPath((LPCTSTR)m_tablebasePath);
  try {
    EndGameTablebase::decompressAll();
    EndGameKeyDefinition::setDbPath(oldDbPath);
  }
  catch (...) {
    EndGameKeyDefinition::setDbPath(oldDbPath);
    throw;
  }

#else
  MessageBox(_T("Function not accesible in TABLEBASE_BUILDER-version"), _T("Error"), MB_ICONWARNING);
#endif
}

void CTablebaseDlg::OnDeltaposSpinMoveCount(NMHDR* pNMHDR, LRESULT* pResult) {
  NM_UPDOWN *pNMUpDown = (NM_UPDOWN*)pNMHDR;
  UpdateData();
  const int newValue = minMax(m_moveCount - pNMUpDown->iDelta, 0, 9999);
  m_moveCount = newValue;
  UpdateData(false);
  *pResult = 0;
}

void CTablebaseDlg::OnDeltaposSpinDefendStrength(NMHDR* pNMHDR, LRESULT* pResult) {
  NM_UPDOWN *pNMUpDown = (NM_UPDOWN*)pNMHDR;
  UpdateData();
  const int newValue = minMax((int)m_defendStrength - pNMUpDown->iDelta, 0, 100);
  m_defendStrength = newValue;
  UpdateData(false);
  *pResult = 0;
}
