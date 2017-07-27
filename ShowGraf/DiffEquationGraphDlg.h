#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/Deletebutton.h>
#include "DiffEquationGraph.h"

#define TOP_EQUATION    0x1
#define BOTTOM_EQUATION 0x2

typedef enum {
  EQ_NAME_EDIT
 ,EQ_LABEL_STATIC
 ,EQ_EXPR_EDIT
 ,EQ_STARTV_EDIT
 ,EQ_VISIBLE_BUTTON
 ,EQ_COLOR_BUTTON
 ,EQ_DELETE_BUTTON
} DiffEquationField;

class CEquationEdit : public CEdit {
private:
  CFont              &m_font;
  CompactArray<CWnd*> m_subWndArray;
  CString             m_name;
  CString             m_expr;
  double              m_startValue;
  BOOL                m_visible;

  CompactArray<CRect> calculateSubWinRect(const CRect &r) const;
private:
  int             m_exprId;
  CEdit           m_editName;
  CStatic         m_label;
  CEdit           m_editStartV;
  CButton         m_checkVisible;
  CMFCColorButton m_colorButton;
  CDeleteButton   m_buttonDelete;
public:
  CEquationEdit(CFont &font);
  ~CEquationEdit();
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
  CompactIntArray getTabOrderArray() const;
  DiffEquationField findFieldByCtrlId(UINT id) const;
  inline DiffEquationField getFocusField() const {
    return findFieldByCtrlId(getFocusCtrlId(this));
  }
  bool  getVisibleChecked();
  void  setVisibleChecked(bool checked);
  void  paramToWin(  const DiffEquationDescription &desc, const EquationAttributes &attr);
  void  winToParam(        DiffEquationDescription &desc,       EquationAttributes &attr);
};

class CDiffEquationGraphDlg : public CDialog {
private:
    static const int             s_bottomIdArray[];
    HACCEL                       m_accelTable;
    SimpleLayoutManager          m_layoutManager;
    int                          m_lowerPanelHeight;
    int                          m_equationHeight;
    int                          m_upperPanelBottom;
    CFont                        m_exprFont;
    String                       m_fullName;
    CompactArray<CEquationEdit*> m_equationControlArray;
    Array<ErrorPosition>         m_errorPosArray;
    CompactIntArray              m_currentAdjustSet;
    String                       m_currentText;

    CComboBox *getStyleCombo();
    void adjustWindowSize();
    void adjustTopPanel();
    void adjustLowerPanel();
    void adjustPanels(int totalEquationHeight);
    void resetLowerPanelHeight();
    inline int findTopPanelBottom(int eqCount) {
      return getWindowRect(this, IDC_EDITNAME).bottom + ((eqCount <= 1)?0:(eqCount*5));
    }
    inline int getTopPanelBottom() {
      return getWindowRect(this, IDC_EDITCOMMON).bottom;
    }
    inline int getLowerPanelHeight() const {
      return m_lowerPanelHeight;
    }
    int getLowerPanelTop();
    inline int getSumEquationHeight() const {
      return m_equationHeight * getEquationCount();
    }
    inline int getSpaceBetweenTopAndBottomPanel(int eqCount) {
      return getLowerPanelTop() - findTopPanelBottom(eqCount);
    }
    CRect getUnionRect(const int *ids, int n);
    int   getMaxHeight(const int *ids, int n);
    void  adjustTabOrder(const CompactIntArray &oldTabOrder);
    bool  validate();
    void  showErrors(const CompilerErrorList &errorList);
    void  clearErrorList();
    bool  isErrorListVisible() {
      return getErrorList()->IsWindowVisible() ? true : false;
    }
    inline int getErrorCount() {
      return getErrorList()->GetCount();
    }
    int getSelectedError() {
      return getErrorCount() ? getErrorList()->GetCurSel() : -1;
    }
    void setSelectedError( int index);
    void gotoErrorPosition(int index);
    void setCurrentAdjustSet(UINT id);
    inline void clearCurrentAdjustSet() {
      m_currentAdjustSet.clear();
      traceCurrentAdjustSet();
    }
    bool isCurrentAdjustSetEmpty() const {
      return m_currentAdjustSet.isEmpty();
    }
    void traceCurrentAdjustSet();
    void adjustErrorPositions(const String &s, int sel, int delta);
    void gotoTextPosition( int ctrlId, const SourcePosition &pos);
    void paramToWin(                 const DiffEquationGraphParameters &param);
    void winToParam(                       DiffEquationGraphParameters &param);
    void equationToWin(size_t index, const DiffEquationDescription &desc, const EquationAttributes &attr);
    void winToEquation(size_t index,       DiffEquationDescription &desc,       EquationAttributes &attr);
    void saveAs(                           DiffEquationGraphParameters &param);
    void save(const String &fileName,      DiffEquationGraphParameters &param);
    void addToRecent(const String &fileName);
    void setEquationCount(size_t n);
    void addEquation();
    void removeEquation(size_t index);
    void addEquationsToLOManager();
    void removeEquationsFromLOManager();
    void addEquationToLOManager(     size_t index);
    void removeEquationFromLOManager(size_t index);
    int            findEquationIndexByCtrlId(UINT id) const; // return -1 if id does not belong to any equation
    CEquationEdit *findEquationByCtrlId(     UINT id); // return NULL if not in any diff-equation
    inline int     getFocusEquationIndex() {           // return -1 if not in any diff-equation
      return findEquationIndexByCtrlId(getFocusCtrlId(this));
    }
    inline CEquationEdit *getFocusEquation() {         // return NULL if not in any diff-equation
      return findEquationByCtrlId(getFocusCtrlId(this));
    }
    void gotoEquation(size_t index);
    UINT getEquationCount() const {
      return (UINT)m_equationControlArray.size();
    }
    inline CEquationEdit *getEquationEdit(size_t index) const {
      return m_equationControlArray[index];
    }
    inline CEquationEdit *getLastEquationEdit() const {
      return getEquationCount() ? getEquationEdit(getEquationCount()-1) : NULL;
    }
    CListBox *getErrorList() {
      return (CListBox*)GetDlgItem(IDC_LISTERRORS);
    }
    void ajourCommonEnabled();
    DECLARE_DYNAMIC(CDiffEquationGraphDlg)

public:
    CDiffEquationGraphDlg(DiffEquationGraphParameters &param, CWnd *pParent = NULL);
    virtual ~CDiffEquationGraphDlg();
    DiffEquationGraphParameters &m_param;

    enum { IDD = IDR_DIFFEQUATION };
  CString   m_style;
  CString   m_name;
  CString   m_commonText;
  double    m_maxError;
  double    m_xFrom;
  double    m_xTo;

protected:
    virtual void DoDataExchange(CDataExchange *pDX);
    virtual BOOL OnInitDialog();
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnDestroy();
    afx_msg void OnBnClickedButtonaddeq();
    afx_msg void OnBnClickedEquation(    UINT id);
    afx_msg void OnEditChangeEquation(   UINT id);
    afx_msg void OnEditSetFocusEquation( UINT id);
    afx_msg void OnEditKillFocusEquation(UINT id);
    afx_msg void OnEditChangeCommon();
    afx_msg void OnEditSetFocusCommon();
    afx_msg void OnEditKillFocusCommon();
    afx_msg void OnOK();
    afx_msg void OnFileNew();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnEditNexterror();
    afx_msg void OnEditPreverror();
    afx_msg void OnLbnSelchangeListerrors();
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoCommon();
    afx_msg void OnGotoMaxError();
    afx_msg void OnGotoXInterval();
    DECLARE_MESSAGE_MAP()
};
