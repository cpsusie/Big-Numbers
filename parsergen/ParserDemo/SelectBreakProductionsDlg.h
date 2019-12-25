#pragma once

#include <MFCUtil/LayoutManager.h>
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
  SimpleLayoutManager m_layoutManager;
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
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

