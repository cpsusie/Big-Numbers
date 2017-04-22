#include "stdafx.h"
#include "dx9_rectDemo.h"
#include "dx9_rectDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialogEx {
public:
  CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
  enum { IDD = IDD_ABOUTBOX };
#endif

  protected:
  virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
  DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


Cdx9_rectDemoDlg::Cdx9_rectDemoDlg(CWnd* pParent /*=NULL*/)
  : CDialogEx(IDD_DX9_RECTDEMO_DIALOG, pParent)
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_device = NULL;
}

void Cdx9_rectDemoDlg::DoDataExchange(CDataExchange* pDX) {
  CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cdx9_rectDemoDlg, CDialogEx)
  ON_WM_SYSCOMMAND()
  ON_WM_PAINT()
  ON_WM_QUERYDRAGICON()
  ON_WM_SIZE()
  ON_CBN_SELCHANGE(IDC_COMBO_ARG1    , OnSelchangeComboArg1    )
  ON_CBN_SELCHANGE(IDC_COMBO_ARG2    , OnSelchangeComboArg2    )
  ON_CBN_SELCHANGE(IDC_COMBO_COLOROP , OnSelchangeComboColorop )
  ON_CBN_SELCHANGE(IDC_COMBO_SRCBLEND, OnSelchangeComboSrcblend)
  ON_CBN_SELCHANGE(IDC_COMBO_DSTBLEND, OnSelchangeComboDstblend)
  ON_WM_SIZING()
  ON_WM_VSCROLL()
END_MESSAGE_MAP()

// Fill the struct that tells D3D how to set up buffers and page flip.
D3DPRESENT_PARAMETERS Cdx9_rectDemoDlg::getPresentParameters() const {

  D3DPRESENT_PARAMETERS param;
  ZeroMemory(&param, sizeof(param));

  CWnd *frameWnd = getGraphicsFrame();
  const CSize sz = getClientRect(frameWnd).Size();
//  frameWnd->m_h
  param.Windowed               = TRUE;
  param.MultiSampleType        = D3DMULTISAMPLE_NONE;
  param.SwapEffect             = D3DSWAPEFFECT_DISCARD;
  param.EnableAutoDepthStencil = FALSE;
  param.BackBufferFormat       = D3DFMT_X8R8G8B8;
  param.hDeviceWindow          = NULL;
  param.Flags                  = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
  param.BackBufferCount        = 1;
  param.BackBufferWidth        = sz.cx;
  param.BackBufferHeight       = sz.cy;
  param.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;
  return param;
}

BOOL Cdx9_rectDemoDlg::OnInitDialog() {
  CDialogEx::OnInitDialog();

  // Add "About..." menu item to system menu.

  // IDM_ABOUTBOX must be in the system command range.
  ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
  ASSERT(IDM_ABOUTBOX < 0xF000);

  CMenu* pSysMenu = GetSystemMenu(FALSE);
  if (pSysMenu != NULL)
  {
    BOOL bNameValid;
    CString strAboutMenu;
    bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
    ASSERT(bNameValid);
    if (!strAboutMenu.IsEmpty())
    {
      pSysMenu->AppendMenu(MF_SEPARATOR);
      pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
    }
  }

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);			// Set big icon
  SetIcon(m_hIcon, FALSE);		// Set small icon

  HDC screenDC = getScreenDC();
  m_app_scale_x = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  m_app_scale_y = (float)(GetDeviceCaps(screenDC, LOGPIXELSX) / 96.0);
  DeleteDC(screenDC);

  m_direct3d = Direct3DCreate9(D3D_SDK_VERSION);

  m_textureOp = D3DTOP_BLENDTEXTUREALPHA;
  m_arg1      = D3DTA_TEXTURE;
  m_arg2      = D3DTA_DIFFUSE;
  m_srcBlend  = D3DBLEND_SRCALPHA;
  m_dstBlend  = D3DBLEND_INVSRCALPHA;
  m_rotation  = 0;

  CFont *font = GetFont();
  LOGFONT lf;
  font->GetLogFont(&lf);
  _tcscpy(lf.lfFaceName, _T("Courier new"));
  m_comboFont.CreateFontIndirect(&lf);

  D3DPRESENT_PARAMETERS present = getPresentParameters();

  m_layoutManager.OnInitDialog(this);

  m_layoutManager.addControl(IDC_STATIC_GRAPHICS , RELATIVE_SIZE                    );
  m_layoutManager.addControl(IDC_COMBO_COLOROP   , RELATIVE_Y_POS | RELATIVE_WIDTH  );
  m_layoutManager.addControl(IDC_COMBO_ARG1      , RELATIVE_Y_POS | RELATIVE_WIDTH  );
  m_layoutManager.addControl(IDC_COMBO_ARG2      , RELATIVE_Y_POS | RELATIVE_WIDTH  );
  m_layoutManager.addControl(IDC_COMBO_SRCBLEND  , RELATIVE_Y_POS | RELATIVE_WIDTH  );
  m_layoutManager.addControl(IDC_COMBO_DSTBLEND  , RELATIVE_Y_POS | RELATIVE_WIDTH  );
  m_layoutManager.addControl(IDC_STATIC_COLOROP  , RELATIVE_Y_POS                   );
  m_layoutManager.addControl(IDC_STATIC_ARG1     , RELATIVE_Y_POS                   );
  m_layoutManager.addControl(IDC_STATIC_ARG2     , RELATIVE_Y_POS                   );
  m_layoutManager.addControl(IDC_STATIC_SRCBLEND , RELATIVE_Y_POS                   );
  m_layoutManager.addControl(IDC_STATIC_DSTBLEND , RELATIVE_Y_POS                   );
  m_layoutManager.addControl(IDC_SLIDER_ROTATE   , RELATIVE_POSITION                );

  m_direct3d->CreateDevice(D3DADAPTER_DEFAULT
    ,D3DDEVTYPE_HAL
    ,*getGraphicsFrame()
    ,D3DCREATE_FPU_PRESERVE | D3DCREATE_SOFTWARE_VERTEXPROCESSING
    ,&present
    ,&m_device);

  if(!m_device) {
    return false;
  }

  initComboboxes();
  CSliderCtrl *slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_ROTATE);
  slider->SetRange(0,360);
  slider->SetPos(m_rotation);

  loadTexture();
  loadBoard();
  m_renderTarget = createRenderTarget(getSize(m_boardSurface));

  return TRUE;  // return TRUE  unless you set the focus to a control
}

void Cdx9_rectDemoDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if ((nID & 0xFFF0) == IDM_ABOUTBOX)	{
    CAboutDlg dlgAbout;
    dlgAbout.DoModal();
  } else {
    CDialogEx::OnSysCommand(nID, lParam);
  }
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Cdx9_rectDemoDlg::OnPaint() {
  if (IsIconic())	{
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
    CDialogEx::OnPaint();
    render();
  }
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Cdx9_rectDemoDlg::OnQueryDragIcon()
{
  return static_cast<HCURSOR>(m_hIcon);
}

void Cdx9_rectDemoDlg::loadTexture() {
  FileNameSplitter spl(__FILE__);
  spl.setDir(FileNameSplitter::getChildName(spl.getDir(), _T("res")));
  spl.setFileName(_T("whiteking128x128")).setExtension(_T("bmp"));
  m_someTexture = loadTextureFromFile(spl.getFullPath());
  //  m_someTexture = loadTextureFromResource(IDR_WHITEKINGJPG,"JPG");
}

void Cdx9_rectDemoDlg::loadBoard() {
  FileNameSplitter spl(__FILE__);
  spl.setDir(FileNameSplitter::getChildName(spl.getDir(), _T("res")));
  spl.setFileName(_T("Board")).setExtension(_T("jpg"));
  m_boardSurface = loadSurfaceFromFile(spl.getFullPath());
  //  m_someTexture = loadTextureFromResource(IDR_WHITEKINGJPG,"JPG");
}

ComboElement::ComboElement(int value, const char *s1, const char *comment) {
  m_value = value;
  m_text = s1 + spaceString(34 - strlen(s1)) + comment;
}

#define COMBOELEMENT(v, s) ComboElement(v, #v, s)

static const ComboElement colorOpElements[] = {
  COMBOELEMENT(D3DTOP_DISABLE                    , ""                                                                           )
  ,COMBOELEMENT(D3DTOP_SELECTARG1                 , "the default"                                                                )
  ,COMBOELEMENT(D3DTOP_SELECTARG2                 , ""                                                                           )
  ,COMBOELEMENT(D3DTOP_MODULATE                   , "multiply args together"                                                     )
  ,COMBOELEMENT(D3DTOP_MODULATE2X                 , "multiply and  1 bit"                                                        )
  ,COMBOELEMENT(D3DTOP_MODULATE4X                 , "multiply and  2 bits"                                                       )
  ,COMBOELEMENT(D3DTOP_ADD                        , "add arguments together"                                                     )
  ,COMBOELEMENT(D3DTOP_ADDSIGNED                  , "add with -0.5 bias"                                                         )
  ,COMBOELEMENT(D3DTOP_ADDSIGNED2X                , "as above but left  1 bit"                                                   )
  ,COMBOELEMENT(D3DTOP_SUBTRACT                   , "Arg1 - Arg2, with no saturation"                                            )
  ,COMBOELEMENT(D3DTOP_ADDSMOOTH                  , "Arg1 + Arg2 - Arg1*Arg2"                                                    )
  ,COMBOELEMENT(D3DTOP_BLENDDIFFUSEALPHA          , "iterated alpha. Arg1*(Alpha) + Arg2*(1-Alpha)"                              )
  ,COMBOELEMENT(D3DTOP_BLENDTEXTUREALPHA          , "texture alpha.  Arg1*(Alpha) + Arg2*(1-Alpha)"                              )
  ,COMBOELEMENT(D3DTOP_BLENDFACTORALPHA           , "alpha from D3DRS_TEXTUREFACTOR. Arg1*(Alpha) + Arg2*(1-Alpha)"              )
  ,COMBOELEMENT(D3DTOP_BLENDTEXTUREALPHAPM        , "texture alpha. pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)"            )
  ,COMBOELEMENT(D3DTOP_BLENDCURRENTALPHA          , "by alpha of current color. pre-multiplied arg1 input: Arg1 + Arg2*(1-Alpha)")
  ,COMBOELEMENT(D3DTOP_PREMODULATE                , "modulate with next texture before use"                                      )
  ,COMBOELEMENT(D3DTOP_MODULATEALPHA_ADDCOLOR     , "Arg1.RGB + Arg1.A*Arg2.RGB     COLOROP only"                                )
  ,COMBOELEMENT(D3DTOP_MODULATECOLOR_ADDALPHA     , "Arg1.RGB*Arg2.RGB + Arg1.A     COLOROP only"                                )
  ,COMBOELEMENT(D3DTOP_MODULATEINVALPHA_ADDCOLOR  , "(1-Arg1.A)*Arg2.RGB + Arg1.RGB COLOROP only"                                )
  ,COMBOELEMENT(D3DTOP_MODULATEINVCOLOR_ADDALPHA  , "(1-Arg1.RGB)*Arg2.RGB + Arg1.A COLOROP only"                                )
  ,COMBOELEMENT(D3DTOP_BUMPENVMAP                 , "per pixel env map perturbation"                                             )
  ,COMBOELEMENT(D3DTOP_BUMPENVMAPLUMINANCE        , "with luminance channel"                                                     )
  ,COMBOELEMENT(D3DTOP_DOTPRODUCT3                , ""                                                                           )
  ,COMBOELEMENT(D3DTOP_MULTIPLYADD                , "Arg0 + Arg1*Arg2"                                                           )
  ,COMBOELEMENT(D3DTOP_LERP                       , "(Arg0)*Arg1 + (1-Arg0)*Arg2"                                                )
};

static const ComboElement argElements[] = {
  COMBOELEMENT(D3DTA_SELECTMASK    ,"mask for arg selector"                               )
  ,COMBOELEMENT(D3DTA_DIFFUSE       ,"select diffuse color (read only)"                    )
  ,COMBOELEMENT(D3DTA_CURRENT       ,"select stage destination register (read/write)"      )
  ,COMBOELEMENT(D3DTA_TEXTURE       ,"select texture color (read only)"                    )
  ,COMBOELEMENT(D3DTA_TFACTOR       ,"select D3DRS_TEXTUREFACTOR (read only)"              )
  ,COMBOELEMENT(D3DTA_SPECULAR      ,"select specular color (read only)"                   )
  ,COMBOELEMENT(D3DTA_TEMP          ,"select temporary register color (read/write)"        )
  ,COMBOELEMENT(D3DTA_CONSTANT      ,"select texture stage constant"                       )
  ,COMBOELEMENT(D3DTA_COMPLEMENT    ,"take 1.0 - x (read modifier)"                        )
  ,COMBOELEMENT(D3DTA_ALPHAREPLICATE,"replicate alpha to color components (read modifier)" )
};

static const ComboElement blendRenderStateElements[] = {
  COMBOELEMENT(D3DBLEND_ZERO               ,"")
  ,COMBOELEMENT(D3DBLEND_ONE                ,"")
  ,COMBOELEMENT(D3DBLEND_SRCCOLOR           ,"")
  ,COMBOELEMENT(D3DBLEND_INVSRCCOLOR        ,"")
  ,COMBOELEMENT(D3DBLEND_SRCALPHA           ,"")
  ,COMBOELEMENT(D3DBLEND_INVSRCALPHA        ,"")
  ,COMBOELEMENT(D3DBLEND_DESTALPHA          ,"")
  ,COMBOELEMENT(D3DBLEND_INVDESTALPHA       ,"")
  ,COMBOELEMENT(D3DBLEND_DESTCOLOR          ,"")
  ,COMBOELEMENT(D3DBLEND_INVDESTCOLOR       ,"")
  ,COMBOELEMENT(D3DBLEND_SRCALPHASAT        ,"")
  ,COMBOELEMENT(D3DBLEND_BOTHSRCALPHA       ,"")
  ,COMBOELEMENT(D3DBLEND_BOTHINVSRCALPHA    ,"")
  ,COMBOELEMENT(D3DBLEND_BLENDFACTOR        ,"")
  ,COMBOELEMENT(D3DBLEND_INVBLENDFACTOR     ,"")
  ,COMBOELEMENT(D3DBLEND_SRCCOLOR2          ,"")
  ,COMBOELEMENT(D3DBLEND_INVSRCCOLOR2       ,"")
};

void Cdx9_rectDemoDlg::initComboboxes() {
  initCombo(IDC_COMBO_COLOROP , m_textureOp, colorOpElements         , ARRAYSIZE(colorOpElements         ));
  initCombo(IDC_COMBO_ARG1    , m_arg1     , argElements             , ARRAYSIZE(argElements             ));
  initCombo(IDC_COMBO_ARG2    , m_arg2     , argElements             , ARRAYSIZE(argElements             ));
  initCombo(IDC_COMBO_SRCBLEND, m_srcBlend , blendRenderStateElements, ARRAYSIZE(blendRenderStateElements));
  initCombo(IDC_COMBO_DSTBLEND, m_dstBlend , blendRenderStateElements, ARRAYSIZE(blendRenderStateElements));
}

void Cdx9_rectDemoDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
  CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
  m_rotation = nPos;
  Invalidate(FALSE);
}

void Cdx9_rectDemoDlg::updateGraphics() {
  m_textureOp = (D3DTEXTUREOP)getComboValue(IDC_COMBO_COLOROP , colorOpElements         );
  m_arg1      =               getComboValue(IDC_COMBO_ARG1    , argElements             );
  m_arg2      =               getComboValue(IDC_COMBO_ARG1    , argElements             );
  m_srcBlend  =     (D3DBLEND)getComboValue(IDC_COMBO_SRCBLEND, blendRenderStateElements);
  m_dstBlend  =     (D3DBLEND)getComboValue(IDC_COMBO_DSTBLEND, blendRenderStateElements);
  Invalidate(FALSE);
}

int Cdx9_rectDemoDlg::getComboValue(int id, const ComboElement *a) const {
  CComboBox *cb = (CComboBox*)GetDlgItem(id);
  int index = cb->GetCurSel();
  return a[index].m_value;
}

void Cdx9_rectDemoDlg::initCombo(int id, int startValue, const ComboElement *a, int n) {
  CComboBox *cb = (CComboBox*)GetDlgItem(id);
  cb->SetFont(&m_comboFont);
  int startIndex = 0;
  for(int i = 0; i < n; i++) {
    const ComboElement &e = a[i];
    if(e.m_value == startValue) {
      startIndex = i;
    }
    cb->AddString(e.m_text.cstr());
  }
  cb->SetCurSel(startIndex);
}

void Cdx9_rectDemoDlg::OnSelchangeComboArg1() {
  updateGraphics();
}

void Cdx9_rectDemoDlg::OnSelchangeComboArg2() {
  updateGraphics();
}

void Cdx9_rectDemoDlg::OnSelchangeComboColorop() {
  updateGraphics();
}

void Cdx9_rectDemoDlg::OnSelchangeComboSrcblend() {
  updateGraphics();
}

void Cdx9_rectDemoDlg::OnSelchangeComboDstblend() {
  updateGraphics();
}

void Cdx9_rectDemoDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  CDialog::OnSizing(fwSide, pRect);
  Invalidate(FALSE);
}

void Cdx9_rectDemoDlg::OnSize(UINT nType, int cx, int cy) {
  CDialog::OnSize(nType, cx, cy);

  m_layoutManager.OnSize(nType, cx, cy);

  //  if(m_device) {
  //    Invalidate(FALSE);
  //  }
  /*
  if(m_device) {
  D3DPRESENT_PARAMETERS present = getPresentParameters();
  if(present.BackBufferWidth && present.BackBufferHeight) {
  if(m_someTexture) {
  m_someTexture->Release();
  m_someTexture = NULL;
  }

  CHECKD3DRESULT(m_device->Reset(&present));
  loadTexture();

  Invalidate(FALSE);
  }
  }
  */
}

// rendering_2d() sets the Direct3D transformation matrix for 2D rendering.
// In an API geared toward 2D rendering you could probably just delete this function.
//
IDirect3DSurface9 *Cdx9_rectDemoDlg::createSurface(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  IDirect3DSurface9 *surface;
  CHECKD3DRESULT(m_device->CreateOffscreenPlainSurface(size.cx, size.cy, format, pool, &surface, NULL));
  return surface;
}

IDirect3DTexture9 *Cdx9_rectDemoDlg::createTexture(const CSize &size, D3DFORMAT format, D3DPOOL pool) {
  IDirect3DTexture9 *texture;

  // Create an alpha texture
  CHECKD3DRESULT(m_device->CreateTexture(size.cx, size.cy, 1, D3DUSAGE_DYNAMIC, format, pool, &texture, NULL));
  int levels = texture->GetLevelCount();
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(texture->GetLevelDesc(0, &desc));
  return texture;
}

IDirect3DSurface9 *Cdx9_rectDemoDlg::createRenderTarget(const CSize &size, D3DFORMAT format) {
  IDirect3DSurface9 *surface;

  IDirect3DSurface9* oldRenderTarget;
  CHECKD3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(oldRenderTarget->GetDesc(&desc));
  oldRenderTarget->Release();

  if(format == D3DFMT_FORCE_DWORD) {
    format = desc.Format;
  }
  CHECKD3DRESULT(m_device->CreateRenderTarget(size.cx, size.cy, format, desc.MultiSampleType, desc.MultiSampleQuality, false, &surface, NULL));
  //  D3DSURFACE_DESC newDesc;
  //  CHECKD3DRESULT(surface->GetDesc(&newDesc));

  return surface;
}

IDirect3DTexture9 *Cdx9_rectDemoDlg::loadTextureFromResource(int resId, const String &typeName) {
  ByteArray tmp;
  tmp.loadFromResource(resId, typeName.cstr());
  IDirect3DTexture9 *result;

  CHECKD3DRESULT(D3DXCreateTextureFromFileInMemoryEx(
     m_device
    ,tmp.getData(), (UINT)tmp.size()
    ,D3DX_DEFAULT, D3DX_DEFAULT, 1
    ,D3DUSAGE_DYNAMIC
    ,D3DFMT_A8R8G8B8
    ,D3DPOOL_DEFAULT
    ,D3DX_FILTER_NONE
    ,D3DX_FILTER_NONE
    ,0xffffffff
    ,NULL
    ,NULL
    ,&result));
  return result;
}

IDirect3DTexture9 *Cdx9_rectDemoDlg::loadTextureFromFile(const String &fileName) {
  IDirect3DTexture9 *result;
  CHECKD3DRESULT(D3DXCreateTextureFromFileEx(
     m_device, fileName.cstr()
    ,D3DX_DEFAULT, D3DX_DEFAULT, 1
    ,D3DUSAGE_DYNAMIC
    ,D3DFMT_A8R8G8B8
    ,D3DPOOL_DEFAULT
    ,D3DX_FILTER_NONE
    ,D3DX_FILTER_NONE
    ,0xffffffff
    ,NULL
    ,NULL
    ,&result));

  //  m_device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
  //  m_device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
  return result;
}

IDirect3DSurface9 *Cdx9_rectDemoDlg::loadSurfaceFromFile(const String &fileName) {
  CPicture picture;
  picture.load(fileName);
  const CSize size = picture.getSize();
  IDirect3DSurface9 *surface = createSurface(size, D3DFMT_X8R8G8B8, D3DPOOL_SYSTEMMEM);
  paintPictureOnSurface(surface, picture);
  return surface;
}

void Cdx9_rectDemoDlg::paintPictureOnSurface(IDirect3DSurface9 *surface, CPicture &picture) {
  HDC     dstDC;
  CHECKD3DRESULT(surface->GetDC(&dstDC));

  picture.show(dstDC);

  CHECKD3DRESULT(surface->ReleaseDC(dstDC));
}

void Cdx9_rectDemoDlg::copySurfaceToTexture(IDirect3DTexture9 *dst, IDirect3DSurface9 *src) {
  /*
  IDirect3DSurface9 *dstSurface;
  CHECKD3DRESULT(dst->GetSurfaceLevel(0, &dstSurface));

  CHECKD3DRESULT(m_device->UpdateSurface(src, NULL, dstSurface, NULL));
  dstSurface->Release();
  */
  const CSize sz = getSize(src);

  D3DLOCKED_RECT srcRect;
  D3DLOCKED_RECT dstRect;
  CHECKD3DRESULT(src->LockRect(   &srcRect, NULL, D3DLOCK_READONLY));
  CHECKD3DRESULT(dst->LockRect(0, &dstRect, NULL, D3DLOCK_NOOVERWRITE));

  D3DCOLOR *srcPixelRow = (D3DCOLOR*)srcRect.pBits;
  D3DCOLOR *dstPixelRow = (D3DCOLOR*)dstRect.pBits;
  const int PixelPerRow = srcRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < sz.cy; y++, srcPixelRow += PixelPerRow, srcPixelRow += PixelPerRow) {
    D3DCOLOR *srcPixel = srcPixelRow;
    D3DCOLOR *dstPixel = dstPixelRow;
    for(int x = sz.cx; x--;) {
      *(dstPixel++) = *(srcPixel++);
    }
  }

  CHECKD3DRESULT(dst->UnlockRect(0));
  CHECKD3DRESULT(src->UnlockRect());
}

void Cdx9_rectDemoDlg::makeWhiteTransparent(IDirect3DTexture9 *texture) {
  D3DLOCKED_RECT lockedRect;
  CHECKD3DRESULT(texture->LockRect(0, &lockedRect, NULL, D3DLOCK_NOOVERWRITE/*D3DLOCK_DISCARD*/));
  makeWhiteTransparent(lockedRect, getSize(texture));
  CHECKD3DRESULT(texture->UnlockRect(0));
}

void Cdx9_rectDemoDlg::makeSemiTransparentOpague(IDirect3DTexture9 *texture) {
  D3DLOCKED_RECT lockedRect;
  CHECKD3DRESULT(texture->LockRect(0, &lockedRect, NULL, D3DLOCK_NOOVERWRITE/*D3DLOCK_DISCARD*/));
  makeSemiTransparentOpague(lockedRect, getSize(texture));
  CHECKD3DRESULT(texture->UnlockRect(0));
}

void Cdx9_rectDemoDlg::makeWhiteTransparent(IDirect3DSurface9 *surface) {
  D3DLOCKED_RECT lockedRect;
  CHECKD3DRESULT(surface->LockRect(&lockedRect, NULL, D3DLOCK_NOOVERWRITE/*D3DLOCK_DISCARD*/));
  makeWhiteTransparent(lockedRect, getSize(surface));
  CHECKD3DRESULT(surface->UnlockRect());
}

void Cdx9_rectDemoDlg::makeWhiteTransparent(D3DLOCKED_RECT &lockedRect, const CSize size) { // static
  D3DCOLOR *pixelRow = (D3DCOLOR*)lockedRect.pBits;
  const int PixelPerRow = lockedRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < size.cy; y++, pixelRow += PixelPerRow) {   
    D3DCOLOR *pixel = pixelRow;
    for(int x = 0; x < size.cx; x++, pixel++) {
      if((*pixel & 0xffffff) == 0xffffff) {
        *pixel = 0x000000ff;
      } else {
        *pixel |= 0xff000000;
      }
    }
  }
}

void Cdx9_rectDemoDlg::makeSemiTransparentOpague(D3DLOCKED_RECT &lockedRect, const CSize size) { // static
  D3DCOLOR *pixelRow = (D3DCOLOR*)lockedRect.pBits;
  const int PixelPerRow = lockedRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < size.cy; y++, pixelRow += PixelPerRow) {   
    D3DCOLOR *pixel = pixelRow;
    for(int x = 0; x < size.cx; x++, pixel++) {
      if((*pixel & 0xff000000) != 0x00000000) {
        *pixel |= 0xff000000;
      }
    }
  }
}

void Cdx9_rectDemoDlg::makeOpaque(D3DLOCKED_RECT &lockedRect, const CSize size) { // static
  D3DCOLOR *pixelRow = (D3DCOLOR*)lockedRect.pBits;
  const int PixelPerRow = lockedRect.Pitch / sizeof(D3DCOLOR);
  for(int y = 0; y < size.cy; y++, pixelRow += PixelPerRow) {   
    D3DCOLOR *pixel = pixelRow;
    for(int x = 0; x < size.cx; x++, pixel++) {
      *pixel |= 0xff000000;
    }
  }
}

void Cdx9_rectDemoDlg::makeOpaque(IDirect3DSurface9 *surface) {
  D3DLOCKED_RECT lockedRect;
  CHECKD3DRESULT(surface->LockRect(&lockedRect, NULL, D3DLOCK_DISCARD));
  makeOpaque(lockedRect, getSize(surface));
  CHECKD3DRESULT(surface->UnlockRect());
}

CSize Cdx9_rectDemoDlg::getSize(IDirect3DSurface9 *surface) {
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(surface->GetDesc(&desc));
  return CSize(desc.Width, desc.Height);
}

CSize Cdx9_rectDemoDlg::getSize(IDirect3DTexture9 *texture) {
  D3DSURFACE_DESC desc;
  CHECKD3DRESULT(texture->GetLevelDesc(0, &desc));
  return CSize(desc.Width, desc.Height);
}

#define D3DFVF_BLENDVERTEX ( D3DFVF_XYZ | D3DFVF_TEX1 )

class BlendVertex {
  float m_x, m_y, m_z, m_tu, m_tv;
public:
  BlendVertex() {
  }
  BlendVertex(int x, int y, int tu, int tv) : m_x((float)x), m_y((float)y), m_z(0), m_tu((float)tu), m_tv((float)tv) {
  }
  BlendVertex(const CPoint &p, const CPoint &tp) : m_x((float)p.x), m_y((float)p.y), m_z(0), m_tu((float)tp.x), m_tv((float)tp.y) {
  }
};

#define ORIGIN CPoint(0,0)

#define SetTextureColorStage( dev, stage, arg1, op, arg2 )                    \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_COLOROP  , op   )); \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_COLORARG1, arg1 )); \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_COLORARG2, arg2 ));

#define SetTextureAlphaStage( dev, stage, arg1, op, arg2 )                    \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_ALPHAOP  , op   )); \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_ALPHAARG1, arg1 )); \
  CHECKD3DRESULT(dev->SetTextureStageState( stage, D3DTSS_ALPHAARG2, arg2 ));

void Cdx9_rectDemoDlg::alphaBlend(IDirect3DTexture9 *texture, const CRect &dstRect) {
  const CPoint topLeft = dstRect.TopLeft();
  const CSize  size    = dstRect.Size();
  BlendVertex p[4];
  p[0] = BlendVertex(topLeft                                         , CPoint(0, 0));
  p[1] = BlendVertex(CPoint(topLeft.x + size.cx, topLeft.y)          , CPoint(1, 0));
  p[2] = BlendVertex(topLeft   + size                                , CPoint(1, 1));
  p[3] = BlendVertex(CPoint(topLeft.x          , topLeft.y + size.cy), CPoint(0, 1));

  //  CHECKD3DRESULT(m_device->SetSamplerState(     0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
  //  CHECKD3DRESULT(m_device->SetSamplerState(     0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR));

  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , TRUE         ));
  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_ALPHATESTENABLE   , TRUE         ));

  SetTextureColorStage(m_device, 0, m_arg1, m_textureOp, m_arg2);
  SetTextureAlphaStage(m_device, 0, D3DTA_TEXTURE, D3DTOP_MODULATE , D3DTA_DIFFUSE);
  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_CULLMODE          , D3DCULL_NONE ));
  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_ZENABLE           , FALSE        ));

  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_SRCBLEND          , m_srcBlend   ));
  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_DESTBLEND         , m_dstBlend   ));
  CHECKD3DRESULT(m_device->SetTexture(0, texture ));
  CHECKD3DRESULT(m_device->SetFVF(D3DFVF_BLENDVERTEX));
  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, p, sizeof(BlendVertex)));

  CHECKD3DRESULT(m_device->SetRenderState( D3DRS_ALPHABLENDENABLE  , FALSE));
}

class LineVertex {
  float m_x, m_y, m_z;
  D3DCOLOR m_color;
public:
  LineVertex() {
  }
  LineVertex(int x, int y, D3DCOLOR color) : m_x((float)x), m_y((float)y), m_z(0), m_color(color) {
  }
  LineVertex(const CPoint &p, D3DCOLOR color) : m_x((float)p.x), m_y((float)p.y), m_z(0), m_color(color) {
  }
};

void Cdx9_rectDemoDlg::line(const CPoint &p1, const CPoint &p2, D3DCOLOR color) {
  LineVertex p[2];

  p[0] = LineVertex(p1,color);
  p[1] = LineVertex(p2,color);

  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_LINELIST, 1, p, sizeof(LineVertex)));
}

void Cdx9_rectDemoDlg::drawSolidRect(CPoint &topLeft, CSize &size, D3DCOLOR color) {
  LineVertex p[4];
  p[0] = LineVertex(topLeft                                 , color);
  p[1] = LineVertex(topLeft.x + size.cx, topLeft.y          , color);
  p[2] = LineVertex(topLeft + size                          , color);
  p[3] = LineVertex(topLeft.x          , topLeft.y + size.cy, color);
  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, p, sizeof(LineVertex)));
}

void Cdx9_rectDemoDlg::rectangle(const CPoint &topLeft, const CSize &size, D3DCOLOR color) {
  LineVertex p[5];
  p[0] = LineVertex(topLeft                                 , color);
  p[1] = LineVertex(topLeft.x + size.cx, topLeft.y          , color);
  p[2] = LineVertex(topLeft + size                          , color);
  p[3] = LineVertex(topLeft.x          , topLeft.y + size.cy, color);
  p[4] = p[0];
  CHECKD3DRESULT(m_device->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, p, sizeof(LineVertex)));
}


bool Cdx9_rectDemoDlg::render() {
  if(!m_device) {
    return false;  // Haven't been initialized yet!
  }

  CHECKD3DRESULT(m_device->BeginScene());

  IDirect3DSurface9* oldRenderTarget;
  CHECKD3DRESULT(m_device->GetRenderTarget(0, &oldRenderTarget));
  CHECKD3DRESULT(m_device->SetRenderTarget(0, m_renderTarget));

  unsigned long clear_color = 0xffffffff;
  CHECKD3DRESULT(m_device->Clear(0, NULL, D3DCLEAR_TARGET, clear_color, 1.0f, 0));

  CHECKD3DRESULT(m_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE));
  CHECKD3DRESULT(m_device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1));
  CHECKD3DRESULT(m_device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE));
  CHECKD3DRESULT(m_device->SetRenderState(D3DRS_LIGHTING, FALSE));

  const CSize winSize = getSize(m_boardSurface); //getGraphicsSize();

  render2d(winSize);
  /*
  D3DCOLOR lineColor = 0xff000000;
  for(int i = 0; i < winSize.cx; i += 10) {
  line(CPoint(i, 0), CPoint(i, winSize.cy), lineColor);
  }

  for(int j = 0; j < winSize.cy; j += 10) {
  line(CPoint(0, j), CPoint(winSize.cx, j), lineColor);
  }

  const CPoint center(winSize.cx/2, winSize.cy/2);

  const D3DCOLOR solidColor = 0x00778899;
  drawSolidRect(CPoint(center.x - 50, center.y - 50), CSize(100,100), solidColor);

  const D3DCOLOR outlineColor = 0xffb0c7ee;
  rectangle(CPoint(center.x - 100, center.y - 100), CSize(200,200), outlineColor);
  */

  D3DSURFACE_DESC boardDesc, renderDesc;
  CHECKD3DRESULT(m_boardSurface->GetDesc(&boardDesc ));
  CHECKD3DRESULT(m_renderTarget->GetDesc(&renderDesc));
  CHECKD3DRESULT(m_device->UpdateSurface(m_boardSurface, NULL, m_renderTarget, NULL));

  CSize textureSize(74,74); // = getSize(m_someTexture);
  CPoint leftTop(0,0); // leftTop(center.x - textureSize.cx, center.y - textureSize.cy);
  alphaBlend(m_someTexture, CRect(leftTop,textureSize));

  CHECKD3DRESULT(m_device->EndScene());
  D3DSURFACE_DESC oldRenderDesc;
  CHECKD3DRESULT(oldRenderTarget->GetDesc(&oldRenderDesc));

  CHECKD3DRESULT(m_device->StretchRect(m_renderTarget, NULL, oldRenderTarget, NULL, D3DTEXF_NONE));
  CHECKD3DRESULT(m_device->SetRenderTarget(0, oldRenderTarget));
  oldRenderTarget->Release();
  CHECKD3DRESULT(m_device->Present(NULL, NULL, NULL, NULL));

  return true;
}


void Cdx9_rectDemoDlg::render2d(const CSize &size) {
  D3DMATRIX m;
  memset(&m, 0, sizeof(D3DMATRIX));

  m._11 = 2.0f * m_app_scale_x / (float)size.cx;
  m._41 = -m_app_scale_x;
  m._22 = -2.0f * m_app_scale_y / (float)size.cy;
  m._42 = m_app_scale_y;

  m._33 = 1;
  m._43 = 0;

  m._34 = 0;
  m._44 = 1;

  CHECKD3DRESULT(m_device->SetTransform(D3DTS_PROJECTION, &m));
  /*
  D3DXMATRIX matWorld;
  D3DXMATRIX matTrans1, matTrans2;
  D3DXMATRIX matRot;

  const CSize sz = getSize(m_someTexture);;

  D3DXMatrixTranslation( &matTrans1, -sz.cx/2, -sz.cy/2, 0);
  D3DXMatrixTranslation( &matTrans2,  sz.cx/2,  sz.cy/2, 0);

  D3DXMatrixRotationZ(  &matRot, GRAD2RAD(m_rotation));
  matWorld = matTrans2 * matRot * matTrans1;
  CHECKD3DRESULT(m_device->SetTransform( D3DTS_WORLD, &matWorld ));
  */
  const CSize sz = getSize(m_someTexture);
  const D3DXVECTOR2 rotationCenter(37,37);
  D3DXMATRIX matWorld;
  CHECKD3DRESULT(m_device->SetTransform( D3DTS_WORLD, D3DXMatrixAffineTransformation2D(&matWorld, 1, &rotationCenter, (float)GRAD2RAD(m_rotation), NULL)));

}

BOOL Cdx9_rectDemoDlg::PreTranslateMessage(MSG* pMsg) {
  return CDialog::PreTranslateMessage(pMsg);
}

String getD3DErrorMsg(HRESULT hr) {

#define casepr(v) case v: return #v

  switch(hr) {
    casepr(D3DERR_WRONGTEXTUREFORMAT               );
    casepr(D3DERR_UNSUPPORTEDCOLOROPERATION        );
    casepr(D3DERR_UNSUPPORTEDCOLORARG              );
    casepr(D3DERR_UNSUPPORTEDALPHAOPERATION        );
    casepr(D3DERR_UNSUPPORTEDALPHAARG              );
    casepr(D3DERR_TOOMANYOPERATIONS                );
    casepr(D3DERR_CONFLICTINGTEXTUREFILTER         );
    casepr(D3DERR_UNSUPPORTEDFACTORVALUE           );
    casepr(D3DERR_CONFLICTINGRENDERSTATE           );
    casepr(D3DERR_UNSUPPORTEDTEXTUREFILTER         );
    casepr(D3DERR_CONFLICTINGTEXTUREPALETTE        );
    casepr(D3DERR_DRIVERINTERNALERROR              );
    casepr(D3DERR_NOTFOUND                         );
    casepr(D3DERR_MOREDATA                         );
    casepr(D3DERR_DEVICELOST                       );
    casepr(D3DERR_DEVICENOTRESET                   );
    casepr(D3DERR_NOTAVAILABLE                     );
    casepr(D3DERR_OUTOFVIDEOMEMORY                 );
    casepr(D3DERR_INVALIDDEVICE                    );
    casepr(D3DERR_INVALIDCALL                      );
    casepr(D3DERR_DRIVERINVALIDCALL                );
    casepr(D3DERR_WASSTILLDRAWING                  );
    casepr(D3DERR_DEVICEREMOVED                    );
    casepr(D3DOK_NOAUTOGEN                         );
    casepr(S_NOT_RESIDENT                          );
    casepr(S_RESIDENT_IN_SHARED_MEMORY             );
    casepr(S_PRESENT_MODE_CHANGED                  );
    casepr(S_PRESENT_OCCLUDED                      );
    casepr(D3DERR_DEVICEHUNG                       );
  default: return format(_T("Unknown D3D-error:%x"), hr);
  }
}

#ifdef _DEBUG

void checkD3DResult(TCHAR *fileName, int line, HRESULT hr) {
  if(hr != D3D_OK) {
    AfxMessageBox(format(_T("D3D-error %s in %s, line %d"), getD3DErrorMsg(hr).cstr(), fileName, line).cstr(), MB_ICONSTOP);
    exit(-1);;
  }
}

#else

void checkD3DResult(HRESULT hr) {
  if(hr != D3D_OK) {
    AfxMessageBox(format(_T("D3D-error %s"), getD3DErrorMsg(hr).cstr()).cstr(), MB_ICONSTOP);
    exit(-1);
  }
}

#endif
