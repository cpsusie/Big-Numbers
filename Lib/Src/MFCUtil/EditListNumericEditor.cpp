#include "pch.h"
#include <MFCUtil/EditListNumericEditor.h>

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CEditListNumericEditor::CEditListNumericEditor() {
}

CEditListNumericEditor::~CEditListNumericEditor() {
}


BEGIN_MESSAGE_MAP(CEditListNumericEditor, CEdit)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

#define LF_ISINTEGERTYPE(type) (((type) == LFT_SHORT) || ((type) == LFT_INT))

BOOL CEditListNumericEditor::Create(CWnd *parent, int id, UINT flags, DoubleInterval *legalInterval) {
  DEFINEMETHODNAME;
  const CRect r(10,10,20,20);
  const BOOL ok = CEdit::Create(WS_CHILD | ES_AUTOHSCROLL | ES_RIGHT, r, parent, id);
  if(ok) {
    SetFont(parent->GetFont());
  }
  m_flags        = flags;
  if(LF_NUM_HASINTERVAL(m_flags) && (legalInterval == NULL)) {
    throwInvalidArgumentException(method, _T("Has Lower- or Upper-limit specified, but no legal interval"));
  }
  const ListFieldType type = LF_GETTYPE(flags);
  m_decimalCount           = LF_NUM_GETDECIMALCOUNT(flags);
  if(m_decimalCount > 0) {
    if(isIntegerType()) {
      throwInvalidArgumentException(method, _T("Field with type %s cannot have decimals"), CTableModel::getListFieldTypeName(type).cstr());
    }
  }

  const DoubleInterval maxInterval = CTableModel::getMaxInterval(type);

  if(legalInterval) {
    m_legalInterval = *legalInterval;
    if(!maxInterval.contains(m_legalInterval)) {
      throwException(_T("Illegal interval for type %s"), CTableModel::getListFieldTypeName(type).cstr());
    }
    if(!(m_flags & LF_NUM_HAS_LOWERLIMIT)) {
      m_legalInterval.setFrom(maxInterval.getFrom());
    }
    if(!(m_flags & LF_NUM_HAS_UPPERLIMIT)) {
      m_legalInterval.setTo(maxInterval.getTo());
    }
  } else {
    m_legalInterval = maxInterval;
  }
  return ok;
}

bool CEditListNumericEditor::isIntegerType() const {
  const ListFieldType type = LF_GETTYPE(m_flags);
  return LF_ISINTEGERTYPE(type);
}

void CEditListNumericEditor::setEditable(bool canEdit) {
  ::SendMessage(m_hWnd, EM_SETREADONLY, canEdit ? FALSE : TRUE, 0);
}

void CEditListNumericEditor::setValue(double v) {
  validateValue(v);
  const String str = valueToString(v);
  setWindowText(this, str);
  SetSel(0, (int)str.length());
}

String CEditListNumericEditor::legalIntervalToString() const {
  const TCHAR leftPar  = (m_flags & LF_NUM_LOWERLIMIT_EXCLUSIVE) ? _T(']') : _T('[');
  const TCHAR rightPar = (m_flags & LF_NUM_UPPERLIMIT_EXCLUSIVE) ? _T('[') : _T(']');
  return format(_T("%c%s..%s%c")
               ,leftPar
               ,valueToString(m_legalInterval.getMin()).cstr()
               ,valueToString(m_legalInterval.getMax()).cstr()
               ,rightPar);
}

short CEditListNumericEditor::getShortValue() {
  const double v = getDoubleValue();
  validateValue(v);
  return (short)v;
}

int CEditListNumericEditor::getIntValue() {
  const double v = getDoubleValue();
  validateValue(v);
  return (int)v;
}

float CEditListNumericEditor::getFloatValue() {
  const double v = getDoubleValue();
  validateValue(v);
  return (float)v;
}

double CEditListNumericEditor::getDoubleValue() {
  return stringToValue(getWindowText(this));
}

String CEditListNumericEditor::valueToString(double v) const {
  if(v == 0) {
    return (m_flags & LF_NUM_SHOW_ZERO) ? _T("0") : EMPTYSTRING;
  } else {
    if(isIntegerType()) {
      return format(_T("%.0lf"), v);
    } else {
      return format(_T("%.*lf"), m_decimalCount, v);
    }
  }
}

double CEditListNumericEditor::stringToValue(const String &s) const {
  const String errMsg = _T("Input not numeric");
  String tmp = trim(s);
  double v;
  if(tmp.length() == 0) {
    if(!(m_flags & LF_NUM_SHOW_ZERO)) {
      return 0;
    } else {
      throwException(errMsg);
    }
  } else if(_stscanf(tmp.cstr(), _T("%le"), &v) != 1) {
    throwException(errMsg);
  }
  return v;
}

void CEditListNumericEditor::outOfRangeException(double v) const {
  throwException(_T("%s out of range. Legal interval:%s")
                ,valueToString(v).cstr(), legalIntervalToString().cstr());
}

void CEditListNumericEditor::validateValue(double v) const {
  if(m_flags & LF_NUM_LOWERLIMIT_EXCLUSIVE) {
    if(v <= m_legalInterval.getMin()) {
      outOfRangeException(v);
    }
  } else if(v < m_legalInterval.getMin()) {
    outOfRangeException(v);
  }
  if(m_flags & LF_NUM_UPPERLIMIT_EXCLUSIVE) {
    if(v >= m_legalInterval.getMax()) {
      outOfRangeException(v);
    }
  } else if(v > m_legalInterval.getMax()) {
    outOfRangeException(v);
  }
}

void CEditListNumericEditor::OnSetFocus(CWnd *pOldWnd) {
//  debugLog("  NumericEditor::OnSetFocus() cell:%s\n", getCellStr().cstr());
  CEdit::OnSetFocus(pOldWnd);
}

void CEditListNumericEditor::OnKillFocus(CWnd *pNewWnd) {
//  debugLog("  NumericEditor::OnKillFocus() cell:%s\n", getCellStr().cstr());
  CEdit::OnKillFocus(pNewWnd);
}

