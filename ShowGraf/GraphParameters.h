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
  // Any combination of above HAS_LOWER_LIMIT,LOWER_LIMIT_INCLUSIVE
  //                         ,HAS_UPPER_LIMIT,UPPER_LIMIT_INCLUSIVE
  int             m_flags;
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
private:
  COLORREF   m_color;
  UINT       m_rollAvgSize;
  GraphStyle m_style;
public:
  GraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style);
  // return old color
  inline COLORREF setColor(COLORREF color) {
    const COLORREF oldColor = m_color; m_color = color; return oldColor;
  }
  inline COLORREF getColor() const {
    return m_color;
  }
  // return old GraphStyle
  inline GraphStyle setGraphStyle(GraphStyle graphStyle) {
    const GraphStyle oldStyle = m_style; m_style = graphStyle; return oldStyle;
  }
  inline GraphStyle getGraphStyle() const {
    return m_style;
  }

  // return old Rolling Average Size
  inline UINT setRollAvgSize(UINT size) {
    const UINT oldSize = m_rollAvgSize; m_rollAvgSize = size; return oldSize;
  }
  inline UINT getRollAvgSize() const {
    return m_rollAvgSize;
  }

  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  static const TCHAR      *graphStyleToStr(GraphStyle style);
  static GraphStyle        graphStyleFromString(const String &s);
  const TCHAR *getGraphStyleStr() const {
    return graphStyleToStr(getGraphStyle());
  }
};

class PointGraphParameters : public GraphParameters {
public:
  PointGraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style)
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

class ExprGraphParameters : public GraphParameters {
private:
  TrigonometricMode m_trigonometricMode;
public:
  ExprGraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style, TrigonometricMode trigonometricMode)
    : GraphParameters(name, color, rollAvgSize, style)
    , m_trigonometricMode(trigonometricMode)
  {
  }
  inline TrigonometricMode getTrigonometricMode() const {
    return m_trigonometricMode;
  }
  // Return old trigonometric mode
  inline TrigonometricMode setTrigonometricMode(TrigonometricMode mode) {
    const TrigonometricMode oldMode = m_trigonometricMode; m_trigonometricMode = mode; return oldMode;
  }
  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  static const TCHAR      *trigonometricModeToStr(TrigonometricMode mode);
  static TrigonometricMode trigonometricModeFromString(const String &str);
  const TCHAR *getTrigonometricModeStr() const {
    trigonometricModeToStr(getTrigonometricMode());
  }
};
