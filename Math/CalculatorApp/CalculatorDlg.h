#pragma once

#include "Calculator.h"

class MyButton : public CButton {
private:
  CBitmap m_normalBitmap;
  CBitmap m_pressedBitmap;
  CBitmap m_disabledBitmap;
  bool m_pressed, m_enabled;
public:
  MyButton();
  void initBitmaps(CButton *button);
  void press(BOOL pressed);
  void enable(bool enabled);
  bool isPressed() const { return m_pressed; }
  bool isEnabled() const { return m_enabled; }
  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
  bool pointInWindow(const CPoint &p);
};

class CCalculatorDlg : public CDialog {
private:
  DWORD                   m_helpid;
  CalculatorThread       *m_calc;
  int                     m_timerInterval;
  time_t                  m_enterTime;
  bool                    m_waitCursorOn;
  CompactArray<MyButton*> m_buttons;
  MyButton               *m_selectedButton;
  String                  m_toolTipText;

  void activateButton(int id);
  void pushButton( MyButton *button);
  void pushButton(int id);
  MyButton *findButton(int id);
  MyButton *findButton(const CPoint &p);
  void setFocus();
  void startTimer(int msec);
  void stopTimer();
  void waitCursor(bool on);
  void enableButton(int id, bool enabled);
  void enableNumButtons(int radix);
  void setDecButtons();
  void setByteButtons();
  void scrollLine(int count);
  void info(_In_z_ _Printf_format_string_ TCHAR const * const format, ...);

public:
    HINSTANCE m_hInstance;
    HICON     m_hIcon;
    HACCEL    m_accelTabel;
    CCalculatorDlg(CWnd *pParent = NULL);
    ~CCalculatorDlg();
    void showStatus();
    void checkRadix(int radix);
    void checkTrigonometricBase(Trigonometric tm);
    void checkOpeandSize(OperandSize size);

  enum { IDD = IDR_MAINFRAME };
  CString   m_display;

  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL OnInitDialog();
  afx_msg void OnDestroy();
  afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg void OnTimer(UINT_PTR nIDEvent);
  afx_msg void OnContextMenu(CWnd *pWnd, CPoint point);
  afx_msg void OnClose();
  afx_msg void OnFileQuit();
  afx_msg void OnEditCopy();
  afx_msg void OnEditPaste();
  afx_msg void OnViewBin();
  afx_msg void OnViewOct();
  afx_msg void OnViewDec();
  afx_msg void OnViewHex();
  afx_msg void OnViewDigitGrouping();
  afx_msg void OnViewPrecision();
  afx_msg void OnHelpAbout();
  afx_msg void OnControlWhatsthis();
  afx_msg void OnSetfocusDisplay();
  afx_msg void OnCheckInv();
  afx_msg void OnCheckHyp();
  afx_msg void OnRadioBin();
  afx_msg void OnRadioOct();
  afx_msg void OnRadioDec();
  afx_msg void OnRadioHex();
  afx_msg void OnF2();
  afx_msg void OnF3();
  afx_msg void OnF4();
  afx_msg void OnF12();
  afx_msg void OnShftF12();
  afx_msg void OnCe();
  afx_msg void OnBack();
  afx_msg void OnClear();
  afx_msg void OnEqual();
  afx_msg void OnNumber0();
  afx_msg void OnNumber1();
  afx_msg void OnNumber2();
  afx_msg void OnNumber3();
  afx_msg void OnNumber4();
  afx_msg void OnNumber5();
  afx_msg void OnNumber6();
  afx_msg void OnNumber7();
  afx_msg void OnNumber8();
  afx_msg void OnNumber9();
  afx_msg void OnNumberA();
  afx_msg void OnNumberB();
  afx_msg void OnNumberC();
  afx_msg void OnNumberD();
  afx_msg void OnNumberE();
  afx_msg void OnNumberF();
  afx_msg void OnComma();
  afx_msg void OnSign();
  afx_msg void OnEE();
  afx_msg void OnAdd();
  afx_msg void OnSub();
  afx_msg void OnMult();
  afx_msg void OnDiv();
  afx_msg void OnMod();
  afx_msg void OnSquare();
  afx_msg void OnPow3();
  afx_msg void OnPow();
  afx_msg void OnLn();
  afx_msg void OnLog();
  afx_msg void OnSin();
  afx_msg void OnCos();
  afx_msg void OnTan();
  afx_msg void OnPi();
  afx_msg void OnReciproc();
  afx_msg void OnFac();
  afx_msg void OnDMS();
  afx_msg void OnAnd();
  afx_msg void OnOr();
  afx_msg void OnXor();
  afx_msg void OnLsh();
  afx_msg void OnNot();
  afx_msg void OnInt();
  afx_msg void OnIgnore();
  afx_msg void OnLpar();
  afx_msg void OnRpar();
  afx_msg void OnShowinfo();
  afx_msg BOOL OnToolTipNotify( UINT id, NMHDR *pNMHDR, LRESULT *pResult );
  DECLARE_MESSAGE_MAP()
};
