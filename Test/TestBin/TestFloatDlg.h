#pragma once

#include <MFCUtil/OBMButton.h>
#include "FloatFields.h"

using namespace std;

class CTestFloatDlg : public CDialog {
private:
  OBMButton      m_copyOutToInButton;
  FloatFields    m_accumulator, m_memory;
  CString        m_streamInString, m_streamOutString;
  CString        m_streamState;
  FloatType      m_winFloatType;
  int            m_width;
  int            m_precision;
  CString        m_fillString;
  TCHAR          m_fill;
  BOOL           m_manipStreamIn;
  BOOL           m_autoUpdateStreamOut;
  bool           m_showffActive, m_setAccTypeActive;

  void             setWinFloatType(FloatType type);
  inline FloatType getWinFloatType() const { return m_winFloatType; }
  void             setAccFloatType(FloatType type);
  inline FloatType getAccFloatType() const { return m_accumulator.getType(); }
  void             floatFieldsToEditFields(const FloatFields &ff);
  FloatFields      editFieldToFloatFields() {
    return FloatFields(getWinFloatType(), getSignBit(), getExpoField(), getSigField());
  }
  void             showFloatFieldsValue(const FloatFields &ff, bool mem);
  inline void      setSignBit(bool v) {
    CheckDlgButton(IDC_CHECKSIGNBIT, v ? BST_CHECKED : BST_UNCHECKED);
  }
  inline bool      getSignBit() {
    return IsDlgButtonChecked(IDC_CHECKSIGNBIT) == BST_CHECKED;
  }
  void             setExpoField(UINT  v, FloatType type);
  inline void      setExpoField(UINT  v) { setExpoField(v, getWinFloatType()); }
  UINT             getExpoField();
  void             setSigField(UINT64 v, FloatType type);
  inline void      setSigField(UINT64 v) { setSigField( v, getWinFloatType()); }
  UINT64           getSigField();
  void             setStreamOpIsMem(bool v);
  inline bool      getStreamOpIsMem() {
    return IsDlgButtonChecked(IDC_RADIOOPMEM) == BST_CHECKED;
  }
  FloatFields     &getSelectedStreamOp() { return getStreamOpIsMem() ? m_memory : m_accumulator; }
  void             setDefaultStreamParam() { paramStreamToWin(wstringstream()); }
  void             paramStreamToWin(wstringstream &stream);
  wstringstream   &paramWinToStream(wstringstream &stream);
  void             formatFlagsToWin(FormatFlags flags);
  FormatFlags      winToFormatFlags();
  void             fillCharToWin(wchar_t ch);
  wchar_t          winToFillChar();
  static CString   streamStateToString(wstringstream &sream);
  void             updateAcc();
  void             setWidth(    int width);
  void             setPrecision(int prec );
  void             updatePrecision();
  void             autoClickStreamOut();
  void             expo2ToWin();
  void             markLabel(int id, bool marked);
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
  afx_msg void OnBnClickedRadioTypeFloat();
  afx_msg void OnBnClickedRadioTypeDouble64();
  afx_msg void OnBnClickedRadioTypeDouble80();
  afx_msg void OnClickedCheckSignBit();
  afx_msg void OnChangeEditExpoValue();
  afx_msg void OnChangeEditSigValue();
  afx_msg void OnEnSetFocusEditExpoValue();
  afx_msg void OnEnSetFocusEditSigValue();
  afx_msg void OnBnClickedButtonSave();
  afx_msg void OnBnClickedButtonLoad();
  afx_msg void OnBnClickedButtonResetMem();
  afx_msg void OnBnClickedButtonSwapAccMem();
  afx_msg void OnBnClickedButtonAdd();
  afx_msg void OnBnClickedButtonSub();
  afx_msg void OnBnClickedButtonMult();
  afx_msg void OnBnClickedButtonDiv();
  afx_msg void OnBnClickedButtonFmod();
  afx_msg void OnBnClickedButtonReciproc();
  afx_msg void OnBnClickedButtonSqr();
  afx_msg void OnBnClickedButtonSqrt();
  afx_msg void OnBnClickedButtonSetZero();
  afx_msg void OnBnClickedButtonSetTrueMin();
  afx_msg void OnBnClickedButtonSetMin();
  afx_msg void OnBnClickedButtonSetEps();
  afx_msg void OnBnClickedButtonSetRnd();
  afx_msg void OnBnClickedButtonSetOne();
  afx_msg void OnBnClickedButtonSetMax();
  afx_msg void OnBnClickedButtonSetInf();
  afx_msg void OnBnClickedButtonSetQNaN();
  afx_msg void OnBnClickedButtonSetSNaN();
  afx_msg void OnDeltaPosSpinExpoValue(  NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaPosSpinSigValue(   NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaPosSpinPrecValue(  NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaPosSpinWidthValue( NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedCheckMaxPrec();
  afx_msg void OnBnClickedRadioOpAcc();
  afx_msg void OnBnClickedRadioOpMem();
  afx_msg void OnBnClickedButtonStreamIn();
  afx_msg void OnBnClickedButtonStreamOut();
  afx_msg void OnBnClickedButtonCopyOutToIn();
  afx_msg void OnBnClickedCheckAutoUpdateStreamOut();
  afx_msg void OnBnClickedCheckIosFlag();
  afx_msg void OnEnChangeEditWidthValue();
  afx_msg void OnEnChangeEditPrecValue();
  afx_msg void OnEnUpdateEditWidthValue();
  afx_msg void OnEnUpdateEditPrecValue();
  afx_msg void OnEnChangeEditFillValue();
  afx_msg void OnEnSetFocusEditFillValue();
  DECLARE_MESSAGE_MAP()
};
