#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/EditListCtrl.h>

class TableElement {
public:
  String m_name;
  String m_address;
  int    m_tlfnr;
  short  m_short;
  float  m_float;
  double m_double;
  bool   m_bool;
  int    m_iComboValue;
  TableElement(int i);
  String toString() const;
};

class TestTableModel : public CTableModel {
private:
  Array<TableElement> m_elementArray;
public:
  TestTableModel();
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
  const TableElement &getElement(UINT index) const {
    return m_elementArray[index];
  }
  const StringArray getStrComboStringArray(UINT column);
};

class CTestEditableTableDlg : public CDialog {
private:
  HICON               m_hIcon;
  HACCEL              m_accelTable;
  SimpleLayoutManager m_layoutManager;

public:
  CTestEditableTableDlg(CWnd *pParent = nullptr);
  TestTableModel m_model;

  enum { IDD = IDD_TESTEDITABLETABLE_DIALOG };
  CEditListCtrl m_list;
  CString       m_someText;

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnSetFocusEdit1();
  afx_msg void OnKillFocusEdit1();
  afx_msg void OnKillFocusList(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnSetFocusList( NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnButtonInsert10();
  afx_msg void OnButtonClearTable();
  afx_msg void OnButtonDeleteCurrent();
  afx_msg void OnButtoLogTable();
  afx_msg void OnButtonResetLog();
  afx_msg void OnFileExit();
  afx_msg void OnEditSetupModel();
  afx_msg void OnCheck1();
  afx_msg void OnClose();
  virtual void OnCancel();
  virtual void OnOK();
  DECLARE_MESSAGE_MAP()
};
