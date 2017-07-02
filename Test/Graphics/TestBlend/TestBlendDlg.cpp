#include "stdafx.h"
#include "TestBlendDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CTestBlendDlg::CTestBlendDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestBlendDlg::IDD, pParent) {
    m_srcConstAlpha = 255;
    m_alpha1        = 255;
    m_alpha2        = 0;
    m_scale         = 100;
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
    ON_BN_CLICKED(IDC_BUTTONDRAW, OnButtondraw)
    ON_BN_CLICKED(IDC_BUTTONBLEND, OnButtonblend)
END_MESSAGE_MAP()

BOOL CTestBlendDlg::OnInitDialog() {
    __super::OnInitDialog();

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    theApp.m_device.attach(m_hWnd);

    return TRUE;
}

void CTestBlendDlg::OnPaint() {
  if (IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  } else {
    __super::OnPaint();
  }
}

HCURSOR CTestBlendDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestBlendDlg::OnButtondraw() {
  CBitmap girlImage;
  girlImage.LoadBitmap(IDB_BITMAPGIRL);
  PixRect imagePr(theApp.m_device, girlImage);
  CClientDC cdc(this);
  PixRect::bitBlt(cdc, 0, 0, imagePr.getWidth(), imagePr.getHeight(), SRCCOPY, &imagePr, 0, 0);
}

void CTestBlendDlg::OnButtonblend() {
  try {
    if(!UpdateData()) {
      return;
    }


    CBitmap srcImage;
    srcImage.LoadBitmap(IDB_BITMAPBLACKKING);
    CBitmap dstImage;
    dstImage.LoadBitmap(IDB_BITMAPBOARD);
    PixRect srcPr(theApp.m_device, srcImage);
    PixRect dstPr(theApp.m_device, dstImage);
//    PixRect srcPr(srcImage), dstPr(dstImage);

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
    MessageBox(e.what(), _T("Exception"));
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
  MessageBox(format(_T("Unknown stretchmode:%s"), (LPCTSTR)m_stretchMode).cstr(),_T("Error"));
  return BLACKONWHITE;
}
