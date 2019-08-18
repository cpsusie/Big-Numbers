#include "stdafx.h"
#include <TinyBitSet.h>
#include "HexEditOverwrite.h"

String createStringByOverwriteCurrent(CEdit *e, TCHAR ch) {
  String text = getWindowText(e);
  int pos = getCaretPos(e);
  if (pos < text.length()) {
    text[pos] = ch;
  }
  return text;
}

UINT64 getValueByOverwriteCurrent(CEdit *e, TCHAR ch, String *newString = NULL) {
  String buf, *tmp = newString ? newString : &buf;
  *tmp = createStringByOverwriteCurrent(e, ch);
  UINT64 v = -1;
  _stscanf(tmp->cstr(), _T("%I64x"), &v);
  return v;
}

void overWriteCurrentChar(CDialog *dlg, TCHAR ascii) {
  CEdit *e = (CEdit*)(dlg->GetFocus());
  const int pos = getCaretPos(e);
  setWindowText(e, createStringByOverwriteCurrent(e, ascii));
  setCaretPos(e, pos + 1);
}

void overWriteCurrentChar(CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue) {
  CEdit *e = (CEdit*)(dlg->GetFocus());
  String newString;
  const UINT64 v = getValueByOverwriteCurrent(e, ascii, &newString);
  if((minValue <= v) && (v <= maxValue)) {
    const int pos = getCaretPos(e);
    setWindowText(e, newString);
    setCaretPos(e, pos + 1);
  }
}

class ValidNavigationKeySet : public BitSet64 {
public:
  ValidNavigationKeySet();
};

ValidNavigationKeySet::ValidNavigationKeySet() {
  add(VK_END);
  add(VK_HOME);
  add(VK_LEFT);
  add(VK_RIGHT);
  add(VK_CONTROL);
  add(VK_TAB);
}

bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg, UINT64 minValue, UINT64 maxValue) {
  if(pMsg->message != WM_KEYDOWN) {
    return false;
  }
  static ValidNavigationKeySet validKeySet;
  if(validKeySet.contains((UINT)pMsg->wParam)) {
    return false;
  }
  const wchar_t ch = toAscii((UINT)pMsg->wParam);
  if(!isxdigit(ch)) {
    return true;
  }
  overWriteCurrentChar(dlg, iswlower(ch) ? towupper(ch) : ch, minValue, maxValue);
  return true;
}

