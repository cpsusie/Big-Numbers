#include "stdafx.h"
#include "TestBin.h"
#include "TestFloatDlg.h"
#include "HexEditOverwrite.h"

#ifdef _DEBUG
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
, m_autoUpdateStreamOut(FALSE)
{
  m_showffActive     = false;
  m_setAccTypeActive = false;
}

void CTestFloatDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDITSTREAMINVALUE, m_streamInString);
  DDX_Text(pDX, IDC_EDITSTREAMOUTVALUE, m_streamOutString);
  DDX_Text(pDX, IDC_EDITWIDTHVALUE, m_width);
  DDX_Text(pDX, IDC_EDITPRECVALUE, m_precision);
  DDX_Text(pDX, IDC_EDITFILLVALUE, m_fillString);
  DDV_MaxChars(pDX, m_fillString, 1);
  DDX_Text(pDX, IDC_STATICSTREAMSTATEVALUE, m_streamState);
  DDX_Check(pDX, IDC_CHECKMANIPSTREAMIN, m_manipStreamIn);
  DDX_Check(pDX, IDC_CHECKAUTOUPDATESTREAMOUT, m_autoUpdateStreamOut);
}

BEGIN_MESSAGE_MAP(CTestFloatDlg, CDialog)
  ON_WM_CLOSE()
  ON_BN_CLICKED(          IDC_RADIOTYPEFLOAT           , OnBnClickedRadioTypeFloat          )
  ON_BN_CLICKED(          IDC_RADIOTYPEDOUBLE64        , OnBnClickedRadioTypeDouble64       )
  ON_BN_CLICKED(          IDC_RADIOTYPEDOUBLE80        , OnBnClickedRadioTypeDouble80       )
  ON_BN_CLICKED(          IDC_CHECKSIGNBIT             , OnClickedCheckSignBit              )
  ON_EN_CHANGE(           IDC_EDITEXPOVALUE            , OnChangeEditExpoValue              )
  ON_EN_CHANGE(           IDC_EDITSIGVALUE             , OnChangeEditSigValue               )
  ON_EN_SETFOCUS(         IDC_EDITEXPOVALUE            , OnEnSetFocusEditExpoValue          )
  ON_EN_SETFOCUS(         IDC_EDITSIGVALUE             , OnEnSetFocusEditSigValue           )
  ON_BN_CLICKED(          IDC_BUTTONSAVE               , OnBnClickedButtonSave              )
  ON_BN_CLICKED(          IDC_BUTTONLOAD               , OnBnClickedButtonLoad              )
  ON_BN_CLICKED(          IDC_BUTTONRESETMEM           , OnBnClickedButtonResetMem          )
  ON_BN_CLICKED(          IDC_BUTTONSWAPACCMEM         , OnBnClickedButtonSwapAccMem        )
  ON_BN_CLICKED(          IDC_BUTTONSETZERO            , OnBnClickedButtonSetZero           )
  ON_BN_CLICKED(          IDC_BUTTONSETTRUEMIN         , OnBnClickedButtonSetTrueMin        )
  ON_BN_CLICKED(          IDC_BUTTONSETMIN             , OnBnClickedButtonSetMin            )
  ON_BN_CLICKED(          IDC_BUTTONSETEPS             , OnBnClickedButtonSetEps            )
  ON_BN_CLICKED(          IDC_BUTTONSETRND             , OnBnClickedButtonSetRnd            )
  ON_BN_CLICKED(          IDC_BUTTONSETONE             , OnBnClickedButtonSetOne            )
  ON_BN_CLICKED(          IDC_BUTTONSETMAX             , OnBnClickedButtonSetMax            )
  ON_BN_CLICKED(          IDC_BUTTONSETINF             , OnBnClickedButtonSetInf            )
  ON_BN_CLICKED(          IDC_BUTTONSETQNAN            , OnBnClickedButtonSetQNaN           )
  ON_BN_CLICKED(          IDC_BUTTONSETSNAN            , OnBnClickedButtonSetSNaN           )
  ON_BN_CLICKED(          IDC_BUTTONADD                , OnBnClickedButtonAdd               )
  ON_BN_CLICKED(          IDC_BUTTONSUB                , OnBnClickedButtonSub               )
  ON_BN_CLICKED(          IDC_BUTTONMULT               , OnBnClickedButtonMult              )
  ON_BN_CLICKED(          IDC_BUTTONDIV                , OnBnClickedButtonDiv               )
  ON_BN_CLICKED(          IDC_BUTTONFMOD               , OnBnClickedButtonFmod              )
  ON_BN_CLICKED(          IDC_BUTTONRECIPROC           , OnBnClickedButtonReciproc          )
  ON_BN_CLICKED(          IDC_BUTTONSQR                , OnBnClickedButtonSqr               )
  ON_BN_CLICKED(          IDC_BUTTONSQRT               , OnBnClickedButtonSqrt              )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINEXPOVALUE            , OnDeltaPosSpinExpoValue            )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINSIGVALUE             , OnDeltaPosSpinSigValue             )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINPRECVALUE            , OnDeltaPosSpinPrecValue            )
  ON_NOTIFY(UDN_DELTAPOS, IDC_SPINWIDTHVALUE           , OnDeltaPosSpinWidthValue           )
  ON_BN_CLICKED(          IDC_CHECK_MAXPREC            , OnBnClickedCheckMaxPrec            )
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
  ON_EN_CHANGE(           IDC_EDITPRECVALUE            , OnEnChangeEditPrecValue            )
  ON_EN_UPDATE(           IDC_EDITWIDTHVALUE           , OnEnUpdateEditWidthValue           )
  ON_EN_UPDATE(           IDC_EDITPRECVALUE            , OnEnUpdateEditPrecValue            )
  ON_EN_CHANGE(           IDC_EDITFILLVALUE            , OnEnChangeEditFillValue            )
END_MESSAGE_MAP()

BOOL CTestFloatDlg::OnInitDialog() {
  __super::OnInitDialog();
  showFloatFieldsValue(m_accumulator, false);
  showFloatFieldsValue(m_memory     , true );

  CButton *oldButton = ((CButton*)GetDlgItem(IDC_BUTTONCOPYOUTTOIN));
  CPoint p = getWindowPosition(this, IDC_BUTTONCOPYOUTTOIN);
  oldButton->DestroyWindow();
  m_copyOutToInButton.Create(this, OBMIMAGE(UPARROW), p, IDC_BUTTONCOPYOUTTOIN);
  
  OnBnClickedCheckIosFlag();
  setStreamOpIsMem(false);
  GetDlgItem(IDC_EDITEXPOVALUE)->SetFocus();
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
  case IDC_EDITEXPOVALUE  :
    { const UINT maxValue = m_accumulator.getMaxExpoValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
    }
    break;
  case IDC_EDITSIGVALUE   :
    { const UINT64 maxValue = m_accumulator.getMaxSigValue();
      if(isOverwriteCurrentHexChar(this, pMsg,0,maxValue)) {
        return true;
      }
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

void CTestFloatDlg::OnBnClickedRadioTypeFloat()    { setAccFloatType(FT_FLOAT   );                                         }
void CTestFloatDlg::OnBnClickedRadioTypeDouble64() { setAccFloatType(FT_DOUBLE  );                                         }
void CTestFloatDlg::OnBnClickedRadioTypeDouble80() { setAccFloatType(FT_DOUBLE80);                                         }
void CTestFloatDlg::OnClickedCheckSignBit()        { updateAcc();                                                          }
void CTestFloatDlg::OnChangeEditExpoValue()        { updateAcc();                                                          }
void CTestFloatDlg::OnChangeEditSigValue()         { updateAcc();                                                          }
void CTestFloatDlg::OnEnSetFocusEditExpoValue()    { setCaretPos((CEdit*)GetDlgItem(IDC_EDITEXPOVALUE ), 0);               }
void CTestFloatDlg::OnEnSetFocusEditSigValue()     { setCaretPos((CEdit*)GetDlgItem(IDC_EDITSIGVALUE  ), 0);               }
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

  wstring        str = (LPCTSTR)m_streamInString;
  wstringstream stream(str);
  FloatFields   tmp(m_accumulator);
  tmp.input(setParameters(stream), m_manipStreamIn == TRUE);
  if (stream) {
    showFloatFieldsValue(tmp, getStreamOpIsMem());
  }
  m_streamState = streamStateToString(stream);

  UpdateData(FALSE);
}

void CTestFloatDlg::OnBnClickedButtonStreamOut() {
  UpdateData();

  wstringstream stream;
  setParameters(stream) << getSelectedStreamOp();

  m_streamOutString = stream.str().c_str();
  m_streamState     = streamStateToString(stream);

  UpdateData(FALSE);
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
  const FormatFlags flags = getStreamFormatFlags();
  setWindowText(this, IDC_STATICSTREAMFLAGSVALUE, format(_T("%08X"), flags));
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
  static const int typeRadioButtonId[] = { IDC_RADIOTYPEFLOAT, IDC_RADIOTYPEDOUBLE64, IDC_RADIOTYPEDOUBLE80 };
  CheckRadioButton(IDC_RADIOTYPEFLOAT, IDC_RADIOTYPEDOUBLE80, typeRadioButtonId[type]);
  m_winFloatType = type;
  updatePrecision();
}

void CTestFloatDlg::updatePrecision() {
  const bool maxIsChecked = IsDlgButtonChecked(IDC_CHECK_MAXPREC) == BST_CHECKED;
  if(maxIsChecked) {
    const int maxPrecision = ((getStreamFormatFlags() & ios::floatfield) == ios::hexfloat)
                           ? FloatFields::getSigHexDigitCount(getWinFloatType())
                           : FloatFields::getMaxDigits10(getWinFloatType());
    setPrecision(maxPrecision);
  }
  GetDlgItem(IDC_EDITPRECVALUE)->EnableWindow(maxIsChecked ? FALSE : TRUE);
  GetDlgItem(IDC_SPINPRECVALUE)->EnableWindow(maxIsChecked ? FALSE : TRUE);
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
  setSignField(ff.getSignBit());
  setExpoField(ff.getExpoField(), ff.getType());
  setSigField( ff.getSig()      , ff.getType());
}

void CTestFloatDlg::showFloatFieldsValue(const FloatFields &ff, bool mem) {
  if(m_showffActive) return;
  m_showffActive = true;
  const int         fpclass  = ff.getFpClass();
  String            valueStr = ff.toHexString();
  int valueWinId, classWinId;
  if(mem) {
    valueWinId = IDC_STATICMEMVALUE;
    classWinId = IDC_STATICMEMCLASSVALUE;
  } else {
    valueWinId = IDC_STATICACCVALUE;
    classWinId = IDC_STATICACCCLASSVALUE;
  }
  if(!mem) {
    m_accumulator = ff;
    setWinFloatType(ff.getType());
    floatFieldsToEditFields(m_accumulator);
    setWindowText(this, IDC_STATICACCBIN, ff.toBinString());
  } else {
    m_memory = ff;
  }
  setWindowText(this, valueWinId, valueStr);
  setWindowText(this, classWinId, fpclassToString(fpclass));
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
void CTestFloatDlg::setSignField(bool v) {
  CheckDlgButton(IDC_CHECKSIGNBIT, v ? BST_CHECKED : BST_UNCHECKED);
}
bool CTestFloatDlg::getSignField() {
  return IsDlgButtonChecked(IDC_CHECKSIGNBIT) == BST_CHECKED;
}

void CTestFloatDlg::setExpoField(UINT v, FloatType type) {
  setWindowText(this, IDC_EDITEXPOVALUE, format(_T("%0*X"), FloatFields::getExpoHexDigitCount(type), v));
  expo2ToWin();
}
void CTestFloatDlg::expo2ToWin() {
  const FloatType type      = getWinFloatType();
  const int       expo2     = FloatFields::expoFieldToExpo2(type, getExpoField());
  const int       hexDigits = FloatFields::getExpoHexDigitCount(type);
  const UINT      expo2Hex  =  expo2 & FloatFields::getExpoMask(type);
  setWindowText(this, IDC_STATICEXPO2VALUE, format(_T("%+7d  %0*X"), expo2, hexDigits, expo2Hex));
}
UINT CTestFloatDlg::getExpoField() {
  UINT v;
  _stscanf(getWindowText(this, IDC_EDITEXPOVALUE).cstr(), _T("%X"), &v);
  return v;
}
void CTestFloatDlg::setSigField(UINT64 v, FloatType type) {
  setWindowText(this, IDC_EDITSIGVALUE, format(_T("%0*I64X"), FloatFields::getSigHexDigitCount(type), v));
}
UINT64 CTestFloatDlg::getSigField() {
  UINT64 v;
  _stscanf(getWindowText(this, IDC_EDITSIGVALUE).cstr(), _T("%I64X"), &v);
  return v;
}

void CTestFloatDlg::markLabel(int id, bool marked) {
  const DWORD change   = WS_BORDER | SS_SUNKEN;
  const DWORD changeEx = WS_EX_CLIENTEDGE;
  const DWORD flags    = SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_DRAWFRAME | SWP_SHOWWINDOW;
  CStatic *st = (CStatic*)GetDlgItem(id);
  BOOL ret;
  if(marked) {
    ret = st->ModifyStyle(  0, change  , flags);
//    ret = st->ModifyStyleEx(0, changeEx, flags);
  } else {
    ret = st->ModifyStyle(  change  , 0, flags);
//    ret = st->ModifyStyleEx(changeEx, 0, flags);
  }
}

bool CTestFloatDlg::getStreamOpIsMem() {
  return IsDlgButtonChecked(IDC_RADIOOPMEM) == BST_CHECKED;
}

void CTestFloatDlg::setStreamOpIsMem(bool v) {
  const int ckeckedRadioButton = v ? IDC_RADIOOPMEM     : IDC_RADIOOPACC;
  CheckRadioButton(IDC_RADIOOPACC, IDC_RADIOOPMEM, ckeckedRadioButton);
  markLabel(IDC_STATICMEMLABEL, v );
  markLabel(IDC_STATICACCLABEL, !v);
  autoClickStreamOut();
}

wstringstream &CTestFloatDlg::setParameters(wstringstream &stream) {
  stream.width(m_width);
  stream.precision(m_precision);
  stream.flags(getStreamFormatFlags());
  stream.fill(getStreamFillChar());
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

FormatFlags CTestFloatDlg::getStreamFormatFlags() {
  FormatFlags result = 0;
  for(size_t i = 0; i < ARRAYSIZE(buttonFlagArray); i++) {
    const ButtunIdFlag &idFlag = buttonFlagArray[i];
    if(IsDlgButtonChecked(idFlag.m_id) == BST_CHECKED) {
      result |= idFlag.m_flag;
    }
  }
  return result;
}

wchar_t CTestFloatDlg::getStreamFillChar() {
  return (m_fillString.GetLength() == 0) ? _T(' ') : m_fillString.GetAt(0);
}

CString CTestFloatDlg::streamStateToString(wstringstream &s) { // static
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
