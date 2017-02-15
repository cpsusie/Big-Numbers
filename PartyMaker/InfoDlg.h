#pragma once

class CInfoDlg : public CDialog {
private:
  SimpleLayoutManager  m_layoutManager;
  const AttributeArray m_attributes;
public:
  CInfoDlg(const MediaFile &mediaFile, CWnd *pParent = NULL);

  enum { IDD = IDD_INFODIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    DECLARE_MESSAGE_MAP()
};
