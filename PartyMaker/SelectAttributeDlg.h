#pragma once

class CSelectAttributeDlg : public CDialog {
public:
    CSelectAttributeDlg(CWnd *pParent = NULL);

    enum { IDD = IDD_SELECTATTRIBUTEDIALOG };


protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

    DECLARE_MESSAGE_MAP()
};

