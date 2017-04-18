#include "stdafx.h"
#include <Math/Expression/Expression.h>
#include <Math/RungekuttaFehlberg.h>
#include "DiffEquationGraph.h"
#include "DiffEquationSystem.h"

DiffEquationGraph::DiffEquationGraph(const DiffEquationGraphParameters &param) : Graph(new DiffEquationGraphParameters(param)) {
  calculate();
}

DiffEquationGraph::~DiffEquationGraph() {
  clear();
}

void DiffEquationGraph::clear() {
  for (size_t i = 0; i < m_pointGraphArray.size(); i++) {
    delete m_pointGraphArray[i];
  }
  m_pointGraphArray.clear();
  updateDataRange();
}

void DiffEquationGraph::updateDataRange() {
  if (isEmpty()) {
    m_dataRange.setToDefault();
  } else {
    m_dataRange = m_pointGraphArray[0]->getDataRange();
    for(size_t i = 1; i < m_pointGraphArray.size(); i++) {
      m_dataRange += m_pointGraphArray[i]->getDataRange();
    }
  }
}

class DiffEquationHandler : public RungeKuttaFehlbergHandler {
private:
  DiffEquationGraph         &m_graph;
  const CompactIntArray     &m_vgMap;
  CompactArray<PointGraph*> &m_pointGraphArray;
public:
  DiffEquationHandler(DiffEquationGraph &graph, const CompactIntArray &vgMap) 
    : m_graph(graph)
    , m_vgMap(vgMap)
    , m_pointGraphArray(graph.m_pointGraphArray)
  {
  }
  void handleData(const RungeKuttaFehlberg &data);
};

void DiffEquationHandler::handleData(const RungeKuttaFehlberg &deqSolver) {
  const Vector &v = deqSolver.getvalue();
  Real x = v[0];
  for (size_t i = 0; i < m_pointGraphArray.size(); i++) {
    m_pointGraphArray[i]->addPoint(Point2D(x, v[m_vgMap[i]]));
  }
}

void DiffEquationGraph::calculate() {
  clear();
  const DiffEquationGraphParameters &param = *(DiffEquationGraphParameters*)m_param;
  DiffEquationSystem  eq;
  CompilerErrorList   errorList;
  if(!eq.compile(param.m_equationsDescription, errorList)) errorList.throwFirstError();
  DiffEquationSet eqSet = param.getVisibleEquationSet();
  const int graphCount = eqSet.size();
  if (graphCount == 0) {
    throwException(_T("%s:No visible graphs selected"), __TFUNCTION__);
  }
  CompactIntArray vectorGraphMap;
  for (Iterator<UINT> it = eqSet.getIterator(); it.hasNext();) {
    const UINT                     index = it.next();
    const EquationAttributes      &attr  = param.m_attrArray[index];
    const DiffEquationDescription &desc  = param.m_equationsDescription[index];
    vectorGraphMap.add(index+1); // v[0] is x, so add 1 to get mapping right
    m_pointGraphArray.add(new PointGraph(new PointGraphParameters(desc.m_name, attr.m_color, 1, param.m_style)));
  }
  DiffEquationHandler handler(*this, vectorGraphMap);
  RungeKuttaFehlberg(eq, handler).calculate(param.getStartVector(), param.m_interval.getTo(), param.m_eps);
  for(size_t i = 0; i < m_pointGraphArray.size(); i++) {
    m_pointGraphArray[i]->updateDataRange();
  }
  updateDataRange();
}

void DiffEquationGraph::setTrigonometricMode(TrigonometricMode mode) {
  DiffEquationGraphParameters &param = *(DiffEquationGraphParameters*)m_param;
  if(mode != param.m_trigonometricMode) {
    param.m_trigonometricMode = mode;
    calculate();
  }
}

void DiffEquationGraph::paint(CCoordinateSystem &cs) {
  for(size_t i = 0; i < m_pointGraphArray.size(); i++) {
    m_pointGraphArray[i]->paint(cs);
  }
}

double DiffEquationGraph::distance(const CPoint &p, const RectangleTransformation &tr) const {
  if(isEmpty()) return EMPTY_DISTANCE;
  const size_t n    = m_pointGraphArray.size();
  double       mind = m_pointGraphArray[0]->distance(p, tr);
  for(size_t i = 1; (i < n) && (mind > 0); i++) {
    const double d = m_pointGraphArray[i]->distance(p, tr);
    if(d < mind) mind = d;
  }
  return mind;
}

double DiffEquationGraph::getSmallestPositiveX() const {
  if(isEmpty()) return 0;
  const size_t n      = m_pointGraphArray.size();
  double       result = m_pointGraphArray[0]->getSmallestPositiveX();
  for(size_t i = 1; i < n; i++) {
    result = getMinPositive(m_pointGraphArray[i]->getSmallestPositiveX(), result);
  }
  return result;
}

double DiffEquationGraph::getSmallestPositiveY() const {
  if(isEmpty()) return 0;
  const size_t n      = m_pointGraphArray.size();
  double       result = m_pointGraphArray[0]->getSmallestPositiveY();
  for(size_t i = 1; i < n; i++) {
    result = getMinPositive(m_pointGraphArray[i]->getSmallestPositiveY(), result);
  }
  return result;
}

void DiffEquationGraph::setStyle(GraphStyle style) {
  __super::setStyle(style);
  const size_t n = m_pointGraphArray.size();
  for(size_t i = 0; i < n; i++) {
    m_pointGraphArray[i]->setStyle(style);
  }
}

void DiffEquationGraph::setVisible(bool visible) {
  __super::setVisible(visible);
  const size_t n = m_pointGraphArray.size();
  for(size_t i = 0; i < n; i++) {
    m_pointGraphArray[i]->setVisible(visible);
  }
}
