#pragma once

class CNameDlg : public CDialog {
private:
  CBitmap m_bitmap;

public:
    CNameDlg(CWnd *pParent = NULL);

    String getName() const {
      return (LPCTSTR)m_name;
    }
    enum { IDD = IDD_NAME_DIALOG };
    CString m_name;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnCancel();
    DECLARE_MESSAGE_MAP()
};

