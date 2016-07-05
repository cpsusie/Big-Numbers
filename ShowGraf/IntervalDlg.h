#pragma once

#include <Math/Transformation.h>

class CIntervalDlg : public CDialog {
private:
  RectangleTransformation m_tr;
  HACCEL                  m_accelTable;
public:
  CIntervalDlg(const RectangleTransformation &tr, CWnd* pParent = NULL);
  Rectangle2D getDataRange();

    //{{AFX_DATA(CIntervalDlg)
	enum { IDD = IDD_INTERVAL_DIALOG };
    double  m_maxx;
    double  m_maxy;
    double  m_minx;
    double  m_miny;
	//}}AFX_DATA

    //{{AFX_VIRTUAL(CIntervalDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CIntervalDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
