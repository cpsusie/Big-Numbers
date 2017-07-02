#pragma once

#include "ListCtrlChildTrait.h"

class CEditListStringEditor : public CEdit {
private:
  UINT           m_flags;
  void validateValue(const String &v) const; // throw Exception on error
public:
  CEditListStringEditor();
  BOOL Create(CWnd *parent, int id, UINT flags = 0);
  void         setValue(const String &v);
  String       getValue();
  void         setEditable(bool canEdit);
  String       valueToString(const String &v) const;

  USE_LISTCTRL_ACCESS_METHODS

public:
  virtual ~CEditListStringEditor();
  afx_msg void OnSetFocus(CWnd *pOldWnd);
  afx_msg void OnKillFocus(CWnd *pNewWnd);

  DECLARE_MESSAGE_MAP()
};

