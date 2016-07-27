#pragma once

#include <Array.h>
#include <BitSet.h>
#include <LRparser.h>

class CSelectBreakStatesDlg : public CDialog {
public:
    CSelectBreakStatesDlg(const ParserTables &tables, BitSet &stateSet, CWnd* pParent = NULL);
    
    //{{AFX_DATA(CSelectBreakStatesDlg)
    enum { IDD = IDD_DIALOGSTATES };
    CString m_states;
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CSelectBreakStatesDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

private:
  const ParserTables &m_tables;
  BitSet             &m_stateSet;
protected:

    //{{AFX_MSG(CSelectBreakStatesDlg)
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
