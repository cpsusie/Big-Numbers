#include "stdafx.h"
#include "SelectBreakProductionsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSelectBreakProductionsDlg::CSelectBreakProductionsDlg(const ParserTables &tables, BitSet &prodSet, CWnd *pParent)
: CDialog(CSelectBreakProductionsDlg::IDD, pParent), m_prodSet(prodSet), m_productionLines(tables, prodSet)
{
}

void CSelectBreakProductionsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSelectBreakProductionsDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTONSELECTALL, OnButtonSelectAll)
  ON_BN_CLICKED(IDC_BUTTONCLEAR    , OnButtonClear    )
END_MESSAGE_MAP()

ProductionLine::ProductionLine(const ParserTables &tables, int production, int leftSideLength, bool selected) {
  m_production = production;
  m_selected   = selected;
  m_leftSide   = tables.getLeftSymbol(production);
  m_text       = format(_T("%3d:%-*.*s -> %s")
                       ,production
                       ,leftSideLength, leftSideLength, tables.getSymbolName(m_leftSide)
                       ,tables.getRightString(production).cstr());
}

static int productionLineCmp(const ProductionLine &p1, const ProductionLine &p2) {
  int c = p1.m_leftSide - p2.m_leftSide;
  if(c) return c;
  return p1.m_production - p2.m_production;
}

ProductionLineArray::ProductionLineArray(const ParserTables &tables, const BitSet &selected) {
  const UINT prodCount      = tables.getProductionCount();
  int        leftSideLength = 0;
  UINT       prod;
  for(prod = 0; prod < tables.getProductionCount(); prod++) {
    int length = (int)_tcsclen(tables.getLeftSymbolName(prod));
    if(length > leftSideLength) {
      leftSideLength = length;
    }
  }
  for(prod = 0; prod < prodCount; prod++) {
    add(ProductionLine(tables, prod, leftSideLength, selected.contains(prod)));
  }
  sort(productionLineCmp);
}

BOOL CSelectBreakProductionsDlg::OnInitDialog() {
  __super::OnInitDialog();
  CListBox *lb = getListBox();

  for(size_t i = 0; i < m_productionLines.size(); i++) {
    lb->InsertString(-1, m_productionLines[i].m_text.cstr());
  }

  for(size_t i = 0; i < m_productionLines.size(); i++) {
    if(m_productionLines[i].m_selected) {
      lb->SetSel((int)i);
    }
  }
  lb->SetFocus();
  return FALSE;
}

CListBox *CSelectBreakProductionsDlg::getListBox() {
  return (CListBox*)GetDlgItem(IDC_PRODUCTIONLIST);
}

void CSelectBreakProductionsDlg::OnOK() {
  CListBox *listBox = getListBox();
  const int count =  listBox->GetSelCount();
  int *items = new int[count];
  listBox->GetSelItems(count, items);
  m_prodSet.clear();
  for(int i = 0; i < count; i++) {
    m_prodSet.add(m_productionLines[items[i]].m_production);
  }
  delete[] items;
  __super::OnOK();
}

void CSelectBreakProductionsDlg::OnButtonSelectAll() {
  CListBox *listBox = getListBox();
  for(int i = 0; i < listBox->GetCount(); i++) {
    listBox->SetSel(i);
  }
}

void CSelectBreakProductionsDlg::OnButtonClear() {
  CListBox *listBox = getListBox();
  for(int i = 0; i < listBox->GetCount(); i++) {
    listBox->SetSel(i, false);
  }
}
