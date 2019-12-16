#include "stdafx.h"
#include <Math.h>
#include <ThreadPool.h>
#include <TimeMeasure.h>
#include "LinePrinterThread.h"

class MeasureResult {
private:
  size_t m_elementCount;
  size_t m_compareCount;
  double m_time;

  double logFactor(double x) const;
  double squareFactor(double x) const;
public:
  MeasureResult(size_t elementCount, size_t compareCount, double time);
  double getLogFactorTime()       const { return logFactor(   m_time);         }
  double getLogFactorCompare()    const { return logFactor(   (double)m_compareCount); }
  double getSquareFactorTime()    const { return squareFactor(m_time);         }
  double getSquareFactorCompare() const { return squareFactor((double)m_compareCount); }
  String toString() const;
};

MeasureResult::MeasureResult(size_t elementCount, size_t compareCount, double time) {
  m_elementCount = elementCount;
  m_compareCount = compareCount;
  m_time         = time;
}

double MeasureResult::logFactor(double x) const {
  return x/(m_elementCount*log(m_elementCount));
}

double MeasureResult::squareFactor(double x) const {
  return x/(m_elementCount*m_elementCount);
}

String MeasureResult::toString() const {
  return format(_T("%7u %-10.3le  %8u  %-15.3le %-19.3le %-18.3le %-22.3le")
               ,(UINT)m_elementCount
               ,m_time
               ,(UINT)m_compareCount
               ,getLogFactorTime()
               ,getLogFactorCompare()
               ,getSquareFactorTime()
               ,getSquareFactorCompare()
               );
}

class SortMethodAnalyzer : public Runnable, public PropertyChangeListener {
private:
  const SortMethodId  *m_sortMethod;
  InitializeParameters m_parameters;
  LinePrinter         *m_printer;
  bool                 m_windowTerminated;
  MeasureResult measureSort(const InitializeParameters &parameters);
  void          analyzeSortMethod();
public:
  SortMethodAnalyzer(const SortMethodId *sortMethod, const InitializeParameters &parameters);
  ~SortMethodAnalyzer();
  inline bool isWindowTerminated() const {
    return m_windowTerminated;
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
  UINT run();
};

SortMethodAnalyzer::SortMethodAnalyzer(const SortMethodId *sortMethod, const InitializeParameters &parameters) {
  m_windowTerminated = false;
  m_sortMethod = sortMethod;
  m_parameters = parameters;
}

SortMethodAnalyzer::~SortMethodAnalyzer() {
  while(!isWindowTerminated()) {
    m_printer->terminate();
    Sleep(30);
  }
}

void SortMethodAnalyzer::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  switch(id) {
  case LINEPRINTER_VISIBLE:
    break;
  case LINEPRINTER_TERMINATED:
    m_windowTerminated = *(bool*)newValue;
    m_printer = LinePrinter::getNullPrinter();
    break;
  }
}

UINT SortMethodAnalyzer::run() {
  m_printer = CLinePrinterThread::newThread(this);
  analyzeSortMethod();
  return 0;
}

void SortMethodAnalyzer::analyzeSortMethod() {
  Array<MeasureResult> data;
  m_printer->setTitle(format(_T("%s - %s element size:%d")
                            ,m_sortMethod->getName().cstr()
                            ,m_parameters.getInitString().cstr()
                            ,m_parameters.m_elementSize
                            )
                     );
  m_printer->clear();
  m_printer->printf(_T("      n  Time(msec) #compare  time/(n*ln(n))  #compare/(n*ln(n))  time/(n*n)         #compare/(n*n)\n"));

  InitializeParameters param(m_parameters);
  size_t &n = param.m_elementCount;
//  for(n = 40; n < 1000; n = (size_t)(n * 1.1)) {
  for(n = 200; n < 20000; n = (size_t)(n * 1.08)) {
    const MeasureResult result = measureSort(param);
    data.add(result);
    if(isWindowTerminated()) return;
    m_printer->printf(_T("%s\n"), result.toString().cstr());
  }

  double logFactorTimeMean = 0, logFactorCompareMean = 0, squareFactorTimeMean = 0, squareFactorCompareMean = 0;

  for(size_t i = 0; i < data.size(); i++) {
    const MeasureResult &m = data[i];
    logFactorTimeMean       += m.getLogFactorTime();
    logFactorCompareMean    += m.getLogFactorCompare();
    squareFactorTimeMean    += m.getSquareFactorTime();
    squareFactorCompareMean += m.getSquareFactorCompare();
  }
  logFactorTimeMean       /= data.size();
  logFactorCompareMean    /= data.size();
  squareFactorTimeMean    /= data.size();
  squareFactorCompareMean /= data.size();

  if(isWindowTerminated()) return;

  m_printer->printf(_T("_______________________________________________________________________________________________\n"));
  m_printer->printf(_T("Average                       %-15.3le %-19.3le %-18.3le %-22.3le\n")
                    ,logFactorTimeMean,logFactorCompareMean,squareFactorTimeMean,squareFactorCompareMean);

}

class SortTimeFunction : public MeasurableFunction {
  const SortMethodId         *m_sortMethod;
  const InitializeParameters &m_parameters;
  DataArray                   m_copy;
  int                        &m_compareCount;
public:
  SortTimeFunction(const SortMethodId *sortMethod, const InitializeParameters &parameters, int &compareCount);
  void f();
};

SortTimeFunction::SortTimeFunction(const SortMethodId *sortMethod, const InitializeParameters &parameters, int &compareCount)
: m_sortMethod(sortMethod)
, m_parameters(parameters)
, m_compareCount(compareCount)
, m_copy(parameters)
{
  m_copy.init();
}

void SortTimeFunction::f() {
  const int n           = m_copy.size();
  const int elementSize = m_copy.getElementSize();
  DataArray data        = m_copy;
  switch(elementSize) {
  case 1 : m_sortMethod->getMethod()(data.getData(), n, elementSize, CountComparator<BYTE>(          m_compareCount)); break;
  case 2 : m_sortMethod->getMethod()(data.getData(), n, elementSize, CountComparator<unsigned short>(m_compareCount)); break;
  default: m_sortMethod->getMethod()(data.getData(), n, elementSize, CountComparator<unsigned int  >(m_compareCount)); break;
  }
}

MeasureResult SortMethodAnalyzer::measureSort(const InitializeParameters &parameters) {
  int compareCount;
  SortTimeFunction stm(m_sortMethod, parameters, compareCount);
  double timeUsage = measureTime(stm, MEASURE_PROCESSTIME);

  return MeasureResult(parameters.m_elementCount, compareCount, timeUsage*1000);
}

class AnalyzerPool {
private:
  CompactArray<SortMethodAnalyzer*> m_analyzerArray;
public:
  void startAnalyzer(const SortMethodId *sortMethod, const InitializeParameters &parameters);
  ~AnalyzerPool();
  void pruneDone();
};

AnalyzerPool::~AnalyzerPool() {
  for(size_t i = 0; i < m_analyzerArray.size(); i++) {
    delete m_analyzerArray[i];
  }
  m_analyzerArray.clear();
}

void AnalyzerPool::pruneDone() {
  for(size_t i = m_analyzerArray.size(); i--;) {
    SortMethodAnalyzer *sma = m_analyzerArray[i];
    if(sma->isWindowTerminated()) {
      delete sma;
      m_analyzerArray.remove(i);
    }
  }
}

void AnalyzerPool::startAnalyzer(const SortMethodId *sortMethod, const InitializeParameters &parameters) {
  SortMethodAnalyzer *sma = new SortMethodAnalyzer(sortMethod, parameters);
  m_analyzerArray.add(sma);
  ThreadPool::executeNoWait(*sma);
}

void analyzeSortMethod(const SortMethodId *sortMethod, const InitializeParameters &parameters) {
  static AnalyzerPool pool;
  pool.pruneDone();
  pool.startAnalyzer(sortMethod, parameters);
}
