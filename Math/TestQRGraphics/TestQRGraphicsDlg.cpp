#include "stdafx.h"
#include "TestQRGraphicsDlg.h"
#include "DimensionDlg.h"

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

CTestQRGraphicsDlg::CTestQRGraphicsDlg(CWnd *pParent) : CDialog(CTestQRGraphicsDlg::IDD, pParent) {
  m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestQRGraphicsDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestQRGraphicsDlg, CDialog)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_COMMAND(ID_FILE_QUIT, OnFileQuit)
  ON_COMMAND(ID_FILE_NEW, OnFileNew)
  ON_COMMAND(ID_FILE_SOLVE, OnFileSolve)
  ON_COMMAND(ID_FILE_UN, OnFileUn)
  ON_COMMAND(ID_FILE_SYMMETRIC, OnFileSymmetric)
  ON_COMMAND(ID_FILE_SPECIAL, OnFileSpecial)
  ON_COMMAND(ID_OPTIONS_PAUSE, OnOptionsPause)
  ON_COMMAND(ID_OPTIONS_DIMENSION, OnOptionsDimension)
  ON_COMMAND(ID_FILE_ILLCONDITIONED, OnFileIllconditioned)
  ON_COMMAND(ID_FILE_RANDOMANTISYMMETRIC, OnFileRandomAntiSymmetric)
END_MESSAGE_MAP()

#define BLACK RGB(0,0,0)
#define WHITE RGB(255,255,255)
#define RED   RGB(255,0,0)

BOOL CTestQRGraphicsDlg::OnInitDialog() {
  __super::OnInitDialog();

  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu *pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL) {
      CString strAboutMenu;
      strAboutMenu.LoadString(IDS_ABOUTBOX);
      if (!strAboutMenu.IsEmpty()) {
          pSysMenu->AppendMenu(MF_SEPARATOR);
          pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
      }
  }

  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  a.setDimension(DEFAULTSIZE);
  randomize();
  m_font.CreateFont( 12, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                    ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                    ,DEFAULT_PITCH | FF_MODERN
                    ,_T("Ariel") ); //"Courier New" );

  return TRUE;
}

void CTestQRGraphicsDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  }
  else {
    __super::OnSysCommand(nID, lParam);
  }
}

class QRDialog : public QRTracer {
private:
  CTestQRGraphicsDlg *m_dlg;
  int                m_lastDeflatedSize;
public:
  QRDialog(CTestQRGraphicsDlg *dlg) { m_dlg = dlg; m_lastDeflatedSize = 0; }
  void handleData(const QRTraceElement &data);
};

void QRDialog::handleData(const QRTraceElement &data) {
#define XPOS(n) ((n)*65+5)
#define YPOS(n) ((n)*20+5)
  TCHAR *format = _T("%+7.5lf ");
  CClientDC dc(m_dlg);
  dc.SelectObject(&m_dlg->m_font);
  CRect windowrect;
  m_dlg->GetClientRect(&windowrect);

  const QRMatrix &QR           = data.m_QR;
  const ComplexVector &eValues = data.m_QR.getEigenValues();

  if(m_lastDeflatedSize == 0) {
    dc.Rectangle(&windowrect);
  } else {
    if(QR.getDeflatedSize() < m_lastDeflatedSize) {
//    char tmpstr[25];
//    sprintf(tmpstr,format,0.0);
//    CSize cs = dc.GetTextExtent(tmpstr).;
      CRect rect(XPOS(QR.getDeflatedSize())-2,YPOS(QR.getDeflatedSize())-5,XPOS(m_lastDeflatedSize)-2,YPOS(m_lastDeflatedSize)-5);
      dc.DrawEdge(&rect,EDGE_RAISED,BF_RECT);
      int evxpos = XPOS(2); int evypos = YPOS(DEFAULTSIZE+1);
      size_t resultDim = m_dlg->a.getColumnCount() - QR.getDeflatedSize();
      for(size_t i = 0; i < resultDim; i++) {
        TCHAR tmp[50];
        _stprintf(tmp,_T("(%le,%le)=%le"),eValues[i].re,eValues[i].im,arg(eValues[i]));
        dc.TextOut(evxpos,evypos,tmp);
        evypos += 15;
        if(evypos >= windowrect.Height()-30) { evypos = YPOS(DEFAULTSIZE+1); evxpos += 350; }
      }
      if(isMenuItemChecked(m_dlg, ID_OPTIONS_PAUSE)) {
        AfxMessageBox( _T("Press Enter..."));
      }
    }
  }
  for(size_t j = 0; j < QR.getRowCount(); j++) {
    for(size_t i = 0; i < QR.getColumnCount(); i++) {
      if(i == j) {
        dc.SetTextColor(RED);
      } else {
        dc.SetTextColor(BLACK);
      }
      TCHAR tmp[25];
      _stprintf(tmp,format,QR(i,j));
      dc.TextOut(XPOS((int)j),YPOS((int)i),tmp);
    }
  }
  m_lastDeflatedSize = QR.getDeflatedSize();

  if(isMenuItemChecked(m_dlg, ID_OPTIONS_PAUSE)) {
    Sleep(1000);
  }
}

void CTestQRGraphicsDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    try {
      QRMatrix QR(a);
      ComplexVector e(a.getColumnCount());
      QRTraceElement element(QR);
      QRDialog qrDialog(this);
      qrDialog.handleData(element);
    }
    catch (Exception e) {
      MessageBox(e.what(), _T("Error"), MB_ICONWARNING);
    }
    __super::OnPaint();
  }
}

HCURSOR CTestQRGraphicsDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestQRGraphicsDlg::OnFileQuit()  {
  EndDialog(IDOK);
}

void CTestQRGraphicsDlg::OnFileNew() {
  try {
    for(size_t r = 0; r < a.getRowCount(); r++) {
      for(size_t c = 0; c < a.getColumnCount(); c++) {
        a(r,c) = randInt(-5,5);
      }
    }
    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }

}

void CTestQRGraphicsDlg::OnFileSolve() {
  try {
    QRDialog qrDialog(this);
    ComplexVector eValues = findEigenValues(a,&qrDialog);
  }
  catch (Exception e) {
    showException(e);
  }
}

void CTestQRGraphicsDlg::OnFileUn() {
  try {
    size_t dim = a.getRowCount();
    a = Matrix::zero(dim,dim);

    for(size_t i = 1; i < dim; i++) {
      a(i,i-1) = 1;
    }
    a(0,dim-1) = 1;

    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }

}

void CTestQRGraphicsDlg::OnFileIllconditioned() {
  try {
    size_t dim = a.getRowCount();
    a = Matrix::zero(dim,dim);

    for(size_t i = 1; i < dim; i++) {
      a(i,i-1) = 1;
    }

    double p1[DEFAULTSIZE+1],p2[DEFAULTSIZE+1];
    for(int i = 0; i <= DEFAULTSIZE; i++) {
      p1[i] = p2[i] = 0;
    }
    p1[0] = 1;
    for(size_t i = 1; i <= dim; i++) {
      p2[0] = 1;
      for(size_t j = 1; j <= i; j++) {
        p2[j] = p1[j] + p1[j-1];
      }
      memcpy(p1,p2,sizeof(p1));
    }
    for(size_t i = 0; i < dim; i++) {
      a(0,i) = -p1[dim-i-1];
    }
    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }
}

void CTestQRGraphicsDlg::OnFileSymmetric() {
  try {
    size_t dim = a.getRowCount();
    for(size_t r = 0; r < dim; r++) {
      for(size_t c = 0; c <= r; c++) {
        a(c,r) = a(r,c) = rand() % 10 - 5;
      }
    }

    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }
}

void CTestQRGraphicsDlg::OnFileRandomAntiSymmetric() {
  try {
    size_t dim = a.getRowCount();
    size_t n = dim - 1;
    a = Matrix::zero(dim,dim);

    double s = 1;
    for(size_t i = 0; i < dim; i++) {
      a(i,n-i) = s;
    }
    a(0,2) = 2;
    a(0,3) = 2;
    a(0,4) = 2;
    a(4,0) = 2;
    a(3,0) = 2;
    a(2,0) = 2;

    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }
}

static double spec[] =  { 2,-3,1,2,-3,-3,-5,-4,-4,2,0,4,-4,3,-4,1 };

void CTestQRGraphicsDlg::OnFileSpecial() {
  try {
    size_t dim = a.getRowCount();
    size_t n = dim;
    for(size_t r = 0; r < n; r++) {
      for(size_t c = 0; c < n; c++) {
        a(r,c) = 0;
      }
    }
    for(size_t c = 0; c < n; c++) {
      a(0,c) = -spec[n-c-1];
    }
    for(size_t r = 1; r < n; r++) {
      a(r,r-1) = 1;
    }
    Invalidate(false);
  }
  catch (Exception e) {
    showException(e);
  }

}

void CTestQRGraphicsDlg::OnOptionsPause() {
  toggleMenuItem(this, ID_OPTIONS_PAUSE);
}

void CTestQRGraphicsDlg::OnOptionsDimension() {
 int dim = (int)a.getRowCount();
  DimensionDlg dlg(dim);
  if(dlg.DoModal() == IDOK) {
    a.setDimension(dlg.m_dimension);
  }
  Invalidate(false);
}
