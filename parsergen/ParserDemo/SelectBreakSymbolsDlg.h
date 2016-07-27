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
    CSelectBreakSymbolsDlg(const ParserTables &tables, BitSet &symbolSet, CWnd* pParent = NULL);

    //{{AFX_DATA(CSelectBreakSymbolsDlg)
    enum { IDD = IDD_DIALOGBREAKSYMBOLS };
    //}}AFX_DATA


    //{{AFX_VIRTUAL(CSelectBreakSymbolsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

private:
  const ParserTables      &m_tables;
  BitSet                  &m_symbolSet;
  StringTreeMap<SymbolPos> m_symbolMap;
protected:

    //{{AFX_MSG(CSelectBreakSymbolsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnButtonclear();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
