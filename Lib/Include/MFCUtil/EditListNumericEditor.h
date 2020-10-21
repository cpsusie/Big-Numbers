#pragma once

#include <NumberInterval.h>
#include "ListCtrlChildTrait.h"

class CEditListNumericEditor : public CEdit {
private:
  UINT           m_flags;
  UINT           m_decimalCount;
  DoubleInterval m_legalInterval;

  double stringToValue(const String &s) const;
  void   validateValue(double v) const; // throw Exception on error
  String legalIntervalToString() const;
  void   outOfRangeException(double v) const;
public:
  CEditListNumericEditor();
  BOOL Create(CWnd *parent, int id, UINT flags = 0, DoubleInterval *legalInterval = nullptr);

  void         setValue(double v);
  short        getShortValue();
  int          getIntValue();
  float        getFloatValue();
  double       getDoubleValue();
  void         setEditable(bool canEdit);
  String       valueToString(double v) const;
  bool         isIntegerType() const;

  USE_LISTCTRL_ACCESS_METHODS

public:
  virtual ~CEditListNumericEditor();

  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnKillFocus(CWnd *pNewWnd);
  DECLARE_MESSAGE_MAP()
};

