#pragma once

#include "IsoCurveGraph.h"
#include <MFCUtil/LayoutManager.h>

class CIsoCurveGraphDlg : public CDialog {
private:
    HACCEL              m_accelTable;
    SimpleLayoutManager m_layoutManager;
    CFont               m_exprFont;
    String              m_fullName;
    CComboBox *getStyleCombo();

    bool validate();
    void paramToWin(            const IsoCurveGraphParameters &param);
    void winToParam(                  IsoCurveGraphParameters &param);
    void saveAs(                      IsoCurveGraphParameters &param);
    void save(const String &fileName, IsoCurveGraphParameters &param);
    void addToRecent(const String &fileName);

public:
    CIsoCurveGraphDlg(IsoCurveGraphParameters &param, CWnd* pParent = NULL);
    IsoCurveGraphParameters &m_param;
    COLORREF                 m_color;

  enum { IDD = IDD_ISOCURVEGRAPH_DIALOG };
  CString	m_style;
  CString	m_expr;
  CString	m_name;
  double	m_cellSize;
  double	m_xFrom;
  double	m_xTo;
  double	m_yFrom;
  double	m_yTo;

  public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

  virtual BOOL OnInitDialog();
  virtual void OnOK();
    afx_msg void OnGotoName();
    afx_msg void OnGotoStyle();
    afx_msg void OnGotoExpr();
    afx_msg void OnGotoXInterval();
    afx_msg void OnGotoYInterval();
    afx_msg void OnGotoCellSize();
    afx_msg void OnButtonColor();
    afx_msg void OnPaint();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnFileOpen();
    afx_msg void OnFileSave();
    afx_msg void OnFileSaveAs();
    afx_msg void OnEditFindmatchingparentesis();
    DECLARE_MESSAGE_MAP()
};

