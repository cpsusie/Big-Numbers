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

    CMFCColorButton *getColorButton() {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }

    void setDegree(int degree);
    COLORREF getColor() {
      return getColorButton()->GetColor();
    }
    void setColor(COLORREF color) {
      getColorButton()->SetColor(color);
    }
public:
    CDegreeDlg(const Point2DArray &m_pointArray, FunctionPlotter &fp, CWnd *pParent = NULL);

    enum { IDD = IDD_DEGREE_DIALOG };
    UINT    m_degree;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

protected:

	afx_msg void OnDeltaposSpinDegree(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};

