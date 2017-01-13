#pragma once

#include <NumberInterval.h>
#include <MFCUtil/EditListCtrl.h>

class ListFieldAttribute {
public:
  String         m_header;
  UINT           m_flags;
  bool           m_enabled;
  short          m_width;
  DoubleInterval m_numberInterval;
  StringArray    m_comboStrings;
  ListFieldAttribute();
  ListFieldType getType() const {
    return LF_GETTYPE(m_flags);
  }
  int getDecimalCount() const {
    return LF_ISINTEGERTYPE(getType()) ? 0 : LF_NUM_GETDECIMALCOUNT(m_flags);
  }
  String toString() const;

};

class CListFieldAttributeDlg : public CDialog {
private:
  HACCEL             m_accelTable;
  ListFieldAttribute m_attr;

  static bool          isNumericType(ListFieldType type) {
    return CTableModel::isNumericType(type);
  }
  void                 setFieldType(ListFieldType type);
  ListFieldType        getFieldType() const {
    return comboValueToFieldType(m_fieldType);
  }
  static ListFieldType comboValueToFieldType(int i);
  static int           fieldTypeToComboValue(ListFieldType type);
  void                 setFieldsEnabled(const int *idArray, int n, bool enabled);
  void paramFromWindow(ListFieldAttribute &attr);
public:
  CListFieldAttributeDlg(const ListFieldAttribute &attr, CWnd *pParent = NULL);
  const ListFieldAttribute &getAttribute() const {
    return m_attr;
  }

  enum { IDD = IDD_LISTFIELDDIALOG };
  CString m_header;
  int     m_fieldType;
  BOOL    m_enabled;
  BOOL    m_hasLowerLimit;
  BOOL    m_hasUpperLimit;
  BOOL    m_lowerLimitExclusive;
  BOOL    m_upperLimitExclusive;
  double  m_lowerLimit;
  double  m_upperLimit;
  BOOL    m_showZero;
  UINT    m_decimalCount;
  int     m_dropdownStyle;
  UINT    m_width;

public:
  virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
  virtual void DoDataExchange(CDataExchange* pDX);

protected:
  afx_msg void OnSelChangeComboFieldType();
  virtual BOOL OnInitDialog();
  virtual void OnOK();
  afx_msg void OnGotoHeader();
  afx_msg void OnGotoDecimals();
  afx_msg void OnGotoWidth();
  DECLARE_MESSAGE_MAP()
};

