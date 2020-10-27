#include "stdafx.h"
#include <TinyBitSet.h>
#include "HexEditOverwrite.h"

String createStringByOverwriteCurrent(CEdit *e, TCHAR ch) {
  String text = getWindowText(e);
  int pos = getCaretPos(e);
  if(pos < text.length()) {
    text[pos] = ch;
  }
  return text;
}

void overWriteCurrentChar(CDialog *dlg, TCHAR ascii) {
  CEdit *e = (CEdit*)(dlg->GetFocus());
  const int pos = getCaretPos(e);
  setWindowText(e, createStringByOverwriteCurrent(e, ascii));
  setCaretPos(e, pos + 1);
}

UINT64 getDecValueByOverwriteCurrent(CEdit *e, TCHAR ch, String *newString = nullptr) {
  String buf, *tmp = newString ? newString : &buf;
  *tmp = createStringByOverwriteCurrent(e, ch);
  UINT64 v = -1;
  _stscanf(tmp->cstr(), _T("%I64d"), &v);
  return v;
}

UINT64 getHexValueByOverwriteCurrent(CEdit *e, TCHAR ch, String *newString = nullptr) {
  String buf, *tmp = newString ? newString : &buf;
  *tmp = createStringByOverwriteCurrent(e, ch);
  UINT64 v = -1;
  _stscanf(tmp->cstr(), _T("%I64x"), &v);
  return v;
}

void overWriteCurrentDecChar(CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue) {
  CEdit *e = (CEdit*)(dlg->GetFocus());
  String newString;
  const UINT64 v = getDecValueByOverwriteCurrent(e, ascii, &newString);
  if ((minValue <= v) && (v <= maxValue)) {
    const int pos = getCaretPos(e);
    setWindowText(e, newString);
    setCaretPos(e, pos + 1);
  }
}

void overWriteCurrentHexChar(CDialog *dlg, TCHAR ascii, UINT64 minValue, UINT64 maxValue) {
  CEdit *e = (CEdit*)(dlg->GetFocus());
  String newString;
  const UINT64 v = getHexValueByOverwriteCurrent(e, ascii, &newString);
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

static const ValidNavigationKeySet validNavigationKeySet;

bool isOverwriteCurrentDecChar(CDialog *dlg, MSG *pMsg, UINT64 minValue, UINT64 maxValue) {
  if ((pMsg->message != WM_KEYDOWN) || validNavigationKeySet.contains((UINT)pMsg->wParam)) {
    return false;
  }
  const wchar_t ch = toAscii((UINT)pMsg->wParam);
  if(isdigit(ch)) {
    overWriteCurrentDecChar(dlg, ch, minValue, maxValue);
  }
  return true;

}

bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg, UINT64 minValue, UINT64 maxValue) {
  if((pMsg->message != WM_KEYDOWN) || validNavigationKeySet.contains((UINT)pMsg->wParam)) {
    return false;
  }
  const wchar_t ch = toAscii((UINT)pMsg->wParam);
  if(isxdigit(ch)) {
    overWriteCurrentHexChar(dlg, iswlower(ch) ? towupper(ch) : ch, minValue, maxValue);
  }
  return true;
}

bool isOverwriteCurrentHexChar(CDialog *dlg, MSG *pMsg) {
  if((pMsg->message != WM_KEYDOWN) || validNavigationKeySet.contains((UINT)pMsg->wParam)) {
    return false;
  }
  const wchar_t ch = toAscii((UINT)pMsg->wParam);
  if(isxdigit(ch)) {
    overWriteCurrentChar(dlg, iswlower(ch) ? towupper(ch) : ch);
  }
  return true;

}

bool isOverwriteCurrentChar(CDialog *dlg, MSG *pMsg, int maxLength) {
  if((pMsg->message != WM_KEYDOWN) || validNavigationKeySet.contains((UINT)pMsg->wParam)) {
    return false;
  }
  const wchar_t ch = toAscii((UINT)pMsg->wParam);
  overWriteCurrentChar(dlg, ch);
  return true;
}
