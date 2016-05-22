#include "pch.h"
#include <Math/Polynomial.h>

DataFit::DataFit() {
  m_dataFit = NULL;
}

DataFit::~DataFit() {
  delete m_dataFit;
}

DataFit::DataFit(const DataFit &src) {
  m_dataFit = src.m_dataFit->clone();
  m_type    = src.m_type;
  m_ok      = src.m_ok;
}

DataFit &DataFit::operator=(const DataFit &src) {
  delete m_dataFit;
  m_dataFit = src.m_dataFit->clone();
  m_type    = src.m_type;
  m_ok      = src.m_ok;
  return *this;
}

AbstractDataFit *DataFit::clone() const {
  return new DataFit(*this);
}

bool DataFit::solve(FittingType type, const CompactArray<DataPoint> &data, const Real &dydx1, const Real &dydx2) {
  if(m_dataFit != NULL) {
    delete m_dataFit;
    m_dataFit = NULL;
  }
  switch(type) {
  case LSSD                 :
    m_dataFit = new LssdPolynomial(data);
    break;
  case NATURAL_SPLINE       :
  case DYDXMATCH_SPLINE     :
  case IGNOREENDPOINT_SPLINE:
    m_dataFit = new CubicSpline(type,data,dydx1,dydx2);
    break;
  case AKIMA_SPLINE         :
    m_dataFit = new AkimaSpline(data);
    break;
  case FRITSCH_CARLSON      :
    m_dataFit = new FritschCarlsonSpline(data);
    break;
  default:
    throwException(_T("Invalid splinetype:%d"),type);
  }
  m_type = type;
  return m_dataFit->ok();
}

Real AbstractDataFit::ssd(const CompactArray<DataPoint> &data) {
  int n = data.size();
  Real sum = 0;
  for(int i = 0; i < n; i++) {
    const DataPoint &p = data[i];
    Real d = (*this)(p.x) - p.y;
    sum += d*d * p.w;
  }
  return sum;
}

DataFit::operator Polynomial() const {
  if(m_type == LSSD) {
    const LssdPolynomial *tmp = (LssdPolynomial*)m_dataFit;
    return Polynomial(*tmp);
  }
  throwException(_T("DataFit::operator Polynomial():Invalid type. (=%d)"), m_type);
  Vector v(1);
  return Polynomial(v);
}
