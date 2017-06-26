#include "stdafx.h"
#include "ParserDemo.h"
#include <Tokenizer.h>
#include "SelectBreakStatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSelectBreakStatesDlg::CSelectBreakStatesDlg(const ParserTables &tables, BitSet &stateSet, CWnd *pParent)
: CDialog(CSelectBreakStatesDlg::IDD, pParent), m_tables(tables), m_stateSet(stateSet)
{
  m_states = EMPTYSTRING;
}

void CSelectBreakStatesDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITSTATES, m_states);
}

BEGIN_MESSAGE_MAP(CSelectBreakStatesDlg, CDialog)
END_MESSAGE_MAP()

BOOL CSelectBreakStatesDlg::OnInitDialog() {
  __super::OnInitDialog();
  String s;

  TCHAR *delimiter = NULL;
  for(Iterator<size_t> it = m_stateSet.getIterator(); it.hasNext(); ) {
    if(delimiter == NULL) {
      delimiter = _T(" ");
    } else {
      s += delimiter;
    }
    s += format(_T("%d"), (int)it.next());
  }
  m_states = s.cstr();
  UpdateData(false);
  gotoEditBox(this, IDC_EDITSTATES);
  return FALSE;
}

void CSelectBreakStatesDlg::OnOK() {
  const int maxStateNumber = m_tables.getStateCount() - 1;

  UpdateData();
  BitSet stateSet(m_tables.getStateCount()+1);
  bool ok = true;
  for(Tokenizer tok(m_states.GetBuffer(m_states.GetLength()), _T(" ;,:")); tok.hasNext() && ok;) {
    int tmp;
    if(_stscanf(tok.next().cstr(), _T("%d"), &tmp) != 1) {
      ok = false;
      MessageBox(format(_T("Please enter state-numbers in the interval [0..%d]"), maxStateNumber).cstr());
    } else if(tmp < 0 || tmp > maxStateNumber) {
      ok = false;
      MessageBox(format(_T("%d is an illegal statenumber. Legal interval is [0..%d]"), tmp, maxStateNumber).cstr());
    }
    if(ok) {
      stateSet.add(tmp);
    }
  }
  if(ok) {
    m_stateSet = stateSet;
    __super::OnOK();
  }
}
