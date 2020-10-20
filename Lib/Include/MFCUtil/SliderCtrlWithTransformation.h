#pragma once

#include "afxcmn.h"
#include <Math/IntervalTransformation.h>

class CSliderCtrlWithTransformation : public CSliderCtrl {
private:
  IntervalTransformation *m_tr;
  void init();
  void cleanup();
  void assertCreated(const TCHAR *method) const;
  DoubleInterval stepsToToInterval(UINT steps);
public:
  CSliderCtrlWithTransformation();
  virtual ~CSliderCtrlWithTransformation();
  void                  substituteControl(CWnd *parent, int id, DoubleInterval &interval, UINT steps=4096, IntervalScale type=LINEAR);
  void                  setTransformation(DoubleInterval &interval, UINT steps, IntervalScale type);
  IntervalScale         getTransformationType() const;
  const DoubleInterval &getInterval() const;
  UINT                  getSteps() const;
  void                  setSteps(UINT steps);
  double                getPos() const;
  void                  setPos(double pos);
};
