#include "stdafx.h"

#define APSTUDIO_INVOKED
#include "resource.h"
#include "VariableEditField.h"

#define FIELDID(i) ((i) + _APS_NEXT_CONTROL_VALUE + 1)

void VariableEditFieldArray::putValues(const ExpressionVariableArray &variables) {
  DEFINEMETHODNAME;
  for(size_t i = 0; i < variables.size(); i++) {
    const ExpressionVariableWithValue &v = variables[i];
    VariableEditField *f = findFieldByName(v.getName());
    if(f == NULL) {
      throwInvalidArgumentException(method, _T("Variable with name <%s> not found"), v.getName().cstr());
    }
    f->putValue(v.getValue());
  }
}

void VariableEditFieldArray::putValues(const Expression &expr) {
  for(size_t i = 0; i < m_fields.size(); i++) {
    VariableEditField        *f    = m_fields[i];
    const String             &name = f->getName();
    const ExpressionVariable *v    = expr.getVariable(name);
    if(v) {
      f->putValue(expr.getValueRef(*v));
    }
  }
}

void VariableEditFieldArray::validate() {
  for(size_t i = 0; i < m_fields.size(); i++) {
    const VariableEditField *f = m_fields[i];
    if(!f->validate()) {
      gotoEditBox(m_parent, FIELDID((int)i));
      throwException(_T("Not numeric input"));
    }
  }
}

ExpressionVariableArray VariableEditFieldArray::getValues() const {
  ExpressionVariableArray result;
  for(size_t i = 0; i < m_fields.size(); i++) {
    const VariableEditField *f = m_fields[i];
    result.add(ExpressionVariableWithValue(f->getName(), false,false,false, f->getValue()));
  }
  return result;
}

VariableEditField *VariableEditFieldArray::findFieldByName(const String &name) {
  for(size_t i = 0; i < m_fields.size(); i++) {
    if(m_fields[i]->getName() == name) {
      return m_fields[i];
    }
  }
  return NULL;
}

void VariableEditFieldArray::Create(CWnd *parent, const Expression &expr) {
  m_parent = parent;
  const ExpressionVariableArray variables = expr.getSymbolTable().getAllVariables();

  int count = 0;
  for(size_t i = 0; i< variables.size(); i++) {
    const ExpressionVariable &v = variables[i];
    if(v.isConstant() || v.isDefined()) continue;
    CRect rect;
    rect.top    = count * 39 + 10;
    rect.bottom = rect.top   + 25;
    rect.left   = 10;
    rect.right  = rect.left + 50;
    VariableEditField *f = new VariableEditField;
    f->Create(v.getName(), parent, rect, FIELDID(count));
    m_fields.add(f);
    count++;
  }
  m_fieldsCreated  = true;
}

VariableEditFieldArray::~VariableEditFieldArray() {
  for(size_t i = 0; i < m_fields.size(); i++) {
    delete m_fields[i];
  }
  m_fields.clear();
}

void VariableEditFieldArray::OnPaint() {
  CFont *font = m_parent->GetFont();
  for(size_t i = 0; i < m_fields.size(); i++) {
    VariableEditField *f = m_fields[i];
    if(!m_fontIsSet) {
      f->SetFont(font,false);
    }
  }
  m_fontIsSet = true;
}

void VariableEditFieldArray::gotoFirstField() {
  if(m_fields.size() > 0) {
    gotoEditBox(m_parent, FIELDID(0));
  }
}

void VariableEditField::Create(const String &name, CWnd *parent, CRect r, int fieldId) {
  m_label.Create(format(_T("%s:"), name.cstr()).cstr(), WS_VISIBLE | SS_CENTERIMAGE, r, parent, IDC_STATIC);
  m_name  = name;
  r.left  = r.right + 20;
  r.right = r.left + 150;
  CEdit::Create(WS_BORDER | WS_VISIBLE | WS_GROUP | WS_TABSTOP, r, parent, fieldId);
}

Real VariableEditField::getValue() const {
  const String str = getWindowText(this);
  double v;
  if(_stscanf(str.cstr(), _T("%le"), &v) != 1) {
    v = 0;
  }
  return v;
}

bool VariableEditField::validate() const {
  const String str = getWindowText(this);
  double v;
  if(_stscanf(str.cstr(), _T("%le"), &v) != 1) {
    return false;
  }
  return true;
}

void VariableEditField::putValue(Real v) {
  const String str = format(_T("%lg"), v);
  setWindowText(this, str);
}
