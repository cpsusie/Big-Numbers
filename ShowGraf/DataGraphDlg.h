#pragma once

#include <MFCUtil/EditListCtrl.h>
#include "DataGraph.h"

class XYTableModel : public CTableModel {
private:
  Point2DArray m_data;
public:
  XYTableModel(PointGraph &pointGraph) : m_data(pointGraph.getDataPoints()) {
  }
  UINT getRowCount() {
    return (UINT)m_data.size();
  }
  UINT getColumnCount() {
    return 2;
  }
  String getColumnName(UINT column) {
    return column?_T("Y"):_T("X");
  }
  UINT getColumnWidth(UINT column) {
    return 60;
  }
  UINT getFieldFlags(UINT column) {
    return LFT_DOUBLE | LF_NUM_DECIMALS(8) | LF_NUM_SHOW_ZERO;
  }
  void *getValueAt(UINT row, UINT column) {
    Point2D &p = m_data[row];
    return column ? &p.y : &p.x;
  }
  bool isEditableCell(UINT row, UINT column) {
    return true;
  }
  void clearTable() {
    m_data.clear();
  }
  void addElements(UINT count) {
    while(count--) {
      m_data.add(Point2D(0,0));
    }
  }
  void removeElement(UINT index) {
    m_data.remove(index);
  }
  const Point2DArray &getData() const {
    return m_data;
  }
};

class CDataGraphDlg : public CDialog {
private:
  SimpleLayoutManager m_layoutManager;
  HACCEL              m_accelTable;
  String              m_fullName;
  XYTableModel        m_model;
  CEditListCtrl       m_list;
  CString             m_name;
  CString             m_style;

  CComboBox *getStyleCombo() {
    return (CComboBox*)GetDlgItem(IDC_COMBOSTYLE);
  }
  CMFCColorButton *getColorButton() {
    return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
  }
public:
    CDataGraphDlg(DataGraph &g, CWnd *pParent = NULL);
    enum { IDD = IDD_DATAGRAPH_DIALOG };
    DataGraph    &m_graph;

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    virtual BOOL OnInitDialog();
    virtual void OnOK();
    DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnSize(UINT nType, int cx, int cy);
};
