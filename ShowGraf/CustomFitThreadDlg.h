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
private:
    HACCEL                    m_accelTable;
    CListCtrl                 m_dataList;
    CString                   m_name;
    CString                   m_expr;
    double                    m_xFrom;
    double                    m_xTo;

    CMFCColorButton *getColorButton() {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }
    COLORREF getColor() {
      return getColorButton()->GetColor();
    }
    void setColor(COLORREF color) {
      getColorButton()->SetColor(color);
    }
    DoubleInterval getXInterval() {
      return DoubleInterval(m_xFrom, m_xTo);
    }
    void setXInterval(const DoubleInterval &i) {
      m_xFrom = i.getFrom();
      m_xTo   = i.getTo();
    }
public:
    CCustomFitThreadDlg(const CString &expr, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, CWnd* pParent = NULL);

    enum { IDD = IDD_FITTHREAD_DIALOG };

    FunctionFitter           *m_functionFitter;
    FitThread                *m_worker;
    Array<CStatic*>           m_infoField;
    const Point2DArray       &m_pointArray;
    FunctionPlotter          &m_fp;
    bool                      m_running;
    FunctionGraphParameters   m_param;

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

  public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
  protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:

    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    virtual void OnOK();
    virtual void OnCancel();
    afx_msg void OnButtonStep();
    afx_msg void OnButtonSolve();
    afx_msg void OnButtonStop();
    afx_msg void OnButtonPlot();
    afx_msg void OnButtonRestart();
    afx_msg void OnGotoName();
    afx_msg void OnGotoFunction();
    afx_msg void OnGotoXInterval();

    DECLARE_MESSAGE_MAP()
};

