#pragma once

#include <MFCUtil/DeleteButton.h>
#define APSTUDIO_INVOKED
#include "resource.h"

#define FIRST_DIFFEUQUATIONFIELDID (_APS_NEXT_CONTROL_VALUE+1)
#define LAST_DIFFEUQUATIONFIELDID  (FIRST_DIFFEUQUATIONFIELDID      + 10*20)

#define DIFFEUQUATIONFIELDID(i) (((i)*(int)m_subWndArray.size()) + FIRST_DIFFEUQUATIONFIELDID)

typedef enum {
  EQ_NAME_EDIT
 ,EQ_LABEL_STATIC
 ,EQ_EXPR_EDIT
 ,EQ_STARTV_EDIT
 ,EQ_VISIBLE_BUTTON
 ,EQ_COLOR_BUTTON
 ,EQ_DELETE_BUTTON
} DiffEquationField;

class CDiffEquationEdit : public CEdit {
private:
  CFont              &m_font;
  CompactArray<CWnd*> m_subWndArray;
  CString             m_name;
  CString             m_expr;
  double              m_startValue;
  BOOL                m_visible;

  int                 m_exprId;
  CEdit               m_editName;
  CStatic             m_label;
  CEdit               m_editStartV;
  CButton             m_checkVisible;
  CMFCColorButton     m_colorButton;
  CDeleteButton       m_buttonDelete;

  CompactArray<CRect> calculateSubWinRect(const CRect &r) const;
public:
  CDiffEquationEdit(CFont &font);
  ~CDiffEquationEdit();
  void  Create(CWnd *parent, UINT eqIndex);
  void  DoDataExchange(CDataExchange *pDX);
  void  addToLayoutManager(     SimpleLayoutManager &layoutManager, int flags);
  void  removeFromLayoutManager(SimpleLayoutManager &layoutManager);
  void  setWindowRect(const CRect &r);
  CRect getWindowRect();
  int   getNameId()    const;
  int   getLabelId()   const;
  int   getExprId()    const;
  int   getStartVId()  const;
  int   getVisibleId() const;
  int   getColorId()   const;
  int   getDeleteId()  const;
  inline int getFieldCount() const {
    return (int)m_subWndArray.size();
  }
  inline int getFirstCtrlId() const {
    return m_exprId;
  }
  inline int getLastCtrlId() const {
    return getFirstCtrlId() + getFieldCount() - 1;
  }
  inline bool containsCtrlId(int ctrlId) const {
    return (ctrlId >= getFirstCtrlId()) && (ctrlId <= getLastCtrlId());
  }
  CompactUintArray getTabOrderArray() const;
  DiffEquationField findFieldByCtrlId(UINT id) const;
  inline DiffEquationField getFocusField() const {
    return findFieldByCtrlId(getFocusCtrlId(this));
  }
  bool  getVisibleChecked();
  void  setVisibleChecked(bool checked);
  void  paramToWin(const DiffEquationDescription &desc, const EquationAttributes &attr);
  void  winToParam(      DiffEquationDescription &desc,       EquationAttributes &attr);
};

class CDiffEquationEditArray : public CompactArray<CDiffEquationEdit*> {
private:
  void addEquationToLayoutManager(     SimpleLayoutManager &layoutManager, size_t index);
  void removeEquationFromLayoutManager(SimpleLayoutManager &layoutManager, size_t index);

public:
  ~CDiffEquationEditArray() override {
    clear();
  }
  int                findEquationIndexByCtrlId(UINT id) const;
  CDiffEquationEdit *findEquationByCtrlId(     UINT id) const;
  inline CDiffEquationEdit *getLastEquationEdit() const {
    return size() ? last() : nullptr;
  }
  void addEquation(CWnd *wnd, CFont &font);
  void remove(size_t index);
  void clear();
  void addAllToLayoutManager(     SimpleLayoutManager &layoutManager);
  void removeAllFromLayoutManager(SimpleLayoutManager &layoutManager);
  void winToParam(      DiffEquationGraphParameters &param);
  // assume size() == param.getEquationCount()
  void paramToWin(const DiffEquationGraphParameters &param);
  void DoDataExchange(CDataExchange *pDX);
};
