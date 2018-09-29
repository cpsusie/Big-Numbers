#include "stdafx.h"
#include "EditColorMapDlg.h"
#include "EditColorMapDlgThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEditColorMapDlg::CEditColorMapDlg(CEditColorMapDlgThread *thread)
: CDialog(IDD, NULL)
, m_thread(*thread)
, m_selectedRectIndex(-1)
, m_state(IDLE)
{
}

void CEditColorMapDlg::DoDataExchange(CDataExchange *pDX) {
  __super::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_COLORMAPCTRL, m_colormap  );
}

BEGIN_MESSAGE_MAP(CEditColorMapDlg, CDialog)
    ON_WM_PAINT()
    ON_WM_CONTEXTMENU()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_COMMAND(ID_DELETE_INTERPOLATIONPOINT  , OnDeleteInterpolationColor)
    ON_BN_CLICKED(ID_BUTTON_SETTODEFAULT     , OnBnClickedSetToDefault   )
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CEditColorMapDlg, CDialog)
  ON_EVENT(CEditColorMapDlg, IDC_COLORMAPCTRL, 1, OnColorchangedColormapctrl, VTS_NONE)
END_EVENTSINK_MAP()

BOOL CEditColorMapDlg::OnInitDialog() {
  __super::OnInitDialog();
  setSelectedInterpolationPoint(-1);
  reposition();
  return TRUE;
}

void CEditColorMapDlg::OnPaint() {
  __super::OnPaint();
  CWnd                  *win     = getColorMapWindow();
  const    CSize         winSize = getColorMapWinSize();
  const    UINT          mapSize = winSize.cx;
  const    ColorMapData &cmd     = getColorMapData();
  const    ColorMap      cm(cmd, mapSize);
  CPaintDC               dc(win);

  m_pixelWidth = 1.0f/mapSize;
  for(UINT index = 0; index < mapSize; index++) {
    const COLORREF cr = cm[index].getColorRef();
    dc.FillSolidRect(index,0,1,winSize.cy, cr);
  }
  m_interpolationRect.clear(cmd.size());
  for(size_t i = 0; i < cmd.size(); i++) {
    const float x = cmd[i].getPos();
    int midtx = (int)(x * winSize.cx);
#define IR_SIZE 10
    int left, right;
    if(i > 0) {
      right = min(midtx+IR_SIZE/2,winSize.cx-1);
      left  = right - IR_SIZE;
    } else {
      left  = 1;
      right = left + IR_SIZE;
    }
    CRect rect(left,1,right,IR_SIZE+1);
    m_interpolationRect.add(rect);
  }
  for (size_t i = 0; i < m_interpolationRect.size(); i++) {
    const CRect r = m_interpolationRect[i];
    dc.FillSolidRect(&r, WHITE);
    CPen pen;
    pen.CreatePen(BS_SOLID,1,BLACK);
    dc.SelectObject(&pen);
    dc.Rectangle(r.left-1,r.top-1,r.right+1,r.bottom+1);
  }
}

void CEditColorMapDlg::reposition() {
  putWindowBesideWindow(this, theApp.GetMainWnd());
}

const ColorMapData &CEditColorMapDlg::getColorMapData() const {
  return m_thread.getColorMapData();
}

void CEditColorMapDlg::setColorMapData(const ColorMapData &cd) {
  m_thread.setColorMapData(cd);
}

int CEditColorMapDlg::findInterpolationRect(const CPoint &p) {
  CPoint tmp = p;
  ClientToScreen(&tmp);
  getColorMapWindow()->ScreenToClient(&tmp);
  for(size_t i = 0; i < m_interpolationRect.size(); i++) {
    const CRect &r = m_interpolationRect[i];
    if(r.PtInRect(tmp)) {
      m_pointInRect = tmp - r.CenterPoint();
      return (int)i;
    }
  }
  return -1;
}

void CEditColorMapDlg::setSelectedInterpolationPoint(int index) {
  m_selectedRectIndex = index;
  GetDlgItem(IDC_COLORMAPCTRL)->EnableWindow(index >= 0);
  if(index >= 0) {
    const ColorMapData &cmd = getColorMapData();
    m_colormap.SetColor(cmd[index].getColorRef());
    UpdateData(FALSE);
  } else {
    setState(IDLE);
  }
}

void CEditColorMapDlg::setState(EditState state) {
  if(state != m_state) {
    switch (state) {
    case IDLE       :
      if(m_state == MOVING_RECT) {
        ClipCursor(NULL);
      }
      break;
    case MOVING_RECT:
      { CRect winRect = getRelativeClientRect(this,IDC_STATIC_COLORMAPWINDOW);
        ClientToScreen(&winRect);
        const CRect clipRect(winRect.left, winRect.top, winRect.right, winRect.top + IR_SIZE);
        ClipCursor(&clipRect);
      }
      break;
    }
    m_state = state;
  }
}

void CEditColorMapDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);
  CRect winRect = getRelativeClientRect(this,IDC_STATIC_COLORMAPWINDOW);
  if(winRect.PtInRect(point)) {
    setSelectedInterpolationPoint(findInterpolationRect(point));
    if(m_selectedRectIndex >= 0) {
      if((m_selectedRectIndex > 0) && (m_selectedRectIndex < m_interpolationRect.size()-1)) {
        setState(MOVING_RECT);
      }
    } else {
      const int x = point.x - winRect.left;
      CClientDC dc(getColorMapWindow());
      ColorMapData cmd(getColorMapData());
      const int index = cmd.insertPoint(ColorMapInterpolationPoint(dc.GetPixel(x,4), (float)x/winRect.Width()));
      if(index >= 0) {
        setColorMapData(cmd);
      }
      setSelectedInterpolationPoint(index);
      setState(IDLE);
      Invalidate(FALSE);
    }
  }
}

void CEditColorMapDlg::OnLButtonUp(UINT nFlags, CPoint point) {
  __super::OnLButtonUp(nFlags, point);
  setState(IDLE);
}

void CEditColorMapDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  __super::OnRButtonDown(nFlags, point);
}

void CEditColorMapDlg::OnRButtonUp(UINT nFlags, CPoint point) {
  __super::OnRButtonUp(nFlags, point);
}

void CEditColorMapDlg::OnMouseMove(UINT nFlags, CPoint point) {
  __super::OnMouseMove(nFlags, point);
  if(getState() == MOVING_RECT) {
    if((nFlags & MK_LBUTTON) == 0) {
      setState(IDLE);
      return;
    }
    const CRect winRect = getColorMapWinRect();
    CPoint tmp = point;
    ClientToScreen(&tmp);
    getColorMapWindow()->ScreenToClient(&tmp);
    tmp -= m_pointInRect;
    ColorMapData cmd = getColorMapData();
    float        pos = (float)(tmp.x - winRect.left) / winRect.Width();
    pos = minMax(pos, cmd[m_selectedRectIndex-1].getPos()+m_pixelWidth,cmd[m_selectedRectIndex+1].getPos()-m_pixelWidth);
    cmd[m_selectedRectIndex].setPos(pos);
    setColorMapData(cmd);
    Invalidate(FALSE);
  }
}

void CEditColorMapDlg::OnContextMenu(CWnd *pWnd, CPoint point) {
  if(getColorMapData().size() <= 2) {
    return;
  }
  CPoint tmp = point;
  ScreenToClient(&tmp);
  setSelectedInterpolationPoint(findInterpolationRect(tmp));
  if(m_selectedRectIndex >= 0) {
    CMenu menu;
    if(!menu.LoadMenu(IDR_EDITCOLORMAP_CONTEXTMENU)) {
      showWarning(_T("LoadMenu failed"));
      return;
    }
    menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, this);
  }
}

void CEditColorMapDlg::OnDeleteInterpolationColor() {
  ColorMapData cmd = getColorMapData();
  cmd.deletePoint(m_selectedRectIndex);
  setColorMapData(cmd);
  setSelectedInterpolationPoint(-1);
  Invalidate(FALSE);
}

void CEditColorMapDlg::OnColorchangedColormapctrl() {
  if(m_selectedRectIndex >= 0) {
    ColorMapData cmd = getColorMapData();
    cmd[m_selectedRectIndex].setColorRef(m_colormap.GetColor());
    setColorMapData(cmd);
    Invalidate(FALSE);
  }
}

void CEditColorMapDlg::OnBnClickedSetToDefault() {
  try {
    getColorMapData().setAsDefault();
    showInformation(_T("Colormap has been saved as default"));
  } catch (Exception e) {
    showException(e);
  }
}

