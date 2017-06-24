#pragma once

#include "FunctionFitter.h"

typedef enum {
  CUSTOM_FIT
 ,POLYNOMIAL_FIT
} FitThreadType;

class CCustomFitThread : public CWinThread {
  DECLARE_DYNCREATE(CCustomFitThread)
protected:
  CCustomFitThread();           // protected constructor used by dynamic creation

public:
  FitThreadType        m_type;
  DoubleInterval       m_range;
  CString              m_expr;
  Point2DArray         m_pointArray;
  FunctionPlotter     *m_fp;

  virtual BOOL InitInstance();
  virtual int ExitInstance();
  virtual ~CCustomFitThread();

  DECLARE_MESSAGE_MAP()
};

CCustomFitThread *startFitThread(FitThreadType type, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, const CString &expr = _T(""));
