#pragma once

#include <PersistentDataTemplate.h>
#include "RollingAvg.h"

typedef enum {
  POINTGRAPH
 ,FUNCTIONGRAPH
 ,PARAMETRICGRAPH
 ,DATAGRAPH
 ,ISOCURVEGRAPH
 ,DIFFEQUATIONGRAPH
} GraphType;

typedef enum {
  GSCURVE
 ,GSPOINT
 ,GSCROSS
} GraphStyle;

class GraphParameters : public PersistentDataTemplate<GraphType> {
private:
  COLORREF    m_color;
  RollingAvg  m_rollingAvg;
  GraphStyle  m_style;
  void getDataFromDocOld(XMLDoc &doc);
public:
  GraphParameters(const String &name, COLORREF color, const RollingAvg &rollingAvg, GraphStyle style);
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

  // return old Rolling Average
  inline RollingAvg setRollingAvg(const RollingAvg &rollingAvg) {
    const RollingAvg old = m_rollingAvg; m_rollingAvg = rollingAvg; return old;
  }
  inline const RollingAvg &getRollingAvg() const {
    return m_rollingAvg;
  }

  void putDataToDoc(  XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);

  static const TCHAR      *graphStyleToStr(GraphStyle style);
  static GraphStyle        graphStyleFromString(const String &s);
  const TCHAR *getGraphStyleStr() const {
    return graphStyleToStr(getGraphStyle());
  }
};
