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
	enum { IDD = IDD_DATAGRAPH_DIALOG };
    CListCtrl m_dataList;
    CString   m_name;
    CString   m_style;


    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    afx_msg void OnButtoncolor();
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};
