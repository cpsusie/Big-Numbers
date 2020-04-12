#include "pch.h"
#include <MFCUtil/SliderCtrlWithTransformation.h>

#define ASSERTCREATED() assertCreated(__TFUNCTION__)

CSliderCtrlWithTransformation::CSliderCtrlWithTransformation() {
  init();
}

CSliderCtrlWithTransformation::~CSliderCtrlWithTransformation() {
  cleanup();
}

void CSliderCtrlWithTransformation::substituteControl(CWnd *parent, int id, DoubleInterval &interval, UINT steps, IntervalScale type) {
  DEFINEMETHODNAME;
  CSliderCtrl *ctrl = (CSliderCtrl*)parent->GetDlgItem(id);
  if(ctrl == NULL) {
    showError(_T("%s:Control %d not found"), method, id);
    return;
  }
  const int   style   = ctrl->GetStyle();
  const int   exStyle = ctrl->GetExStyle();
  const CRect rect    = getWindowRect(ctrl);
  ctrl->DestroyWindow();
  if(!Create(style, rect, parent, id)) {
    showError(_T("%s:Create failed"), method);
    return;
  }
  ModifyStyleEx(0, exStyle);
  setTransformation(interval, steps, type);
}

void CSliderCtrlWithTransformation::init() {
  m_tr = NULL;
}

void CSliderCtrlWithTransformation::cleanup() {
  SAFEDELETE(m_tr);
  init();
}

void CSliderCtrlWithTransformation::assertCreated(const TCHAR *method) const {
  if(m_tr == NULL) {
    throwException(_T("%s:Not initialized"), method);
  }
}

void CSliderCtrlWithTransformation::setTransformation(DoubleInterval &interval, UINT steps, IntervalScale type) {
  DEFINEMETHODNAME;
  const DoubleInterval toInterval = stepsToToInterval(steps);
  cleanup();
  switch(type) {
  case LINEAR             :
    m_tr = new LinearTransformation(interval, toInterval); TRACE_NEW(m_tr);
    break;
  case LOGARITHMIC        :
    m_tr = new LogarithmicTransformation(interval, toInterval); TRACE_NEW(m_tr);
    break;
  case NORMAL_DISTRIBUTION:
    m_tr = new NormalDistributionTransformation(interval, toInterval); TRACE_NEW(m_tr);
    break;
  default:
    throwInvalidArgumentException(method, _T("type=%d"), type);
  }
}

IntervalScale CSliderCtrlWithTransformation::getTransformationType() const {
  ASSERTCREATED();
  return m_tr->getScale();
}

const DoubleInterval &CSliderCtrlWithTransformation::getInterval() const {
  ASSERTCREATED();
  return m_tr->getFromInterval();
}

UINT CSliderCtrlWithTransformation::getSteps() const {
  int mMin, mMax;
  GetRange(mMin, mMax);
  return mMax - mMin + 1;
}

void CSliderCtrlWithTransformation::setSteps(UINT steps) {
  ASSERTCREATED();
  m_tr->setToInterval(stepsToToInterval(steps));
}

DoubleInterval CSliderCtrlWithTransformation::stepsToToInterval(UINT steps) {
  DEFINEMETHODNAME;
  if(steps < 2 || steps > 10000) {
    throwInvalidArgumentException(method, _T("steps=%u, must be in range [2..10000]"), steps);
  }
  const int minRange = 0;
  const int maxRange = steps-1;
  SetRange(minRange, maxRange);
  return DoubleInterval(minRange, maxRange);
}

double CSliderCtrlWithTransformation::getPos() const {
  ASSERTCREATED();
  const int p = GetPos();
  return m_tr->backwardTransform(p);
}

void CSliderCtrlWithTransformation::setPos(double pos) {
  ASSERTCREATED();
  const DoubleInterval &iv = m_tr->getFromInterval();
  pos = minMax(pos, iv.getFrom(), iv.getTo());
  const int p = (int)m_tr->forwardTransform(pos);
  SetPos(p);
}
