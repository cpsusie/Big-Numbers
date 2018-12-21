#pragma once

#include <MFCUtil/LayoutManager.h>
#include <MFCUtil/Deletebutton.h>
#include "DiffEquationGraph.h"
#include "DiffEquationEdit.h"

class CDiffEquationGraphDlg : public CDialog {
private:
  static const int            s_bottomIdArray[];
  HACCEL                      m_accelTable;
  SimpleLayoutManager         m_layoutManager;
  int                         m_lowerPanelHeight;
  int                         m_equationHeight;
  int                         m_upperPanelBottom;
  CFont                       m_exprFont;
  String                      m_fullName;
  CDiffEquationEditArray      m_equationControlArray;
  CompactArray<ErrorPosition> m_errorPosArray;
  BitSet                      m_currentAdjustSet;
  String                      m_currentText;

  CString   m_name;
  CString   m_style;
  BOOL      m_createListFile;
  CString   m_commonText;
  double    m_maxError;
  double    m_xFrom;
  double    m_xTo;

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
  inline bool isErrorListVisible() {
    return getErrorList()->IsWindowVisible() ? true : false;
  }
  inline int getErrorCount() {
    return getErrorList()->GetCount();
  }
  inline int getSelectedError() {
    return getErrorCount() ? getErrorList()->GetCurSel() : -1;
  }
  void setSelectedError( int index);
  void gotoErrorPosition(int index);
  void setCurrentAdjustSet(UINT id);
  inline void clearCurrentAdjustSet() {
    m_currentAdjustSet.clear().setCapacity(getErrorCount() + 1);
    traceCurrentAdjustSet();
  }
  inline bool isCurrentAdjustSetEmpty() const {
    return m_currentAdjustSet.isEmpty();
  }
  void traceCurrentAdjustSet();
  void adjustErrorPositions(const String &s, int sel, int delta);
  void gotoTextPosition( int ctrlId, const SourcePosition &pos);
  void paramToWin(                 const DiffEquationGraphParameters &param);
  void winToParam(                       DiffEquationGraphParameters &param);
  void saveAs(                           DiffEquationGraphParameters &param);
  void save(const String &fileName,      DiffEquationGraphParameters &param);
  void addToRecent(const String &fileName);
  void setEquationCount(size_t n);
  void addEquation();
  void removeEquation(size_t index);
  inline void addEquationsToLOManager() {
    m_equationControlArray.addAllToLayoutManager(m_layoutManager);
  }
  inline void removeEquationsFromLOManager() {
    m_equationControlArray.removeAllFromLayoutManager(m_layoutManager);
  }
  // return -1 if id does not belong to any equation
  inline int                findEquationIndexByCtrlId(UINT id) const {
    return m_equationControlArray.findEquationIndexByCtrlId(id);
  }
  // return NULL if not in any diff-equation
  inline CDiffEquationEdit *findEquationByCtrlId(     UINT id) const {
    return m_equationControlArray.findEquationByCtrlId(id);
  }
  // return -1 if not in any diff-equation
  inline int                getFocusEquationIndex() const {
    return findEquationIndexByCtrlId(getFocusCtrlId(this));
  }
  // return NULL if not in any diff-equation
  inline CDiffEquationEdit *getFocusEquation() const {
    return findEquationByCtrlId(getFocusCtrlId(this));
  }
  void gotoEquation(size_t index);
  inline UINT getEquationCount() const {
    return (UINT)m_equationControlArray.size();
  }
  inline CDiffEquationEdit *getEquationEdit(size_t index) const {
    return m_equationControlArray[index];
  }
  inline CDiffEquationEdit *getLastEquationEdit() const {
    return m_equationControlArray.getLastEquationEdit();
  }
  inline CListBox *getErrorList() {
    return (CListBox*)GetDlgItem(IDC_LISTERRORS);
  }
  void ajourCommonEnabled();
  DECLARE_DYNAMIC(CDiffEquationGraphDlg)
protected:
  String getListFileName(const DiffEquationGraphParameters &param) const;
public:
  CDiffEquationGraphDlg(DiffEquationGraphParameters &param, CWnd *pParent = NULL);
  virtual ~CDiffEquationGraphDlg();
  DiffEquationGraphParameters &m_param;

  enum { IDD = IDR_DIFFEQUATION };

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
