#pragma once

#include <MFCUtil/LayoutManager.h>
#include "ListFieldAttributeDlg.h"

class ListCtrlModelSchema : public Array<ListFieldAttribute> {
public:
  ListCtrlModelSchema() {}
  ListCtrlModelSchema(CTableModel &model);
};

class ListCtrlField {
private:
  bool useString() const;
public:
  UINT      m_flags;
  union {
    short   m_short;
    int     m_int;
    float   m_float;
    double  m_double;
    bool    m_bool;
    String *m_string;
  };
  ListCtrlField(const ListFieldAttribute &attr);
 ~ListCtrlField();
  ListFieldType getType() const {
    return LF_GETTYPE(m_flags);
  }
  void *getValue();
  String toString() const;
};

class ListCtrlItem {
public:
  Array<ListCtrlField> m_fields;
  ListCtrlItem(const ListCtrlModelSchema &schema);
  String toString() const;
};

class ListFieldDefinition {
public:
  String         m_header;
  ListFieldType  m_type;
  bool           m_enabled;
  short          m_width;
  bool           m_hasLowerLimit;
  bool           m_lowerLimitExclusive;
  double         m_lowerLimit;
  bool           m_hasUpperLimit;
  bool           m_upperLimitExclusive;
  double         m_upperLimit;
  bool           m_showZero;
  short          m_decimalCount;
  int            m_dropDownStyle;
  StringArray    m_comboStrings;
  ListFieldDefinition(const ListFieldAttribute &attr);
  operator ListFieldAttribute() const;
};

class BaseTableModel : public CTableModel {
private:
  Array<ListFieldDefinition> m_elementArray;
public:
  BaseTableModel() {
  }
  BaseTableModel(const ListCtrlModelSchema &modelSchema);
  void          setModelSchema(const ListCtrlModelSchema &modelSchema);
  ListCtrlModelSchema   getModelSchema() const;
  UINT          getRowCount();
  UINT          getColumnCount();
  String        getColumnName(   UINT column);
  UINT          getColumnWidth(  UINT column);
  UINT          getFieldFlags(   UINT column);
  void         *getValueAt(      UINT row, UINT column);
  bool          isEditableCell(  UINT row, UINT column);
  void          clearTable();
  void          addElements(     UINT count);
  void          removeElement(   UINT index);
  const StringArray getStrComboStringArray(UINT column);

  const ListFieldDefinition &getElement(UINT index) const {
    return m_elementArray[index];
  }
};

class CEditModelSchemaDlg : public CDialog {
private:
  SimpleLayoutManager m_layoutManager;
  ListCtrlModelSchema m_schema;
  BaseTableModel      m_model;
public:
  CEditModelSchemaDlg(const ListCtrlModelSchema &schema, CWnd *pParent = nullptr);

  const ListCtrlModelSchema &getSchema() const {
    return m_schema;
  }
  enum { IDD = IDD_EDITSCHEMADIALOG };
  CEditListCtrl   m_list;

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnButtonUp();
  afx_msg void OnButtonDown();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  DECLARE_MESSAGE_MAP()
};

