// ColormapCtrl.cpp : Implementation of the CColormapCtrl ActiveX Control class.

#include "stdafx.h"
#include <msstkppg.h>
#include <MyUtil.h>
#include "Colormap.h"
#include "ColormapCtrl.h"
#include "ColormapPropPage.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CColormapCtrl, COleControl)

// Message map

BEGIN_MESSAGE_MAP(CColormapCtrl, COleControl)
  ON_WM_CREATE()
  ON_WM_DESTROY()
  ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEMOVE()
  ON_WM_LBUTTONUP()
  ON_WM_SETFOCUS()
  ON_WM_SIZE()
  ON_WM_ENABLE()
  ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
  ON_EN_CHANGE(  IDC_EDITRED        ,OnChangeRed         )
  ON_EN_CHANGE(  IDC_EDITGREEN      ,OnChangeGreen       )
  ON_EN_CHANGE(  IDC_EDITBLUE       ,OnChangeBlue        )
  ON_EN_CHANGE(  IDC_EDITHUE        ,OnChangeHue         )
  ON_EN_CHANGE(  IDC_EDITSATURATION ,OnChangeSaturation  )
  ON_EN_CHANGE(  IDC_EDITLUMINATION ,OnChangeLumination  )
  ON_EN_SETFOCUS(IDC_EDITRED        ,OnSetFocusRed       )
  ON_EN_SETFOCUS(IDC_EDITGREEN      ,OnSetFocusGreen     )
  ON_EN_SETFOCUS(IDC_EDITBLUE       ,OnSetFocusBlue      )
  ON_EN_SETFOCUS(IDC_EDITHUE        ,OnSetFocusHue       )
  ON_EN_SETFOCUS(IDC_EDITSATURATION ,OnSetFocusSaturation)
  ON_EN_SETFOCUS(IDC_EDITLUMINATION ,OnSetFocusLumination)
END_MESSAGE_MAP()

// Dispatch map

BEGIN_DISPATCH_MAP(CColormapCtrl, COleControl)
  DISP_PROPERTY_EX(CColormapCtrl, "Color"        , GetColor        , SetColor        , VT_I4  )
  DISP_PROPERTY_EX(CColormapCtrl, "Sunken"       , GetSunken       , SetSunken       , VT_BOOL)
  DISP_PROPERTY_EX(CColormapCtrl, "HasBorder"    , GetHasBorder    , SetHasBorder    , VT_BOOL)
  DISP_PROPERTY_EX(CColormapCtrl, "Caption"      , GetCaption      , SetCaption      , VT_BSTR)
  DISP_PROPERTY_EX(CColormapCtrl, "ClientEdge"   , GetClientEdge   , SetClientEdge   , VT_BOOL)
  DISP_PROPERTY_EX(CColormapCtrl, "StaticEdge"   , GetStaticEdge   , SetStaticEdge   , VT_BOOL)
  DISP_PROPERTY_EX(CColormapCtrl, "ModalFrame"   , GetModalFrame   , SetModalFrame   , VT_BOOL)
  DISP_PROPERTY_EX(CColormapCtrl, "TextAlignment", GetTextAlignment, SetTextAlignment, VT_I4  )
  DISP_STOCKPROP_FONT()
  DISP_STOCKPROP_ENABLED()
  DISP_FUNCTION_ID(CColormapCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()

// Event map

BEGIN_EVENT_MAP(CColormapCtrl, COleControl)
  EVENT_CUSTOM("ColorChanged", FireColorChanged, VTS_NONE)
END_EVENT_MAP()

// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CColormapCtrl, 2)
    PROPPAGEID(CColormapPropPage::guid)
  PROPPAGEID(CLSID_StockFontPage)
END_PROPPAGEIDS(CColormapCtrl)

// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CColormapCtrl, "COLORMAP.ColormapCtrl.1",
    0xb4097dda, 0x446a, 0x4ab9, 0xb2, 0x10, 0x3a, 0xf1, 0x92, 0x76, 0xdf, 0x6c)

// Type library ID and version

IMPLEMENT_OLETYPELIB(CColormapCtrl, _tlid, _wVerMajor, _wVerMinor)

// Interface IDs

const IID IID_DColormap = { 0x5C84D2A7, 0x5B07, 0x4B1E, { 0x93, 0x4B, 0xB1, 0xA3, 0x8, 0x9A, 0x79, 0xF1 } };
const IID IID_DColormapEvents = { 0xCE00D37, 0x27E5, 0x4C2E, { 0xA6, 0x41, 0xA5, 0x78, 0xBE, 0x23, 0x44, 0xBF } };

// Control type information

static const DWORD _dwColormapOleMisc =
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST |
    OLEMISC_INSIDEOUT |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CColormapCtrl, IDS_COLORMAP, _dwColormapOleMisc)

// CColormapCtrl::CColormapCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CColormapCtrl

BOOL CColormapCtrl::CColormapCtrlFactory::UpdateRegistry(BOOL bRegister)
{
    // TODO: Verify that your control follows apartment-model threading rules.
    // Refer to MFC TechNote 64 for more information.
    // If your control does not conform to the apartment-model rules, then
    // you must modify the code below, changing the 6th parameter from
    // afxRegApartmentThreading to 0.

    if (bRegister)
        return AfxOleRegisterControlClass(
            AfxGetInstanceHandle(),
            m_clsid,
            m_lpszProgID,
            IDS_COLORMAP,
            IDB_COLORMAP,
            afxRegApartmentThreading,
            _dwColormapOleMisc,
            _tlid,
            _wVerMajor,
            _wVerMinor);
    else
        return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


// CColormapCtrl::CColormapCtrl - Constructor

CColormapCtrl::CColormapCtrl()
{
    InitializeIIDs(&IID_DColormap, &IID_DColormapEvents);
  m_currentField  = IDC_EDITRED;
  m_updateActive  = false;
  m_firstDrawDone = false;
  m_font.CreateFont(10, 8, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
    CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
    DEFAULT_PITCH | FF_MODERN,
    _T("Courier")
  );
  m_colorMap.setControl(this);
}

// CColormapCtrl::~CColormapCtrl - Destructor

CColormapCtrl::~CColormapCtrl()
{
  m_font.DeleteObject();
}

// CColormapCtrl::OnDraw - Drawing function

void CColormapCtrl::OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& /* rcInvalid */) {
  if (!pdc) {
    return;
  }

  m_firstDrawDone = true;
  pdc->SelectObject(&m_font);
  const BOOL enabled = GetEnabled();
  m_colorMap.draw(*pdc, rcBounds, enabled);

  if(m_hWnd != NULL) {
    setFieldPositions(enabled);
  } else {
    drawEditFields(pdc, enabled);
  }

  DoSuperclassPaint(pdc,rcBounds);
}

  void CColormapCtrl::setFieldPositions(BOOL enabled) {
    setFieldPosition(m_redEdit       , m_colorMap.getRedRect()       , enabled);
    setFieldPosition(m_greenEdit     , m_colorMap.getGreenRect()     , enabled);
    setFieldPosition(m_blueEdit      , m_colorMap.getBlueRect()      , enabled);
    setFieldPosition(m_hueEdit       , m_colorMap.getHueRect()       , enabled);
    setFieldPosition(m_saturationEdit, m_colorMap.getSaturationRect(), enabled);
    setFieldPosition(m_luminationEdit, m_colorMap.getLuminationRect(), enabled);
  }

  void CColormapCtrl::setFieldPosition(CEdit &f, const CRect &r, BOOL enabled) {
    WINDOWPLACEMENT wp;
    f.GetWindowPlacement(&wp);
    wp.rcNormalPosition = r;
    f.SetWindowPlacement(&wp);
    f.SetFont(&m_font);
    f.EnableWindow(enabled);
  }

  void CColormapCtrl::drawEditFields(CDC *pdc, BOOL enabled) {
    drawEditField(pdc, m_colorMap.getRedRect()       , enabled);
    drawEditField(pdc, m_colorMap.getGreenRect()     , enabled);
    drawEditField(pdc, m_colorMap.getBlueRect()      , enabled);
    drawEditField(pdc, m_colorMap.getHueRect()       , enabled);
    drawEditField(pdc, m_colorMap.getSaturationRect(), enabled);
    drawEditField(pdc, m_colorMap.getLuminationRect(), enabled);
  }

  void CColormapCtrl::drawEditField(CDC *pdc, const CRect &r, BOOL enabled) {
    pdc->DrawEdge(&(CRect&)r,EDGE_SUNKEN,BF_RECT);
    const COLORREF fieldColor = enabled ? RGB(255,255,255) : m_colorMap.getBackgroundColor();
    pdc->FillSolidRect(r.left+2, r.top+2, r.Width()-4, r.Height()-4, fieldColor);
  }

  void CColormapCtrl::drawMap() {
    if(m_hWnd == NULL) {
      return;
    }
    CClientDC dc(this);
    dc.SelectObject(&m_font);
    m_colorMap.draw(dc, GetEnabled());
  }

  // CColormapCtrl::DoPropExchange - Persistence support

void CColormapCtrl::DoPropExchange(CPropExchange* pPX) {
    ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
    COleControl::DoPropExchange(pPX);

  BOOL          isSunken      = m_colorMap.getIsSunken();
  BOOL          hasBorder     = m_colorMap.getHasBorder();
  CString       caption       = m_colorMap.getCaption();
  BOOL          hasClientEdge = m_colorMap.getHasClientEdge();
  BOOL          hasStaticEdge = m_colorMap.getHasStaticEdge();
  BOOL          hasModalFrame = m_colorMap.getHasModalFrame();
  long          textAlignment = textAlignmentToLong(m_colorMap.getTextAlignment());

  PX_Bool(   pPX, _T("Sunken"       ) , isSunken      );
  PX_Bool(   pPX, _T("HasBorder"    ) , hasBorder     );
  PX_String( pPX, _T("Caption"      ) , caption       );
  PX_Bool(   pPX, _T("ClientEdge"   ) , hasClientEdge );
  PX_Bool(   pPX, _T("StaticEdge"   ) , hasStaticEdge );
  PX_Bool(   pPX, _T("ModalFrame"   ) , hasModalFrame );
  PX_Long(   pPX, _T("TextAlignment") , textAlignment );

  m_colorMap.setIsSunken( isSunken ?true:false);
  m_colorMap.setHasBorder(hasBorder?true:false);
  m_colorMap.setCaption(caption);
  m_colorMap.setHasClientEdge(hasClientEdge?true:false);
  m_colorMap.setHasStaticEdge(hasStaticEdge?true:false);
  m_colorMap.setHasModalFrame(hasModalFrame?true:false);
  m_colorMap.setTextAlignment(longToTextAlignment(textAlignment));
}


// CColormapCtrl::OnResetState - Reset control to default state

void CColormapCtrl::OnResetState()
{
    COleControl::OnResetState();  // Resets defaults found in DoPropExchange

}


// CColormapCtrl::AboutBox - Display an "About" box to the user

void CColormapCtrl::AboutBox()
{
    CDialogEx dlgAbout(IDD_ABOUTBOX_COLORMAP);
    dlgAbout.DoModal();
}


BOOL CColormapCtrl::GetSunken() {
  return m_colorMap.getIsSunken();
}

void CColormapCtrl::SetSunken(BOOL bNewValue) {
  m_colorMap.setIsSunken(bNewValue?true:false);
  setModifiedAndInvalidate();
}

BOOL CColormapCtrl::GetHasBorder() {
  return m_colorMap.getHasBorder();
}

void CColormapCtrl::SetHasBorder(BOOL bNewValue) {
  m_colorMap.setHasBorder(bNewValue?true:false);
  setModifiedAndInvalidate();
}

BSTR CColormapCtrl::GetCaption() {
  CString strResult = m_colorMap.getCaption();
  return strResult.AllocSysString();
}

void CColormapCtrl::SetCaption(LPCTSTR lpszNewValue) {
  m_colorMap.setCaption(lpszNewValue);
  setModifiedAndInvalidate();
}

BOOL CColormapCtrl::GetClientEdge() {
  return m_colorMap.getHasClientEdge();
}

void CColormapCtrl::SetClientEdge(BOOL bNewValue) {
  m_colorMap.setHasClientEdge(bNewValue?true:false);
  setModifiedAndInvalidate();
}

BOOL CColormapCtrl::GetStaticEdge() {
  return m_colorMap.getHasStaticEdge();
}

void CColormapCtrl::SetStaticEdge(BOOL bNewValue) {
  m_colorMap.setHasStaticEdge(bNewValue?true:false);
  setModifiedAndInvalidate();
}

BOOL CColormapCtrl::GetModalFrame() {
  return m_colorMap.getHasModalFrame();
}

void CColormapCtrl::SetModalFrame(BOOL bNewValue) {
  m_colorMap.setHasModalFrame(bNewValue?true:false);
  setModifiedAndInvalidate();
}

long CColormapCtrl::GetTextAlignment() {
  return textAlignmentToLong(m_colorMap.getTextAlignment());
}

void CColormapCtrl::SetTextAlignment(long nNewValue) {
  m_colorMap.setTextAlignment(longToTextAlignment(nNewValue));
  setModifiedAndInvalidate();
}

void CColormapCtrl::setModifiedAndInvalidate() {
  SetModifiedFlag();
  if(m_firstDrawDone) {
    InvalidateControl();
  }
}

long CColormapCtrl::GetColor() {
  return m_colorMap.getCurrentColor();
}

void CColormapCtrl::SetColor(long nNewValue) {
  m_colorMap.setCurrentColor(nNewValue);
  if(m_firstDrawDone) {
    drawMap();
  }
  updateFields();
  SetModifiedFlag();
}

CRect CColormapCtrl::getMapRect() {
  CRect wr;
  GetWindowRect(&wr);
  return CRect(m_colorMap.getMapRect() + wr.TopLeft());
}

CRect CColormapCtrl::getScalePosRect() {
  CRect wr;
  GetWindowRect(&wr);
  return CRect(m_colorMap.getScalePosRect() + wr.TopLeft());
}

void CColormapCtrl::createEditField(CEdit &f, const CRect &r, int id) {
  f.CreateEx(WS_EX_CLIENTEDGE|WS_EX_WINDOWEDGE,_T("EDIT"),NULL,WS_CHILD | WS_VISIBLE | WS_GROUP | WS_BORDER | WS_TABSTOP,r-m_colorMap.getRect().TopLeft(),this,id);
}

int CColormapCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) {
  if(COleControl::OnCreate(lpCreateStruct) == -1) {
    return -1;
  }

  CRect rect(lpCreateStruct->x,lpCreateStruct->y,lpCreateStruct->x+lpCreateStruct->cx,lpCreateStruct->y+lpCreateStruct->cy);
  m_colorMap.init(rect);

  createEditField(m_redEdit       ,m_colorMap.getRedRect()        ,IDC_EDITRED       );
  createEditField(m_greenEdit     ,m_colorMap.getGreenRect()      ,IDC_EDITGREEN     );
  createEditField(m_blueEdit      ,m_colorMap.getBlueRect()       ,IDC_EDITBLUE      );

  createEditField(m_hueEdit       ,m_colorMap.getHueRect()        ,IDC_EDITHUE       );
  createEditField(m_saturationEdit,m_colorMap.getSaturationRect() ,IDC_EDITSATURATION);
  createEditField(m_luminationEdit,m_colorMap.getLuminationRect() ,IDC_EDITLUMINATION);

  initFields(UPDATE_ALL);
  return 0;
}

void CColormapCtrl::OnDestroy() {
  m_redEdit.DestroyWindow();
  m_greenEdit.DestroyWindow();
  m_blueEdit.DestroyWindow();
  m_hueEdit.DestroyWindow();
  m_saturationEdit.DestroyWindow();
  m_luminationEdit.DestroyWindow();

  COleControl::OnDestroy();
}

void CColormapCtrl::gotoField(int field) {
  m_currentField = field;
  switch(field) {
  case IDC_COLORMAP    :
  case IDC_COLORSCALE  :
    SetFocus();
    break;
  case IDC_EDITRED        :
  case IDC_EDITGREEN      :
  case IDC_EDITBLUE       :
  case IDC_EDITHUE        :
  case IDC_EDITSATURATION :
  case IDC_EDITLUMINATION :
    gotoEditBox(this, field);
    break;
  default:
    { CWnd *wnd = GetDlgItem(field);
      if(wnd) wnd->SetFocus();
    }
    break;
  }
}

void CColormapCtrl::OnLButtonDown(UINT nFlags, CPoint point) {
  COleControl::OnLButtonDown(nFlags, point);
  if(!GetEnabled()) {
    return;
  }
  if(m_colorMap.getMapRect().PtInRect(point)) {
    m_colorMap.setCurrentMapPoint(point);
    drawMap();
    updateFields();
    gotoField(IDC_COLORMAP);
    ClipCursor(getMapRect());
  } else if(m_colorMap.getScalePosRect().PtInRect(point) || m_colorMap.getScaleRect().PtInRect(point)) {
    m_colorMap.setCurrentScalePoint(point);
    drawMap();
    updateFields();
    gotoField(IDC_COLORSCALE);
    ClipCursor(getScalePosRect());
  } else if(m_colorMap.getRedRect().PtInRect(point)) {
    gotoField(IDC_EDITRED);
  } else if(m_colorMap.getGreenRect().PtInRect(point)) {
    gotoField(IDC_EDITGREEN);
  } else if(m_colorMap.getBlueRect().PtInRect(point)) {
    gotoField(IDC_EDITBLUE);
  } else if(m_colorMap.getHueRect().PtInRect(point)) {
    gotoField(IDC_EDITHUE);
  } else if(m_colorMap.getSaturationRect().PtInRect(point)) {
    gotoField(IDC_EDITSATURATION);
  } else if(m_colorMap.getLuminationRect().PtInRect(point)) {
    gotoField(IDC_EDITLUMINATION);
  } else {
    m_currentField = -1;
  }
}

void CColormapCtrl::OnMouseMove(UINT nFlags, CPoint point) {
  COleControl::OnMouseMove(nFlags, point);
  if(!GetEnabled()) {
    return;
  }
  if(nFlags & MK_LBUTTON) {
    switch(m_currentField) {
    case IDC_COLORMAP:
    { m_colorMap.setCurrentMapPoint(point);
    drawMap();
    updateFields();
    break;
    }
    case IDC_COLORSCALE:
    { m_colorMap.setCurrentScalePoint(point);
    drawMap();
    updateFields();
    break;
    }
    }
  }
}

void CColormapCtrl::OnLButtonUp(UINT nFlags, CPoint point) {
  COleControl::OnLButtonUp(nFlags, point);
  if(!GetEnabled()) {
    return;
  }
  ClipCursor(NULL);
}

BOOL CColormapCtrl::PreTranslateMessage(MSG *pMsg) {
  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_TAB:
      switch(m_currentField) {
      case IDC_COLORMAP:
      case IDC_COLORSCALE:
        gotoField(IDC_EDITRED);
        return TRUE;
      case IDC_EDITRED:
        if(shiftKeyPressed()) {
          GetParent()->GetNextDlgTabItem(this,true)->SetFocus();
        } else {
          gotoField(IDC_EDITGREEN);
          return TRUE;
        }
        break;
      case IDC_EDITGREEN:
        if(shiftKeyPressed()) {
          gotoField(IDC_EDITRED);
        } else {
          gotoField(IDC_EDITBLUE);
        }
        return TRUE;
      case IDC_EDITBLUE:
        if(shiftKeyPressed()) {
          gotoField(IDC_EDITGREEN);
        } else {
          gotoField(IDC_EDITHUE);
        }
        return TRUE;
      case IDC_EDITHUE:
        if(shiftKeyPressed()) {
          gotoField(IDC_EDITBLUE);
        } else {
          gotoField(IDC_EDITSATURATION);
        }
        return TRUE;
      case IDC_EDITSATURATION:
        if(shiftKeyPressed()) {
          gotoField(IDC_EDITHUE);
        } else {
          gotoField(IDC_EDITLUMINATION);
        }
        return TRUE;
      case IDC_EDITLUMINATION:
        if(shiftKeyPressed()) {
          gotoField(IDC_EDITSATURATION);
          return TRUE;
        } else {
          GetParent()->GetNextDlgTabItem(this)->SetFocus();
        }
        break;
      }
      break;
    case VK_LEFT:
      switch(m_currentField) {
      case IDC_COLORMAP:
        adjustHue(-1);
        return TRUE;
      default:
        break;
      }
      break;
    case VK_RIGHT:
      switch(m_currentField) {
      case IDC_COLORMAP:
        adjustHue(1);
        return TRUE;
      default:
        break;
      }
      break;
    case VK_UP:
      switch(m_currentField) {
      case IDC_COLORMAP:
        adjustSaturation(1);
        return TRUE;
      case IDC_COLORSCALE:
        adjustLumination(1);
        return TRUE;
      default:
        break;
      }
      break;
    case VK_DOWN:
      switch(m_currentField) {
      case IDC_COLORMAP:
        adjustSaturation(-1);
        return TRUE;
      case IDC_COLORSCALE:
        adjustLumination(-1);
        return TRUE;
      default:
        break;
      }
      break;
    }
    break;
  }
  return COleControl::PreTranslateMessage(pMsg);
}

void CColormapCtrl::OnSetFocus(CWnd *pOldWnd) {
  gotoField(m_currentField);
}

void CColormapCtrl::fireIfChanged(COLORREF color) {
  if(color != m_oldColor) {
    FireColorChanged();
    m_oldColor = color;
  }
}

void CColormapCtrl::initFields(FieldUpdateSet set) {
  m_updateActive = true;
  COLORREF color = m_colorMap.getCurrentColor();
  if(set & UPDATE_RGB) {
    setFieldValue(m_redEdit  ,GetRValue(color));
    setFieldValue(m_greenEdit,GetGValue(color));
    setFieldValue(m_blueEdit ,GetBValue(color));
  }

  LSHColor lsh = m_colorMap.getCurrentLSHColor();
  if(set & UPDATE_LSH) {
    setFieldValue(m_hueEdit       ,(int)(lsh.m_hue*255)       );
    setFieldValue(m_saturationEdit,(int)(lsh.m_saturation*255));
    setFieldValue(m_luminationEdit,(int)(lsh.m_lumination*255));
  }
  m_updateActive = false;
}

void CColormapCtrl::updateFields(FieldUpdateSet set) {
  if(m_updateActive) {
    return;
  }
  initFields(set);
  fireIfChanged(m_colorMap.getCurrentColor());
}

void CColormapCtrl::updateFields() {
  updateFields(UPDATE_ALL);
}

void CColormapCtrl::setFieldValue(CEdit &f, int value) {
  TCHAR tmp[20];
  _itow_s(value, tmp, ARRAYSIZE(tmp), 10);
  f.SetWindowText(tmp);
}

int CColormapCtrl::getFieldValue(CEdit &f) {
  TCHAR str[10];
  f.GetWindowText(str,ARRAYSIZE(str));
  return _wtoi(str);
}

void CColormapCtrl::updateColor(COLORREF color) {
  if (m_updateActive) {
    return;
  }
  if(color != m_oldColor) {
    m_colorMap.setCurrentColor(color);
    updateFields(UPDATE_LSH);
    drawMap();
    fireIfChanged(color);
  }
}

void CColormapCtrl::updateLSHColor(const LSHColor &lsh) {
  if(m_updateActive) {
    return;
  }
  if(lsh != m_colorMap.getCurrentLSHColor()) {
    m_colorMap.setCurrentLSHColor(lsh);
    updateFields(UPDATE_RGB);
    drawMap();
    fireIfChanged(m_colorMap.getCurrentColor());
  }
}

void CColormapCtrl::OnChangeRed() {
  updateColor(RGB(getFieldValue(m_redEdit),GetGValue(m_oldColor),GetBValue(m_oldColor)));
}

void CColormapCtrl::OnChangeGreen() {
  updateColor(RGB(GetRValue(m_oldColor), getFieldValue(m_greenEdit),GetBValue(m_oldColor)));
}

void CColormapCtrl::OnChangeBlue() {
  updateColor(RGB(GetRValue(m_oldColor), GetGValue(m_oldColor), getFieldValue(m_blueEdit)));
}

void CColormapCtrl::OnChangeHue() {
  LSHColor lsh = m_colorMap.getCurrentLSHColor();
  updateLSHColor(LSHColor((float)getFieldValue(m_hueEdit)/255,lsh.m_saturation,lsh.m_lumination));
}

bool CColormapCtrl::adjustField(CEdit &f, int dv) {
  int v = getFieldValue(f);
  v += dv;
  if (v < 0 || v > 255) {
    return false;
  }
  setFieldValue(f,v);
  return true;
}

void CColormapCtrl::adjustHue(int dv) {
  if(adjustField(m_hueEdit,dv))
    OnChangeHue();
}

void CColormapCtrl::adjustSaturation(int dv) {
  if (adjustField(m_saturationEdit, dv)) {
    OnChangeSaturation();
  }
}

void CColormapCtrl::adjustLumination(int dv) {
  if (adjustField(m_luminationEdit, dv)) {
    OnChangeLumination();
  }
}

void CColormapCtrl::OnChangeSaturation() {
  LSHColor lsh = m_colorMap.getCurrentLSHColor();
  updateLSHColor(LSHColor(lsh.m_hue,(float)getFieldValue(m_saturationEdit)/255,lsh.m_lumination));
}

void CColormapCtrl::OnChangeLumination() {
  LSHColor lsh = m_colorMap.getCurrentLSHColor();
  updateLSHColor(LSHColor(lsh.m_hue,lsh.m_saturation,(float)getFieldValue(m_luminationEdit)/255));
}

void CColormapCtrl::OnSetFocusRed() {
  m_currentField = IDC_EDITRED;
}

void CColormapCtrl::OnSetFocusGreen() {
  m_currentField = IDC_EDITGREEN;
}

void CColormapCtrl::OnSetFocusBlue() {
  m_currentField = IDC_EDITBLUE;
}

void CColormapCtrl::OnSetFocusHue() {
  m_currentField = IDC_EDITHUE;
}

void CColormapCtrl::OnSetFocusSaturation() {
  m_currentField = IDC_EDITSATURATION;
}

void CColormapCtrl::OnSetFocusLumination() {
  m_currentField = IDC_EDITLUMINATION;
}
