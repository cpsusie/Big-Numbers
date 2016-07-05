#pragma once

#include "DataGraph.h"

class CDataGraphDlg : public CDialog {
private:
  String m_fullName;
  CComboBox *CDataGraphDlg::getStyleCombo();
public:
    CDataGraphDlg(DataGraph &g, CWnd* pParent = NULL);
    DataGraph &m_graph;
    COLORREF   m_color;
    //{{AFX_DATA(CDataGraphDlg)
	enum { IDD = IDD_DATAGRAPH_DIALOG };
    CListCtrl m_dataList;
    CString   m_name;
    CString   m_style;
	//}}AFX_DATA


    //{{AFX_VIRTUAL(CDataGraphDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    //{{AFX_MSG(CDataGraphDlg)
    afx_msg void OnButtoncolor();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
