#pragma once

class VariableEditField : public CEdit {
private:
  CStatic m_label;
  String  m_name;
public:
  void Create(const String &name, CWnd *parent, CRect r, int fieldId);
  Real getValue() const;
  void putValue(Real v);
  bool validate() const;
  const String &getName() const {
    return m_name;
  }
};

class VariableEditFieldArray {
private:
  CWnd                            *m_parent;
  CompactArray<VariableEditField*> m_fields;
  bool                             m_fieldsCreated, m_fontIsSet;

  VariableEditField *findFieldByName(const String &name);
public:
  VariableEditFieldArray() {
    m_parent = nullptr;
    m_fieldsCreated = m_fontIsSet = false;
  }
  ~VariableEditFieldArray();
  void                    putValues(const ExpressionVariableArray &parameters);
  void                    putValues(const Expression &expr);
  ExpressionVariableArray getValues() const;
  void Create(CWnd *parent, const Expression &expr);
  void OnPaint();
  void gotoFirstField();
  void validate();
};
