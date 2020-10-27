#pragma once

#include <Math/RectangleTransformation.h>

class CIntervalDlg : public CDialog {
private:
  RectangleTransformation m_tr;
  HACCEL                  m_accelTable;
public:
  CIntervalDlg(const RectangleTransformation &tr, CWnd *pParent = nullptr);
  Rectangle2D getDataRange();

  enum { IDD = IDR_INTERVAL };
    double  m_maxx;
    double  m_maxy;
    double  m_minx;
    double  m_miny;

public:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    DECLARE_MESSAGE_MAP()
};
