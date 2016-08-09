#include "stdafx.h"
#include <ostream>
#include <Float.h>
#include <Math/MathLib.h>
#include <Random.h>
#include <Math/MathException.h>
#include "FunctionFitter.h"

void FunctionFitter::randomVarList() {
  for(size_t i = 0; i < m_parameterValue.size(); i++) {
    *m_parameterValue[i] = random(0,1);
  }
}

double FunctionFitter::computeSSD() {
  double sum = 0;
  for(size_t i = 0; i < m_data.size(); i++) {
    if(m_x) {
      *m_x = m_data[i].x;
    }
    const double d = evaluate() - m_data[i].y;
    sum += d * d;
  }
  return sqrt(sum);
}

double FunctionFitter::eval(double x) {
  if(m_x) {
    *m_x = x;
  }
  return evaluate();
}

double FunctionFitter::operator()(const double &x) {
  return eval(x);
}

#define EPS 1e-4

static double addEpsilon(double x) {
  return (x == 0) ? EPS : (x * (1+EPS));
}

double FunctionFitter::computeGradient(Vector &gradient) { // return SSD of current parameterset
  const double y0 = computeSSD();
  const int    n  = getParamCount();
  for(int i = 0; i < n; i++) {
    const double x0 = getParamValue(i);
    const double x1 = addEpsilon(x0);
    setParamValue(i,x1);
    const double y1 = computeSSD();
    gradient[i] = (y1 - y0) / (x1 - x0);
    setParamValue(i, x0);
  }
//  cout << "gradient:" << m_gradient << "\n";
//  cout.flush();

  return y0;
}

void FunctionFitter::dumpVarList(FILE *f) {
  const ExpressionVariableArray a = getAllVariables();
  for(size_t i = 0; i < a.size(); i++) {
    const ExpressionVariableWithValue &v = a[i];
    if(v.getName() == "x") {
      continue;
    }
    _ftprintf(f, _T("%s:%le\n"), v.getName().cstr(), v.getValue());
  }
}

String FunctionFitter::toString() const {
  const int n = getParamCount();
  String result;
  for(int i = 0; i < n; i++) {
    const String &name = getParamName(i);
    if(name == "x") {
      continue;
    }
    result += format(_T("%s = %lg;\r\n"), name.cstr(), getParamValue(i));
  }

  return result + Expression::toString();
}

Vector FunctionFitter::getParam() const {
  const int n = getParamCount();
  Vector result(n);
  for(int i = 0; i < n; i++) {
    result[i] = *m_parameterValue[i];
  }
  return result;
}

void FunctionFitter::setParam(const Vector &values) {
  const int n = getParamCount();
  if(values.getDimension() != n) {
    throwMathException(_T("Invalid dimension on Vector. (=%d) number of parameters:%d"), values.getDimension(), m_parameterValue.size());
  }
  for(int i = 0; i < n; i++) {
    setParamValue(i,values[i]);
  }
}

class GradientFunction : public VectorFunction {
private:
  FunctionFitter &m_f;
public:
  GradientFunction(FunctionFitter *f) : m_f(*f) {
  }
  Vector operator()(const Vector &x);
};

Vector GradientFunction::operator()(const Vector &x) {
  m_f.setParam(x);
  Vector g(x.getDimension());
  m_f.computeGradient(g);
  return g;
}

/*
#undef EPS
#define EPS 1e-3
double FunctionFitter::d2SSD_dx2(const Vector &gradient, double &dydx, double x0) {
  Vector origParam = getParam();
  setParam(origParam - gradient * x0);
  double ssd0 = computeSSD();
  double x1 = (x0==0)?(-EPS):(x0*(1-EPS));
  setParam(origParam - gradient * x1);
  double ssd1 = computeSSD();
  double x2 = (x0==0)?EPS:(x0*(1+EPS));
  setParam(origParam - gradient * x2);
  double ssd2 = computeSSD();
  setParam(origParam);
  dydx = (ssd1 - ssd2) / (x1-x2);
  double d2x = (x2-x1);
  return (ssd1 - 2 * ssd0 + ssd2 ) / (d2x*d2x) * 4;
}
*/

void FunctionFitter::plotSSD() {
  String fname = format(_T("c:\\temp\\ssd%05d.dat"),m_iteration);

  FILE *f = fopen(fname,"w");
  Vector gradient(m_parameterValue.size());
  computeGradient(gradient);
  Vector origParam = getParam();
  gradient /= gradient.length();
  for(double x = -100; x < 100; x += 0.1) {
    setParam(origParam - gradient * x);
    _ftprintf(f,_T("%le %le\n"), x, computeSSD());
  }
  fclose(f);
}

void FunctionFitter::stepIteration() {
  if(m_done) {
    return;
  }
  double bestNewSSD   = m_SSD;
  double bestStepSize = 0;
  bool   stepFound    = false;
  Vector origParam    = getParam();

  double gl = m_gradient.length();
  if(gl == 0) {
    m_done = true;
    return;
  }
  m_gradient /= gl;

  for(int i = 0; i < 100; i++) {
    double step = m_stepSize;
    double bestNewSSD = m_SSD;
    for(int NANcount = 0; NANcount < 4; step *= 1.2) {
      setParam(origParam - m_gradient * step);
      double newSSD = computeSSD();
      if(_isnan(newSSD)) {
        NANcount++;
        continue;
      }
      if(newSSD < bestNewSSD) {
        bestNewSSD   = newSSD;
        bestStepSize = step;
        stepFound    = true;
      } else if(newSSD >= bestNewSSD) {
        break;
      }
    }

    if(!stepFound || bestNewSSD >= m_SSD) {
      setParam(origParam);
      m_stepSize /= 2;
      continue;
    }
    break;
  }

  if(!stepFound) {
    m_done = true;
    return;
  }

  setParam(origParam - m_gradient * bestStepSize);
  bestNewSSD   = computeGradient(m_gradient);
  m_SSDDescent = (m_SSD - bestNewSSD) / m_SSD;
  m_SSD        = bestNewSSD;
  m_stepSize   = bestStepSize;
  m_iteration++;
}

void FunctionFitter::solve() {
  while(!m_done) {
    stepIteration();
  }
}

FunctionFitter::FunctionFitter(const String &expr, const Point2DArray &data) : m_data(data) {
  if(m_data.size() == 0) {
    addError(_T("No datapoints to fit"));
    return;
  }
  compile(expr,true);
  if(!isOk()) {
    return;
  }
  m_x = NULL;
  const ExpressionVariableArray va = getAllVariables();
  for(size_t i = 0; i < va.size(); i++) {
    const ExpressionVariableWithValue &v = va[i];
    if(v.getName() == "x") {
      m_x = &getValueRef(v);
      continue;
    }
    if(v.isConstant()) {
      continue;
    }
    m_parameterName.add(v.getName());
    m_parameterValue.add(&getValueRef(v));
  }
  init();
}

void FunctionFitter::init() {
  randomize();
  randomVarList();

  m_SSDDescent = 0;
  m_iteration  = 0;
  m_stepSize   = 1e-5;

  if(m_parameterValue.size() == 0) { // no unknown parameters
    m_SSD      = computeSSD();
    m_done     = true;
  } else {
    m_done     = false;
    m_gradient = Vector(m_parameterValue.size());
    m_SSD      = computeGradient(m_gradient);
  }
}
