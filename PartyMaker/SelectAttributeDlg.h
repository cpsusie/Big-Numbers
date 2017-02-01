#pragma once

class CSelectAttributeDlg : public CDialog {
public:
    CSelectAttributeDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_SELECTATTRIBUTEDIALOG };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    DECLARE_MESSAGE_MAP()
};

