#pragma once

#include <LRParser.h>

class ProductionLine {
public:
  int    m_production;
  bool   m_selected;
  int    m_leftSide;
  String m_text;
  ProductionLine(const ParserTables &tables, int production, int leftSideLength, bool selected);
};

class ProductionLineArray : public Array<ProductionLine> {
public:
  ProductionLineArray(const ParserTables &tables, const BitSet &selected);
};

class CSelectBreakProductionsDlg : public CDialog {
private:
  BitSet             &m_prodSet;
  ProductionLineArray m_productionLines;

  CListBox *getListBox();
public:
  CSelectBreakProductionsDlg(const ParserTables &tables, BitSet &prodSet, CWnd *pParent = NULL);

  enum { IDD = IDD_DIALOGPRODUCTIONS };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnButtonSelectAll();
  afx_msg void OnButtonClear();
  DECLARE_MESSAGE_MAP()
};

