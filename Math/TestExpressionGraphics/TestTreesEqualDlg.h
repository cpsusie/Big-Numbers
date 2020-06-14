#pragma once

#include <MFCUtil/ComboBoxWithHistory.h>
#include <MFCUtil/DialogWithDynamicLayout.h>
#include "ExpressionTreeCtrl.h"

class ExprData {
public:
  CComboBoxWithHistory m_cb;
  Expression           m_expr;
  CExpressionTreeCtrl  m_tree;
  ExpressionImage      m_image;
};

class CTestTreesEqualDlg : public CDialogWithDynamicLayout {
private:
  HACCEL                  m_accelTabel;
  ExprData                m_edata[2];
  int                     m_contextWinIndex, m_focusCtrlId;
  CString                 m_expr1;
  CString                 m_expr2;

  inline PixRectDevice &getDevice() const {
    return theApp.m_device;
  }
  bool     compile(     int index);
  void     makeImage(   int index);
  void     paintImage(  int index);
  void     destroyImage(int index);
  void     paintImage(int winId, const ExpressionImage &image);
  ExpressionImage makeImage(const Expression &e);
  void ajourButtons();

public:
  CTestTreesEqualDlg(CWnd *pParent = NULL);

  enum { IDD = IDD_TREESEQUAL_DIALOG };

  virtual BOOL OnInitDialog();
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual void OnCancel();
  virtual void OnOK();
  afx_msg void OnPaint();
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
  afx_msg void OnTestTreesEqual();
  afx_msg void OnTestTreesEqualMinus();
  afx_msg void OnButtonCompile();
  afx_msg void OnBnClickedClose();
  afx_msg void OnEditFindMatchingParentesis();
  afx_msg void OnButtonConvert();
  afx_msg void OnContextMenuShowTree();
  afx_msg void OnSetFocusEditExpr1();
  afx_msg void OnKillFocusEditExpr1();
  afx_msg void OnSetFocusEditExpr2();
  afx_msg void OnKillFocusEditExpr2();
  DECLARE_MESSAGE_MAP()
  afx_msg void OnClose();
};
