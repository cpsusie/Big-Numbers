#include "stdafx.h"
#include "ParserDemo.h"
#include "SelectBreakSymbolsDlg.h"
#include <HashMap.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSelectBreakSymbolsDlg::CSelectBreakSymbolsDlg(const ParserTables &tables, BitSet &symbolSet, CWnd* pParent)
: CDialog(CSelectBreakSymbolsDlg::IDD, pParent), m_tables(tables), m_symbolSet(symbolSet)
{
	//{{AFX_DATA_INIT(CSelectBreakSymbolsDlg)
	//}}AFX_DATA_INIT
}

void CSelectBreakSymbolsDlg::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectBreakSymbolsDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSelectBreakSymbolsDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectBreakSymbolsDlg)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, OnButtonclear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CSelectBreakSymbolsDlg::OnInitDialog() {
  CDialog::OnInitDialog();
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTSYMBOLS);

  for(int s = 0; s < m_tables.getTerminalCount(); s++) {
    m_symbolMap.put(m_tables.getSymbolName(s), SymbolPos(s, m_symbolSet.contains(s)));
  }
  s = 0;
  for(Iterator<Entry<String, SymbolPos> > it = m_symbolMap.entrySet().getIterator(); it.hasNext();) {
    Entry<String, SymbolPos> &entry = it.next();
    entry.getValue().m_symbolSetIndex = s++;
    lb->InsertString(-1, entry.getKey().cstr());
  }

  for(it = m_symbolMap.entrySet().getIterator(); it.hasNext();) {
    SymbolPos &symbolPos = it.next().getValue();
    if(symbolPos.m_selected)
      lb->SetSel(symbolPos.m_symbolSetIndex);
  }

  lb->SetFocus();
  return FALSE;
}

void CSelectBreakSymbolsDlg::OnOK() {
  CListBox* listbox = (CListBox*)GetDlgItem(IDC_LISTSYMBOLS);
  int count =  listbox->GetSelCount();
  int *items = new int[count];
  listbox->GetSelItems(count, items);
  IntHashMap<int> symbolMap;
  for(Iterator<Entry<String, SymbolPos> > it = m_symbolMap.entrySet().getIterator(); it.hasNext();) {
    SymbolPos &pos = it.next().getValue();
    symbolMap.put(pos.m_symbolSetIndex, pos.m_symbolNo);
  }
  m_symbolSet.clear();
  for(int i = 0; i < count; i++) {
    m_symbolSet.add(*symbolMap.get(items[i]));
  }
  delete[] items;
  CDialog::OnOK();
}

void CSelectBreakSymbolsDlg::OnButtonclear() {
  CListBox* listbox = (CListBox*)GetDlgItem(IDC_LISTSYMBOLS);
  for(int i = 0; i < listbox->GetCount(); i++) {
    listbox->SetSel(i, false);
  }
}
