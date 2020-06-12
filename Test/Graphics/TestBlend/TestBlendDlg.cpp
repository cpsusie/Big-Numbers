#include "stdafx.h"
#include "TestBlendDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CTestBlendDlg::CTestBlendDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestBlendDlg::IDD, pParent) {
    m_srcConstAlpha = 255;
    m_alpha1        = 255;
    m_alpha2        = 0;
    m_scale         = 100;
    m_pickMode      = PICK_IDLE;
    m_stretchMode   = _T("BLACKONWHITE");
    m_hIcon = theApp.LoadIcon(IDR_MAINFRAME);
}

void CTestBlendDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDITSRCCONSTALPHA, m_srcConstAlpha);
    DDV_MinMaxUInt(pDX, m_srcConstAlpha, 0, 255);
    DDX_Text(pDX, IDC_EDITALPHA1, m_alpha1);
    DDV_MinMaxUInt(pDX, m_alpha1, 0, 255);
    DDX_Text(pDX, IDC_EDITALPHA2, m_alpha2);
    DDV_MinMaxUInt(pDX, m_alpha2, 0, 255);
    DDX_Text(pDX, IDC_EDITSCALE, m_scale);
    DDV_MinMaxUInt(pDX, m_scale, 1, 200);
    DDX_CBString(pDX, IDC_COMBOSTRETCHMODE, m_stretchMode);
}

BEGIN_MESSAGE_MAP(CTestBlendDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_BUTTONDRAW                   , OnButtonDraw                            )
    ON_BN_CLICKED(IDC_BUTTONBLEND                  , OnButtonBlend                           )
    ON_BN_CLICKED(IDC_BUTTONPICKROTATIONDESTINATION, OnBnClickedButtonPickRotationDestination)
    ON_BN_CLICKED(IDC_BUTTONPICKROTATIONCENTER     , OnBnClickedButtonPickRotationCenter     )
END_MESSAGE_MAP()

BOOL CTestBlendDlg::OnInitDialog() {
    __super::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    theApp.m_device.attach(m_hWnd);
    setRotationCenter(CPoint(37,37));
    setRotationDestination(CPoint(37,37));
    CSliderCtrl *slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDERDEGREE);
    slider->SetRange(0,360);
    return TRUE;
}

void CTestBlendDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM)dc.GetSafeHdc(), 0);

    const int   cxIcon = GetSystemMetrics(SM_CXICON);
    const int   cyIcon = GetSystemMetrics(SM_CYICON);
    const CRect rect   = getClientRect(this);
    const int   x      = (rect.Width() - cxIcon + 1) / 2;
    const int   y      = (rect.Height() - cyIcon + 1) / 2;

    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTestBlendDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestBlendDlg::OnOK() {
  EndDialog(IDOK);
}

void CTestBlendDlg::OnCancel() {
}

void CTestBlendDlg::OnClose() {
  OnOK();
}

void CTestBlendDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  switch (m_pickMode) {
  case PICK_IDLE       :
    break;
  case PICK_CENTER     :
    { CPoint p = point;
      ClientToScreen(&p);
      GetDlgItem(IDC_STATICKINGIMAGE)->ScreenToClient(&p);
      setRotationCenter(p);
      setPickMode(PICK_IDLE);
    }
    break;
  case PICK_DESTINATION:
    setRotationDestination(point);
    setPickMode(PICK_IDLE);
    break;
  }
  CDialog::OnLButtonDown(nFlags, point);
}

void CTestBlendDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar) {
  setRotationAngle(nPos);
  drawKingRotated();
  CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CTestBlendDlg::OnBnClickedButtonPickRotationDestination() {
  setPickMode(PICK_DESTINATION);
}

void CTestBlendDlg::OnBnClickedButtonPickRotationCenter() {
  setPickMode(PICK_CENTER);
}

void CTestBlendDlg::OnButtonDraw() {
  CBitmap girlImage;
  girlImage.LoadBitmap(IDB_BITMAPGIRL);
  PixRect imagePr(theApp.m_device, girlImage);
  CClientDC cdc(this);
  PixRect::bitBlt(cdc, 0, 0, imagePr.getWidth(), imagePr.getHeight(), SRCCOPY, &imagePr, 0, 0);
}

void CTestBlendDlg::OnButtonBlend() {
  try {
    if(!UpdateData()) {
      return;
    }
    CBitmap srcImage, dstImage;
    srcImage.LoadBitmap(IDB_BITMAPBLACKKING);
    dstImage.LoadBitmap(IDB_BITMAPBOARD);
    PixRect srcPr(theApp.m_device, srcImage);
    PixRect dstPr(theApp.m_device, dstImage);

    const CSize srcSize = srcPr.getSize();
    const CSize dstSize = dstPr.getSize();

    srcPr.apply(SetAlpha(m_alpha1));
    srcPr.apply(SubstituteColor(ARGB_SETALPHA(WHITE,m_alpha1), ARGB_SETALPHA(WHITE, m_alpha2)));

    PixRect::alphaBlend(dstPr, srcPr.getRect(), srcPr, srcPr.getRect(), m_srcConstAlpha);

    CClientDC screenDC(this);

    int dstWidth  = dstSize.cx * m_scale / 100;
    int dstHeight = dstSize.cy * m_scale / 100;

    SetStretchBltMode(screenDC, getStretchMode());
    PixRect::stretchBlt(screenDC,0,0,dstWidth,dstHeight,SRCCOPY,&dstPr,0,0,dstSize.cx, dstSize.cy);
  } catch(Exception e) {
    showException(e);
  }
}

int CTestBlendDlg::getStretchMode() {
  if(m_stretchMode == _T("BLACKONWHITE")) {
    return BLACKONWHITE;
  }
  if(m_stretchMode == _T("COLORONCOLOR")) {
    return COLORONCOLOR;
  }
  if(m_stretchMode == _T("HALFTONE")) {
    return HALFTONE;
  }
  if(m_stretchMode == _T("WHITEONBLACK")) {
    return WHITEONBLACK;
  }
  showWarning(_T("Unknown stretchmode:%s"), (LPCTSTR)m_stretchMode);
  return BLACKONWHITE;
}

void CTestBlendDlg::setRotationAngle(int a) {
  m_rotationAngle = a;
  setWindowText(this, IDC_STATICROTATIONANGLE, format(_T("%d"), a));
}

void CTestBlendDlg::setRotationDestination(const CPoint &p) {
  m_rotationDesination = p;
  setWindowText(this, IDC_STATICROTATIONDESTINATION, format(_T("%d,%d"), p.x,p.y));
}

void CTestBlendDlg::setRotationCenter(const CPoint &p) {
  m_rotationCenter = p;
  setWindowText(this, IDC_STATICROTATIONCENTER, format(_T("%d,%d"), p.x,p.y));
}

void CTestBlendDlg::setPickMode(PickMode mode) {
  m_pickMode = mode;
  switch(mode) {
  case PICK_IDLE      :
    setCursors(OCR_NORMAL,OCR_NORMAL);
    ClipCursor(NULL);
    break;
  case PICK_CENTER    :
    { CRect r = getWindowRect(this, IDC_STATICKINGIMAGE);
      ClientToScreen(&r);
      CPoint c = r.CenterPoint();
      setCursors(OCR_NORMAL,OCR_CROSS);
      SetCursorPos(c.x,c.y);
      r.left++; r.top++;
      ClipCursor(&r);
    }
    break;
  case PICK_DESTINATION:
    { CRect r = getClientRect(this);
      ClientToScreen(&r);
      ClipCursor(&r);
      setCursors(OCR_CROSS,OCR_NORMAL);
    }
    break;
  }
}

void CTestBlendDlg::setCursors(int winCursor, int imageCursor) {
  setWindowCursor(this, MAKEINTRESOURCE(winCursor));
  setWindowCursor(GetDlgItem(IDC_STATICKINGIMAGE), MAKEINTRESOURCE(imageCursor));
}

void CTestBlendDlg::drawKingRotated() {
  try {
    CBitmap srcImage, dstImage;
    srcImage.LoadBitmap(IDB_BITMAPBLACKKING);
    dstImage.LoadBitmap(IDB_BITMAPBOARD);
    PixRect srcPr(theApp.m_device, srcImage, D3DPOOL_FORCE_DWORD, D3DFMT_A8R8G8B8);
    PixRect dstPr(theApp.m_device, dstImage);

    const CSize srcSize = srcPr.getSize();
    const CSize dstSize = dstPr.getSize();

    srcPr.apply(SetAlpha(m_alpha1));
    srcPr.apply(SubstituteColor(ARGB_SETALPHA(WHITE,m_alpha1), ARGB_SETALPHA(WHITE, m_alpha2)));
    dstPr.drawRotated(&srcPr, m_rotationDesination, m_rotationAngle, m_rotationCenter);

    CClientDC screenDC(this);
    int dstWidth  = dstSize.cx * m_scale / 100;
    int dstHeight = dstSize.cy * m_scale / 100;

    SetStretchBltMode(screenDC, getStretchMode());
    PixRect::stretchBlt(screenDC,0,0,dstWidth,dstHeight,SRCCOPY,&dstPr,0,0,dstSize.cx, dstSize.cy);
  } catch(Exception e) {
    showException(e);
  }
}
