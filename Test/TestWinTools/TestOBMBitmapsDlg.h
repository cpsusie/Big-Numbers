#pragma once

typedef struct {
  int          m_id;
  const TCHAR *m_name;
} OBMResource;

class CTestOBMBitmapsDlg : public CDialog {
public:
    CTestOBMBitmapsDlg(CWnd* pParent = NULL);

    //{{AFX_DATA(CTestOBMBitmapsDlg)
    enum { IDD = IDD_TESTOBMBITMAPS_DIALOG };
    //}}AFX_DATA

    //{{AFX_VIRTUAL(CTestOBMBitmapsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:
    void paintOBMBitmaps();
    const CSize paintOBMBitmap(const OBMResource &res, const CPoint &p);
    //{{AFX_MSG(CTestOBMBitmapsDlg)
    afx_msg void OnPaint();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
