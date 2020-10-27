#pragma once

class CSelectOpeningDlg : public CDialog {
private:
    const StringArray m_names;
    int               m_selectedIndex;

public:
    CSelectOpeningDlg(const StringArray &names, CWnd *pParent = nullptr);
    const TCHAR *getSelectedName() const;
    int getSelectedIndex() const {
      return m_selectedIndex;
    }
    enum { IDD = IDD_SELECTOPENING_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnDblclkNamelist();
    DECLARE_MESSAGE_MAP()
};

