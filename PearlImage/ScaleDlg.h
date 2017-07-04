#pragma once

class CScaleDlg : public CDialog {
public:
  CScaleDlg(const ScaleParameters &scale, CWnd *pParent = NULL);

  const ScaleParameters &getScaleParameters() const {
    return m_scale;
  }

private:
  HACCEL m_accelTable;

  ScaleParameters m_scale;
  void setFocusAndMark(int id);
  void setLabels();
  void gotoField(int id);

  enum { IDD = IDD_SCALEDIALOG };
  int     m_toSize;
  double  m_scaleWidth;
  double  m_scaleHeight;

public:
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnOK();
  virtual BOOL OnInitDialog();
  afx_msg void OnRadiopercent();
  afx_msg void OnRadiopixels();
  afx_msg void OnGotoWidth();
  afx_msg void OnGotoHeight();
  DECLARE_MESSAGE_MAP()
};
