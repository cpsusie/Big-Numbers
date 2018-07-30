#pragma once

#include "MBCalculator.h"

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
  const MBPoint2D             m_point;
  MBRectangleTransformation   m_transform;
  HRGN                        m_imageRGN;
  JuliaCalculatorThread      *m_thread;
  void adjustToRectangle();
public:
    CJuliaDlg(const MBPoint2D &point, CWnd *pParent = NULL);
   ~CJuliaDlg();

    const MBPoint2D &getC() const {
      return m_point;
    }

    const MBRectangleTransformation &getTransform() const {
      return m_transform;
    }
    enum { IDD = IDD_JULIA_DIALOG };

    virtual BOOL OnInitDialog();
    DECLARE_MESSAGE_MAP()
};

