#pragma once

#include <Math/Polynomial.h>
#include "ExpressionGraph.h"

class CDegreeDlg : public CDialog {
private:
    HACCEL                    m_accelTable;
    CompactArray<CStatic*>    m_infoField;
    CompactArray<DataPoint>   m_data;
    FunctionPlotter          &m_fp;
    DataFit                   m_fit;
    COLORREF                  m_color;

    void setDegree(int degree);
public:
    CDegreeDlg(const Point2DArray &m_pointArray, FunctionPlotter &fp, CWnd* pParent = NULL);

    //{{AFX_DATA(CDegreeDlg)
    enum { IDD = IDD_DEGREE_DIALOG };
    UINT    m_degree;
    //}}AFX_DATA


    //{{AFX_VIRTUAL(CDegreeDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CDegreeDlg)
	afx_msg void OnDeltaposSpinDegree(NMHDR* pNMHDR, LRESULT* pResult);
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
