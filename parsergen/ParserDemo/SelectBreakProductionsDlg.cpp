#include "stdafx.h"
#include "SelectBreakProductionsDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CSelectBreakProductionsDlg::CSelectBreakProductionsDlg(const AbstractParserTables &tables, BitSet &selectedProductionSet, CWnd *pParent)
: CDialog(IDD, pParent)
, m_selectedProductionSet(selectedProductionSet)
, m_productionLines(      tables               )
, m_groupByLeftSide(      FALSE                )
{
  m_productionLines.sortByProduction();
}

void CSelectBreakProductionsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Radio(pDX, IDC_RADIO_SORTBYPROD, m_groupByLeftSide);
}

BEGIN_MESSAGE_MAP(CSelectBreakProductionsDlg, CDialog)
  ON_BN_CLICKED(IDC_BUTTONSELECTALL      , OnButtonSelectAll              )
  ON_BN_CLICKED(IDC_BUTTONCLEAR          , OnButtonClear                  )
  ON_BN_CLICKED(IDC_RADIO_SORTBYPROD     , OnBnClickedRadioSortByProd     )
  ON_BN_CLICKED(IDC_RADIO_GROUPBYLEFTSIDE, OnBnClickedRadioGroupByLeftside)
END_MESSAGE_MAP()

BOOL CSelectBreakProductionsDlg::OnInitDialog() {
  __super::OnInitDialog();
  m_productionLines.setSelectProductions(m_selectedProductionSet);
  refillListBox();
  return FALSE;
}

void CSelectBreakProductionsDlg::refillListBox() {
  CListBox *lb = getListBox();
  lb->ResetContent();
  for(auto it = m_productionLines.getIterator(); it.hasNext();) {
    lb->InsertString(-1, it.next().m_text.cstr());
  }
  for(size_t i = 0; i < m_productionLines.size(); i++) {
    if(m_productionLines[i].m_selected) {
      lb->SetSel((int)i);
    }
  }
  lb->SetFocus();
}

void CSelectBreakProductionsDlg::OnOK() {
  m_selectedProductionSet = getSelectedProductions();
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

void CSelectBreakProductionsDlg::OnBnClickedRadioSortByProd() {
  const BOOL oldValue = m_groupByLeftSide;
  UpdateData();
  if(oldValue == m_groupByLeftSide) return;
  ajourSelectedProductions();
  m_productionLines.sortByProduction();
  refillListBox();
}


void CSelectBreakProductionsDlg::OnBnClickedRadioGroupByLeftside() {
  const BOOL oldValue = m_groupByLeftSide;
  UpdateData();
  if(oldValue == m_groupByLeftSide) return;
  ajourSelectedProductions();
  m_productionLines.sortGroupByLeftSide();
  refillListBox();
}

CListBox *CSelectBreakProductionsDlg::getListBox() const {
  return (CListBox*)GetDlgItem(IDC_PRODUCTIONLIST);
}

void CSelectBreakProductionsDlg::ajourSelectedProductions() {
  m_productionLines.setSelectProductions(getSelectedProductions());
}

BitSet CSelectBreakProductionsDlg::getSelectedProductions() const {
  BitSet selectedLines = getSelectedLines();
  BitSet result(m_productionLines.size());
  for(auto it = selectedLines.getIterator(); it.hasNext();) {
    result.add(m_productionLines[it.next()].m_production);
  }
  return result;
}

void ProductionLineArray::setSelectProductions(const BitSet &productionSet) {
  unselectAll();
  for(auto it = getIterator(); it.hasNext();) {
    ProductionLine &pl = it.next();
    if(productionSet.contains(pl.m_production)) {
      pl.m_selected = true;
    }
  }
}

ProductionLine::ProductionLine(const AbstractParserTables &tables, UINT production, UINT leftSideLength)
: m_production(production)
, m_leftSide(tables.getLeftSymbol(production))
, m_text(format(_T("%3u:%-*.*s -> %s")
               ,production
               ,leftSideLength, leftSideLength, tables.getSymbolName(m_leftSide).cstr()
               ,tables.getRightString(production).cstr()))
, m_selected(false)
, m_index(-1)
{
}

ProductionLineArray::ProductionLineArray(const AbstractParserTables &tables) {
  const UINT       terminalCount  = tables.getTermCount();
  const UINT       symbolCount    = tables.getSymbolCount();
  const UINT       NTCount        = tables.getNTermCount();
  const UINT       prodCount      = tables.getProductionCount();
  Array<BitSet>    productionsUsingLeftSide; // indexed by NT-index (left-side of each production)
                                          // bitSet has capacity prductionCount and contains
                                          // all productions using the Nonterminal corresponding to NtIndex
  productionsUsingLeftSide.setCapacity(NTCount);
  for(UINT ntIndex = 0; ntIndex < NTCount; ntIndex++) {
    productionsUsingLeftSide.add(BitSet(prodCount));
  }
  UINT leftSideLength = 0;
  for(UINT prod = 0; prod < prodCount; prod++) {
    const UINT length = (UINT)tables.getLeftSymbolName(prod).length();
    if(length > leftSideLength) {
      leftSideLength = length;
    }
  }
  for(UINT prod = 0; prod < prodCount; prod++) {
    const UINT NTindex = tables.getLeftSymbol(prod) - terminalCount;
    productionsUsingLeftSide[NTindex].add(prod);
    add(ProductionLine(tables, prod, leftSideLength));
  }
  int indexCounter = 0;
  for(UINT prod = 0; prod < prodCount; prod++) {
    ProductionLine &pl = (*this)[prod];
    if(pl.m_index >= 0) continue;
    const UINT ntIndex = pl.m_leftSide - terminalCount;
    for(auto it = productionsUsingLeftSide[ntIndex].getIterator(); it.hasNext();) {
      (*this)[it.next()].m_index = indexCounter++;
    }
  }
}

void ProductionLineArray::unselectAll() {
  for(auto it = getIterator(); it.hasNext();) {
    it.next().m_selected = false;
  }
}

BitSet CSelectBreakProductionsDlg::getSelectedLines() const {
  CListBox *listBox = getListBox();
  const int count   =  listBox->GetSelCount();
  int      *items   = new int[count];
  listBox->GetSelItems(count, items);
  BitSet result(m_productionLines.size());
  for(int i = 0; i < count; i++) {
    result.add(items[i]);
  }
  delete[] items;
  return result;
}

static int productionLineProdCmp(const ProductionLine &p1, const ProductionLine &p2) {
  return (int)p1.m_production - (int)p2.m_production;
}

void ProductionLineArray::sortByProduction() {
  sort(productionLineProdCmp);
}

static int productionLineIndexCmp(const ProductionLine &p1, const ProductionLine &p2) {
  return p1.m_index - p2.m_index;
}

void ProductionLineArray::sortGroupByLeftSide() {
  sort(productionLineIndexCmp);
}
