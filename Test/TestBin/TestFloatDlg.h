#pragma once

#include "FloatFields.h"

class CTestFloatDlg : public CDialog {
private:
  FloatFields    m_accumulator, m_memory;
  CString        m_floatingPointAsString;
  int            m_width;
  int            m_precision;
  CString        m_fillString;
  TCHAR          m_fill;
  bool           m_showffActive, m_setAccTypeActive;

  void           setWinFloatType(FloatType type);
  FloatType      getWinFloatType();
  FloatType      getAccFloatType() const {
    return m_accumulator.getType();
  }
  void           setAccFloatType(FloatType type);
  void           floatFieldsToEditFields(const FloatFields &ff);
  void           expo2ToWin();
  FloatFields    editFieldToFloatFields();
  void           showFloatFieldsValue(const FloatFields &ff, bool mem);
  void           updateAccumulator();
  void           setSignField(bool v);
  bool           getSignField();
  void           setExpoField(UINT v, FloatType type);
  UINT           getExpoField();
  void           setSigField(UINT64 v, FloatType type);
  UINT64         getSigField();
  void           updatePrecision();
  bool           useFloatManip();
  bool           streamOperandIsMemory();
  FloatFields   &getSelectedOperand() {
    return streamOperandIsMemory() ? m_memory : m_accumulator;
  }
  FormatFlags    getStreamOutFormatFlags();
  wchar_t        getFillChar();
  CString        streamOutSelectedOp();
  void           streamInSelectedOp(const CString &str);
public:
  CTestFloatDlg(CWnd *pParent = NULL);
  enum { IDD = IDD_TESTFLOAT_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();

  afx_msg void OnClose();
  afx_msg void OnSelChangeComboFloatType();
  afx_msg void OnClickedCheckSignBit();
  afx_msg void OnChangeEditExpoField();
  afx_msg void OnChangeEditSigField();
  afx_msg void OnEnSetFocusEditExpoField();
  afx_msg void OnEnSetFocusEditSignificand();
  afx_msg void OnBnClickedButtonToFloat();
  afx_msg void OnBnClickedButtonToDouble();
  afx_msg void OnBnClickedButtonToDouble80();
  afx_msg void OnBnClickedButtonSave();
  afx_msg void OnBnClickedButtonLoad();
  afx_msg void OnBnClickedButtonResetMem();
  afx_msg void OnBnClickedButtonSwapAccMem();
  afx_msg void OnBnClickedButtonAdd();
  afx_msg void OnBnClickedButtonSub();
  afx_msg void OnBnClickedButtonMult();
  afx_msg void OnBnClickedButtonDiv();
  afx_msg void OnBnClickedButtonFmod();
  afx_msg void OnBnClickedButtonSqr();
  afx_msg void OnBnClickedButtonSqrt();
  afx_msg void OnBnClickedButtonSetZero();
  afx_msg void OnBnClickedButtonSetTrueMin();
  afx_msg void OnBnClickedButtonSetMin();
  afx_msg void OnBnClickedButtonSetEps();
  afx_msg void OnBnClickedButtonSetOne();
  afx_msg void OnBnClickedButtonSetMax();
  afx_msg void OnBnClickedButtonSetPInf();
  afx_msg void OnBnClickedButtonSetQNaN();
  afx_msg void OnBnClickedButtonSetSNaN();
  afx_msg void OnDeltaPosSpinpWidth(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaPosSpinPrecision(NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedCheckMaxPrecision();
  afx_msg void OnBnClickedButtonStreamOut();
  afx_msg void OnBnClickedButtonStreamIn();
  afx_msg void OnBnClickedCheckIosFlag();
  DECLARE_MESSAGE_MAP()
public:
};
