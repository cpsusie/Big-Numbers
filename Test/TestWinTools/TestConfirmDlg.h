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

    enum { IDD = IDD_TESTCONFIRMDLG_DIALOG };
    CString m_confirmCaption;
    CString m_confirmMessage;


protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    afx_msg void OnButtonTestConfirm();
    afx_msg void OnButtonShowStandardMessageBox();
    DECLARE_MESSAGE_MAP()
};

