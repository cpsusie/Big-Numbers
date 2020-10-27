#pragma once

class dataedit : public CDialog {
public:
    dataedit(CWnd *pParent = nullptr);
    int        m_ndata;
    datapoint *m_data;

    enum { IDD = IDD_EDIT_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

