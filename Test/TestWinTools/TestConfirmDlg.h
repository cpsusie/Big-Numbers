#pragma once

class CTestConfirmDlg : public CDialog {
private:
  CComboBox *getConfirmButtonCombo() {
    return (CComboBox*)GetDlgItem(IDC_COMBOCONFIRMBUTTONS);
  }
  CComboBox *getConfirmIconCombo() {
    return (CComboBox*)GetDlgItem(IDC_COMBOCONFIRMICONS);
  }
  int getSelectedConfirmButtons();
  int getSelectedConfirmIcon();
  void initComboItems();
public:
    CTestConfirmDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(CTestConfirmDlg)
    enum { IDD = IDD_TESTCONFIRMDLG_DIALOG };
	CString	m_confirmCaption;
	CString	m_confirmMessage;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(CTestConfirmDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CTestConfirmDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonTestConfirm();
	afx_msg void OnButtonShowStandardMessageBox();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
