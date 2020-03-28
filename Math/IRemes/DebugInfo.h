#pragma once

#include <Math/Rectangle2D.h>

class Remes;

class CoefWindowData {
private:
  void initData(const Remes &r);
public:
  int m_M, m_K;
  StringArray m_coefStrings;
  CoefWindowData();
  CoefWindowData(const Remes &r);
  CoefWindowData &operator=(const Remes &r);
  void clear();
};

class DebugInfo {
private:
  mutable FastSemaphore   m_lock, m_approxLock;
  CoefWindowData          m_coefWinData;
  ExtremaStringArray      m_extrStrArray;
  String                  m_searchEString, m_remesStateString, m_warning;
  RationalFunction        m_lastApprox;
  double                  m_maxError;
  Point2DArray            m_errorPointArray;
  int                     m_coefVectorIndexForPointArray;
public:
  DebugInfo();
  void   setCoefWinData(       const CoefWindowData &src);
  void   getCoefWinData(             CoefWindowData &dst) const;
  void   setExtremaStringArray(const ExtremaStringArray &src);
  void   getExtremaStringArray(      ExtremaStringArray &dst) const;
  void   setSearchEString(     const String &src);
  void   getSearchEString(           String &dst) const;
  void   setRemesStateString(  const String &src);
  void   getRemesStateString(        String &dst) const;
  void   setWarningString(     const String &src);
  void   getWarningString(           String &dst) const;
  void   setApproximation(     const RationalFunction &src);
  void   getApproximation(           RationalFunction &dst) const;
  bool   isApproxEmpty() const;
  void   setMaxError(double maxError);
  double getMaxError() const;
  void   setPointArray(         const Point2DArray &a, int key);
  void   getPointArray(               Point2DArray &dst);
  int    getPointArrayKey();
  void   clearPointArray();
  void   clear();
};
