#pragma once

#include <Thread.h>
#include "FunctionFitter.h"

class CCustomFitThreadDlg;

class FitThread : public Thread {
private:
  FunctionFitter       &m_functionFitter;
  CCustomFitThreadDlg  &m_dlg;
  bool                  m_killed;
public:
  int m_loopCount;
  unsigned int run();
  FitThread(CCustomFitThreadDlg *dlg, FunctionFitter &functionFitter);
  void kill();
};

class CCustomFitThreadDlg : public CDialog {
public:
    CCustomFitThreadDlg(const CString &expr, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, CWnd* pParent = NULL);

    //{{AFX_DATA(CCustomFitThreadDlg)
	enum { IDD = IDD_FITTHREAD_DIALOG };
	CListCtrl	m_dataList;
    CString     m_expr;
    double      m_xfrom;
    double      m_xto;
    CString     m_name;
	//}}AFX_DATA

    HACCEL                    m_accelTable;
    FunctionFitter           *m_functionFitter;
    FitThread                *m_worker;
    Array<CStatic*>           m_infoField;
    const Point2DArray       &m_pointArray;
    FunctionPlotter          &m_fp;
    bool                      m_running;
    COLORREF                  m_color;
    ExpressionGraphParameters m_param;
    void allocateInfoFields(int n);
    void deallocateInfoFields();
    void allocateFunctionFitter();
    void deallocateFunctionFitter();
    void updateButtons();
    void startWorker(int loopCounter=0);
    CStatic *infofield(int i);
    void printf(int field, const TCHAR *format, ...);
    void showInfo();
    void startTimer();
    void stopTimer();

    //{{AFX_VIRTUAL(CCustomFitThreadDlg)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
    virtual void DoDataExchange(CDataExchange* pDX);
	//}}AFX_VIRTUAL

protected:

    //{{AFX_MSG(CCustomFitThreadDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnPaint();
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnButtonStep();
    afx_msg void OnButtonSolve();
    afx_msg void OnButtonStop();
    afx_msg void OnButtonPlot();
    afx_msg void OnButtonRestart();
    afx_msg void OnButtonColor();
    afx_msg void OnGotoName();
    afx_msg void OnGotoFunction();
    afx_msg void OnGotoXInterval();

    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
