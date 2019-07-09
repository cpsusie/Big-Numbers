#pragma once

#include "GraphParameters.h"

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
  void putDataToDoc(XMLDoc &doc);
  void getDataFromDoc(XMLDoc &doc);
  String getTrigonometricModeStr() const {
    return toString(getTrigonometricMode());
  }
};

void setValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode  trigoMode);
void getValue(XMLDoc &doc, XMLNodePtr n, TrigonometricMode &trigoMode);
