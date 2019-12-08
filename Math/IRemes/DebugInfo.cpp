#include "StdAfx.h"
#include "BigRealRemes2.h"
#include "DebugInfo.h"

void CoefWindowData::initData(const Remes &r) {
  m_M = r.getM();
  m_K = r.getK();
  m_coefStrings = r.getCoefficientStringArray();
}

CoefWindowData::CoefWindowData() {
  clear();
}
  
CoefWindowData::CoefWindowData(const Remes &r) {
  initData(r);
}

CoefWindowData &CoefWindowData::operator=(const Remes &r) {
  initData(r);
  return *this;
}

void CoefWindowData::clear() {
  m_M = m_K = 0;
  m_coefStrings.clear();
}

DebugInfo::DebugInfo() : m_coefVectorIndexForPointArray(-1) {
}

void DebugInfo::setCoefWinData(const CoefWindowData &src) {
  m_lock.wait();
  m_coefWinData = src;
  m_lock.notify();
}
void DebugInfo::getCoefWinData(CoefWindowData &dst) const {
  m_lock.wait();
  dst = m_coefWinData;
  m_lock.notify();
}

void DebugInfo::setExtremaStringArray(const ExtremaStringArray &src) {
  m_lock.wait();
  m_extrStrArray = src;
  m_lock.notify();
}
void DebugInfo::getExtremaStringArray(ExtremaStringArray &dst) const {
  m_lock.wait();
  dst = m_extrStrArray;
  m_lock.notify();
}

void DebugInfo::setSearchEString(const String &src) {
  m_lock.wait();
  m_searchEString = src;
  m_lock.notify();
}
void DebugInfo::getSearchEString(String &dst) const {
  m_lock.wait();
  dst = m_searchEString;
  m_lock.notify();
}

void DebugInfo::setRemesStateString(const String &src) {
  m_lock.wait();
  m_remesStateString = src;
  m_lock.notify();
}
void DebugInfo::getRemesStateString(String &dst) const {
  m_lock.wait();
  dst = m_remesStateString;
  m_lock.notify();
}

void DebugInfo::setErrorString(const String &src) {
  m_lock.wait();
  m_error = src;
  m_lock.notify();
}
void DebugInfo::getErrorString(String &dst) const {
  m_lock.wait();
  dst = m_error;
  m_lock.notify();
}

void DebugInfo::setWarningString(const String &src) {
  m_lock.wait();
  m_warning = src;
  m_lock.notify();
}
void DebugInfo::getWarningString(String &dst) const {
  m_lock.wait();
  dst = m_warning;
  m_lock.notify();
}

void DebugInfo::setApproximation(const RationalFunction &src) {
  m_approxLock.wait();
  m_lastApprox = src;
  m_approxLock.notify();
}
void DebugInfo::getApproximation(RationalFunction &dst) const {
  m_approxLock.wait();
  dst = m_lastApprox;
  m_approxLock.notify();
}
bool DebugInfo::isApproxEmpty() const {
  m_approxLock.wait();
  const bool b = m_lastApprox.isEmpty();
  m_approxLock.notify();
  return b;
}

void DebugInfo::setMaxError(double maxError) {
  m_lock.wait();
  m_maxError = maxError;
  m_lock.notify();
}
double DebugInfo::getMaxError() const {
  m_lock.wait();
  const double result = m_maxError;
  m_lock.notify();
  return result;
}

void DebugInfo::setPointArray(const Point2DArray &a, int key) {
  m_lock.wait();
  m_errorPointArray = a;
  m_coefVectorIndexForPointArray = key;
  m_lock.notify();
}
void DebugInfo::getPointArray(Point2DArray &dst) {
  m_lock.wait();
  dst = m_errorPointArray;
  m_lock.notify();
}
int DebugInfo::getPointArrayKey() {
  return m_coefVectorIndexForPointArray;
}

void DebugInfo::clearPointArray() {
  m_lock.wait();
  m_errorPointArray.clear();
  m_coefVectorIndexForPointArray = -1;
  m_lock.notify();
}

void DebugInfo::clear() {
  m_lock.wait();
  m_coefWinData.clear();
  m_extrStrArray.clear();
  m_searchEString    = EMPTYSTRING;
  m_remesStateString = EMPTYSTRING;
  m_warning          = EMPTYSTRING;
  m_error            = EMPTYSTRING;
  m_lastApprox.clear();
  m_maxError = 0;
  m_errorPointArray;
  m_errorPointArray.clear();
  m_coefVectorIndexForPointArray = -1;
  m_lock.notify();
}
