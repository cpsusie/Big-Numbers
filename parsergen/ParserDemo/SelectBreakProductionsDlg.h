#pragma once

#include <LRParser.h>
#include <CompactHashMap.h>
class ProductionLine {
public:
  const UINT   m_production;
  const UINT   m_leftSide;
  const String m_text;
  int          m_index;
  bool         m_selected;
  ProductionLine(const ParserTables &tables, UINT production, UINT leftSideLength);
};

class ProductionLineArray : public Array<ProductionLine> {
  void unselectAll();
public:
  ProductionLineArray(const ParserTables &tables);
  void setSelectProductions(const BitSet &productionSet);
  void sortByProduction();
  void sortGroupByLeftSide();
};

class CSelectBreakProductionsDlg : public CDialog {
private:
  BitSet             &m_selectedProductionSet;
  ProductionLineArray m_productionLines;
  BOOL                m_groupByLeftSide;

  void      refillListBox();
  BitSet    getSelectedLines()             const;
  BitSet    getSelectedProductions()       const;
  void      ajourSelectedProductions();
  CListBox *getListBox()                   const;
public:
  CSelectBreakProductionsDlg(const ParserTables &tables, BitSet &selectedProductionSet, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGPRODUCTIONS };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnButtonSelectAll();
  afx_msg void OnButtonClear();
  afx_msg void OnBnClickedRadioSortByProd();
  afx_msg void OnBnClickedRadioGroupByLeftside();
  DECLARE_MESSAGE_MAP()
};

