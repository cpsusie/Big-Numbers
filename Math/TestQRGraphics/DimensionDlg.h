#pragma once

class DimensionDlg : public CDialog {
public:
    DimensionDlg(int dimension, CWnd* pParent = NULL);


    enum { IDD = IDD_DIMENSIONDIALOG };
    UINT    m_dimension;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

