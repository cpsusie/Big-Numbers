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

    CMFCColorButton *getColorButton() const {
      return (CMFCColorButton*)GetDlgItem(IDC_BUTTONCOLOR);
    }
    COLORREF getColor() const {
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
    CCustomFitThreadDlg(const CString &expr, const DoubleInterval &range, const Point2DArray &pointArray, FunctionPlotter &fp, CWnd *pParent = nullptr);

    enum { IDD = IDR_CUSTOMFIT };

    FunctionFitter           *m_functionFitter;
    FitThread                *m_worker;
    CompactArray<CStatic*>    m_infoField;
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
    void printf(int field, _In_z_ _Printf_format_string_ TCHAR const * const format, ...);
    void showInfo();
    void startTimer();
    void stopTimer();

protected:
    virtual BOOL PreTranslateMessage(MSG *pMsg);
    virtual void DoDataExchange(CDataExchange *pDX);
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

