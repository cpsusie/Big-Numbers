#include "stdafx.h"
#include "TestEditableTable.h"
#include "TestEditableTableDlg.h"
#include "EditModelSchemaDlg.h"
#include "ListFieldAttributeDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  CAboutDlg();

  enum { IDD = IDD_ABOUTBOX };

protected:
  virtual void DoDataExchange(CDataExchange *pDX);

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestEditableTableDlg::CTestEditableTableDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestEditableTableDlg::IDD, pParent) {
    m_someText = "JESPER";
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestEditableTableDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST, m_list);
    DDX_Text(pDX, IDC_EDIT1, m_someText);
}

BEGIN_MESSAGE_MAP(CTestEditableTableDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_EN_SETFOCUS( IDC_EDIT1            , OnSetFocusEdit1          )
    ON_EN_KILLFOCUS(IDC_EDIT1            , OnKillFocusEdit1         )
    ON_NOTIFY(NM_KILLFOCUS               , IDC_LIST, OnKillFocusList)
    ON_NOTIFY(NM_SETFOCUS                , IDC_LIST, OnSetFocusList )
    ON_BN_CLICKED(IDC_BUTTONINSERT10     , OnButtonInsert10         )
    ON_BN_CLICKED(IDC_BUTTONCLEARTABLE   , OnButtonClearTable       )
    ON_BN_CLICKED(IDC_BUTTONDELETECURRENT, OnButtonDeleteCurrent    )
    ON_BN_CLICKED(IDC_BUTTOLOGTABLE      , OnButtoLogTable          )
    ON_BN_CLICKED(IDC_BUTTONRESETLOG     , OnButtonResetLog         )
    ON_BN_CLICKED(IDC_CHECK1             , OnCheck1                 )
    ON_COMMAND(ID_FILE_EXIT              , OnFileExit               )
    ON_COMMAND(ID_EDIT_SETUPMODEL        , OnEditSetupModel         )
END_MESSAGE_MAP()

BOOL CTestEditableTableDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != NULL) {
    CString strAboutMenu;
    strAboutMenu.LoadString(IDS_ABOUTBOX);
    if(!strAboutMenu.IsEmpty()) {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  SetIcon(m_hIcon, TRUE);
  SetIcon(m_hIcon, FALSE);

  m_accelTable = LoadAccelerators(theApp.m_hInstance,MAKEINTRESOURCE(IDR_ACCELERATOR_MAINDIALOG));

  debugLogSetTimePrefix(false, true);
  m_list.substituteControl(this, IDC_LIST, m_model);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_EDIT1               , PCT_RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_CHECK1              , PCT_RELATIVE_X_POS);
  m_layoutManager.addControl(IDC_COMBO1              , PCT_RELATIVE_X_POS);

  m_layoutManager.addControl(IDC_LIST                , RELATIVE_SIZE | RESIZE_LISTHEADERS | INIT_LISTHEADERS );

  m_layoutManager.addControl(IDC_BUTTONINSERT10      , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_BUTTONDELETECURRENT , RELATIVE_Y_POS);
  m_layoutManager.addControl(IDC_BUTTONCLEARTABLE    , RELATIVE_Y_POS);

  m_layoutManager.addControl(IDC_BUTTOLOGTABLE       , RELATIVE_POSITION);
  m_layoutManager.addControl(IDC_BUTTONRESETLOG      , RELATIVE_POSITION);

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestEditableTableDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CTestEditableTableDlg::OnPaint()  {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTestEditableTableDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestEditableTableDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);    
}

TestTableModel::TestTableModel() {
  addElements(10);
}

UINT TestTableModel::getRowCount() {
  return (UINT)m_elementArray.size();
}

UINT TestTableModel::getColumnCount() {
  return 8;
}

String TestTableModel::getColumnName(UINT column) {
  switch(column) {
  case 0 : return "Name";
  case 1 : return "Adresse";
  case 2 : return "Tlf";
  case 3 : return "short";
  case 4 : return "float";
  case 5 : return "double";
  case 6 : return "bool";
  case 7 : return "ICombo";
  default: return "?";
  }
}

UINT TestTableModel::getColumnWidth(UINT column) {
  static const UINT width[] = { 70, 75, 60,60,60,70,60,70 };
  return width[column];
}

UINT TestTableModel::getFieldFlags(UINT column) {
  switch(column) {
  case 0 : return LFT_STRING;
  case 1 : return LFT_STRING;
  case 2 : return LFT_INT      | LF_NUM_SHOW_ZERO;
  case 3 : return LFT_SHORT;
  case 4 : return LFT_FLOAT    | LF_NUM_SHOW_ZERO | LF_NUM_DECIMALS(2);
  case 5 : return LFT_DOUBLE   | LF_NUM_DECIMALS(3);
  case 6 : return LFT_BOOL;
  case 7 : return LFT_STRCOMBO | LF_STRCOMBO_DROPLIST;
  default: return LFT_STRING;
  }
}

void *TestTableModel::getValueAt(UINT row, UINT column) {
  TableElement &e = m_elementArray[row];
  switch(column) {
  case 0 : return &e.m_name;
  case 1 : return &e.m_address;
  case 2 : return &e.m_tlfnr;
  case 3 : return &e.m_short;
  case 4 : return &e.m_float;
  case 5 : return &e.m_double;
  case 6 : return &e.m_bool;
  case 7 : return &e.m_iComboValue;
  default: throwInvalidArgumentException(__TFUNCTION__, _T("Invalid column:%d"), column);
           return NULL;
  }
}

bool TestTableModel::isEditableCell(UINT row, UINT column) {
  switch(column) {
  case 0 : return true;
  case 1 : return false;
  case 2 : return true;
  case 3 : return true;
  case 4 : return true;
  case 5 : return true;
  case 6 : return true;
  case 7 : return true;
  default: return false;
  }
}

static const TCHAR *comboValues[] = {
  _T("Ged")
 ,_T("Ko")
 ,_T("Hest")
 ,_T("Får")
 ,_T("Tyr")
 ,_T("Kylling med lang tekst efter")
 ,NULL
};

const StringArray TestTableModel::getStrComboStringArray(UINT column) {
  switch(column) {
  case 7 : return StringArray(comboValues);
  default: throwException(_T("No StrComboStringArray for column %d"), column);
  }
  return StringArray();
}

void TestTableModel::clearTable() {
  m_elementArray.clear();
}

void TestTableModel::addElements(UINT count) {
  const UINT n = getRowCount();
  for(UINT i = n; i < n+count; i++) {
    m_elementArray.add(TableElement(i));
  }
}

TableElement::TableElement(int i) {
  m_name        = format(_T("Peter %d"), i);
  m_address     = format(_T("Fortoftvej %d"), i);
  m_tlfnr       = 86202120 + i;
  m_short       = 23 + i;
  m_float       = 345 + (float)i/10;
  m_double      = (double)i/100;
  m_bool        = (i&1)?true:false;
  m_iComboValue = i%(ARRAYSIZE(comboValues)-1);
}

String TableElement::toString() const {
  return format(_T("%-20s, %-20s, %9d, %6d, %7.2lf, %8.3lf, %-5s, %d")
               ,m_name.cstr()
               ,m_address.cstr()
               ,m_tlfnr
               ,m_short
               ,m_float
               ,m_double
               ,boolToStr(m_bool)
               ,m_iComboValue
              );
}

void TestTableModel::removeElement(UINT index) {
  m_elementArray.removeIndex(index);
}

void CTestEditableTableDlg::OnSetFocusEdit1() {
//  debugLog("Dialog:Editbox got focus\n");
}

void CTestEditableTableDlg::OnKillFocusEdit1() {
//  debugLog("Dialog:Editbox lost focus\n");
}

void CTestEditableTableDlg::OnSetFocusList(NMHDR *pNMHDR, LRESULT *pResult) {
//  debugLog("Dialog:List got focus\n");
  *pResult = 0;
}

void CTestEditableTableDlg::OnKillFocusList(NMHDR *pNMHDR, LRESULT *pResult) {
//  debugLog("Dialog:List lost focus\n");
  *pResult = 0;
}

void CTestEditableTableDlg::OnButtonInsert10() {
  const int n0 = m_model.getRowCount();
  m_model.addElements(10);
  const int n1 = m_model.getRowCount();
  for(int r = n0; r < n1; r++) {
    m_list.insertNewItem(r);
  }
//  Invalidate();
}

void CTestEditableTableDlg::OnButtonDeleteCurrent() {
  if(m_list.hasCurrentCell()) {
    const int r = m_list.getCurrentRow();
    m_model.removeElement(r);
    m_list.removeItem(r);
  }
}

void CTestEditableTableDlg::OnButtonClearTable() {
  m_model.clearTable();
  m_list.DeleteAllItems();
//  Invalidate();
}

void CTestEditableTableDlg::OnButtoLogTable() {
  const int n = m_model.getRowCount();
  for(int r = 0; r < n; r++) {
    debugLog(_T("%s\n"), m_model.getElement(r).toString().cstr());
  }
}

void CTestEditableTableDlg::OnButtonResetLog() {
  redirectDebugLog();
  debugLog(_T("\n"));
}

void CTestEditableTableDlg::OnCheck1() {
}

void CTestEditableTableDlg::OnFileExit() {
  EndDialog(IDOK);
}

void CTestEditableTableDlg::OnEditSetupModel() {
  ListCtrlModelSchema schema(m_model);
  CEditModelSchemaDlg dlg(schema);
  if(dlg.DoModal() == IDOK) {
  }
}

void CTestEditableTableDlg::OnClose() {
  OnFileExit();
}

void CTestEditableTableDlg::OnCancel() {
}

void CTestEditableTableDlg::OnOK() {
}


BOOL CTestEditableTableDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}
