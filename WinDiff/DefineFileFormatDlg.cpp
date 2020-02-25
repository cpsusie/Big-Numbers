#include "stdafx.h"
#include <Tokenizer.h>
#include "WinDiff.h"
#include "FindDlg.h"
#include "DefineFileFormatDlg.h"
#include "EnterFormatNameDlg.h"
#include "FileFormatsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CDefineFileFormatDlg::CDefineFileFormatDlg(FileFormat &param, const String &sampleline, CWnd *pParent)  : CDialog(IDD, pParent), m_param(param) {
  m_columnFrom         = 1;
  m_columnTo           = 1;
  m_fieldDelimiter     = EMPTYSTRING;
  m_textQualifier      = EMPTYSTRING;
  m_sampleLine         = sampleline.cstr();
  m_multipleDelimiters = FALSE;

  m_currentControl = 0;
}

void CDefineFileFormatDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(    pDX, IDC_EDITFROM, m_columnFrom);
  DDX_Text(    pDX, IDC_EDITTO, m_columnTo);
  DDX_Text(    pDX, IDC_EDITFIELDDELIMITER, m_fieldDelimiter);
  DDX_Text(    pDX, IDC_EDITTEXTQUALIFIER, m_textQualifier);
  DDV_MaxChars(pDX, m_textQualifier, 1);
  DDX_Text(    pDX, IDC_EDITSAMPLE, m_sampleLine);
  DDX_Check(   pDX, IDC_CHECKMULTIPLEDELIMITERS, m_multipleDelimiters);
}

BEGIN_MESSAGE_MAP(CDefineFileFormatDlg, CDialog)
  ON_WM_DRAWITEM()
  ON_COMMAND(      ID_FORMAT_NEW                    , OnFormatNew                       )
  ON_COMMAND(      ID_FILE_OPEN                     , OnFileOpen                        )
  ON_COMMAND(      ID_FILE_SAVE                     , OnFileSave                        )
  ON_COMMAND(      ID_FILE_SAVE_AS                  , OnFileSaveAs                      )
  ON_COMMAND(      ID_DELIMESCSYMBOL_TAB            , OnDelimEscSymbolTab               )
  ON_COMMAND(      ID_DELIMESCSYMBOL_SPACE          , OnDelimEscSymbolSpace             )
  ON_COMMAND(      ID_DELIMESCSYMBOL_CR             , OnDelimEscSymbolCr                )
  ON_COMMAND(      ID_DELIMESCSYMBOL_NEWLINE        , OnDelimEscSymbolNewLine           )
  ON_COMMAND(      ID_DELIMESCSYMBOL_FORM           , OnDelimEscSymbolFormFeed          )
  ON_COMMAND(      ID_DELIMESCSYMBOL_BACK           , OnDelimEscSymbolBackSpace         )
  ON_COMMAND(      ID_DELIMESCSYMBOL_ESC            , OnDelimEscSymbolEsc               )
  ON_COMMAND(      ID_DELIMESCSYMBOL_UNICODE        , OnDelimEscSymbolUnicode           )
  ON_COMMAND(      ID_DELIMESCSYMBOL_BACKSLASH      , OnDelimEscSymbolBackslash         )
  ON_COMMAND(      ID_DELIMESCSYMBOL_CTRL           , OnDelimEscSymbolCtrl              )
  ON_BN_CLICKED(   IDC_BUTTONADD                    , OnButtonAdd                       )
  ON_BN_CLICKED(   IDC_BUTTONDELETE                 , OnButtonDelete                    )
  ON_BN_CLICKED(   IDC_RADIODELIMITED               , OnRadioDelimited                  )
  ON_BN_CLICKED(   IDC_CHECKMULTIPLEDELIMITERS      , OnCheckMultipleDelimiters         )
  ON_BN_CLICKED(   IDC_BUTTONDELIMITERMENU          , OnButtonDelimiterMenu             )
  ON_BN_CLICKED(   IDC_RADIOFIXEDWIDTH              , OnRadioDelimited                  )
  ON_COMMAND(      ID_GOTOCOLUMNFROM                , OnGotoColumnFrom                  )
  ON_COMMAND(      ID_GOTOCOLUMNTO                  , OnGotoColumnTo                    )
  ON_COMMAND(      ID_GOTOFIELDDELIMITER            , OnGotoFieldDelimiter              )
  ON_COMMAND(      ID_GOTOTEXTQUALIFIER             , OnGotoTextQualifier               )
  ON_COMMAND(      ID_GOTOSAMPLE                    , OnGotoSample                      )
  ON_COMMAND(      ID_GOTOCOLUMNLIST                , OnGotoColumnList                  )
  ON_EN_SETFOCUS(  IDC_EDITSAMPLE                   , OnSetFocusEditSample              )
  ON_EN_KILLFOCUS( IDC_EDITSAMPLE                   , OnKillFocusEditSample             )
  ON_LBN_SETFOCUS( IDC_LISTCOLUMNS                  , OnSetFocusListColumns             )
  ON_LBN_KILLFOCUS(IDC_LISTCOLUMNS                  , OnKillFocusListColumns            )
  ON_LBN_SELCHANGE(IDC_LISTCOLUMNS                  , OnSelChangeListColumns            )
  ON_EN_CHANGE(    IDC_EDITTEXTQUALIFIER            , OnChangeEditTextQualifier         )
  ON_EN_CHANGE(    IDC_EDITFIELDDELIMITER           , OnChangeEditFieldDelimiter        )
  ON_EN_CHANGE(    IDC_EDITFROM                     , OnChangeEditFrom                  )
  ON_EN_CHANGE(    IDC_EDITTO                       , OnChangeEditTo                    )
END_MESSAGE_MAP()

void CDefineFileFormatDlg::enableDropdowns(bool enable) {
  GetDlgItem(IDC_STATICFIELDDELIMITER   )->EnableWindow(enable);
  GetDlgItem(IDC_EDITFIELDDELIMITER     )->EnableWindow(enable);
  GetDlgItem(IDC_STATICTEXTQUALIFIER    )->EnableWindow(enable);
  GetDlgItem(IDC_EDITTEXTQUALIFIER      )->EnableWindow(enable);
  GetDlgItem(IDC_CHECKMULTIPLEDELIMITERS)->EnableWindow(enable);
  if(enable) {
    GetDlgItem(IDC_BUTTONDELIMITERMENU)->ShowWindow(SW_SHOW);
  } else {
    GetDlgItem(IDC_BUTTONDELIMITERMENU)->ShowWindow(SW_HIDE);
  }
}

void CDefineFileFormatDlg::setDelimited(bool value) {
  CButton *rdel = (CButton*)GetDlgItem(IDC_RADIODELIMITED);
  CButton *rfix = (CButton*)GetDlgItem(IDC_RADIOFIXEDWIDTH);
  if(value) {
    rdel->SetCheck(true);
    rfix->SetCheck(false);
  } else {
    rdel->SetCheck(false);
    rfix->SetCheck(true);
  }
  enableDropdowns(value);
}

void CDefineFileFormatDlg::OnRadioDelimited() {
  if(IsDlgButtonChecked(IDC_RADIODELIMITED)) {
    setDelimited(true);
  } else {
    setDelimited(false);
  }
  UpdateData();
  ajourSample();
}

bool CDefineFileFormatDlg::validateAndAdd() {
  UpdateData();

  if(m_columnFrom > m_columnTo) {
    showWarning(_T("From > To"));
    return false;
  }
  try {
    ColumnInterval iv(m_columnFrom,m_columnTo);
    FileFormat param;
    paramFromWindow(param);
    param.addInterval(iv);
    CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
    int r = lb->AddString(iv.toString().cstr());
    if(lb->GetCurSel() < 0) {
      lb->SetCurSel(r);
    }
    ajourDeleteButton();
    return true;
  } catch(Exception e) {
    showException(e);
    return false;
  }
}

void CDefineFileFormatDlg::OnButtonAdd() {
  validateAndAdd();
}

void CDefineFileFormatDlg::OnButtonDelete() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
  int sel = lb->GetCurSel();
  if(sel >= 0) {
    lb->DeleteString(sel);
    if(sel < lb->GetCount()) {
      lb->SetCurSel(sel);
    } else {
      lb->SetCurSel(lb->GetCount()-1);
    }
    ajourDeleteButton();
  }
}

void CDefineFileFormatDlg::setTitle() {
  String title = _T("Ignore columns (") + m_name + _T(")");
  SetWindowText(title.cstr());
}

void CDefineFileFormatDlg::paramToWindow(const FileFormat &param) {
  m_name = param.m_name;
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
  lb->ResetContent();
  for(size_t i = 0; i < param.m_columns.size(); i++) {
    lb->AddString(param.m_columns[i].toString().cstr());
  }

  setDelimited(param.m_delimited);
  m_fieldDelimiter     = expandEscape(param.m_delimiters).cstr();
  m_textQualifier      = param.m_textQualifier;
  m_multipleDelimiters = param.m_multipleDelimiters;
  UpdateData(false);
  setTitle();
}

void CDefineFileFormatDlg::paramFromWindow(FileFormat &param) {
  UpdateData();
  param.m_delimited     = IsDlgButtonChecked(IDC_RADIODELIMITED)?true:false;
  param.m_delimiters    = convertEscape(m_fieldDelimiter.GetBuffer(m_fieldDelimiter.GetLength()));
  int l = m_textQualifier.GetLength();
  if(l == 0) {
    param.m_textQualifier = 0;
  } else {
    param.m_textQualifier = m_textQualifier.GetBuffer(1)[0];
  }
  param.m_multipleDelimiters = m_multipleDelimiters ? true : false;
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
  param.m_columns.clear();
  for(int i = 0; i < lb->GetCount(); i++) {
    CString str;
    lb->GetText(i,str);
    param.m_columns.add(ColumnInterval(str.GetBuffer(str.GetLength())));
  }
  param.m_name = m_name;
}

BOOL CDefineFileFormatDlg::OnInitDialog() {
  __super::OnInitDialog();

  paramToWindow(m_param);
  m_lastCheckpoint = m_param;

  ((CListBox*)GetDlgItem(IDC_LISTCOLUMNS))->SetCurSel(0);
  ajourSample();
  ajourDeleteButton();
  OnGotoColumnFrom();
  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATORCOLUMNS));

  return false;
}

BOOL CDefineFileFormatDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  BOOL ret = __super::PreTranslateMessage(pMsg);
  if(m_currentControl == IDC_EDITSAMPLE) {
    ajourFromTo();
  }
  return ret;
}

void CDefineFileFormatDlg::OnGotoColumnFrom() {
  GetDlgItem(IDC_EDITFROM)->SetFocus();
}

void CDefineFileFormatDlg::OnGotoColumnTo() {
  GetDlgItem(IDC_EDITTO)->SetFocus();
}

void CDefineFileFormatDlg::OnGotoFieldDelimiter() {
  GetDlgItem(IDC_EDITFIELDDELIMITER)->SetFocus();
}

void CDefineFileFormatDlg::OnGotoTextQualifier() {
  GetDlgItem(IDC_EDITTEXTQUALIFIER)->SetFocus();
}

void CDefineFileFormatDlg::OnGotoSample() {
  GetDlgItem(IDC_EDITSAMPLE)->SetFocus();
}

void CDefineFileFormatDlg::OnGotoColumnList() {
  GetDlgItem(IDC_LISTCOLUMNS)->SetFocus();
}

bool CDefineFileFormatDlg::validate() {
  FileFormat param;
  paramFromWindow(param);
  if(param.m_delimited) {
    if(param.m_delimiters.length() == 0) {
      OnGotoFieldDelimiter();
      showWarning(_T("Must specify fielddelimiters"));
      return false;
    }
  }
  if(param.m_columns.size() == 0) {
    OnGotoColumnFrom();
    showWarning(_T("No columns specified"));
    return false;
  }
  if((param.m_textQualifier != 0) && _tcschr(param.m_delimiters.cstr(),param.m_textQualifier) != NULL) {
    OnGotoTextQualifier();
    showWarning(_T("The textqualifier is member of the set of fielddelimiters"));
    return false;
  }
  param.validate(); // just to be sure
  return true;
}

bool CDefineFileFormatDlg::save() { // return true if succeeded or false on failure or cancel
  if(!validate()) {
    return false;
  }
  FileFormat param;
  paramFromWindow(param);
  if(param.m_name != _T("Untitled")) {
    bool ret = param.save();
    if(ret) m_lastCheckpoint = param;
    return ret;
  } else {
    CGetFormatNameDlg dlg(param.m_name);
    if(dlg.DoModal() == IDOK) {
      param.m_name = dlg.m_name;
      if(!param.save()) {
        return false;
      }
      paramToWindow(param);
      m_lastCheckpoint = param;
      return true;
    } else {
      return false;
    }
  }
}

void CDefineFileFormatDlg::OnFileSave() {
  save();
}

void CDefineFileFormatDlg::OnFileSaveAs() {
  if(!validate()) {
    return;
  }
  FileFormat param;
  paramFromWindow(param);
  CGetFormatNameDlg dlg(param.m_name);
  if(dlg.DoModal() == IDOK) {
    param.m_name = dlg.m_name;
    if(param.save()) {
      paramToWindow(param);
      paramFromWindow(m_lastCheckpoint);
    }
  }
}

bool CDefineFileFormatDlg::checkSave() { // return true if the useroperation should continue
  FileFormat param;
  paramFromWindow(param);
  if(param == m_lastCheckpoint) {
    return true;
  }

  switch(MessageBox(_T("Do you want to save the changes"), _T("Save"), MB_YESNOCANCEL|MB_ICONQUESTION)) {
  case IDYES:
    if(!save()) {
      return false;
    }
    paramFromWindow(m_lastCheckpoint);
    return true;
  case IDNO:
    return true;
  case IDCANCEL:
    return false;
  }
  return true;
}

void CDefineFileFormatDlg::OnFormatNew() {
  if(!checkSave()) {
    return;
  }

  FileFormat param;
  paramToWindow(param);
  m_lastCheckpoint = param;
  ajourDeleteButton();
}

void CDefineFileFormatDlg::OnFileOpen() {
  if(!checkSave()) {
    return;
  }

  CFileFormatsDlg dlg;
  if(dlg.DoModal() == IDOK) {
    if(dlg.m_selected.length() > 0) {
      FileFormat param;
      param.load(dlg.m_selected);
      paramToWindow(param);
      m_lastCheckpoint = param;
      ((CListBox*)GetDlgItem(IDC_LISTCOLUMNS))->SetCurSel(0);
      ajourDeleteButton();
    }
  }
}

void CDefineFileFormatDlg::OnCancel() {
  if(!checkSave()) {
    return;
  }

  __super::OnCancel();
}

void CDefineFileFormatDlg::OnOK() {
  if(!validate()) {
    return;
  }

  if(!checkSave()) {
    return;
  }

  paramFromWindow(m_param);
  __super::OnOK();
}

int CDefineFileFormatDlg::getFrom() {
  String str = getWindowText(this, IDC_EDITFROM);
  int n;

  if((_stscanf(str.cstr(), _T("%u"),&n) == 1) && (n >= 1)) {
    return n;
  } else {
    return 0;
  }
}

void CDefineFileFormatDlg::setFrom(int value) {
  setWindowText(this, IDC_EDITFROM, format(_T("%d"),value));
}

void CDefineFileFormatDlg:: setTo(int value) {
  setWindowText(this, IDC_EDITTO, format(_T("%d"),value));
}

int CDefineFileFormatDlg::getTo() {
  String tmp = getWindowText(this, IDC_EDITTO);
  int n;

  if(_stscanf(tmp.cstr(),_T("%u"),&n) == 1 && n >= 1) {
    return n;
  } else {
    return 0;
  }
}

String CDefineFileFormatDlg::getDelimiters() {
  String str = getWindowText(this, IDC_EDITFIELDDELIMITER);
  if(str.length() > 0) {
    return convertEscape(str);
  } else {
    return str;
  }
}

bool CDefineFileFormatDlg::delimiterMode() {
  if(!IsDlgButtonChecked(IDC_RADIODELIMITED)) {
    return false;
  } else {
    return getDelimiters().length() > 0;
  }
}

_TUCHAR CDefineFileFormatDlg::getTextQualifier() {
  String str = getWindowText(this, IDC_EDITTEXTQUALIFIER);
  if(str.length() == 0) {
    return 0;
  } else {
    return str[0];
  }
}

int CDefineFileFormatDlg::flags() const {
  return m_multipleDelimiters ? 0 : TOK_SINGLEDELIMITERS;
}

int CDefineFileFormatDlg::findSampleStartSel(int fromField) {
  const String  sample = m_sampleLine.GetBuffer(m_sampleLine.GetLength());
  _TUCHAR       tq     = getTextQualifier();
  const String  del    = getDelimiters();
  int i = 1;
  for(Tokenizer tok(sample,del,tq,flags()); tok.hasNext() && (i <= fromField); i++) {
    const StringIndex index = tok.nextIndex();
    if(i == fromField) {
      return (int)index.getStart();
    }
  }
  return 0;
}

int CDefineFileFormatDlg::findSampleEndSel(int toField) {
  const String  sample = m_sampleLine.GetBuffer(m_sampleLine.GetLength());
  _TUCHAR       tq     = getTextQualifier();
  const String  del    = getDelimiters();
  int i = 1;
  for(Tokenizer tok(sample,del,tq,flags()); tok.hasNext() && i <= toField+1; i++) {
    const StringIndex index = tok.nextIndex();
    if(i == toField+1) {
      return (int)index.getStart()-1;
    }
  }
  return (int)sample.length();
}

int CDefineFileFormatDlg::findFrom() {
  int from,to;
  CEdit *es = (CEdit*)GetDlgItem(IDC_EDITSAMPLE);
  es->GetSel(from,to);
  from++;

  const String  sample = m_sampleLine.GetBuffer(m_sampleLine.GetLength());
  _TUCHAR       tq     = getTextQualifier();
  const String  del    = getDelimiters();
  int i = 0;
  for(Tokenizer tok(sample,del,tq,flags()); tok.hasNext(); i++) {
    const StringIndex index = tok.nextIndex();
    if((int)index.getStart() >= from) {
      break;
    }
  }
  return i;
}

int CDefineFileFormatDlg::findTo() {
  int from,to;
  CEdit *es = (CEdit*)GetDlgItem(IDC_EDITSAMPLE);
  es->GetSel(from,to);

  const    String sample = m_sampleLine.GetBuffer(m_sampleLine.GetLength());
  _TUCHAR         tq     = getTextQualifier();
  const    String del    = getDelimiters();
  int i = 0;
  for(Tokenizer tok(sample,del,tq,flags()); tok.hasNext(); i++) {
    const StringIndex index = tok.nextIndex();
    if((int)index.getStart() >= to) {
      break;
    }
  }
  return i;
}

void CDefineFileFormatDlg::ajourSample(int fromfield, int tofield) {
  int from, to;
  if(delimiterMode()) {
    from = findSampleStartSel(fromfield);
    to   = findSampleEndSel(tofield);
  } else {
    from = fromfield - 1;
    to   = tofield;
  }

  CEdit *es = (CEdit*)GetDlgItem(IDC_EDITSAMPLE);
  if(from <= to) {
    es->SetSel(from,to);
  }
}

void CDefineFileFormatDlg::ajourSample() {
  ajourSample(getFrom(),getTo());
  ajourAddButton();
}

void CDefineFileFormatDlg::ajourFromTo() {
  int from,to;
  if(delimiterMode()) {
    from = findFrom();
    to   = findTo();
  } else {
    CEdit *es = (CEdit*)GetDlgItem(IDC_EDITSAMPLE);
    es->GetSel(from,to);
    from++;
  }

  int f = getFrom();
  int t = getTo();
  if(from != f || to != t) {
    setFrom(from);
    setTo(to);
    ajourAddButton();
  }
}

void CDefineFileFormatDlg::ajourDeleteButton() {
  CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
  CButton *db = (CButton*)GetDlgItem(IDC_BUTTONDELETE);
  db->EnableWindow((lb->GetCurSel() >= 0) ? TRUE : FALSE);
}

void CDefineFileFormatDlg::ajourAddButton() {
  CButton *ab = (CButton*)GetDlgItem(IDC_BUTTONADD);
  ab->EnableWindow((getFrom() <= getTo()) ? TRUE : FALSE);
}

void CDefineFileFormatDlg::OnSetFocusEditSample() {
  m_currentControl = IDC_EDITSAMPLE;
  ajourSample();
}

void CDefineFileFormatDlg::OnKillFocusEditSample() {
  m_currentControl = 0;
}

void CDefineFileFormatDlg::OnSetFocusListColumns() {
  m_currentControl = IDC_LISTCOLUMNS;
}

void CDefineFileFormatDlg::OnKillFocusListColumns() {
  m_currentControl = 0;
}

void CDefineFileFormatDlg::OnSelChangeListColumns() {
  if(m_currentControl == IDC_LISTCOLUMNS) {
    CListBox *lb = (CListBox*)GetDlgItem(IDC_LISTCOLUMNS);
    int sel = lb->GetCurSel();
    if(sel >= 0) {
      CString str;
      lb->GetText(sel,str);
      ColumnInterval iv(str.GetBuffer(str.GetLength()));
      ajourSample(iv.m_from,iv.m_to);
    }
  }
}

void CDefineFileFormatDlg::OnButtonDelimiterMenu() {
  CMenu menu;
  int ret = menu.LoadMenu(IDR_MENUDELIMITERESCAPESYMBOLS);
  if(!ret) {
    showError(_T("Loadmenu failed"));
    return;
  }
  CRect r;
  GetDlgItem(IDC_BUTTONDELIMITERMENU)->GetWindowRect(&r);
  menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,r.right,r.top, this );
}

void CDefineFileFormatDlg::addDelimiter(const TCHAR *s) {
  UpdateData();

  String del = m_fieldDelimiter.GetBuffer(m_fieldDelimiter.GetLength());
  del += s;
  m_fieldDelimiter = del.cstr();
  UpdateData(false);
}

void CDefineFileFormatDlg::OnDelimEscSymbolTab()       { addDelimiter(_T("\\t") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolSpace()     { addDelimiter(_T("\\S") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolCr()        { addDelimiter(_T("\\r") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolNewLine()   { addDelimiter(_T("\\n") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolFormFeed()  { addDelimiter(_T("\\f") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolBackSpace() { addDelimiter(_T("\\b") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolEsc()       { addDelimiter(_T("\\e") ); }
void CDefineFileFormatDlg::OnDelimEscSymbolBackslash() { addDelimiter(_T("\\\\")); }
void CDefineFileFormatDlg::OnDelimEscSymbolCtrl()      { addDelimiter(_T("\\^A")); }
void CDefineFileFormatDlg::OnDelimEscSymbolUnicode()   {
#ifdef UNICODE
  addDelimiter(_T("\\u"));
#endif
}

void CDefineFileFormatDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) {
  if(nIDCtl == IDC_BUTTONDELIMITERMENU) {
    drawTriangle(GetDlgItem(IDC_BUTTONDELIMITERMENU));
  }
  __super::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CDefineFileFormatDlg::OnCheckMultipleDelimiters() {
  UpdateData();
  ajourSample();
}

void CDefineFileFormatDlg::OnChangeEditTextQualifier() {
  UpdateData();
  ajourSample();
}

void CDefineFileFormatDlg::OnChangeEditFieldDelimiter() {
  UpdateData();
  ajourSample();
}

void CDefineFileFormatDlg::OnChangeEditFrom() {
  if(m_currentControl == IDC_EDITSAMPLE) {
    return;
  }
  ajourSample();
}

void CDefineFileFormatDlg::OnChangeEditTo() {
  if(m_currentControl == IDC_EDITSAMPLE) {
    return;
  }
  ajourSample();
}

