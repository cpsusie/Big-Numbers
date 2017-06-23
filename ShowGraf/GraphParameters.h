#pragma once

#include <PersistentData.h>
#include <MFCUtil/Viewport2D.h>
#include <Math/Expression/Expression.h>

typedef enum {
  POINTGRAPH
 ,FUNCTIONGRAPH
 ,PARAMETRICGRAPH
 ,DATAGRAPH
 ,ISOCURVEGRAPH
 ,DIFFEQUATIONGRAPH
} GraphType;

typedef enum {
  GSCURVE,
  GSPOINT,
  GSCROSS
} GraphStyle;

typedef double(*StringConverter)(const TCHAR *);

#define HAS_LOWER_LIMIT       0x1
#define LOWER_LIMIT_INCLUSIVE 0x2
#define HAS_UPPER_LIMIT       0x4
#define UPPER_LIMIT_INCLUSIVE 0x8

class DataReader {
private:
  StringConverter m_convert;
  DoubleInterval  m_legalInterval;
  int             m_flags; // any combination of above attributes
public:
  DataReader(StringConverter convert, double from, double to, int flags);
  double convertString(const TCHAR *s) const;
  static DataReader LinearDataReader, LogarithmicDataReader, NormalDistributionDataReader, DateTimeDataReader;
};


void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode  trigoMode);
void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &trigoMode);
void setValue(XMLDoc &doc, XMLNodePtr n, GraphStyle         style    );
void getValue(XMLDoc &doc, XMLNodePtr n, GraphStyle        &style    );

class GraphParameters : public PersistentData {
protected:
  void setStdValues(XMLDoc &doc, XMLNodePtr n);
  void getStdValues(XMLDoc &doc, XMLNodePtr n);
public:
  COLORREF   m_color;
  int        m_rollAvgSize;
  GraphStyle m_style;
  GraphParameters(const String &name, COLORREF color, int rollAvgSize, GraphStyle style);
  
  static const TCHAR      *graphStyleToString(GraphStyle style);
  static GraphStyle        graphStyleFromString(const String &s);
  static const TCHAR      *trigonometricModeToString(TrigonometricMode mode);
  static TrigonometricMode trigonometricModeFromString(const String &str);
};

class PointGraphParameters : public GraphParameters {
public:
  PointGraphParameters(const String &name, COLORREF color, int rollAvgSize, GraphStyle style)
    : GraphParameters(name, color, rollAvgSize, style)
  {
  }
  void putDataToDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  void getDataFromDoc(XMLDoc &doc) {
    throwUnsupportedOperationException(__TFUNCTION__);
  }
  int getType() const {
    return POINTGRAPH;
  }
};
