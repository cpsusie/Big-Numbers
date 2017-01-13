#pragma once

#include "ListCtrlChildTrait.h"

class CEditListStringCombo : public CComboBox {
private:
  UINT m_flags;
  void assertIsDropDown(const TCHAR *function) const;
  void assertIsDropList(const TCHAR *function) const;
  static CRect getArrowRect(const CRect &cellRect);
public:
  CEditListStringCombo();
  BOOL Create(CWnd *parent, int id, const StringArray &stringArray, UINT flags = 0);
  void         setEditable(bool canEdit);
  String       getStringValue();       // Used for style LF_STRCOMBO_DROPDOWN
  void         setStringValue(const String &value);
  int          getIntValue();
  void         setIntValue(int value); // Used for style LF_STRCOMBO_DROPLIST
  bool         useStringValue() const {
    return (m_flags & LF_STRCOMBO_DROPDOWN) != 0;
  }
  String       getListString(int index) const;
  static CSize getComboArrowSize() {
    return CSize(17,17);
  }
  static void paintComboBoxArrow(HDC hdc, const CRect &rect, bool enabled);

  USE_LISTCTRL_ACCESS_METHODS

public:
    virtual ~CEditListStringCombo();

protected:
    afx_msg void OnSetFocus();
    afx_msg void OnKillFocus();
    afx_msg void OnDropDown();
    afx_msg void OnCloseUp();

  DECLARE_MESSAGE_MAP()
};

