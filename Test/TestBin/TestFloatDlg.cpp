#include "stdafx.h"
#include <StrStream.h>
#include "TestBin.h"
#include "TestFloatDlg.h"
#include "HexEditOverwrite.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

using namespace std;

CTestFloatDlg::CTestFloatDlg(CWnd *pParent)
: CDialog(CTestFloatDlg::IDD, pParent)
, m_streamInString(_T(""))
, m_streamOutString(_T(""))
, m_width(6)
, m_precision(2)
, m_fillString(_T(" "))
, m_streamState(_T(""))
, m_manipStreamIn(FALSE)
, m_widecharStream(FALSE)
, m_autoUpdateStreamOut(FALSE)
{
  m_showffActive     = false;
  m_setAccTypeActive = false;
}

void CTestFloatDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_CBString( pDX, IDC_COMBO_STREAMINVALUE      , m_streamInString     );
  DDX_Text(     pDX, IDC_EDIT_STREAMOUTVALUE      , m_streamOutString    );
  DDX_Text(     pDX, IDC_EDIT_WIDTHVALUE          , m_width              );
  DDX_Text(     pDX, IDC_EDIT_PRECVALUE           , m_precision          );
  DDX_Text(     pDX, IDC_EDIT_FILLVALUE           , m_fillString         );
  DDV_MaxChars( pDX                               , m_fillString, 1      );
  DDX_Text(     pDX, IDC_STATIC_STREAMSTATEVALUE  , m_streamState        );
  DDX_Check(    pDX, IDC_CHECK_MANIPSTREAMIN      , m_manipStreamIn      );
  DDX_Check(    pDX, IDC_CHECK_WIDECHARSTREAM     , m_widecharStream     );
  DDX_Check(    pDX, IDC_CHECK_AUTOUPDATESTREAMOUT, m_autoUpdateStreamOut);
}

BEGIN_MESSAGE_MAP(CTestFloatDlg, CDialog)
  ON_WM_CLOSE()
  ON_BN_CLICKED(          IDC_RADIO_TYPEFLOAT           , OnBnClickedRadioTypeFloat          )
  ON_BN_CLICKED(          IDC_RADIO_TYPEDOUBLE64        , OnBnClickedRadioTypeDouble64       )
  ON_BN_CLICKED(          IDC_RADIO_TYPEDOUBLE80        , OnBnClickedRadioTypeDouble80       )
  ON_BN_CLICKED(          IDC_CHECK_SIGNBIT             , OnClickedCheckSignBit              )
  ON_EN_SETFOCUS(         IDC_EDIT_EXPOVALUE            , OnEnSetFocusEditExpoValue          )
  ON_EN_CHANGE(           IDC_EDIT_EXPOVALUE            , OnChangeEditExpoValue              )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXPOVALUE            , OnDeltaPosSpinExpoValue            )
  ON_EN_SETFOCUS(         IDC_EDIT_SIGVALUE             , OnEnSetFocusEditSigValue           )
  ON_EN_CHANGE(           IDC_EDIT_SIGVALUE             , OnChangeEditSigValue               )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_SIGVALUE             , OnDeltaPosSpinSigValue             )
  ON_BN_CLICKED(          IDC_BUTTON_SAVE               , OnBnClickedButtonSave              )
  ON_BN_CLICKED(          IDC_BUTTON_LOAD               , OnBnClickedButtonLoad              )
  ON_BN_CLICKED(          IDC_BUTTON_RESETMEM           , OnBnClickedButtonResetMem          )
  ON_BN_CLICKED(          IDC_BUTTON_SWAPACCMEM         , OnBnClickedButtonSwapAccMem        )
  ON_BN_CLICKED(          IDC_BUTTON_SETZERO            , OnBnClickedButtonSetZero           )
  ON_BN_CLICKED(          IDC_BUTTON_SETTRUEMIN         , OnBnClickedButtonSetTrueMin        )
  ON_BN_CLICKED(          IDC_BUTTON_SETMIN             , OnBnClickedButtonSetMin            )
  ON_BN_CLICKED(          IDC_BUTTON_SETEPS             , OnBnClickedButtonSetEps            )
  ON_BN_CLICKED(          IDC_BUTTON_SETRND             , OnBnClickedButtonSetRnd            )
  ON_BN_CLICKED(          IDC_BUTTON_SETONE             , OnBnClickedButtonSetOne            )
  ON_BN_CLICKED(          IDC_BUTTON_SETMAX             , OnBnClickedButtonSetMax            )
  ON_BN_CLICKED(          IDC_BUTTON_SETINF             , OnBnClickedButtonSetInf            )
  ON_BN_CLICKED(          IDC_BUTTON_SETQNAN            , OnBnClickedButtonSetQNaN           )
  ON_BN_CLICKED(          IDC_BUTTON_SETSNAN            , OnBnClickedButtonSetSNaN           )
  ON_BN_CLICKED(          IDC_BUTTON_ADD                , OnBnClickedButtonAdd               )
  ON_BN_CLICKED(          IDC_BUTTON_SUB                , OnBnClickedButtonSub               )
  ON_BN_CLICKED(          IDC_BUTTON_MULT               , OnBnClickedButtonMult              )
  ON_BN_CLICKED(          IDC_BUTTON_DIV                , OnBnClickedButtonDiv               )
  ON_BN_CLICKED(          IDC_BUTTON_FMOD               , OnBnClickedButtonFmod              )
  ON_BN_CLICKED(          IDC_BUTTON_RECIPROC           , OnBnClickedButtonReciproc          )
  ON_BN_CLICKED(          IDC_BUTTON_SQR                , OnBnClickedButtonSqr               )
  ON_BN_CLICKED(          IDC_BUTTON_SQRT               , OnBnClickedButtonSqrt              )
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
  ON_EN_CHANGE(           IDC_EDIT_PRECVALUE            , OnEnChangeEditPrecValue            )
  ON_EN_UPDATE(           IDC_EDIT_PRECVALUE            , OnEnUpdateEditPrecValue            )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_PRECVALUE            , OnDeltaPosSpinPrecValue            )
  ON_BN_CLICKED(          IDC_CHECK_MAXPREC             , OnBnClickedCheckMaxPrec            )
  ON_EN_CHANGE(           IDC_EDIT_FILLVALUE            , OnEnChangeEditFillValue            )
  ON_EN_SETFOCUS(         IDC_EDIT_FILLVALUE            , OnEnSetFocusEditFillValue          )
END_MESSAGE_MAP()

BOOL CTestFloatDlg::OnInitDialog() {
  __super::OnInitDialog();
  showFloatFieldsValue(m_accumulator, false);
  showFloatFieldsValue(m_memory     , true );

  CButton *oldButton = ((CButton*)GetDlgItem(IDC_BUTTON_COPYOUTTOIN));
  CPoint p = getWindowPosition(this, IDC_BUTTON_COPYOUTTOIN);
  oldButton->DestroyWindow();
  m_copyOutToInButton.Create(this, OBMIMAGE(UPARROW), p, IDC_BUTTON_COPYOUTTOIN);

  setDefaultStreamParam();
  OnBnClickedCheckIosFlag();
  setStreamOpIsMem(false);
  GetDlgItem(IDC_EDIT_EXPOVALUE)->SetFocus();

  return FALSE;
}

void CTestFloatDlg::OnOK() {
}

void CTestFloatDlg::OnCancel() {
}

void CTestFloatDlg::OnClose() {
  EndDialog(IDOK);
}

BOOL CTestFloatDlg::PreTranslateMessage(MSG *pMsg) {
  switch(getFocusCtrlId(this)) {
  case IDC_EDIT_EXPOVALUE  :
    { const UINT maxValue = m_accumulator.getMaxExpoValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
    }
    break;
  case IDC_EDIT_SIGVALUE   :
    { const UINT64 maxValue = m_accumulator.getMaxSigValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
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

void CTestFloatDlg::OnBnClickedRadioTypeFloat()    { setAccFloatType(FT_FLOAT   );                                         }
void CTestFloatDlg::OnBnClickedRadioTypeDouble64() { setAccFloatType(FT_DOUBLE  );                                         }
void CTestFloatDlg::OnBnClickedRadioTypeDouble80() { setAccFloatType(FT_DOUBLE80);                                         }
void CTestFloatDlg::OnClickedCheckSignBit()        { updateAcc();                                                          }
void CTestFloatDlg::OnChangeEditExpoValue()        { updateAcc();                                                          }
void CTestFloatDlg::OnChangeEditSigValue()         { updateAcc();                                                          }
void CTestFloatDlg::OnEnSetFocusEditExpoValue()    { setCaretPos((CEdit*)GetDlgItem(IDC_EDIT_EXPOVALUE ), 0);               }
void CTestFloatDlg::OnEnSetFocusEditSigValue()     { setCaretPos((CEdit*)GetDlgItem(IDC_EDIT_SIGVALUE  ), 0);               }
void CTestFloatDlg::updateAcc()                    { showFloatFieldsValue(editFieldToFloatFields()               , false); }
void CTestFloatDlg::OnBnClickedButtonSave()        { showFloatFieldsValue(m_accumulator                          , true ); }
void CTestFloatDlg::OnBnClickedButtonLoad()        { showFloatFieldsValue(m_memory                               , false); }
void CTestFloatDlg::OnBnClickedButtonResetMem()    { showFloatFieldsValue(FloatFields(m_accumulator).setZero()   , true ); }
void CTestFloatDlg::OnBnClickedButtonSetZero()     { showFloatFieldsValue(FloatFields(m_accumulator).setZero()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetTrueMin()  { showFloatFieldsValue(FloatFields(m_accumulator).setTrueMin(), false); }
void CTestFloatDlg::OnBnClickedButtonSetMin()      { showFloatFieldsValue(FloatFields(m_accumulator).setMin()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetEps()      { showFloatFieldsValue(FloatFields(m_accumulator).setEps()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetRnd()      { showFloatFieldsValue(FloatFields(m_accumulator).setRnd()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetOne()      { showFloatFieldsValue(FloatFields(m_accumulator).setOne()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetMax()      { showFloatFieldsValue(FloatFields(m_accumulator).setMax()    , false); }
void CTestFloatDlg::OnBnClickedButtonSetInf()      { showFloatFieldsValue(FloatFields(m_accumulator).setPInf()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetQNaN()     { showFloatFieldsValue(FloatFields(m_accumulator).setQNaN()   , false); }
void CTestFloatDlg::OnBnClickedButtonSetSNaN()     { showFloatFieldsValue(FloatFields(m_accumulator).setSNaN()   , false); }
void CTestFloatDlg::OnBnClickedButtonAdd()         { showFloatFieldsValue(m_accumulator + m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonSub()         { showFloatFieldsValue(m_accumulator - m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonMult()        { showFloatFieldsValue(m_accumulator * m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonDiv()         { showFloatFieldsValue(m_accumulator / m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonFmod()        { showFloatFieldsValue(m_accumulator % m_memory               , false); }
void CTestFloatDlg::OnBnClickedButtonReciproc()    { showFloatFieldsValue(reciproc(m_accumulator)                , false); }
void CTestFloatDlg::OnBnClickedButtonSqr()         { showFloatFieldsValue(sqr( m_accumulator)                    , false); }
void CTestFloatDlg::OnBnClickedButtonSqrt()        { showFloatFieldsValue(sqrt(m_accumulator)                    , false); }
void CTestFloatDlg::OnBnClickedCheckMaxPrec()      { updatePrecision();                                                    }
void CTestFloatDlg::OnBnClickedRadioOpAcc()        { setStreamOpIsMem(false);                                              }
void CTestFloatDlg::OnBnClickedRadioOpMem()        { setStreamOpIsMem(true);                                               }
void CTestFloatDlg::OnEnChangeEditWidthValue()     { autoClickStreamOut();                                                 }
void CTestFloatDlg::OnEnChangeEditPrecValue()      { autoClickStreamOut();                                                 }
void CTestFloatDlg::OnEnUpdateEditWidthValue()     { autoClickStreamOut();                                                 }
void CTestFloatDlg::OnEnUpdateEditPrecValue()      { autoClickStreamOut();                                                 }
void CTestFloatDlg::OnEnChangeEditFillValue()      { autoClickStreamOut();                                                 }
void CTestFloatDlg::OnEnSetFocusEditFillValue()    { setCaretPos((CEdit*)GetDlgItem(IDC_EDIT_FILLVALUE), 0);                }

void CTestFloatDlg::OnBnClickedButtonSwapAccMem() {
  const FloatFields mem = m_memory, acc = m_accumulator;
  showFloatFieldsValue(mem, false);
  showFloatFieldsValue(acc, true );
}

void CTestFloatDlg::OnDeltaPosSpinExpoValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const UINT oldValue  = getExpoField();
  const UINT maxValue  = m_accumulator.getMaxExpoValue();
  const int  add       = -pNMUpDown->iDelta;
  if(((add < 0) && (-add > (int)oldValue)) || ((add > 0) && (oldValue + add > maxValue))) {
    return;
  }
  setExpoField(oldValue + add);
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinSigValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN   pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  const UINT64 oldValue  = getSigField();
  const UINT64 maxValue  = m_accumulator.getMaxSigValue();
  const int    add       = -pNMUpDown->iDelta;
  if(((add < 0) && (-add > oldValue)) || ((add > 0) && (oldValue + add < oldValue))
                                      || ((add > 0) && (oldValue + add > maxValue))
     ) {
    return;
  }
  setSigField(oldValue + add);
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinWidthValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UpdateData();
  setWidth(max(m_width - pNMUpDown->iDelta, 0));
  *pResult = 0;
}
void CTestFloatDlg::OnDeltaPosSpinPrecValue(NMHDR *pNMHDR, LRESULT *pResult) {
  LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
  UpdateData();
  setPrecision(max(m_precision - pNMUpDown->iDelta, 0));
  *pResult = 0;
}

void CTestFloatDlg::OnBnClickedButtonStreamIn() {
  UpdateData();

  if(m_widecharStream) {
    const wstring str = (LPCTSTR)m_streamInString;
    wstringstream stream(str);
    FloatFields   tmp(m_accumulator);
    tmp.input(paramWinToStream(stream), m_manipStreamIn == TRUE);
    if(stream) {
      showFloatFieldsValue(tmp, getStreamOpIsMem());
    }
    m_streamState = streamStateToString(stream).cstr();
  } else {
    USES_CONVERSION;
    const char  *instringascii = T2A((LPCTSTR)m_streamInString);
    const string str           = instringascii;
    stringstream stream(str);
    FloatFields  tmp(m_accumulator);
    tmp.input(paramWinToStream(stream), m_manipStreamIn == TRUE);
    if(stream) {
      showFloatFieldsValue(tmp, getStreamOpIsMem());
    }
    m_streamState = streamStateToString(stream).cstr();
  }
  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedButtonStreamOut() {
  UpdateData();

  if(m_widecharStream) {
    wstringstream stream;
    paramWinToStream(stream) << getSelectedStreamOp();
    m_streamOutString        =  stream.str().c_str();
    m_streamState            =  streamStateToString(stream).cstr();
  } else {
    stringstream stream;
    paramWinToStream(stream) << getSelectedStreamOp();
    m_streamOutString        =  stream.str().c_str();
    m_streamState            =  streamStateToString(stream).cstr();
  }
  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedCheckWideCharStream() {
  UpdateData();
  autoClickStreamOut();
}

void CTestFloatDlg::OnBnClickedCheckAutoUpdateStreamOut() {
  UpdateData();
  autoClickStreamOut();
}

void CTestFloatDlg::OnBnClickedButtonCopyOutToIn() {
  UpdateData();
  m_streamInString = m_streamOutString;
  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedCheckIosFlag() {
  const FormatFlags flags = winToFormatFlags();
  setWindowText(this, IDC_STATIC_STREAMFLAGSVALUE, format(_T("%08X"), flags));
  updatePrecision();
  autoClickStreamOut();
}

// -------------------------------------------------------------------------------------

void CTestFloatDlg::setAccFloatType(FloatType type) {
  if(m_setAccTypeActive || (type == getAccFloatType())) {
    return;
  }
  m_setAccTypeActive = true;
  FloatFields acc = m_accumulator, mem = m_memory;
  showFloatFieldsValue(acc.setType(type), false);
  showFloatFieldsValue(mem.setType(type), true);
  m_setAccTypeActive = false;
}

void CTestFloatDlg::setWinFloatType(FloatType type) {
  static const int typeRadioButtonId[] = { IDC_RADIO_TYPEFLOAT, IDC_RADIO_TYPEDOUBLE64, IDC_RADIO_TYPEDOUBLE80 };
  CheckRadioButton(IDC_RADIO_TYPEFLOAT, IDC_RADIO_TYPEDOUBLE80, typeRadioButtonId[type]);
  m_winFloatType = type;
  updatePrecision();
}

void CTestFloatDlg::updatePrecision() {
  const bool maxIsChecked = IsDlgButtonChecked(IDC_CHECK_MAXPREC) == BST_CHECKED;
  if(maxIsChecked) {
    const int maxPrecision = ((winToFormatFlags() & ios::floatfield) == ios::hexfloat)
                           ? FloatFields::getSigHexDigitCount(getWinFloatType())
                           : FloatFields::getMaxDigits10(getWinFloatType());
    setPrecision(maxPrecision);
  }
  GetDlgItem(IDC_EDIT_PRECVALUE)->EnableWindow(maxIsChecked ? FALSE : TRUE);
  GetDlgItem(IDC_SPIN_PRECVALUE)->EnableWindow(maxIsChecked ? FALSE : TRUE);
}

void CTestFloatDlg::setWidth(int width) {
  if(width != m_width) {
    m_width = width;
    UpdateData(FALSE);
    autoClickStreamOut();
  }
}
void CTestFloatDlg::setPrecision(int prec) {
  if(prec != m_precision) {
    m_precision = prec;
    UpdateData(FALSE);
    autoClickStreamOut();
  }
}

void CTestFloatDlg::autoClickStreamOut() {
  if(m_autoUpdateStreamOut) {
    OnBnClickedButtonStreamOut();
  }
}

void CTestFloatDlg::floatFieldsToEditFields(const FloatFields &ff) {
  setSignBit(ff.getSignBit());
  setExpoField(ff.getExpoField(), ff.getType());
  setSigField( ff.getSig()      , ff.getType());
}

void CTestFloatDlg::showFloatFieldsValue(const FloatFields &ff, bool mem) {
  if(m_showffActive) return;
  m_showffActive = true;

  const String      displayStr = ff.getDisplayString();
  const int         fpclass    = ff.getFpClass();
  int displayWinId, classWinId;
  if(mem) {
    displayWinId = IDC_STATIC_MEMVALUE;
    classWinId   = IDC_STATIC_MEMCLASSVALUE;
  } else {
    displayWinId = IDC_STATIC_ACCVALUE;
    classWinId   = IDC_STATIC_ACCCLASSVALUE;
  }
  if(!mem) {
    m_accumulator = ff;
    setWinFloatType(ff.getType());
    floatFieldsToEditFields(m_accumulator);
    setWindowText(this, IDC_STATIC_ACCBIN, ff.toBinString());
  } else {
    m_memory = ff;
  }
  setWindowText(this, displayWinId, displayStr);
  setWindowText(this, classWinId  , fpclassToString(fpclass));
  if(mem && (ff.getType() != getWinFloatType())) {
    showWarning(_T("Memory-floatType=%s. .winFloatType=%s")
               ,toString(ff.getType()).cstr(), toString(getWinFloatType()).cstr());
  }
  if(mem == getStreamOpIsMem()) {
    autoClickStreamOut();
  }
  m_showffActive = false;
}

// ----------------------------------------- Edit fields:sign, exponent and significand ----------------------------

void CTestFloatDlg::setExpoField(UINT v, FloatType type) {
  setWindowText(this, IDC_EDIT_EXPOVALUE, format(_T("%0*X"), FloatFields::getExpoHexDigitCount(type), v));
  expo2ToWin();
}
void CTestFloatDlg::expo2ToWin() {
  const FloatType type      = getWinFloatType();
  const int       expo2     = FloatFields::expoFieldToExpo2(type, getExpoField());
  const int       hexDigits = FloatFields::getExpoHexDigitCount(type);
  const UINT      expo2Hex  =  expo2 & FloatFields::getExpoMask(type);
  setWindowText(this, IDC_STATIC_EXPO2VALUE, format(_T("%+7d  %0*X"), expo2, hexDigits, expo2Hex));
}
UINT CTestFloatDlg::getExpoField() {
  UINT v;
  _stscanf(getWindowText(this, IDC_EDIT_EXPOVALUE).cstr(), _T("%X"), &v);
  return v;
}
void CTestFloatDlg::setSigField(UINT64 v, FloatType type) {
  setWindowText(this, IDC_EDIT_SIGVALUE, format(_T("%0*I64X"), FloatFields::getSigHexDigitCount(type), v));
}
UINT64 CTestFloatDlg::getSigField() {
  UINT64 v;
  _stscanf(getWindowText(this, IDC_EDIT_SIGVALUE).cstr(), _T("%I64X"), &v);
  return v;
}

void CTestFloatDlg::markLabel(int id, bool marked) {
  const DWORD change = WS_BORDER  | SS_SUNKEN;
  const DWORD flags  = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW;
  CStatic *st = (CStatic*)GetDlgItem(id);
  if(marked) {
    st->ModifyStyle(  0, change  , flags);
  } else {
    st->ModifyStyle(  change  , 0, flags);
  }
}

void CTestFloatDlg::setStreamOpIsMem(bool v) {
  const int radiobuttons[] = { IDC_RADIO_OPACC, IDC_RADIO_OPMEM };
  CheckRadioButton(radiobuttons[0], radiobuttons[1], radiobuttons[ordinal(v)]);
  markLabel(IDC_STATIC_MEMLABEL, v );
  markLabel(IDC_STATIC_ACCLABEL, !v);
  autoClickStreamOut();
}

StreamParameters CTestFloatDlg::winToStreamParameters() {
  return StreamParameters(m_precision, m_width, winToFormatFlags(), winToFillChar());
}

void CTestFloatDlg::streamParametersToWin(const StreamParameters &param) {
  setWidth(        (int)param.width()    );
  setPrecision(    (int)param.precision());
  formatFlagsToWin(     param.flags()    );
  fillCharToWin(        param.fill()     );
}

wstringstream &CTestFloatDlg::paramWinToStream(wstringstream &stream) {
  return setFormat(stream, winToStreamParameters());
}

stringstream &CTestFloatDlg::paramWinToStream(stringstream  &stream) {
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

void CTestFloatDlg::formatFlagsToWin(FormatFlags flags) {
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    CheckDlgButton(idFlag.m_id, (flags & idFlag.m_flag) ? BST_CHECKED : BST_UNCHECKED);
  }
}

FormatFlags CTestFloatDlg::winToFormatFlags() {
  FormatFlags result = 0;
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    if(IsDlgButtonChecked(idFlag.m_id) == BST_CHECKED) {
      result |= idFlag.m_flag;
    }
  }
  return result;
}

void CTestFloatDlg::fillCharToWin(wchar_t ch) {
  if(m_fillString.GetLength() == 0) {
    m_fillString = _T(" ");
  }
  m_fillString.SetAt(0, ch);
}

wchar_t CTestFloatDlg::winToFillChar() {
  return (m_fillString.GetLength() == 0) ? _T(' ') : m_fillString.GetAt(0);
}
