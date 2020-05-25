#pragma once

#include "GraphArray.h"
#include "DataGraph.h"
#include "FunctionGraph.h"
#include "ParametricGraph.h"
#include "IsoCurveGraph.h"
#include "DiffEquationGraph.h"
#include "CustomFitThread.h"

class CShowGrafView;

class AxisOptions {
public:
  AxisType    m_type;
  COLORREF    m_color;
  bool        m_showValues;
  bool        m_showValueMarks;
  bool        m_showGridLines;
  bool        m_relativeToFirst;
  DataReader *m_reader;
  AxisOptions();
};

class InitialOptions {
public:
  COLORREF          m_backgroundColor;
  AxisOptions       m_axisOptions[2];
  GraphStyle        m_graphStyle;
  TrigonometricMode m_trigoMode;
  bool              m_ignoreErrors;
  bool              m_onePerLine;
  bool              m_rangeSpecified;
  DataRange         m_explicitRange;
  RollingAvg        m_rollingAvg;
  InitialOptions();
};

class CShowGrafDoc : public CDocument {
private:
  GraphArray     m_graphArray;
  InitialOptions m_options;
  String         m_errorMsg;
  TCHAR        **m_argvFileNames;
  void init();
  void initScaleIfSingleGraph();
  bool addInitialDataGraph(const String &name, COLORREF color);
  CShowGrafView *getView() const ;
  CCoordinateSystem &getSystem() const;
protected:
  CShowGrafDoc();
  DECLARE_DYNCREATE(CShowGrafDoc)

public:
  const GraphArray &getGraphArray() const {
    return m_graphArray;
  }
  GraphArray &getGraphArray() {
    return m_graphArray;
  }
  const GraphItem *getSelectedGraphItem() const {
    return m_graphArray.getSelectedGraphItem();
  }
  MoveablePoint *getSelectedPoint() const {
    return m_graphArray.getSelectedPoint();
  }
  DataRange getDataRange() const {
    return getGraphArray().getDataRange();
  }
  DoubleInterval getXInterval() const {
    return getDataRange().getXInterval();
  }
  DoubleInterval getYInterval() const {
    return getDataRange().getYInterval();
  }
  const InitialOptions &getOptions() const {
    return m_options;
  }
  void setTrigoMode(TrigonometricMode mode);
  TrigonometricMode  getTrigoMode() const {
    return m_options.m_trigoMode;
  }
  void setRollingAvg(const RollingAvg &v);
  const RollingAvg &getRollingAvg() const {
    return m_options.m_rollingAvg;
  }
  void addArgvGraphs();
  void addGraphFromFile(    const String &fileName);
  void readDataFile(        const String &fileName);
  void readFunctionFile(    const String &fileName);
  void readParametricFile(  const String &fileName);
  void readIsoFile(         const String &fileName);
  void readDiffEqFile(      const String &fileName);
  void addFunctionGraph(    const FunctionGraphParameters     &param);
  void addParametricGraph(  const ParametricGraphParameters   &param);
  void addIsoCurveGraph(    const IsoCurveGraphParameters     &param);
  void addDiffEquationGraph(const DiffEquationGraphParameters &param);
  bool isOK() const {
    return m_errorMsg.length() == 0;
  }
  const String &getErrorMessage() const {
    return m_errorMsg;
  }
  void clear() {
    m_graphArray.clear();
  }
  // return true if any data-graphs were updated
  bool refreshFiles();
  virtual void Serialize(CArchive& ar);
  virtual ~CShowGrafDoc();
#ifdef _DEBUG
  virtual void AssertValid() const;
  virtual void Dump(CDumpContext& dc) const;
#endif

protected:
  DECLARE_MESSAGE_MAP()
};
