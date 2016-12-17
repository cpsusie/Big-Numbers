#pragma once

#include <MFCUtil/LayoutManager.h>
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
  static CBitmap      s_deleteBitmap;
  CFont              &m_font;
  CompactArray<CWnd*> m_subWndArray;
  CString             m_name;
  CString             m_expr;
  double              m_startValue;
  BOOL                m_visible;

  HBITMAP             getDeleteBitmap();
  CompactArray<CRect> calculateSubWinRect(const CRect &r) const;
private:
  int             m_exprId;
  CEdit           m_editName;
  CStatic         m_label;
  CEdit           m_editStartV;
  CButton         m_checkVisible;
  CMFCColorButton m_colorButton;
  CBitmapButton   m_buttonDelete;
public:
  CEquationEdit(CFont &font);
  ~CEquationEdit();
  void  Create(CWnd *parent, int eqIndex);
  void  DoDataExchange(CDataExchange* pDX);
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
  void  paramToWin(  const DiffEquationDescription &desc, const EquationAttributes &attr);
  void  winToParam(        DiffEquationDescription &desc,       EquationAttributes &attr);
};

class CDiffEquationGraphDlg : public CDialog {
private:
    HACCEL                       m_accelTable;
    SimpleLayoutManager          m_layoutManager;
    CFont                        m_exprFont;
    String                       m_fullName;
    CompactArray<CEquationEdit*> m_equationControlArray;
    CComboBox *getStyleCombo();
    void adjustWindowSize();
    void distributeEquationRectangles(int totalEquationsHeight);
    void adjustTabOrder(const CompactIntArray &oldTabOrder);

    bool validate();
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
    void addEquationToLOManager(     size_t index);
    void removeEquationFromLOManager(size_t index);
    int            findEquationIndexByCtrlId(UINT id); // return -1 if id does not belong to any equation
    CEquationEdit *findEquationByCtrlId(     UINT id); // return NULL if not in any diff-equation
    inline int     getFocusEquationIndex() {           // return -1 if not in any diff-equation
      return findEquationIndexByCtrlId(getFocusCtrlId(this));
    }
    inline CEquationEdit *getFocusEquation() {         // return NULL if not in any diff-equation
      return findEquationByCtrlId(getFocusCtrlId(this));
    }
    void gotoEquation(size_t index);
    int  getEquationCount() const {
      return (int)m_equationControlArray.size();
    }
    inline CEquationEdit *getEquationEdit(size_t index) const {
      return m_equationControlArray[index];
    }
    inline CEquationEdit *getLastEquationEdit() const {
      return getEquationCount() ? getEquationEdit(getEquationCount()-1) : NULL;
    }
    CRect getTotalEquationRect() const;

    DECLARE_DYNAMIC(CDiffEquationGraphDlg)

public:
	CDiffEquationGraphDlg(DiffEquationGraphParameters &param, CWnd *pParent = NULL);
	virtual ~CDiffEquationGraphDlg();
  DiffEquationGraphParameters &m_param;

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIFFEQUATIONGRAPH_DIALOG };
#endif
  CString	m_style;
  CString	m_name;
  double	m_maxError;
  double	m_xFrom;
  double	m_xTo;

protected:
  	virtual void DoDataExchange(CDataExchange* pDX);

  	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedButtonaddeq();
    afx_msg void OnBnClickedEquation(UINT id);
    virtual BOOL OnInitDialog();
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoMaxError();
    afx_msg void OnGotoXInterval();
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    afx_msg void OnBnClickedOk();
};
