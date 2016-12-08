#pragma once

typedef struct {
  int          m_id;
  const TCHAR *m_name;
} OBMResource;

class CTestOBMBitmapsDlg : public CDialog {
public:
    CTestOBMBitmapsDlg(CWnd* pParent = NULL);

    enum { IDD = IDD_TESTOBMBITMAPS_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    void paintOBMBitmaps();
    const CSize paintOBMBitmap(const OBMResource &res, const CPoint &p);
    afx_msg void OnPaint();
    DECLARE_MESSAGE_MAP()
};

