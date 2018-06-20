#pragma once

class CExpressionTreeCtrl : public CTreeCtrl {
	DECLARE_DYNAMIC(CExpressionTreeCtrl)
private:
  bool        m_extendedText;
  ParserTree *m_tree;
  void   traverse(HTREEITEM p, const ExpressionNode *n);

public:
	CExpressionTreeCtrl();
	virtual ~CExpressionTreeCtrl();
  void substituteControl(CWnd *parent, int id);
  void showTree(const ExpressionNode *n, bool extendedText);
  void setExtendedText(bool extendedText);
  void expandAllNodes();
  void updateItemText(HTREEITEM item);
protected:
	DECLARE_MESSAGE_MAP()
};


