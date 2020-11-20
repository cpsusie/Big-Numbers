#pragma once

#include <BitSet.h>
#include <TreeMap.h>
#include <LRParser.h>

class SymbolPos {
public:
  int  m_symbolNo;
  int  m_symbolSetIndex;
  bool m_selected;
  SymbolPos(int symbolNo, bool selected) { m_symbolNo = symbolNo; m_symbolSetIndex = -1; m_selected = selected; }
};

class CSelectBreakSymbolsDlg : public CDialog {
public:
  CSelectBreakSymbolsDlg(const AbstractParserTables &tables, BitSet &symbolSet, CWnd *pParent = nullptr);

  enum { IDD = IDD_DIALOGBREAKSYMBOLS };

private:
  const AbstractParserTables &m_tables;
  BitSet                     &m_symbolSet;
  StringTreeMap<SymbolPos>    m_symbolMap;
protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnButtonclear();
  DECLARE_MESSAGE_MAP()
};
