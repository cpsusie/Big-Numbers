#include "stdafx.h"

#if !defined(TABLEBASE_BUILDER)

#include <MFCUtil/WinTools.h>
#include "EngineOptionsDlg.h"

EngineOptionControl::EngineOptionControl(CEngineOptionsDlg *parent, const EngineOptionDescription &option) : m_option(option) {

#define STD_STYLES WS_VISIBLE|WS_TABSTOP|WS_CHILD
  const CRect r(10,10,20,20);

  m_ctrlId     = parent->nextCtrlId(option.getType());
  m_spinCtrlId = -1;
  m_column     = -1;
  m_label = m_label2 = NULL;
  m_ctrl  = m_spinCtrl = NULL;
  switch(option.getType()) {
  case OptionTypeCheckbox:
    { CButton *ctrl = new CButton(); TRACE_NEW(ctrl);
      ctrl->Create(option.getLabelName().cstr(), STD_STYLES | BS_AUTOCHECKBOX | BS_LEFTTEXT, r, parent, m_ctrlId);
      m_ctrl = ctrl;
    }
    break;

  case OptionTypeSpin    :
    { m_label  = newStaticLabel(parent, option.getLabelName());
      m_label2 = newStaticLabel(parent, format(_T("(%d-%d)"), option.getMin(), option.getMax()));
      CEdit           *ctrl     = new CEdit();           TRACE_NEW(ctrl    );
      CSpinButtonCtrl *spinCtrl = new CSpinButtonCtrl(); TRACE_NEW(spinCtrl);

      ctrl->Create(STD_STYLES, r, parent, m_ctrlId);
      ctrl->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
      m_spinCtrlId = parent->nextSpinCtrlId();
      spinCtrl->Create(WS_VISIBLE | WS_CHILD | UDS_ALIGNRIGHT | UDS_ARROWKEYS, r, parent, m_spinCtrlId);

      m_ctrl     = ctrl;
      m_spinCtrl = spinCtrl;
    }
    break;

  case OptionTypeCombo   :
    { m_label = newStaticLabel(parent, option.getLabelName());
      CComboBox *ctrl  = new CComboBox(); TRACE_NEW(ctrl);
      ctrl->Create(STD_STYLES | WS_VSCROLL | CBS_DROPDOWN , r, parent, m_ctrlId);
      const StringArray &values = m_option.getComboValues();
      for(size_t i = 0; i < values.size(); i++) {
        ctrl->AddString(values[i].cstr());
      }
      m_ctrl  = ctrl;
    }
    break;

  case OptionTypeString  :
    { m_label = newStaticLabel(parent, option.getLabelName());
      CEdit   *ctrl  = new CEdit(); TRACE_NEW(ctrl);
      ctrl->Create(STD_STYLES | ES_AUTOHSCROLL, r, parent, m_ctrlId);
      ctrl->ModifyStyleEx(0,WS_EX_CLIENTEDGE);
      m_ctrl  = ctrl;
    }
    break;

  case OptionTypeButton  :
    { CButton *ctrl = new CButton(); TRACE_NEW(ctrl);
      ctrl->Create(option.getLabelName().cstr(), STD_STYLES | BS_PUSHBUTTON, r, parent, m_ctrlId);
      ctrl->ModifyStyleEx(0, WS_EX_NOPARENTNOTIFY);
      m_ctrl = ctrl;
    }
    break;

  default                :
    invalidTypeException(option.getType(), option.getName());
    return;
  }
}

CStatic *EngineOptionControl::newStaticLabel(CWnd *parent, const String &s) { // static
  const CRect r(10,10,20,20);
  CStatic *label = new CStatic(); TRACE_NEW(label);
  label->Create(s.cstr(), WS_VISIBLE|SS_CENTERIMAGE, r, parent, IDC_STATIC);
  return label;
}

#define SPACE_BETWEEN_SPIN_LABELS     15
#define SPACE_BETWEEN_LABEL_AND_FIELD 10

int EngineOptionControl::setPosition(const CPoint &p, const DialogColumnInfoArray &a) {
  switch(getType()) {
  case OptionTypeSpin    :
    { const EngineOptionDialogColumnInfo &ci = a[m_column];
      int x = p.x;
      setWindowPosition(m_label , CPoint(x, p.y));
      x += ci.m_maxSpinLabelTextWidth + SPACE_BETWEEN_SPIN_LABELS;
      setWindowPosition(m_label2, CPoint(x, p.y));
      x += ci.m_maxLabel2TextWidth + SPACE_BETWEEN_LABEL_AND_FIELD;
      setWindowPosition(m_ctrl  , CPoint(x, p.y));
      x = ::getWindowRect(m_ctrl).right+2;
      setWindowPosition(m_spinCtrl, CPoint(x, p.y));
    }
    break;
  case OptionTypeCombo   :
  case OptionTypeString  :
    { const EngineOptionDialogColumnInfo &ci = a[m_column];
      int x = p.x;
      setWindowPosition(m_label, CPoint(x, p.y));
      x += ci.m_maxNonSpinLabelTextWidth + SPACE_BETWEEN_LABEL_AND_FIELD;
      setWindowPosition(m_ctrl, CPoint(x, p.y));
    }
    break;
  case OptionTypeButton  :
  case OptionTypeCheckbox:
    setWindowPosition(m_ctrl, p);
    break;
  default:
    invalidTypeException();
  }
  return getWindowRect().right;
}

CRect EngineOptionControl::getWindowRect() const {
  switch(getType()) {
  case OptionTypeSpin    :
    { const CRect r1 = ::getWindowRect(m_label   );
      const CRect r2 = ::getWindowRect(m_spinCtrl);
      return CRect(r1.left, r1.top, r2.right, r2.bottom);
    }
  case OptionTypeCombo   :
  case OptionTypeString  :
    { const CRect r1 = ::getWindowRect(m_label);
      const CRect r2 = ::getWindowRect(m_ctrl );
      return CRect(r1.left, r1.top, r2.right, r2.bottom);
    }

  case OptionTypeButton  :
  case OptionTypeCheckbox:
    return ::getWindowRect(m_ctrl);
  default:
    invalidTypeException();
    break;
  }
  return CRect(0,0,0,0);
}

void EngineOptionControl::setEnable(bool enabled) {
  if(m_ctrl   )  m_ctrl->EnableWindow(enabled?TRUE:FALSE);
  if(m_label  )  m_label->EnableWindow(enabled?TRUE:FALSE);
  if(m_label2 )  m_label2->EnableWindow(enabled?TRUE:FALSE);
  if(m_spinCtrl) m_spinCtrl->EnableWindow(enabled?TRUE:FALSE);
}

void EngineOptionControl::setFont(CFont *font) {
  if(m_ctrl) {
    m_ctrl->SetFont(font);
  }
  if(m_label) {
    m_label->SetFont(font);
  }
  if(m_label2) {
    m_label2->SetFont(font);
  }
}

int EngineOptionControl::findEditBoxWidth(CDC &dc) const {
  const EngineOptionDescription &option = m_option;
  switch(getType()) {
  case OptionTypeSpin:
    { const int minWidth = getTextExtent(dc, format(_T("%d"), option.getMin())).cx;
      const int maxWidth = getTextExtent(dc, format(_T("%d"), option.getMax())).cx;
      return max(minWidth, maxWidth) + 20;
    }
  case OptionTypeCombo:
    return getMaxTextExtent(dc, option.getComboValues()).cx + 30;
  case OptionTypeString:
    return 250;
  default:
    return 40;
  }
}

void EngineOptionControl::cleanup() {
  SAFEDELETE(m_ctrl    );
  SAFEDELETE(m_label   );
  SAFEDELETE(m_label2  );
  SAFEDELETE(m_spinCtrl);
}

void EngineOptionControl::setValue(bool value) {
  assert(getType() == OptionTypeCheckbox);
  ((CButton*)m_ctrl)->SetCheck(value ? BST_CHECKED : BST_UNCHECKED);
}

void EngineOptionControl::setValue(int value) {
  assert(getType()== OptionTypeSpin);
  m_ctrl->SetWindowText(format(_T("%d"), value).cstr());
}

void EngineOptionControl::setValue(const String &value) {
  assert((getType() == OptionTypeCombo) || (getType() == OptionTypeString));
  switch(getType()) {
  case OptionTypeCombo :
    ((CComboBox*)m_ctrl)->SetCurSel(m_option.getValueIndex(value));
    break;
  case OptionTypeString:
    m_ctrl->SetWindowText(value.cstr());
    break;
  }
}

bool EngineOptionControl::getBoolValue() const {
  assert(getType() == OptionTypeCheckbox);
  return ((CButton*)m_ctrl)->GetCheck() == BST_CHECKED;
}

int EngineOptionControl::getIntValue(bool validate) const {
  assert(getType()== OptionTypeSpin);
  CString s;
  m_ctrl->GetWindowText(s);
  int result = 0;
  if(_stscanf((LPCTSTR)s, _T("%d"), &result) != 1) {
    if(validate) {
      throwUserException(IDS_MSG_s_NOTNUMERIC, (LPCTSTR)s);
    }
  }
  return result;
}

String EngineOptionControl::getStringValue() const {
  switch(getType()) {
  case OptionTypeCombo :
    { const int selectedIndex = ((CComboBox*)m_ctrl)->GetCurSel();
      if(selectedIndex >= 0 && selectedIndex < (int)m_option.getComboValues().size()) {
        return m_option.getComboValues()[selectedIndex];
      }
    }
    break;
  case OptionTypeString:
    return getWindowText(m_ctrl);
  default:;
  }
  return EMPTYSTRING;
}

void EngineOptionControl::validate() const {
  switch(getType()) {
  case OptionTypeSpin    :
    { const int value = getIntValue(true);
      if(value < m_option.getMin() || value > m_option.getMax()) {
        throwUserException(IDS_MSG_d_ILLEGALINTVALUE_d_d
                          ,value
                          ,m_option.getMin()
                          ,m_option.getMax()
                          );
      }
    }
    break;
  default:
    break;
  }
}

void EngineOptionControl::invalidTypeException() const {
  invalidTypeException(getType(), m_option.getName());
}

void EngineOptionControl::invalidTypeException(int type, const String &name) const {
  throwException(_T("Invalid type=%d for option %s"), type, name.cstr());
}

EngineOptionControlArray::~EngineOptionControlArray() {
  for(size_t i = size(); i--;) {
    (*this)[i].cleanup();
  }
  clear();
}

EngineOptionControl *EngineOptionControlArray::findControlById(int ctrlId) {
  for(size_t i = 0; i < size(); i++) {
    EngineOptionControl &ctrl = (*this)[i];
    if((ctrl.m_ctrlId == ctrlId) || (ctrl.m_spinCtrlId == ctrlId)) {
      return &ctrl;
    }
  }
  return NULL;
}

EngineOptionControl *EngineOptionControlArray::findControlByName(const String &name) {
  for(size_t i = 0; i < size(); i++) {
    EngineOptionControl &ctrl = (*this)[i];
    if(ctrl.m_option.getName() == name) {
      return &ctrl;
    }
  }
  return NULL;
}

CompactIntArray EngineOptionControlArray::selectCtrlIdByType(EngineOptionType type) const {
  CompactIntArray result;
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionControl &ctrl = (*this)[i];
    if(ctrl.getType() == type) {
      result.add(ctrl.m_ctrlId);
    }
  }
  return result;
}

int EngineOptionControlArray::getNonPushbuttonCount() const {
  return (int)(size() - selectCtrlIdByType(OptionTypeButton).size());
}

DialogColumnInfoArray EngineOptionControlArray::findColumnInfo(CWnd *wnd, int controlHeight, const CSize &space) {
  CClientDC dc(wnd);
  CFont *font = wnd->GetFont();
  dc.SelectObject(font);

  const CSize screenSize   = getScreenSize(false);
  const int nonButtonCount = getNonPushbuttonCount();
  int maxControlsPerColumn;
  for(int columnCount = 1; columnCount < 5; columnCount++) {
    maxControlsPerColumn = nonButtonCount / columnCount;
    const int h = maxControlsPerColumn * (controlHeight + space.cy);
    if(h < 3*screenSize.cy / 5) {
      break;
    }
  }

  DialogColumnInfoArray result(maxControlsPerColumn, controlHeight, space);

  for(int from = 0; from < (int)size();) {
    int to = min(from + maxControlsPerColumn, (int)size());
    result.add(EngineOptionDialogColumnInfo(findMaxCheckBoxTextWidth(dc, from, to)
                                           ,findMaxLabelTextWidth(   dc, from, to, true)
                                           ,findMaxLabel2TextWidth(  dc, from, to)
                                           ,findMaxSpinBoxWidth(     dc, from, to)
                                           ,findMaxLabelTextWidth(   dc, from, to, false)
                                           )
              );
    setColumn((char)result.size()-1, from, to);
    from = to;
  }

  for(size_t i = 0; i < size(); i++) {
    EngineOptionControl &ctrl = (*this)[i];
    ctrl.setFont(font);
    const int col = ctrl.getColumn();
    switch(ctrl.getType()) {
    case OptionTypeCheckbox:
      setClientRectSize(ctrl.m_ctrl, CSize(result[col].m_maxCheckBoxTextWidth+30, controlHeight));
      break;
    case OptionTypeSpin    :
      { const EngineOptionDialogColumnInfo &ci = result[col];
        setClientRectSize(ctrl.m_label   , CSize(ci.m_maxSpinLabelTextWidth, controlHeight));
        setClientRectSize(ctrl.m_label2  , CSize(ci.m_maxLabel2TextWidth   , controlHeight));
        setClientRectSize(ctrl.m_ctrl    , CSize(ci.m_maxSpinBoxWidth      , controlHeight));
        setClientRectSize(ctrl.m_spinCtrl, CSize(16                        , controlHeight));
      }
      break;
    case OptionTypeCombo   :
      { const EngineOptionDialogColumnInfo &ci = result[col];
        setClientRectSize(ctrl.m_label, CSize(ci.m_maxNonSpinLabelTextWidth, controlHeight));
        const int editWidth = ctrl.findEditBoxWidth(dc);
        setClientRectSize(ctrl.m_ctrl,  CSize(editWidth, 4 * controlHeight + 10));
      }
      break;
    case OptionTypeString  :
      { const EngineOptionDialogColumnInfo &ci = result[col];
        setClientRectSize(ctrl.m_label, CSize(ci.m_maxNonSpinLabelTextWidth , controlHeight));
        const int editWidth = ctrl.findEditBoxWidth(dc);
        setClientRectSize(ctrl.m_ctrl,  CSize(editWidth, controlHeight));
      }
      break;
    case OptionTypeButton  :
      setClientRectSize(ctrl.m_ctrl, CSize(getTextExtent(dc, getWindowText(ctrl.m_ctrl)).cx+20, controlHeight));
      break;
    }
  }
  return result;
}

CPoint EngineOptionControlArray::createGrid(const DialogColumnInfoArray &columnInfo, const CPoint &ul) {
  CPoint ctrlPos = ul;
  const CSize &space   = columnInfo.m_space;
  int maxRightSide  = 0;
  int maxY          = 0;
  int controlsInColumn = 0;
  for(size_t i = 0; i < size(); i++) {
    EngineOptionControl &ctrl = (*this)[i];
    switch(ctrl.getType()) {
    case OptionTypeSpin    :
    case OptionTypeCombo   :
    case OptionTypeString  :
      { const int right = ctrl.setPosition(ctrlPos, columnInfo);
        maxRightSide = max(right, maxRightSide);
      }
      break;
    default:
      continue;
    }
    controlsInColumn++;
    maxY = max(maxY, ctrlPos.y);
    ctrlPos.y += columnInfo.m_ctrlHeight + space.cy;
    if(controlsInColumn == columnInfo.m_maxControlsPerColumn) {
      ctrlPos.x = maxRightSide + space.cx;
      ctrlPos.y = ul.y;
      controlsInColumn = 0;
    }
  }
  for(size_t i = 0; i < size(); i++) {
    EngineOptionControl &ctrl = (*this)[i];
    switch(ctrl.getType()) {
    case OptionTypeCheckbox:
      { const int right = ctrl.setPosition(ctrlPos, columnInfo);
        maxRightSide = max(right, maxRightSide);
      }
      break;
    default:
      continue;
    }
    controlsInColumn++;
    maxY = max(maxY, ctrlPos.y);
    ctrlPos.y += columnInfo.m_ctrlHeight + space.cy;
    if(controlsInColumn == columnInfo.m_maxControlsPerColumn) {
      ctrlPos.x = maxRightSide + space.cx;
      ctrlPos.y = ul.y;
      controlsInColumn = 0;
    }
  }
  return CPoint(maxRightSide, maxY + columnInfo.m_ctrlHeight);
}

#define FINDMAXWIDTH(filter, m, w)                    \
  int maxWidth = 0;                                   \
  for(int i = from; i < to; i++) {                    \
    const EngineOptionControl &ctrl = (*this)[i];     \
    if(ctrl.m == NULL || !(filter(ctrl))) continue;   \
    const int textWidth = w;                          \
    maxWidth = max(textWidth, maxWidth);              \
  }                                                   \
  return maxWidth;

#define FINDMAXWIDTH2(filter, m) FINDMAXWIDTH(filter, m, getTextExtent(dc, getWindowText(ctrl.m)).cx)

#define SELECT_CHECKBOX(ctrl)    ctrl.getType() == OptionTypeCheckbox
#define SELECT_SPINBOX(ctrl)     ctrl.getType() == OptionTypeSpin
#define SELECT_NONSPINBOX(ctrl)  ctrl.getType() != OptionTypeSpin
#define SELECT_ALL(ctrl)         true

int EngineOptionControlArray::findMaxCheckBoxTextWidth(CDC &dc, int from, int to) const {
  FINDMAXWIDTH2(SELECT_CHECKBOX, m_ctrl)
}

int EngineOptionControlArray::findMaxLabelTextWidth(CDC &dc, int from, int to, bool intFields) const {
  if(intFields) {
    FINDMAXWIDTH2(SELECT_SPINBOX, m_label)
  } else {
    FINDMAXWIDTH2(SELECT_NONSPINBOX, m_label)
  }
}

int EngineOptionControlArray::findMaxLabel2TextWidth(CDC &dc, int from, int to) const {
  FINDMAXWIDTH2(SELECT_SPINBOX, m_label2)
}

int EngineOptionControlArray::findMaxSpinBoxWidth(CDC &dc, int from, int to) const {
  FINDMAXWIDTH(SELECT_SPINBOX, m_ctrl, ctrl.findEditBoxWidth(dc))
}

void EngineOptionControlArray::setColumn(char column, int from, int to) {
  for(int i = from; i < to; i++) {
    (*this)[i].m_column = column;
  }
}

void EngineOptionControlArray::putValues(const EngineOptionValueArray &valueArray) {
  for(size_t i = 0; i < valueArray.size(); i++) {
    const EngineOptionValue &v    = valueArray[i];
    EngineOptionControl     *ctrl = findControlByName(v.getName());
    if(ctrl == NULL) continue;
    switch(ctrl->getType()) {
    case OptionTypeCheckbox: ctrl->setValue(v.getBoolValue()  ); break;
    case OptionTypeSpin    : ctrl->setValue(v.getIntValue()   ); break;
    case OptionTypeCombo   :
    case OptionTypeString  : ctrl->setValue(v.getStringValue()); break;
    }
  }
}

EngineOptionValueArray EngineOptionControlArray::getValues(const String &engineName) const {
  EngineOptionValueArray result(engineName);
  for(size_t i = 0; i < size(); i++) {
    const EngineOptionControl &ctrl = (*this)[i];
    switch(ctrl.getType()) {
    case OptionTypeCheckbox:
      result.setValue(ctrl.m_option.getName(), ctrl.getBoolValue());
      break;

    case OptionTypeSpin    :
      result.setValue(ctrl.m_option.getName(), ctrl.getIntValue());
      break;

    case OptionTypeCombo   :
    case OptionTypeString  :
      result.setValue(ctrl.m_option.getName(), ctrl.getStringValue());
      break;
    }
  }
  return result;
}

#endif
