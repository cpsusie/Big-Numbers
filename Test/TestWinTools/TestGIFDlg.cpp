#include "stdafx.h"
#include "TestGIFDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTestGIFDlg::CTestGIFDlg(CWnd *pParent /*=nullptr*/) : CDialog(CTestGIFDlg::IDD, pParent) {
}

void CTestGIFDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestGIFDlg, CDialog)
    ON_WM_SIZE()
    ON_BN_CLICKED(IDC_BUTTON_LOADGIF          , OnButtonLoadGif          )
    ON_BN_CLICKED(IDC_BUTTON_LOADRESOURCE     , OnButtonLoadResource     )
    ON_BN_CLICKED(IDC_BUTTON_LOADRESOURCEERROR, OnButtonLoadResourceError)
    ON_BN_CLICKED(IDC_BUTTON_START            , OnButtonStart            )
    ON_BN_CLICKED(IDC_BUTTON_STOP             , OnButtonStop             )
    ON_BN_CLICKED(IDC_BUTTON_UNLOAD           , OnButtonUnload           )
    ON_BN_CLICKED(IDC_BUTTON_PAINTALL         , OnButtonPaintAll         )
    ON_BN_CLICKED(IDC_BUTTON_PAINTFIRST       , OnButtonPaintFirst       )
    ON_BN_CLICKED(IDC_BUTTON_PAINTNEXT        , OnButtonPaintNext        )
    ON_BN_CLICKED(IDC_BUTTON_PAINTPREV        , OnButtonPaintPrev        )
    ON_BN_CLICKED(IDC_BUTTON_CLEAR            , OnButtonClear            )
    ON_BN_CLICKED(IDC_BUTTON_HIDE             , OnButtonHide             )
END_MESSAGE_MAP()

BOOL CTestGIFDlg::OnInitDialog() {
    __super::OnInitDialog();

    m_gif.substituteControl(this, IDC_STATIC_GIFAREA);

    m_layoutManager.OnInitDialog(this);
    m_layoutManager.addControl(IDC_STATIC_GIFAREA          , RELATIVE_SIZE );
    m_layoutManager.addControl(IDC_BUTTON_LOADGIF          , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_LOADRESOURCE     , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_LOADRESOURCEERROR, RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_START            , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_STOP             , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_PAINTALL         , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_PAINTFIRST       , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_PAINTNEXT        , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_PAINTPREV        , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_CLEAR            , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_HIDE             , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_BUTTON_UNLOAD           , RELATIVE_X_POS);
    m_layoutManager.addControl(IDCANCEL                    , RELATIVE_X_POS);
    m_layoutManager.addControl(IDC_STATIC_STATE            , RELATIVE_X_POS | RELATIVE_HEIGHT);

    m_frameIndex = 0;

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestGIFDlg::OnButtonLoadGif() {
  CFileDialog dlg(TRUE);
  dlg.m_ofn.lpstrTitle      = _T("Load GIF");
  dlg.m_ofn.lpstrFilter     = _T("Gif files (*.gif)\0*.gif\0"
                                 "All files (*.*)\0*.*\0\0");
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
    showException(e);
  }
}

void CTestGIFDlg::OnButtonLoadResource() {
  loadGifResource(IDR_GIFLOEBER);
}

void CTestGIFDlg::OnButtonLoadResourceError() {
  loadGifResource(IDR_GIFLOEBER+1);
}

void CTestGIFDlg::loadGifResource(int resId) {
  try {
    m_gif.loadFromResource(resId, _T("GIF"));
    m_frameIndex = 0;
    updateState();
  } catch(Exception e) {
    updateState();
    showException(e);
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
  __super::OnSize(nType, cx, cy);
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
    GetDlgItem(IDC_BUTTON_HIDE)->SetWindowText(_T("Show"));
  } else {
    m_gif.ShowWindow(SW_SHOW);
    GetDlgItem(IDC_BUTTON_HIDE)->SetWindowText(_T("Hide"));
  }
  updateState();
}

void CTestGIFDlg::updateState() {
  const CSize &sz = m_gif.getImageSize();

//  const String comment = m_ai.isLoaded() ? m_ai.getComment().cstr() : "";
  GetDlgItem(IDC_STATIC_STATE)->SetWindowText(format(_T("Loaded   :%s\r\n"
                                                      "Suspended:%s\r\n"
                                                      "Painted  :%s\r\n"
                                                      "Playing  :%s\r\n"
                                                      "Visible  :%s\r\n"
                                                      "Size:(%d,%d)\r\n"
                                                      "Current frame:%d\r\n")
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
