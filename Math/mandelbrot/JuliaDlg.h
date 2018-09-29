#pragma once

#include "MBRealCalculator.h"

class CJuliaDlg;

class JuliaCalculatorThread : public Thread {
private:
  CJuliaDlg &m_dlg;
  bool       m_killed;
public:
  JuliaCalculatorThread(CJuliaDlg &dlg);
  ~JuliaCalculatorThread();
  void kill();
  UINT run();
};

class CJuliaDlg : public CDialog {
  const RealPoint2D             m_point;
  RealRectangleTransformation   m_transform;
  HRGN                          m_imageRGN;
  JuliaCalculatorThread        *m_thread;
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
