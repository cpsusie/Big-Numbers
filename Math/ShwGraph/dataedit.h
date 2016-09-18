#pragma once

class dataedit : public CDialog {
public:
    dataedit(CWnd* pParent = NULL);
    int m_ndata;
    datapoint *m_data;


    enum { IDD = IDD_EDIT_DIALOG };


    protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

