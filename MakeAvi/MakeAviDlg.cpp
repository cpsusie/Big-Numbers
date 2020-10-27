#include "stdafx.h"
#include <io.h>
#include <FileNameSplitter.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/AviFile.h>
#include <MFCUtil/ProgressWindow.h>
#include <MFCUtil/SelectDirDlg.h>
#include "MakeAvi.h"
#include "MakeAviDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
  enum { IDD = IDD_ABOUTBOX };
  CAboutDlg() : CDialog(IDD) {
  }

  DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CMakeAviDlg::CMakeAviDlg(CWnd *pParent /*=nullptr*/) : CDialog(CMakeAviDlg::IDD, pParent) {
  m_framePerSecond = 15;
  m_useEvery       = 1;
  m_hIcon          = theApp.LoadIcon(IDR_MAINFRAME);
}

void CMakeAviDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX   , IDC_LISTNAMES      , m_nameList      );
  DDX_Text(pDX      , IDC_EDITFRAMEPERSEC, m_framePerSecond);
  DDV_MinMaxUInt(pDX, m_framePerSecond   , 1, 30           );
  DDX_Text(pDX      , IDC_EDITUSEEVERY   , m_useEvery      );
  DDV_MinMaxUInt(pDX, m_useEvery         , 1, 1000         );
}

BEGIN_MESSAGE_MAP(CMakeAviDlg, CDialog)
  ON_WM_QUERYDRAGICON()
  ON_WM_SYSCOMMAND()
  ON_WM_SIZE()
  ON_WM_PAINT()
  ON_WM_CLOSE()
  ON_COMMAND(ID_FILE_EXIT          , OnFileExit                           )
  ON_COMMAND(ID_FILE_ADDFILES      , OnFileAddfFiles                      )
  ON_COMMAND(ID_EDIT_DELETESELECTED, OnEditDeleteSelected                 )
  ON_COMMAND(ID_EDIT_CLEARLIST     , OnEditClearList                      )
  ON_COMMAND(ID_HELP_ABOUTMAKEAVI  , OnHelpAbout                          )
  ON_BN_CLICKED(IDC_BUTTONREADAVI  , OnButtonReadAVI                      )
  ON_BN_CLICKED(IDC_BUTTONMAKEAVI  , OnButtonMakeAVI                      )
  ON_NOTIFY(LVN_ITEMCHANGED        , IDC_LISTNAMES, OnItemChangedListNames)
  ON_EN_CHANGE(IDC_EDITFRAMEPERSEC , OnChangeEditFramePerSec              )
  ON_EN_CHANGE(IDC_EDITUSEEVERY    , OnChangeEditUseEvery                 )
END_MESSAGE_MAP()

BOOL CMakeAviDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if(pSysMenu != nullptr) {
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
  CListCtrl &nameList = m_nameList;

  nameList.InsertColumn(0,EMPTYSTRING      , LVCFMT_LEFT, 290);
  nameList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_STATICPREVIEW         , RELATIVE_SIZE      );
  m_layoutManager.addControl(IDC_LISTNAMES             , RELATIVE_HEIGHT    );
  m_layoutManager.addControl(IDC_BUTTONREADAVI         , RELATIVE_POSITION  );
  m_layoutManager.addControl(IDC_BUTTONMAKEAVI         , RELATIVE_POSITION  );
  m_layoutManager.addControl(IDC_STATICUSEEVERYLABEL   , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_EDITUSEEVERY          , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICIMAGE           , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICFRAMPERSECELABEL, RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_EDITFRAMEPERSEC       , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICIMAGECOUNTLABEL , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICIMAGECOUNT      , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICUSEIMAGESLABEL  , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICUSEDIMAGECOUNT  , RELATIVE_Y_POS     );
  m_layoutManager.addControl(IDC_STATICTOTALLENGTH     , RELATIVE_Y_POS     );
  return TRUE;
}

HCURSOR CMakeAviDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CMakeAviDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

void CMakeAviDlg::OnSize(UINT nType, int cx, int cy) {
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  if((cx > 0) && (cy > 0)) {
    updatePreview();
  }
}

void CMakeAviDlg::OnPaint()  {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
    updateWindowState();
  }
}

void CMakeAviDlg::OnOK() {
}

void CMakeAviDlg::OnCancel() {
}

void CMakeAviDlg::OnClose() {
  OnFileExit();
}

void CMakeAviDlg::OnFileExit() {
  EndDialog(IDOK);
}

static int stringCmp(const String &s1, const String &s2) {
  return _tcsicmp(s1.cstr(), s2.cstr());
}

static const TCHAR *fileDialogExtensions = _T("JPG files\0*.jpg\0"
                                              "BMP files\0*.bmp\0"
                                              "All files\0*.*\0"
                                              "\0");

void CMakeAviDlg::OnFileAddfFiles() {
  const StringArray selected = selectMultipleFileNames(_T("Add files"), fileDialogExtensions);

  if(selected.isEmpty()) {
    return;
  }
  m_nameArray = selected;
  m_nameArray.sort(stringCmp);
  m_nameList.DeleteAllItems();
  for(size_t i = 0; i < m_nameArray.size(); i++) {
    addData(m_nameList, (int)i, 0, m_nameArray[i], true);
  }
  Invalidate();
}

void CMakeAviDlg::OnEditDeleteSelected() {
  int index = getSelectedIndex();
  if(index >= 0) {
    m_nameArray.removeIndex(index);
    m_nameList.DeleteItem(index);
    if(m_nameArray.size() > 0) {
      index = min(index, (int)m_nameArray.size()-1);
      loadPicture(m_nameArray[index]);
      if(m_currentPicture.isLoaded()) {
        setSelectedIndex(m_nameList, index);
      }
    } else {
      unloadPicture();
    }
    updateWindowState();
    updatePreview();
  }
}

void CMakeAviDlg::OnEditClearList() {
  m_nameArray.clear();
  m_nameList.DeleteAllItems();
  unloadPicture();
  updateWindowState();
  updatePreview();
}

void CMakeAviDlg::OnHelpAbout() {
  CAboutDlg().DoModal();
}

int CMakeAviDlg::getSelectedIndex() {
  return m_nameList.GetSelectionMark();
}

void CMakeAviDlg::loadPicture(const String &fileName) {
  try {
    m_currentPicture.load(fileName);
  } catch(Exception e) {
    showException(e);
  }
}

void CMakeAviDlg::unloadPicture() {
  m_currentPicture.unload();
}

void CMakeAviDlg::updatePreview() {
  CStatic *previewWindow = (CStatic*)GetDlgItem(IDC_STATICPREVIEW);
  if(m_currentPicture.isLoaded()) {
    CClientDC dc(previewWindow);
    CRect drawRect = getClientRect(previewWindow);
    if((drawRect.Width() > 0) && (drawRect.Height() > 0)) {
      m_currentPicture.show(dc, drawRect);
    }
  } else {
    Invalidate();
  }
}

void CMakeAviDlg::updateWindowState() {
  if(!UpdateData()) {
    return;
  }
  const UINT totalFrameCount = (UINT)m_nameArray.size();
  UINT usedFrameCount = 0;
  if(totalFrameCount == 0) {
    setWindowText(this, IDC_STATICTOTALLENGTH, EMPTYSTRING);
  } else {
    usedFrameCount = totalFrameCount / m_useEvery;
    setWindowText(this, IDC_STATICTOTALLENGTH, format(_T("Total length:%.1lf sec"), (double)usedFrameCount / m_framePerSecond));
  }
  setWindowText(this, IDC_STATICIMAGECOUNT    , format(_T("%d"), m_nameArray.size()));
  setWindowText(this, IDC_STATICUSEDIMAGECOUNT, format(_T("%d"), usedFrameCount    ));
  GetDlgItem(IDC_BUTTONMAKEAVI)->EnableWindow(usedFrameCount>0);
}

void CMakeAviDlg::OnItemChangedListNames(NMHDR *pNMHDR, LRESULT *pResult) {
  NM_LISTVIEW *pNMListView = (NM_LISTVIEW*)pNMHDR;

  if((pNMListView->uChanged & LVIF_STATE) && (pNMListView->uNewState & LVNI_SELECTED)) {
    int index = getSelectedIndex();
    if(index >= 0) {
      loadPicture(m_nameArray[index]);
      updatePreview();
    }
  }
  *pResult = 0;
}

void CMakeAviDlg::OnChangeEditFramePerSec() {
  updateWindowState();
}

void CMakeAviDlg::OnChangeEditUseEvery() {
  updateWindowState();
}

class AviConverter : public InteractiveRunnable {
private:
  const String       m_outFileName;
  const StringArray &m_nameArray;
  const UINT         m_framesPerSecond;
  const UINT         m_useEvery;
  int                m_index;
  HDC                m_dc;
  HBITMAP            m_bitmap;
public:
  AviConverter(const String &outFileName, const StringArray &nameArray, UINT framesPerSecond, UINT useEvery)
    : m_outFileName(outFileName)
    , m_nameArray(nameArray)
    , m_framesPerSecond(framesPerSecond)
    , m_useEvery(useEvery)
  {
    m_index   = 0;
    m_dc      = nullptr;
    m_bitmap  = 0;
  }
  ~AviConverter();
  double getMaxProgress() const {
    return (double)m_nameArray.size();
  }
  double getProgress() const {
    return m_index;
  };
  String getTitle() {
    return _T("Generating AVI file");
  }

  virtual int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE | IR_SHOWTIMEESTIMATE | IR_SHOWPERCENT;
  }
  UINT safeRun();
};

AviConverter::~AviConverter() {
  if(m_bitmap != nullptr) {
    DeleteObject(m_bitmap);
  }
  if(m_dc != nullptr) {
    DeleteDC(m_dc);
  }
}

UINT AviConverter::safeRun() {
  CPicture picture;
  picture.load(m_nameArray[0]);
  const CSize imageSize = picture.getSize();

  HDC screenDC = getScreenDC();
  m_dc     = CreateCompatibleDC(screenDC);
  m_bitmap = CreateCompatibleBitmap(screenDC, imageSize.cx, imageSize.cy);
  DeleteDC(screenDC);
  DWORD codec = 0; // mmioFOURCC('w','m','v',' ');

  if(ACCESS(m_outFileName, 0) == 0) {
    UNLINK(m_outFileName);
  }
  CAviFile aviFile(m_outFileName, true, codec, m_framesPerSecond);

  for(m_index = 0; m_index < (int)m_nameArray.size(); m_index += m_useEvery) {
    if(isInterrupted()) {
      throwException(_T("Interrupted by user"));
    }
    picture.load(m_nameArray[m_index]);
    HGDIOBJ oldGDI = SelectObject(m_dc, m_bitmap);
    picture.show(m_dc);
    SelectObject(m_dc, oldGDI);
    aviFile.appendNewFrame(m_bitmap);
  }
  return 0;
}

void CMakeAviDlg::OnButtonMakeAVI() {
  if(m_nameArray.isEmpty()) return;
  const String &first = m_nameArray[0];
  String dir = FileNameSplitter(first).getDir();
  if((dir.length() > 0) && (dir[dir.length()-1] == '\\')) {
    dir = substr(dir,0,dir.length()-1);
  }
  String outFileName = FileNameSplitter(dir).getFileName();
  if(outFileName.length() == 0) {
    outFileName = _T("MakeAVIOut");
  }

  String outName = FileNameSplitter(first).setFileName(outFileName).setExtension(_T("avi")).getFullPath();

  FileNameSplitter nameInfo(outName);
  String initialDir = nameInfo.getDrive() + nameInfo.getDir();

  String extensions = format(_T("Avi files%c*.avi%c%c"),0, 0, 0);
  CFileDialog dlg(FALSE, _T("avi"), outFileName.cstr());
  dlg.m_ofn.lpstrFilter = extensions.cstr();

  if(initialDir.length() > 0) {
    dlg.m_ofn.lpstrInitialDir = initialDir.cstr();
  }
  if(dlg.DoModal() == IDOK) {
    outName = dlg.m_ofn.lpstrFile;
  } else {
    return;
  }

  AviConverter converter(outName, m_nameArray, m_framePerSecond, m_useEvery);
  ProgressWindow(this, converter);
  if(!converter.isOk()) {
    showWarning(converter.getErrorMsg());
  }
}

class AviReader : public InteractiveRunnable {
private:
  const String       m_inFileName;
  int                m_maxIndex;
  int                m_index;
public:
  AviReader(const String &inFileName) : m_inFileName(inFileName) {
    m_maxIndex = 0;
    m_index    = 0;
  }
  ~AviReader();
  double getMaxProgress() const {
    return m_maxIndex;
  }
  double getProgress() const {
    return m_index;
  };
  String getTitle() {
    return _T("Reading AVI file");
  }

  virtual int getSupportedFeatures() {
    return IR_PROGRESSBAR | IR_INTERRUPTABLE | IR_SHOWTIMEESTIMATE | IR_SHOWPERCENT;
  }
  UINT safeRun();
};

AviReader::~AviReader() {
}

UINT AviReader::safeRun() {
  CAviFile aviFile(m_inFileName, false);
  FileNameSplitter spl(m_inFileName);
  const String dir = spl.getDrive() + spl.getDir();
  m_maxIndex = aviFile.getStreamInfo(). dwLength;
  HBITMAP bitmap;
  while((bitmap = aviFile.readFrame()) != nullptr) {
    m_index++;
    if(isInterrupted()) {
      throwException(_T("Interrupted by user"));
    }
    const String fileName = FileNameSplitter::getChildName(dir, format(_T("out%05d.jpg"), m_index));
    writeAsJPG(bitmap, ByteOutputFile(fileName));
  }
  return 0;
}

static const TCHAR *aviFileExtensions = _T("AVI files\0*.avi\0"
                                           "All files\0*.*\0"
                                          "\0");

void CMakeAviDlg::OnButtonReadAVI() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle      = _T("Open AVI file");
  dlg.m_ofn.lpstrFilter     = aviFileExtensions;
  if(dlg.DoModal() != IDOK) {
    return;
  }
  String aviFileName = dlg.m_ofn.lpstrFile;
  if(aviFileName.length() == 0) {
    return;
  }

  AviReader reader(aviFileName);
  ProgressWindow(this, reader);
  if(!reader.isOk()) {
    showWarning(reader.getErrorMsg());
  }
}

BOOL CMakeAviDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd,m_accelTable,pMsg)) {
    return true;
  }
  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_UP      :
      scrollLines(-(int)m_useEvery);
      return true;
    case VK_DOWN    :
      scrollLines( m_useEvery);
      return true;
    }
    break;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CMakeAviDlg::scrollLines(int count) {
  const int index = getSelectedIndex() + count;
  setSelectedIndex(m_nameList, minMax(index, 0, (int)m_nameArray.size()));
}
