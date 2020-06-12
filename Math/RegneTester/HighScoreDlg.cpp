#include "stdafx.h"
#include "RegneTester.h"
#include "HighScore.h"
#include "HighScoreDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CHighScoreDlg::CHighScoreDlg(int execiszeType, CWnd *pParent /*=NULL*/)
: m_execiszeType(execiszeType)
, CDialog(CHighScoreDlg::IDD, pParent) {
}

void CHighScoreDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TABEXECISE, m_execiszeTab);
    DDX_Control(pDX, IDC_LISTBESTTIME, m_listBestTime);
}

BEGIN_MESSAGE_MAP(CHighScoreDlg, CDialog)
    ON_NOTIFY(TCN_SELCHANGE, IDC_TABEXECISE, OnSelchangeTabExecise)
END_MESSAGE_MAP()

BOOL CHighScoreDlg::OnInitDialog() {
  __super::OnInitDialog();

  CTabCtrl  &tab  = m_execiszeTab;
  tab.InsertItem(0,_T("+"));
  tab.InsertItem(1,_T("-"));
  tab.InsertItem(2,_T("x"));
  tab.InsertItem(3,_T(":"));

  CListCtrl &list = m_listBestTime;
  list.InsertColumn(0,_T("Tabel")    , LVCFMT_LEFT, 100);
  list.InsertColumn(1,_T("Navn")     , LVCFMT_LEFT, 312);
  list.InsertColumn(2,_T("Tid")      , LVCFMT_LEFT, 130);
  list.InsertColumn(3,_T("Tidspunkt"), LVCFMT_LEFT, 145);
  list.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

  const int index = HighScore::execiseTypeToIndex(m_execiszeType);
  tab.SetCurSel(index);
  showHighScore(index);

  list.SetFocus();
  setSelectedIndex(m_listBestTime, 0);
  return TRUE;
}

void CHighScoreDlg::OnSelchangeTabExecise(NMHDR *pNMHDR, LRESULT *pResult) {
  *pResult = 0;
  showHighScore(m_execiszeTab.GetCurSel());
}

void CHighScoreDlg::showHighScore(int index) {
  HighScore highScore;
  CListCtrl &list = m_listBestTime;
  list.DeleteAllItems();
  int row = 0;
  const int et = HighScore::allExeciseTypes[index];
  for(int j = 0; j < 10; j++, row++) {
    const BestTime &t = highScore.getBestTime(et, j);
    addData(list, row, 0, format(_T("%d"), j+1), true);
    addData(list, row, 1, t.isDefined() ? t.m_name                  : EMPTYSTRING);
    addData(list, row, 2, t.isDefined() ? secondsToString(t.m_time) : EMPTYSTRING);
    addData(list, row, 3, t.isDefined() ? t.m_timestamp.toString()  : EMPTYSTRING);
  }
}
