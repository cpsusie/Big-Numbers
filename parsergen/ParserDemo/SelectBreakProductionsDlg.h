#pragma once

#include <BitSet.h>
#include <LayoutManager.h>
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
  CSelectBreakProductionsDlg(const ParserTables &tables, BitSet &prodSet, CWnd* pParent = NULL);
    
    //{{AFX_DATA(CSelectBreakProductionsDlg)
    enum { IDD = IDD_DIALOGPRODUCTIONS };
    //}}AFX_DATA


    //{{AFX_VIRTUAL(CSelectBreakProductionsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CSelectBreakProductionsDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    afx_msg void OnButtonSelectAll();
    afx_msg void OnButtonClear();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
