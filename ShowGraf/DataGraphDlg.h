#pragma once

#include "DataGraph.h"

class CDataGraphDlg : public CDialog {
private:
  String m_fullName;

  CComboBox *getStyleCombo() {
    return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
  }
  CMFCColorButton *getColorButton() {
    return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
  }
public:
    CDataGraphDlg(DataGraph &g, CWnd* pParent = NULL);
    DataGraph &m_graph;
	enum { IDD = IDD_DATAGRAPH_DIALOG };
    CListCtrl m_dataList;
    CString   m_name;
    CString   m_style;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
};
