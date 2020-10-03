#include "stdafx.h"
#include "SelectBreakSymbolsDlg.h"
#include <HashMap.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSelectBreakSymbolsDlg::CSelectBreakSymbolsDlg(const ParserTables &tables, BitSet &symbolSet, CWnd *pParent)
: CDialog(CSelectBreakSymbolsDlg::IDD, pParent), m_tables(tables), m_symbolSet(symbolSet)
{
}

void CSelectBreakSymbolsDlg::DoDataExchange(CDataExchange *pDX) {
	__super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelectBreakSymbolsDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTONCLEAR, OnButtonclear)
END_MESSAGE_MAP()

BOOL CSelectBreakSymbolsDlg::OnInitDialog() {
  __super::OnInitDialog();
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTSYMBOLS);
  UINT s;
  for(s = 0; s < m_tables.getTerminalCount(); s++) {
    m_symbolMap.put(m_tables.getSymbolName(s), SymbolPos(s, m_symbolSet.contains(s)));
  }
  s = 0;
  for(Iterator<Entry<String, SymbolPos> > it = m_symbolMap.getIterator(); it.hasNext();) {
    Entry<String, SymbolPos> &entry = it.next();
    entry.getValue().m_symbolSetIndex = s++;
    lb->InsertString(-1, entry.getKey().cstr());
  }

  for(Iterator<Entry<String, SymbolPos> > it = m_symbolMap.getIterator(); it.hasNext();) {
    SymbolPos &symbolPos = it.next().getValue();
    if(symbolPos.m_selected) {
      lb->SetSel(symbolPos.m_symbolSetIndex);
    }
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
  for(Iterator<Entry<String, SymbolPos> > it = m_symbolMap.getIterator(); it.hasNext();) {
    SymbolPos &pos = it.next().getValue();
    symbolMap.put(pos.m_symbolSetIndex, pos.m_symbolNo);
  }
  m_symbolSet.clear();
  for(int i = 0; i < count; i++) {
    m_symbolSet.add(*symbolMap.get(items[i]));
  }
  delete[] items;
  __super::OnOK();
}

void CSelectBreakSymbolsDlg::OnButtonclear() {
  CListBox* listbox = (CListBox*)GetDlgItem(IDC_LISTSYMBOLS);
  for(int i = 0; i < listbox->GetCount(); i++) {
    listbox->SetSel(i, false);
  }
}
