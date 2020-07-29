#include "stdafx.h"
#include <StrStream.h>
#include "TestBin.h"
#include "TestIntDlg.h"
#include "HexEditOverwrite.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

using namespace std;

CTestIntDlg::CTestIntDlg(CWnd *pParent)
: CDialog(CTestIntDlg::IDD, pParent)
, m_streamInString(_T(""))
, m_streamOutString(_T(""))
, m_width(6)
, m_fillString(_T(" "))
, m_streamState(_T(""))
, m_widecharStream(     FALSE)
, m_autoUpdateStreamOut(FALSE)
, m_radix(2)
{
  m_showItActive     = false;
  m_setAccTypeActive = false;
}

void CTestIntDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(      pDX, IDC_EDIT_STREAMINVALUE       , m_streamInString     );
  DDX_Text(      pDX, IDC_EDIT_STREAMOUTVALUE      , m_streamOutString    );
  DDX_Text(      pDX, IDC_EDIT_WIDTHVALUE          , m_width              );
  DDX_Text(      pDX, IDC_EDIT_FILLVALUE           , m_fillString         );
  DDV_MaxChars(  pDX, m_fillString, 1);
  DDX_Text(      pDX, IDC_STATIC_STREAMSTATEVALUE  , m_streamState        );
  DDX_Check(     pDX, IDC_CHECK_WIDECHARSTREAM     , m_widecharStream     );
  DDX_Check(     pDX, IDC_CHECK_AUTOUPDATESTREAMOUT, m_autoUpdateStreamOut);
  DDX_Text(      pDX, IDC_EDIT_RADIXVALUE          , m_radix              );
}

BEGIN_MESSAGE_MAP(CTestIntDlg, CDialog)
  ON_WM_CLOSE()
  ON_BN_CLICKED(          IDC_RADIO_TYPEI32             , OnBnClickedRadioTypeI32            )
  ON_BN_CLICKED(          IDC_RADIO_TYPEU32             , OnBnClickedRadioTypeU32            )
  ON_BN_CLICKED(          IDC_RADIO_TYPEI64             , OnBnClickedRadioTypeI64            )
  ON_BN_CLICKED(          IDC_RADIO_TYPEU64             , OnBnClickedRadioTypeU64            )
  ON_BN_CLICKED(          IDC_RADIO_TYPEI128            , OnBnClickedRadioTypeI128           )
  ON_BN_CLICKED(          IDC_RADIO_TYPEU128            , OnBnClickedRadioTypeU128           )
  ON_EN_SETFOCUS(         IDC_EDIT_INTVALUE             , OnEnSetFocusEditIntValue           )
  ON_EN_CHANGE(           IDC_EDIT_INTVALUE             , OnChangeEditIntValue               )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_INTVALUE             , OnDeltaPosSpinIntValue             )
  ON_EN_CHANGE(           IDC_EDIT_RADIXVALUE           , OnEnChangeEditRadixValue           )
  ON_EN_UPDATE(           IDC_EDIT_RADIXVALUE           , OnEnUpdateEditRadixValue           )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_RADIXVALUE           , OnDeltaposSpinRadixValue           )
  ON_BN_CLICKED(          IDC_BUTTON_SAVE               , OnBnClickedButtonSave              )
  ON_BN_CLICKED(          IDC_BUTTON_LOAD               , OnBnClickedButtonLoad              )
  ON_BN_CLICKED(          IDC_BUTTON_RESETMEM           , OnBnClickedButtonResetMem          )
  ON_BN_CLICKED(          IDC_BUTTON_SWAPACCMEM         , OnBnClickedButtonSwapAccMem        )
  ON_BN_CLICKED(          IDC_BUTTON_SETZERO            , OnBnClickedButtonSetZero           )
  ON_BN_CLICKED(          IDC_BUTTON_SETONE             , OnBnClickedButtonSetOne            )
  ON_BN_CLICKED(          IDC_BUTTON_SETMIN             , OnBnClickedButtonSetMin            )
  ON_BN_CLICKED(          IDC_BUTTON_SETMAX             , OnBnClickedButtonSetMax            )
  ON_BN_CLICKED(          IDC_BUTTON_SETRND             , OnBnClickedButtonSetRnd            )
  ON_BN_CLICKED(          IDC_BUTTON_ADD                , OnBnClickedButtonAdd               )
  ON_BN_CLICKED(          IDC_BUTTON_SUB                , OnBnClickedButtonSub               )
  ON_BN_CLICKED(          IDC_BUTTON_MULT               , OnBnClickedButtonMult              )
  ON_BN_CLICKED(          IDC_BUTTON_DIV                , OnBnClickedButtonDiv               )
  ON_BN_CLICKED(          IDC_BUTTON_FMOD               , OnBnClickedButtonFmod              )
  ON_BN_CLICKED(          IDC_BUTTON_SQR                , OnBnClickedButtonSqr               )
  ON_BN_CLICKED(          IDC_BUTTON_AND                , OnBnClickedButtonAnd               )
  ON_BN_CLICKED(          IDC_BUTTON_OR                 , OnBnClickedButtonOr                )
  ON_BN_CLICKED(          IDC_BUTTON_XOR                , OnBnClickedButtonXor               )
  ON_BN_CLICKED(          IDC_BUTTON_NEG                , OnBnClickedButtonNeg               )
  ON_BN_CLICKED(          IDC_BUTTON_NOT                , OnBnClickedButtonNot               )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SKIPWS          , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_UNITBUF         , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_UPPERCASE       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWBASE        , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWPOINT       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWPOS         , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_LEFT            , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_RIGHT           , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_INTERNAL        , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_DEC             , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_OCT             , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_HEX             , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SCIENTIFIC      , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_FIXED           , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_HEXFLOAT        , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_BOOLALPHA       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_STDIO           , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_RADIO_OPACC               , OnBnClickedRadioOpAcc              )
  ON_BN_CLICKED(          IDC_RADIO_OPMEM               , OnBnClickedRadioOpMem              )
  ON_BN_CLICKED(          IDC_BUTTON_STREAMIN           , OnBnClickedButtonStreamIn          )
  ON_BN_CLICKED(          IDC_BUTTON_STREAMOUT          , OnBnClickedButtonStreamOut         )
  ON_BN_CLICKED(          IDC_BUTTON_COPYOUTTOIN        , OnBnClickedButtonCopyOutToIn       )
  ON_BN_CLICKED(          IDC_CHECK_WIDECHARSTREAM      , OnBnClickedCheckWideCharStream     )
  ON_BN_CLICKED(          IDC_CHECK_AUTOUPDATESTREAMOUT , OnBnClickedCheckAutoUpdateStreamOut)
  ON_EN_CHANGE(           IDC_EDIT_WIDTHVALUE           , OnEnChangeEditWidthValue           )
  ON_EN_UPDATE(           IDC_EDIT_WIDTHVALUE           , OnEnUpdateEditWidthValue           )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_WIDTHVALUE           , OnDeltaPosSpinWidthValue           )
  ON_EN_CHANGE(           IDC_EDIT_FILLVALUE            , OnEnChangeEditFillValue            )
  ON_EN_SETFOCUS(         IDC_EDIT_FILLVALUE            , OnEnSetFocusEditFillValue          )
END_MESSAGE_MAP()

BOOL CTestIntDlg::OnInitDialog() {
  __super::OnInitDialog();
  showIntTypeValue(m_accumulator, false);
  showIntTypeValue(m_memory     , true );

  CButton *oldButton = ((CButton*)GetDlgItem(IDC_BUTTON_COPYOUTTOIN));
  CPoint p = getWindowPosition(this, IDC_BUTTON_COPYOUTTOIN);
  oldButton->DestroyWindow();
  m_copyOutToInButton.Create(this, OBMIMAGE(UPARROW), p, IDC_BUTTON_COPYOUTTOIN);

  setWindowText(this, IDC_EDIT_RADIXVALUE, _T("2"));
  setDefaultStreamParam();
  OnBnClickedCheckIosFlag();
  setStreamOpIsMem(false);
  GetDlgItem(IDC_EDIT_INTVALUE)->SetFocus();
  return FALSE;
}

void CTestIntDlg::OnOK() {
}

void CTestIntDlg::OnCancel() {
}

void CTestIntDlg::OnClose() {
  EndDialog(IDOK);
}

BOOL CTestIntDlg::PreTranslateMessage(MSG *pMsg) {
  switch(getFocusCtrlId(this)) {
  case IDC_EDIT_INTVALUE  :
    if(isOverwriteCurrentHexChar(this, pMsg)) {
      return true;
    }
    break;
  case IDC_EDIT_RADIXVALUE :
    if(isOverwriteCurrentDecChar(this, pMsg,2,36)) {
      return true;
    }
    break;
  case IDC_EDIT_FILLVALUE  :
    if(isOverwriteCurrentChar(this, pMsg)) {
      return true;
    }
    break;
  }
  try {
    return __super::PreTranslateMessage(pMsg);
  } catch(Exception e) {
    showWarning(_T("%s"), e.what());
    return true;
  }
}

void CTestIntDlg::OnBnClickedRadioTypeI32()      { setAccIntegerType(IT_I32     );                                       }
void CTestIntDlg::OnBnClickedRadioTypeU32()      { setAccIntegerType(IT_U32     );                                       }
void CTestIntDlg::OnBnClickedRadioTypeI64()      { setAccIntegerType(IT_I64     );                                       }
void CTestIntDlg::OnBnClickedRadioTypeU64()      { setAccIntegerType(IT_U64     );                                       }
void CTestIntDlg::OnBnClickedRadioTypeI128()     { setAccIntegerType(IT_I128    );                                       }
void CTestIntDlg::OnBnClickedRadioTypeU128()     { setAccIntegerType(IT_U128    );                                       }
void CTestIntDlg::OnChangeEditIntValue()         { updateAcc();                                                          }
void CTestIntDlg::OnEnSetFocusEditIntValue()     { setCaretPos((CEdit*)GetDlgItem(IDC_EDIT_INTVALUE  ), 0);              }
void CTestIntDlg::updateAcc()                    { showIntTypeValue(editFieldToIntType()                       , false); }
void CTestIntDlg::OnBnClickedButtonSave()        { showIntTypeValue(m_accumulator                              , true ); }
void CTestIntDlg::OnBnClickedButtonLoad()        { showIntTypeValue(m_memory                                   , false); }
void CTestIntDlg::OnBnClickedButtonResetMem()    { showIntTypeValue(IntType(m_accumulator).setZero()           , true ); }
void CTestIntDlg::OnBnClickedButtonSetZero()     { showIntTypeValue(IntType(m_accumulator).setZero()           , false); }
void CTestIntDlg::OnBnClickedButtonSetOne()      { showIntTypeValue(IntType(m_accumulator).setOne()            , false); }
void CTestIntDlg::OnBnClickedButtonSetMin()      { showIntTypeValue(IntType(m_accumulator).setMin()            , false); }
void CTestIntDlg::OnBnClickedButtonSetMax()      { showIntTypeValue(IntType(m_accumulator).setMax()            , false); }
void CTestIntDlg::OnBnClickedButtonSetRnd()      { showIntTypeValue(IntType(m_accumulator).setRnd()            , false); }
void CTestIntDlg::OnBnClickedButtonAdd()         { showIntTypeValue(m_accumulator + m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonSub()         { showIntTypeValue(m_accumulator - m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonMult()        { showIntTypeValue(m_accumulator * m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonDiv()         { showIntTypeValue(m_accumulator / m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonFmod()        { showIntTypeValue(m_accumulator % m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonSqr()         { showIntTypeValue(sqr( m_accumulator)                        , false); }
void CTestIntDlg::OnBnClickedButtonAnd()         { showIntTypeValue(m_accumulator & m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonOr()          { showIntTypeValue(m_accumulator | m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonXor()         { showIntTypeValue(m_accumulator ^ m_memory                   , false); }
void CTestIntDlg::OnBnClickedButtonNeg()         { showIntTypeValue(-m_accumulator                             , false); }
void CTestIntDlg::OnBnClickedButtonNot()         { showIntTypeValue(~m_accumulator                             , false); }
void CTestIntDlg::OnBnClickedRadioOpAcc()        { setStreamOpIsMem(false);                                              }
void CTestIntDlg::OnBnClickedRadioOpMem()        { setStreamOpIsMem(true);                                               }
void CTestIntDlg::OnEnChangeEditWidthValue()     { autoClickStreamOut();                                                 }
void CTestIntDlg::OnEnUpdateEditWidthValue()     { autoClickStreamOut();                                                 }
void CTestIntDlg::OnEnChangeEditFillValue()      { autoClickStreamOut();                                                 }
void CTestIntDlg::OnEnSetFocusEditFillValue()    { setCaretPos((CEdit*)GetDlgItem(IDC_EDIT_FILLVALUE), 0);               }

void CTestIntDlg::OnBnClickedButtonSwapAccMem() {
  const IntType mem = m_memory, acc = m_accumulator;
  showIntTypeValue(mem, false);
  showIntTypeValue(acc, true );
}

void CTestIntDlg::OnDeltaPosSpinIntValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const int  add       = -pNMUpDown->iDelta;
  IntType    tmp       = m_accumulator;
  if(add > 0) {
    if(m_accumulator != tmp.setMax()) {
      showIntTypeValue(m_accumulator + tmp.setOne(), false);
    }
  } else if(add < 0) {
    if(m_accumulator != tmp.setMin()) {
      showIntTypeValue(m_accumulator - tmp.setOne(), false);
    }
  }
  *pResult = 0;
}

void CTestIntDlg::OnEnChangeEditRadixValue() {
  const UINT oldValue = m_radix;
  if(!UpdateData()) {
    m_radix = oldValue;
    UpdateData(FALSE);
  } else {
    updateAccRadix();
  }
}

void CTestIntDlg::OnEnUpdateEditRadixValue() {
  updateAccRadix();
}

void CTestIntDlg::OnDeltaposSpinRadixValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const int  add       = -pNMUpDown->iDelta;
  const int  newValue  = m_radix + add;
  if((2 <= newValue) && (newValue <= 36)) {
    m_radix = newValue;
    UpdateData(FALSE);
    updateAccRadix();
  }
  *pResult = 0;
}

void CTestIntDlg::OnDeltaPosSpinWidthValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UpdateData();
  setWidth(max(m_width - pNMUpDown->iDelta, 0));
  *pResult = 0;
}

void CTestIntDlg::OnBnClickedButtonStreamIn() {
  UpdateData();

  if(m_widecharStream) {
    wstring       str = (LPCTSTR)m_streamInString;
    wstringstream stream(str);
    IntType       tmp(m_accumulator);
    tmp.setZero();
    paramWinToStream(stream) >> tmp;
    if(stream) {
      showIntTypeValue(tmp, getStreamOpIsMem());
    }
    m_streamState = streamStateToString(stream).cstr();
  } else {
    USES_CONVERSION;
    const char  *instringascii = T2A((LPCTSTR)m_streamInString);
    const string str = instringascii;
    stringstream stream(str);
    IntType      tmp(m_accumulator);
    tmp.setZero();
    paramWinToStream(stream) >> tmp;
    if(stream) {
      showIntTypeValue(tmp, getStreamOpIsMem());
    }
    m_streamState = streamStateToString(stream).cstr();
  }
  UpdateData(FALSE);
}

void CTestIntDlg::OnBnClickedButtonStreamOut() {
  UpdateData();

  if(m_widecharStream) {
    wstringstream stream;
    paramWinToStream(stream) << getSelectedStreamOp();
    m_streamOutString = stream.str().c_str();
    m_streamState     = streamStateToString(stream).cstr();
  } else {
    stringstream stream;
    paramWinToStream(stream) << getSelectedStreamOp();
    m_streamOutString = stream.str().c_str();
    m_streamState     = streamStateToString(stream).cstr();
  }
  UpdateData(FALSE);
}

void CTestIntDlg::OnBnClickedCheckWideCharStream() {
  UpdateData();
  autoClickStreamOut();
}

void CTestIntDlg::OnBnClickedCheckAutoUpdateStreamOut() {
  UpdateData();
  autoClickStreamOut();
}

void CTestIntDlg::OnBnClickedButtonCopyOutToIn() {
  UpdateData();
  m_streamInString = m_streamOutString;
  UpdateData(FALSE);
}

void CTestIntDlg::OnBnClickedCheckIosFlag() {
  const FormatFlags flags = winToFormatFlags();
  setWindowText(this, IDC_STATIC_STREAMFLAGSVALUE, format(_T("%08X"), flags));
  autoClickStreamOut();
}

// -------------------------------------------------------------------------------------

void CTestIntDlg::setAccIntegerType(IntegerType type) {
  if(m_setAccTypeActive || (type == getAccIntegerType())) {
    return;
  }
  m_setAccTypeActive = true;
  IntType acc = m_accumulator, mem = m_memory;
  showIntTypeValue(acc.setType(type), false);
  showIntTypeValue(mem.setType(type), true);
  m_setAccTypeActive = false;
}

void CTestIntDlg::setWinIntegerType(IntegerType type) {
  static const int typeRadioButtonId[] = { IDC_RADIO_TYPEI32 ,IDC_RADIO_TYPEU32
                                          ,IDC_RADIO_TYPEI64 ,IDC_RADIO_TYPEU64
                                          ,IDC_RADIO_TYPEI128,IDC_RADIO_TYPEU128
                                         };
  CheckRadioButton(IDC_RADIO_TYPEI32, IDC_RADIO_TYPEU128, typeRadioButtonId[type]);
  m_winIntegerType = type;
}

void CTestIntDlg::setWidth(int width) {
  if(width != m_width) {
    m_width = width;
    UpdateData(FALSE);
    autoClickStreamOut();
  }
}

void CTestIntDlg::autoClickStreamOut() {
  if(m_autoUpdateStreamOut) {
    OnBnClickedButtonStreamOut();
  }
}

void CTestIntDlg::showIntTypeValue(const IntType &i, bool mem) {
  if(m_showItActive) return;
  m_showItActive = true;
  const String displayStr   = i.getDisplayString();
  const int    displayWinId = mem ? IDC_STATIC_MEMVALUE : IDC_STATIC_ACCVALUE;
  if(!mem) {
    m_accumulator = i;
    setWinIntegerType(i.getType());
    intTypeToEditField(m_accumulator);
    updateAccRadix();
  } else {
    m_memory = i;
  }
  setWindowText(this, displayWinId, displayStr);
  if(mem && (i.getType() != getWinIntegerType())) {
    showWarning(_T("Memory-IntegerType=%s. .win.IntegerType=%s")
               ,toString(i.getType()).cstr(), toString(getWinIntegerType()).cstr());
  }
  if(mem == getStreamOpIsMem()) {
    autoClickStreamOut();
  }
  m_showItActive = false;
}

void CTestIntDlg::updateAccRadix() {
  if(m_radix == 2) {
    setWindowText(this, IDC_STATIC_ACCBIN, m_accumulator.toBinString());
  } else {
    setWindowText(this, IDC_STATIC_ACCBIN, m_accumulator.toString(m_radix));
  }
}

// ----------------------------------------- Edit field --------------------------------------------

void CTestIntDlg::intTypeToEditField(const IntType &i) {
  setWindowText(this, IDC_EDIT_INTVALUE, i.toHexString());
}

IntType CTestIntDlg::editFieldToIntType() {
  return IntType(getWinIntegerType(), getWindowText(this, IDC_EDIT_INTVALUE).cstr(), 16);
}

void CTestIntDlg::markLabel(int id, bool marked) {
  const DWORD change = WS_BORDER  | SS_SUNKEN;
  const DWORD flags  = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW;
  CStatic *st = (CStatic*)GetDlgItem(id);
  if(marked) {
    st->ModifyStyle(  0, change  , flags);
  } else {
    st->ModifyStyle(  change  , 0, flags);
  }
}

void CTestIntDlg::setStreamOpIsMem(bool v) {
  const int radiobuttons[] = { IDC_RADIO_OPACC, IDC_RADIO_OPMEM };
  CheckRadioButton(radiobuttons[0], radiobuttons[1], radiobuttons[ordinal(v)]);
  markLabel(IDC_STATIC_MEMLABEL, v );
  markLabel(IDC_STATIC_ACCLABEL, !v);
  autoClickStreamOut();
}

StreamParameters CTestIntDlg::winToStreamParameters() {
  return StreamParameters(0, m_width, winToFormatFlags(), winToFillChar());
}

void CTestIntDlg::streamParametersToWin(const StreamParameters &param) {
  setWidth(        (int)param.width());
  formatFlagsToWin(param.flags());
  fillCharToWin(   param.fill());
}

wstringstream &CTestIntDlg::paramWinToStream(wstringstream &stream) {
  return setFormat(stream, winToStreamParameters());
}

stringstream &CTestIntDlg::paramWinToStream(stringstream  &stream) {
  return setFormat(stream, winToStreamParameters());
}

struct ButtunIdFlag {
  int         m_id;
  FormatFlags m_flag;
};

static const ButtunIdFlag buttonFlagArray[] = {
  IDC_CHECK_IOS_SKIPWS    , ios::skipws
 ,IDC_CHECK_IOS_UNITBUF   , ios::unitbuf
 ,IDC_CHECK_IOS_UPPERCASE , ios::uppercase
 ,IDC_CHECK_IOS_SHOWBASE  , ios::showbase
 ,IDC_CHECK_IOS_SHOWPOINT , ios::showpoint
 ,IDC_CHECK_IOS_SHOWPOS   , ios::showpos
 ,IDC_CHECK_IOS_LEFT      , ios::left
 ,IDC_CHECK_IOS_RIGHT     , ios::right
 ,IDC_CHECK_IOS_INTERNAL  , ios::internal
 ,IDC_CHECK_IOS_DEC       , ios::dec
 ,IDC_CHECK_IOS_OCT       , ios::oct
 ,IDC_CHECK_IOS_HEX       , ios::hex
 ,IDC_CHECK_IOS_SCIENTIFIC, ios::scientific
 ,IDC_CHECK_IOS_FIXED     , ios::fixed
 ,IDC_CHECK_IOS_HEXFLOAT  , ios::hexfloat
 ,IDC_CHECK_IOS_BOOLALPHA , ios::boolalpha
 ,IDC_CHECK_IOS_STDIO     , ios::_Stdio
};

void CTestIntDlg::formatFlagsToWin(FormatFlags flags) {
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    CheckDlgButton(idFlag.m_id, (flags & idFlag.m_flag) ? BST_CHECKED : BST_UNCHECKED);
  }
}

FormatFlags CTestIntDlg::winToFormatFlags() {
  FormatFlags result = 0;
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    if(IsDlgButtonChecked(idFlag.m_id) == BST_CHECKED) {
      result |= idFlag.m_flag;
    }
  }
  return result;
}

void CTestIntDlg::fillCharToWin(wchar_t ch) {
  if(m_fillString.GetLength() == 0) {
    m_fillString = _T(" ");
  }
  m_fillString.SetAt(0, ch);
}

wchar_t CTestIntDlg::winToFillChar() {
  return (m_fillString.GetLength() == 0) ? _T(' ') : m_fillString.GetAt(0);
}
