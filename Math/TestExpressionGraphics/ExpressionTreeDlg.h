#pragma once

#include <PropertyContainer.h>
#include <MFCUtil/DialogWithDynamicLayout.h>
#include "ExpressionTreeCtrl.h"

typedef enum {
  TREE_SELECTEDNODE            // const ExpressionNode *
} ExpressionTreeDlgProperties;

class CExpressionTreeDlg : public CDialogWithDynamicLayout, public PropertyContainer {
private:
  const Expression     &m_expr;
  const ExpressionNode *m_node;
  ExpressionNode       *m_selectedNode;
  CExpressionTreeCtrl   m_treeCtrl;
  BOOL                  m_extendedInfo;
  inline PixRectDevice &getDevice() const {
    return theApp.m_device;
  }
  HTREEITEM findItemFromNode(const ExpressionNode *n);
  const ExpressionNode *getNodeFromPoint(CPoint p);

  void setSelectedNode(const ExpressionNode *m_selectedNode); // property
  void updateNodeText(const ExpressionNode *n);
public:
    CExpressionTreeDlg(const Expression     &expr, CWnd *pParent = nullptr);
    CExpressionTreeDlg(const ExpressionNode *n   , CWnd *pParent = nullptr);

    enum { IDD = IDD_EXPRESSIONTREE_DIALOG };

    virtual void DoDataExchange(CDataExchange *pDX);
    afx_msg void OnClose();
    virtual BOOL OnInitDialog();
    afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
    afx_msg void OnClearBreakPoint();
    afx_msg void OnSetBreakPoint();
    afx_msg void OnSelchangedTreeExpression(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnBnClickedCheckExtendedInfo();
    DECLARE_MESSAGE_MAP()
};
