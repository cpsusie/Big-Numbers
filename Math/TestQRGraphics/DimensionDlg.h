#pragma once

class DimensionDlg : public CDialog {
public:
    DimensionDlg(int dimension, CWnd *pParent = NULL);

    enum { IDD = IDD_DIMENSIONDIALOG };
    UINT    m_dimension;

    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};
