#include "stdafx.h"
#include <WinTools.h>
#include "ParserDemo.h"
#include <Tokenizer.h>
#include "SelectBreakStatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSelectBreakStatesDlg::CSelectBreakStatesDlg(const ParserTables &tables, BitSet &stateSet, CWnd* pParent)
: CDialog(CSelectBreakStatesDlg::IDD, pParent), m_tables(tables), m_stateSet(stateSet)
{
  //{{AFX_DATA_INIT(CSelectBreakStatesDlg)
  m_states = _T("");
  //}}AFX_DATA_INIT
}

void CSelectBreakStatesDlg::DoDataExchange(CDataExchange* pDX) {
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CSelectBreakStatesDlg)
  DDX_Text(pDX, IDC_EDITSTATES, m_states);
  //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectBreakStatesDlg, CDialog)
  //{{AFX_MSG_MAP(CSelectBreakStatesDlg)
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelectBreakStatesDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  String s;

  char *delimiter = NULL;
  for(Iterator<unsigned int> it = m_stateSet.getIterator(); it.hasNext(); ) {
    if(delimiter == NULL) {
      delimiter = " ";
    } else {
      s += delimiter;
    }
    s += format("%d", it.next());
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
  for(Tokenizer tok(m_states.GetBuffer(m_states.GetLength()), " ;,:"); tok.hasNext() && ok;) {
    int tmp;
    if(sscanf(tok.next().cstr(), "%d", &tmp) != 1) {
      ok = false;
      MessageBox(format("Please enter state-numbers in the interval [0..%d]", maxStateNumber).cstr());
    } else if(tmp < 0 || tmp > maxStateNumber) {
      ok = false;
      MessageBox(format("%d is an illegal statenumber. Legal interval is [0..%d]", tmp, maxStateNumber).cstr());
    }
    if(ok) {
      stateSet.add(tmp);
    }
  }
  if(ok) {
    m_stateSet = stateSet;
    CDialog::OnOK();
  }
}
