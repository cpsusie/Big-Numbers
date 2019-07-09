#pragma once

#include <PersistentData.h>

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

void setValue(XMLDoc &doc, XMLNodePtr n, GraphStyle         style    );
void getValue(XMLDoc &doc, XMLNodePtr n, GraphStyle        &style    );

class GraphParameters : public PersistentData {
private:
  COLORREF   m_color;
  UINT       m_rollAvgSize;
  GraphStyle m_style;
public:
  GraphParameters(const String &name, COLORREF color, UINT rollAvgSize, GraphStyle style);
  virtual GraphParameters *clone() const = 0;
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
