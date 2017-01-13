#pragma once

#include "ListCtrlChildTrait.h"

class CEditListBooleanEditor : public CButton {
private:
  bool m_checked;
public:
  CEditListBooleanEditor();
  BOOL Create(CWnd *parent, int id);
  void setValue(bool checked);
  bool getValue();
  void setEditable(bool canEdit);
  static void paintCheckBox(HDC hdc, const CPoint &p, bool checked, bool enabled);

  static CSize getCheckBoxSize() {
    return CSize(13,13);
  }
  static CPoint getCheckBoxPosition(const CRect &cellRect);

  USE_LISTCTRL_ACCESS_METHODS

public:
  virtual ~CEditListBooleanEditor();

protected:
  afx_msg void OnClicked();
  afx_msg void OnSetFocus(CWnd *pOldWnd);

  DECLARE_MESSAGE_MAP()
};

