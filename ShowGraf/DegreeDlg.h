#pragma once

#include <Math/Polynomial.h>
#include "FunctionGraph.h"

class CDegreeDlg : public CDialog {
private:
    HACCEL                    m_accelTable;
    CompactArray<CStatic*>    m_infoField;
    CompactArray<DataPoint>   m_data;
    FunctionPlotter          &m_fp;
    DataFit                   m_fit;

    CMFCColorButton *getColorButton() const {
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
    CDegreeDlg(const Point2DArray &m_pointArray, FunctionPlotter &fp, CWnd *pParent = nullptr);

    enum { IDD = IDD_DEGREE_DIALOG };
    UINT    m_degree;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnDeltaposSpinDegree(NMHDR *pNMHDR, LRESULT *pResult);
    DECLARE_MESSAGE_MAP()
};

