#pragma once

#include <MFCUtil/OBMButton.h>
#include "IntType.h"

using namespace std;

class CTestIntDlg : public CDialog {
private:
  OBMButton      m_copyOutToInButton;
  IntType        m_accumulator, m_memory;
  CString        m_streamInString, m_streamOutString;
  CString        m_streamState;
  IntegerType    m_winIntegerType;
  UINT           m_radix;
  int            m_width;
  CString        m_fillString;
  TCHAR          m_fill;
  BOOL           m_autoUpdateStreamOut;
  bool           m_showItActive, m_setAccTypeActive;

  void               setWinIntegerType(IntegerType type);
  inline IntegerType getWinIntegerType() const { return m_winIntegerType; }
  void               setAccIntegerType(IntegerType type);
  inline IntegerType getAccIntegerType() const { return m_accumulator.getType(); }
  void               intTypeToEditField(const IntType &i);
  IntType            editFieldToIntType();
  void               showIntTypeValue(const IntType &i, bool mem);
  void               setStreamOpIsMem(bool v);
  inline bool        getStreamOpIsMem() {
    return IsDlgButtonChecked(IDC_RADIOOPMEM) == BST_CHECKED;
  }
  IntType           &getSelectedStreamOp() { return getStreamOpIsMem() ? m_memory : m_accumulator; }
  void               setDefaultStreamParam() { paramStreamToWin(wstringstream()); }
  void               paramStreamToWin(wstringstream &stream);
  wstringstream     &paramWinToStream(wstringstream &stream);
  void               formatFlagsToWin(FormatFlags flags);
  FormatFlags        winToFormatFlags();
  void               fillCharToWin(wchar_t ch);
  wchar_t            winToFillChar();
  static CString     streamStateToString(wstringstream &sream);
  void               updateAcc();
  void               updateAccRadix();
  void               setWidth(    int width);
  void               autoClickStreamOut();
  void               markLabel(int id, bool marked);
public:
  CTestIntDlg(CWnd *pParent = NULL);
  enum { IDD = IDD_TESTINT_DIALOG };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);
  virtual BOOL PreTranslateMessage(MSG *pMsg);
  virtual void OnOK();
  virtual void OnCancel();
  virtual BOOL OnInitDialog();

  afx_msg void OnClose();
  afx_msg void OnBnClickedRadioTypeI32();
  afx_msg void OnBnClickedRadioTypeU32();
  afx_msg void OnBnClickedRadioTypeI64();
  afx_msg void OnBnClickedRadioTypeU64();
  afx_msg void OnBnClickedRadioTypeI128();
  afx_msg void OnBnClickedRadioTypeU128();
  afx_msg void OnChangeEditIntValue();
  afx_msg void OnEnSetFocusEditIntValue();
  afx_msg void OnEnChangeEditRadixValue();
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
  afx_msg void OnBnClickedButtonAnd();
  afx_msg void OnBnClickedButtonOr();
  afx_msg void OnBnClickedButtonXor();
  afx_msg void OnBnClickedButtonNeg();
  afx_msg void OnBnClickedButtonNot();
  afx_msg void OnBnClickedButtonSetZero();
  afx_msg void OnBnClickedButtonSetOne();
  afx_msg void OnBnClickedButtonSetMin();
  afx_msg void OnBnClickedButtonSetMax();
  afx_msg void OnBnClickedButtonSetRnd();
  afx_msg void OnDeltaPosSpinIntValue(   NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaposSpinRadixValue( NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnDeltaPosSpinWidthValue( NMHDR *pNMHDR, LRESULT *pResult);
  afx_msg void OnBnClickedRadioOpAcc();
  afx_msg void OnBnClickedRadioOpMem();
  afx_msg void OnBnClickedButtonStreamIn();
  afx_msg void OnBnClickedButtonStreamOut();
  afx_msg void OnBnClickedButtonCopyOutToIn();
  afx_msg void OnBnClickedCheckAutoUpdateStreamOut();
  afx_msg void OnBnClickedCheckIosFlag();
  afx_msg void OnEnChangeEditWidthValue();
  afx_msg void OnEnUpdateEditWidthValue();
  afx_msg void OnEnChangeEditFillValue();
  afx_msg void OnEnSetFocusEditFillValue();
  DECLARE_MESSAGE_MAP()
public:
  afx_msg void OnEnUpdateEditRadixValue();
};
