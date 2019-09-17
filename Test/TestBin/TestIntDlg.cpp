#include "stdafx.h"
#include "TestBin.h"
#include "TestIntDlg.h"
#include "HexEditOverwrite.h"

#ifdef _DEBUG
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
, m_autoUpdateStreamOut(FALSE)
, m_radix(2)
{
  m_showItActive     = false;
  m_setAccTypeActive = false;
}

void CTestIntDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(      pDX, IDC_EDITSTREAMINVALUE       , m_streamInString);
  DDX_Text(      pDX, IDC_EDITSTREAMOUTVALUE      , m_streamOutString);
  DDX_Text(      pDX, IDC_EDITWIDTHVALUE          , m_width);
  DDX_Text(      pDX, IDC_EDITFILLVALUE           , m_fillString);
  DDV_MaxChars(  pDX, m_fillString, 1);
  DDX_Text(      pDX, IDC_STATICSTREAMSTATEVALUE  , m_streamState);
  DDX_Check(     pDX, IDC_CHECKAUTOUPDATESTREAMOUT, m_autoUpdateStreamOut);
  DDX_Text(      pDX, IDC_EDITRADIXVALUE          , m_radix);
}


BEGIN_MESSAGE_MAP(CTestIntDlg, CDialog)
  ON_WM_CLOSE()
  ON_BN_CLICKED(          IDC_RADIOTYPEI32             , OnBnClickedRadioTypeI32            )
  ON_BN_CLICKED(          IDC_RADIOTYPEU32             , OnBnClickedRadioTypeU32            )
  ON_BN_CLICKED(          IDC_RADIOTYPEI64             , OnBnClickedRadioTypeI64            )
  ON_BN_CLICKED(          IDC_RADIOTYPEU64             , OnBnClickedRadioTypeU64            )
  ON_BN_CLICKED(          IDC_RADIOTYPEI128            , OnBnClickedRadioTypeI128           )
  ON_BN_CLICKED(          IDC_RADIOTYPEU128            , OnBnClickedRadioTypeU128           )
  ON_EN_CHANGE(           IDC_EDITINTVALUE             , OnChangeEditIntValue               )
  ON_EN_SETFOCUS(         IDC_EDITINTVALUE             , OnEnSetFocusEditIntValue           )
  ON_EN_CHANGE(           IDC_EDITRADIXVALUE           , OnEnChangeEditRadixValue           )
  ON_BN_CLICKED(          IDC_BUTTONSAVE               , OnBnClickedButtonSave              )
  ON_BN_CLICKED(          IDC_BUTTONLOAD               , OnBnClickedButtonLoad              )
  ON_BN_CLICKED(          IDC_BUTTONRESETMEM           , OnBnClickedButtonResetMem          )
  ON_BN_CLICKED(          IDC_BUTTONSWAPACCMEM         , OnBnClickedButtonSwapAccMem        )
  ON_BN_CLICKED(          IDC_BUTTONSETZERO            , OnBnClickedButtonSetZero           )
  ON_BN_CLICKED(          IDC_BUTTONSETONE             , OnBnClickedButtonSetOne            )
  ON_BN_CLICKED(          IDC_BUTTONSETMIN             , OnBnClickedButtonSetMin            )
  ON_BN_CLICKED(          IDC_BUTTONSETMAX             , OnBnClickedButtonSetMax            )
  ON_BN_CLICKED(          IDC_BUTTONSETRND             , OnBnClickedButtonSetRnd            )
  ON_BN_CLICKED(          IDC_BUTTONADD                , OnBnClickedButtonAdd               )
  ON_BN_CLICKED(          IDC_BUTTONSUB                , OnBnClickedButtonSub               )
  ON_BN_CLICKED(          IDC_BUTTONMULT               , OnBnClickedButtonMult              )
  ON_BN_CLICKED(          IDC_BUTTONDIV                , OnBnClickedButtonDiv               )
  ON_BN_CLICKED(          IDC_BUTTONFMOD               , OnBnClickedButtonFmod              )
  ON_BN_CLICKED(          IDC_BUTTONSQR                , OnBnClickedButtonSqr               )
  ON_BN_CLICKED(          IDC_BUTTONAND                , OnBnClickedButtonAnd               )
  ON_BN_CLICKED(          IDC_BUTTONOR                 , OnBnClickedButtonOr                )
  ON_BN_CLICKED(          IDC_BUTTONXOR                , OnBnClickedButtonXor               )
  ON_BN_CLICKED(          IDC_BUTTONNEG                , OnBnClickedButtonNeg               )
  ON_BN_CLICKED(          IDC_BUTTONNOT                , OnBnClickedButtonNot               )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPININTVALUE             , OnDeltaPosSpinIntValue             )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINRADIXVALUE           , OnDeltaposSpinRadixValue           )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINWIDTHVALUE           , OnDeltaPosSpinWidthValue           )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SKIPWS         , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_UNITBUF        , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_UPPERCASE      , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWBASE       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWPOINT      , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SHOWPOS        , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_LEFT           , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_RIGHT          , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_INTERNAL       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_DEC            , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_OCT            , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_HEX            , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_SCIENTIFIC     , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_FIXED          , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_HEXFLOAT       , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_BOOLALPHA      , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_CHECK_IOS_STDIO          , OnBnClickedCheckIosFlag            )
  ON_BN_CLICKED(          IDC_RADIOOPACC               , OnBnClickedRadioOpAcc              )
  ON_BN_CLICKED(          IDC_RADIOOPMEM               , OnBnClickedRadioOpMem              )
  ON_BN_CLICKED(          IDC_BUTTONSTREAMIN           , OnBnClickedButtonStreamIn          )
  ON_BN_CLICKED(          IDC_BUTTONSTREAMOUT          , OnBnClickedButtonStreamOut         )
  ON_BN_CLICKED(          IDC_BUTTONCOPYOUTTOIN        , OnBnClickedButtonCopyOutToIn       )
  ON_BN_CLICKED(          IDC_CHECKAUTOUPDATESTREAMOUT , OnBnClickedCheckAutoUpdateStreamOut)
  ON_EN_CHANGE(           IDC_EDITWIDTHVALUE           , OnEnChangeEditWidthValue           )
  ON_EN_UPDATE(           IDC_EDITWIDTHVALUE           , OnEnUpdateEditWidthValue           )
  ON_EN_CHANGE(           IDC_EDITFILLVALUE            , OnEnChangeEditFillValue            )
  ON_EN_SETFOCUS(         IDC_EDITFILLVALUE            , OnEnSetFocusEditFillValue          )
  ON_EN_UPDATE(IDC_EDITRADIXVALUE, &CTestIntDlg::OnEnUpdateEditRadixValue)
END_MESSAGE_MAP()

BOOL CTestIntDlg::OnInitDialog() {
  __super::OnInitDialog();
  showIntTypeValue(m_accumulator, false);
  showIntTypeValue(m_memory     , true );

  CButton *oldButton = ((CButton*)GetDlgItem(IDC_BUTTONCOPYOUTTOIN));
  CPoint p = getWindowPosition(this, IDC_BUTTONCOPYOUTTOIN);
  oldButton->DestroyWindow();
  m_copyOutToInButton.Create(this, OBMIMAGE(UPARROW), p, IDC_BUTTONCOPYOUTTOIN);

  setWindowText(this, IDC_EDITRADIXVALUE, _T("2"));
  setDefaultStreamParam();
  OnBnClickedCheckIosFlag();
  setStreamOpIsMem(false);
  GetDlgItem(IDC_EDITINTVALUE)->SetFocus();
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
  case IDC_EDITINTVALUE  :
    if(isOverwriteCurrentHexChar(this, pMsg)) {
      return true;
    }
    break;
  case IDC_EDITRADIXVALUE :
    if(isOverwriteCurrentDecChar(this, pMsg,2,36)) {
      return true;
    }
    break;
  case IDC_EDITFILLVALUE  :
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
void CTestIntDlg::OnEnSetFocusEditIntValue()     { setCaretPos((CEdit*)GetDlgItem(IDC_EDITINTVALUE  ), 0);               }
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
void CTestIntDlg::OnEnSetFocusEditFillValue()    { setCaretPos((CEdit*)GetDlgItem(IDC_EDITFILLVALUE), 0);                }

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

  wstring       str = (LPCTSTR)m_streamInString;
  wstringstream stream(str);
  IntType       tmp(m_accumulator);
  tmp.setZero();
  paramWinToStream(stream) >> tmp;
  if(stream) {
    showIntTypeValue(tmp, getStreamOpIsMem());
  }
  m_streamState = streamStateToString(stream);

  UpdateData(FALSE);
}

void CTestIntDlg::OnBnClickedButtonStreamOut() {
  UpdateData();

  wstringstream stream;
  paramWinToStream(stream) << getSelectedStreamOp();

  m_streamOutString = stream.str().c_str();
  m_streamState     = streamStateToString(stream);

  UpdateData(FALSE);
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
  setWindowText(this, IDC_STATICSTREAMFLAGSVALUE, format(_T("%08X"), flags));
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
  static const int typeRadioButtonId[] = { IDC_RADIOTYPEI32 ,IDC_RADIOTYPEU32
                                          ,IDC_RADIOTYPEI64 ,IDC_RADIOTYPEU64
                                          ,IDC_RADIOTYPEI128,IDC_RADIOTYPEU128
                                         };
  CheckRadioButton(IDC_RADIOTYPEI32, IDC_RADIOTYPEU128, typeRadioButtonId[type]);
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
  const int    displayWinId = mem ? IDC_STATICMEMVALUE : IDC_STATICACCVALUE;
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
    setWindowText(this, IDC_STATICACCBIN, m_accumulator.toBinString());
  } else {
    setWindowText(this, IDC_STATICACCBIN, m_accumulator.toString(m_radix));
  }
}

// ----------------------------------------- Edit field --------------------------------------------

void CTestIntDlg::intTypeToEditField(const IntType &i) {
  setWindowText(this, IDC_EDITINTVALUE, i.toHexString());
}

IntType CTestIntDlg::editFieldToIntType() {
  return IntType(getWinIntegerType(), getWindowText(this, IDC_EDITINTVALUE).cstr(), 16);
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
  const int radiobuttons[] = { IDC_RADIOOPACC, IDC_RADIOOPMEM };
  CheckRadioButton(radiobuttons[0], radiobuttons[1], radiobuttons[ordinal(v)]);
  markLabel(IDC_STATICMEMLABEL, v );
  markLabel(IDC_STATICACCLABEL, !v);
  autoClickStreamOut();
}

void CTestIntDlg::paramStreamToWin(wstringstream &stream) {
  setWidth(    (int)stream.width()    );
  formatFlagsToWin( stream.flags()    );
  fillCharToWin(    stream.fill()     );
}

wstringstream &CTestIntDlg::paramWinToStream(wstringstream &stream) {
  stream.width(m_width);
  stream.flags(winToFormatFlags());
  stream.fill(winToFillChar());
  return stream;
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

CString CTestIntDlg::streamStateToString(wstringstream &s) { // static
  String result;
  if(s) {
    result = _T("ok ");
  }
  if(s.fail()) result += _T("fail ");
  if(s.bad()) result  += _T("bad " );
  if(s.eof()) {
    result += _T("eof ");
  } else {
    const wchar_t next = s.peek();
    result += format(_T("next:'%c'"), next);
  }
  return result.cstr();
}
