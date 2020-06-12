#include "stdafx.h"
#include <limits.h>
#include <math.h>
#include "ParameterDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CParameterDlg::CParameterDlg(const InitializeParameters &parameters, CWnd *pParent)
: m_parameters(parameters)
, CDialog(IDD, pParent) {
  m_elementCount = (UINT)m_parameters.m_elementCount;
  m_seed         = m_parameters.m_seed;
  m_periodCount  = m_parameters.m_periodCount;
  m_fileName     = m_parameters.m_fileName.cstr();
}

void CParameterDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Text(pDX, IDC_EDIT_ELEMENTCOUNT, m_elementCount);
  DDV_MinMaxUInt(pDX, m_elementCount, 2, 1000);
  DDX_Text(pDX, IDC_EDIT_PERIODCOUNT, m_periodCount);
  DDX_Text(pDX, IDC_EDIT_FILENAME, m_fileName);
  DDX_Text(pDX, IDC_EDIT_SEED, m_seed);
}

BEGIN_MESSAGE_MAP(CParameterDlg, CDialog)
  ON_BN_CLICKED(IDC_RADIO_RANDOM       , OnRadioRandom        )
  ON_BN_CLICKED(IDC_RADIO_SORTED       , OnRadioSorted        )
  ON_BN_CLICKED(IDC_RADIO_INVERSESORTED, OnRadioInverseSorted )
  ON_BN_CLICKED(IDC_RADIO_SINUS        , OnRadioSinus         )
  ON_BN_CLICKED(IDC_RADIO_FILEDATA     , OnRadioFileData      )
  ON_BN_CLICKED(IDC_BUTTON_BROWSE      , OnButtonBrowse       )
  ON_COMMAND(ID_GOTO_ELEMENTCOUNT      , OnGotoElementCount   )
  ON_COMMAND(ID_GOTO_ELEMENTSIZE       , OnGotoElementSize    )
  ON_COMMAND(ID_GOTO_PERIODCOUNT       , OnGotoPeriodCount    )
  ON_COMMAND(ID_GOTO_FILENAME          , OnGotoFileName       )
  ON_COMMAND(ID_GOTO_SEED              , OnGotoSeed           )
  ON_CBN_SELCHANGE(IDC_COMBO_RANDOMIZE , OnSelchangeComboRandomize)
END_MESSAGE_MAP()

BOOL CParameterDlg::OnInitDialog() {
  __super::OnInitDialog();

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_PARAMETER_ACCELERATOR));

  CheckRadioButton(IDC_RADIO_RANDOM, IDC_RADIO_FILEDATA, m_parameters.m_initMethod);
  radioChecked(          m_parameters.m_initMethod);
  setElementSize(        m_parameters.m_elementSize);
  setRandomizationMethod(m_parameters.m_randomizationMethod);
  gotoEditBox(this, (m_parameters.m_initMethod == IDC_RADIO_FILEDATA) ? IDC_EDIT_FILENAME : IDC_EDIT_ELEMENTCOUNT);

  enableSeed();

  return FALSE;
}

BOOL CParameterDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CParameterDlg::OnGotoElementCount() {
  gotoEditBox(this,IDC_EDIT_ELEMENTCOUNT);
}

void CParameterDlg::OnGotoElementSize() {
  getComboElementSize()->SetFocus();
}

void CParameterDlg::OnGotoSeed() {
  gotoEditBox(this,IDC_EDIT_SEED);
}

void CParameterDlg::OnGotoPeriodCount() {
  gotoEditBox(this,IDC_EDIT_PERIODCOUNT);
}

void CParameterDlg::OnGotoFileName() {
  gotoEditBox(this,IDC_EDIT_FILENAME);
}

CComboBox *CParameterDlg::getComboElementSize() const {
  return (CComboBox*)GetDlgItem(IDC_COMBO_ELEMENTSIZE);
}

CComboBox *CParameterDlg::getComboRandomize() const {
  return (CComboBox*)GetDlgItem(IDC_COMBO_RANDOMIZE);
}

void CParameterDlg::OnSelchangeComboRandomize() {
  enableSeed();
}

void CParameterDlg::enableSeed() {
  GetDlgItem(IDC_EDIT_SEED)->EnableWindow(IsDlgButtonChecked(IDC_RADIO_RANDOM) && (getRandomizationMethod() == FIXED_SEED));
}


void CParameterDlg::OnRadioRandom() {
  radioChecked(IDC_RADIO_RANDOM);
}

void CParameterDlg::OnRadioSorted() {
  radioChecked(IDC_RADIO_SORTED);
}

void CParameterDlg::OnRadioInverseSorted() {
  radioChecked(IDC_RADIO_INVERSESORTED);
}

void CParameterDlg::OnRadioSinus() {
  radioChecked(IDC_RADIO_SINUS);
}

void CParameterDlg::OnRadioFileData() {
  radioChecked(IDC_RADIO_FILEDATA);
}

int CParameterDlg::getElementSize() const {
  CComboBox *cb = getComboElementSize();
  int selected = cb->GetCurSel();
  if(selected < 0) {
    return 1;
  }
  CString s;
  cb->GetLBText(selected, s);
  int size;
  if(_stscanf((LPCTSTR)s, _T("%d"), &size) != 1) {
    return 1;
  }
  return size;
}

void CParameterDlg::setElementSize(int v) {
  CComboBox *cb = getComboElementSize();
  const int n = cb->GetCount();
  for(int i = 0; i < n; i++) {
    CString s;
    cb->GetLBText(i, s);
    int lbv;
    if(_stscanf((LPCTSTR)s, _T("%d"), &lbv) != 1) {
      continue;
    }
    if(lbv == v) {
      cb->SetCurSel(i);
      return;
    }
  }
  cb->SetCurSel(0);
}

void CParameterDlg::setRandomizationMethod(RandomizationMethod method) {
  CComboBox *cb = getComboRandomize();
  cb->SetCurSel(method);
}

RandomizationMethod CParameterDlg::getRandomizationMethod() {
  CComboBox *cb = getComboRandomize();
  return (RandomizationMethod)cb->GetCurSel();
}

void CParameterDlg::radioChecked(int radioChecked) {
  const BOOL isRandomRadio = (radioChecked == IDC_RADIO_RANDOM  ) ? TRUE : FALSE;
  const BOOL isFileRadio   = (radioChecked == IDC_RADIO_FILEDATA) ? TRUE : FALSE;
  const BOOL isSinusRadio  = (radioChecked == IDC_RADIO_SINUS   ) ? TRUE : FALSE;

  GetDlgItem(IDC_STATIC_ELEMENTCOUNT)->EnableWindow(!isFileRadio);
  GetDlgItem(IDC_EDIT_ELEMENTCOUNT  )->EnableWindow(!isFileRadio);

  getComboRandomize()->EnableWindow(isRandomRadio);
  enableSeed();

  GetDlgItem(IDC_STATIC_PERIODCOUNT )->EnableWindow(isSinusRadio);
  GetDlgItem(IDC_EDIT_PERIODCOUNT   )->EnableWindow(isSinusRadio);

  GetDlgItem(IDC_EDIT_FILENAME      )->EnableWindow(isFileRadio);
  GetDlgItem(IDC_STATIC_FILENAME    )->EnableWindow(isFileRadio);
  GetDlgItem(IDC_BUTTON_BROWSE      )->EnableWindow(isFileRadio);

  m_initMethod = radioChecked;
}

void CParameterDlg::OnButtonBrowse() {
  static const TCHAR *fileDialogExtensions = _T("Text-files (*.txt, *.dat)\0*.txt; *.dat;\0All files (*.*)\0*.*\0\0");

  CFileDialog dlg(TRUE);

  dlg.m_ofn.lpstrTitle  = _T("Select file");
  dlg.m_ofn.lpstrFilter = fileDialogExtensions;
  if(m_fileName.GetLength() > 0) {
    _tcscpy(dlg.m_ofn.lpstrFile, m_fileName.GetBuffer(m_fileName.GetLength()));
  }

  for(;;) {
    if((dlg.DoModal() == IDOK) && (_tcslen(dlg.m_ofn.lpstrFile) != 0)) {
      if(readTextFile(dlg.m_ofn.lpstrFile)) {
        UpdateData(FALSE);
        return;
      }
    } else {
      return;
    }
  }
}

bool CParameterDlg::readTextFile(const String &fileName) {
  try {
    m_parameters.readTextFile(fileName);
    m_elementCount = (UINT)m_parameters.m_elementCount;
    m_fileName     = m_parameters.m_fileName.cstr();
    return true;
  } catch(Exception e) {
    errorMessage(_T("%s"), e.what());
    return false;
  }
}

void CParameterDlg::OnOK() {
  if(!UpdateData()) {
    return;
  }

  m_parameters.m_initMethod          = m_initMethod;
  m_parameters.m_elementCount        = m_elementCount;
  m_parameters.m_elementSize         = getElementSize();
  m_parameters.m_randomizationMethod = getRandomizationMethod();
  m_parameters.m_seed                = m_seed;
  m_parameters.m_periodCount         = m_periodCount;
  m_parameters.m_fileName            = m_fileName;

  if(m_parameters.m_initMethod == IDC_RADIO_FILEDATA) {
    if(m_parameters.m_fileName.length() == 0) {
      errorMessage(_T("Must specify filename"));
      gotoEditBox(this,IDC_EDIT_FILENAME);
      return;
    }
    if(!readTextFile(m_parameters.m_fileName)) {
      return;
    }
  }

  if(m_parameters.m_elementCount < 2) {
    errorMessage(_T("Number of elements to sort must be > 1"));
    return;
  }

  m_parameters.save();
  __super::OnOK();
}
