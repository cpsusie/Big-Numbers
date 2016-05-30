#include "stdafx.h"
#include "TestGIFDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTestGIFDlg::CTestGIFDlg(CWnd* pParent /*=NULL*/) : CDialog(CTestGIFDlg::IDD, pParent) {
    //{{AFX_DATA_INIT(CTestGIFDlg)
    //}}AFX_DATA_INIT
}

void CTestGIFDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTestGIFDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTestGIFDlg, CDialog)
    //{{AFX_MSG_MAP(CTestGIFDlg)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTONLOADGIF          , OnButtonLoadGif          )
	ON_BN_CLICKED(IDC_BUTTONLOADRESROUCE     , OnButtonLoadResouce      )
	ON_BN_CLICKED(IDC_BUTTONLOADRESROUCEERROR, OnButtonLoadResroucEerror)
	ON_BN_CLICKED(IDC_BUTTONSTART            , OnButtonStart            )
	ON_BN_CLICKED(IDC_BUTTONSTOP             , OnButtonStop             )
	ON_BN_CLICKED(IDC_BUTTONUNLOAD           , OnButtonUnload           )
	ON_BN_CLICKED(IDC_BUTTONPAINTALL         , OnButtonPaintAll         )
	ON_BN_CLICKED(IDC_BUTTONPAINTFIRST       , OnButtonPaintFirst       )
	ON_BN_CLICKED(IDC_BUTTONPAINTNEXT        , OnButtonPaintNext        )
	ON_BN_CLICKED(IDC_BUTTONPAINTPREV        , OnButtonPaintPrev        )
	ON_BN_CLICKED(IDC_BUTTONCLEAR            , OnButtonClear            )
	ON_BN_CLICKED(IDC_BUTTONHIDE             , OnButtonHide             )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CTestGIFDlg::OnInitDialog() {
    CDialog::OnInitDialog();

    m_gif.substituteControl(this, IDC_STATICGIFAREA);

    m_layoutManager.OnInitDialog(this);
    m_layoutManager.addControl(IDC_STATICGIFAREA          , RELATIVE_SIZE );
    m_layoutManager.addControl(IDC_BUTTONLOADGIF          , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONLOADRESROUCE     , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONLOADRESROUCEERROR, RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONSTART            , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONSTOP             , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONPAINTALL         , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONPAINTFIRST       , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONPAINTNEXT        , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONPAINTPREV        , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONCLEAR            , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONHIDE             , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTONUNLOAD           , RELATIVE_X_POS);
    m_layoutManager.addControl(IDCANCEL                   , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_STATICSTATE            , RELATIVE_X_POS | RELATIVE_HEIGHT);

    m_frameIndex = 0;
    
    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestGIFDlg::OnButtonLoadGif() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle      = "Load GIF";
  dlg.m_ofn.lpstrFilter     = "Gif files (*.gif)\0*.gif\0"
                              "All files (*.*)\0*.*\0\0";
  if(dlg.DoModal() != IDOK) {
    return;
  }
  const String fileName = dlg.m_ofn.lpstrFile;

  try {
    m_gif.load(fileName);
    m_frameIndex = 0;
    updateState();
  } catch(Exception e) {
    updateState();
    MessageBox(e.what(), "Error", MB_ICONWARNING);
  }
}

void CTestGIFDlg::OnButtonLoadResouce() {
  loadGifResource(IDR_GIFLOEBER);
}

void CTestGIFDlg::OnButtonLoadResroucEerror() {
  loadGifResource(IDR_GIFLOEBER+1);
}

void CTestGIFDlg::loadGifResource(int resId) {
  try {
    m_gif.loadFromResource(resId, "GIF");
    m_frameIndex = 0;
    updateState();
  } catch(Exception e) {
    updateState();
    MessageBox(e.what(), "Error", MB_ICONWARNING);
  }
}

void CTestGIFDlg::OnButtonStart() {
  m_gif.play();
  m_frameIndex = 0;
  updateState();
}

void CTestGIFDlg::OnButtonStop() {
  m_gif.stop();
  m_frameIndex = 0;
  updateState();
}

void CTestGIFDlg::OnButtonUnload() {
  m_gif.unload();
  m_frameIndex = 0;
  updateState();
}

void CTestGIFDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
}

void CTestGIFDlg::OnButtonPaintAll() {
  m_gif.paintAllFrames();
  m_frameIndex = 0;
  updateState();
}

void CTestGIFDlg::OnButtonPaintFirst() {
  if(m_gif.isLoaded() && !m_gif.isPlaying()) {
    m_frameIndex = 0;
    m_gif.paintFrame(m_frameIndex);
    updateState();
  }
}

void CTestGIFDlg::OnButtonPaintNext() {
  if(m_gif.isLoaded() && !m_gif.isPlaying()) {
    m_frameIndex = (m_frameIndex + 1) % m_gif.getFrameCount();
    m_gif.paintFrame(m_frameIndex);
    updateState();
  }
}

void CTestGIFDlg::OnButtonPaintPrev() {
  if(m_gif.isLoaded() && !m_gif.isPlaying()) {
    m_frameIndex--;
    if(m_frameIndex < 0) {
      m_frameIndex = m_gif.getFrameCount()-1;
    }
    m_gif.paintFrame(m_frameIndex);
    updateState();
  }
}

void CTestGIFDlg::OnButtonClear() {
  m_gif.hide();
  m_frameIndex = 0;
  updateState();
}

void CTestGIFDlg::OnButtonHide() {
  if(m_gif.IsWindowVisible()) {
    m_gif.ShowWindow(SW_HIDE);
    GetDlgItem(IDC_BUTTONHIDE)->SetWindowText("Show");
  } else {
    m_gif.ShowWindow(SW_SHOW);
    GetDlgItem(IDC_BUTTONHIDE)->SetWindowText("Hide");
  }
  updateState();
}

void CTestGIFDlg::updateState() {
  const CSize &sz = m_gif.getImageSize();

//  const String comment = m_ai.isLoaded() ? m_ai.getComment().cstr() : "";
  GetDlgItem(IDC_STATICSTATE)->SetWindowText(format("Loaded   :%s\r\n"
                                                    "Suspended:%s\r\n"
                                                    "Painted  :%s\r\n"
                                                    "Playing  :%s\r\n"
                                                    "Visible  :%s\r\n"
                                                    "Size:(%d,%d)\r\n"
                                                    "Current frame:%d\r\n"
                                                    ,boolToStr(m_gif.isLoaded())
                                                    ,boolToStr(m_gif.isSuspended())
                                                    ,boolToStr(m_gif.isPainted())
                                                    ,boolToStr(m_gif.isPlaying())
                                                    ,boolToStr(m_gif.IsWindowVisible()?true:false)
                                                    ,sz.cx, sz.cy
                                                    ,m_frameIndex
                                                   ).cstr()
                                            );
}
