#pragma once

#include "MBRealCalculator.h"

class CJuliaDlg;

class JuliaCalculatorJob : public SafeRunnable {
private:
  CJuliaDlg &m_dlg;
  bool       m_killed;
public:
  JuliaCalculatorJob(CJuliaDlg &dlg);
  ~JuliaCalculatorJob();
  void kill();
  UINT safeRun();
};

class CJuliaDlg : public CDialog {
  const RealPoint2D             m_point;
  RealRectangleTransformation   m_transform;
  HRGN                          m_imageRGN;
  JuliaCalculatorJob           *m_job;
  void adjustToRectangle();
public:
  CJuliaDlg(const RealPoint2D &point, CWnd *pParent = NULL);
 ~CJuliaDlg();

  const RealPoint2D &getC() const {
    return m_point;
  }

  const RealRectangleTransformation &getTransform() const {
    return m_transform;
  }
  enum { IDD = IDD_JULIA_DIALOG };

  virtual BOOL OnInitDialog();
  DECLARE_MESSAGE_MAP()
};
