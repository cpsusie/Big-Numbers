#include "stdafx.h"
#include <MFCUtil/Point2DP.h>
#include "TestEdgeDetectionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CAboutDlg : public CDialog {
public:
    CAboutDlg();

    enum { IDD = IDD_ABOUTBOX };

protected:
    virtual void DoDataExchange(CDataExchange *pDX);

    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD) {
}

void CAboutDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

CTestEdgeDetectionDlg::CTestEdgeDetectionDlg(CWnd *pParent /*=NULL*/) : CDialog(CTestEdgeDetectionDlg::IDD, pParent) {
    m_hIcon            = theApp.LoadIcon(IDR_MAINFRAME);
    m_dc               = NULL;
    m_arrowDirection   = NODIR;
    m_edgeMatrixCenter = CPoint(-1,-1);
    m_edgeThread       = NULL;
    m_currentDirBitmap = NULL;
    m_pixelWindow      = NULL;
    m_fillInfo         = NULL;
}

void CTestEdgeDetectionDlg::DoDataExchange(CDataExchange *pDX) {
    __super::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestEdgeDetectionDlg, CDialog)
    ON_WM_SYSCOMMAND()
    ON_WM_QUERYDRAGICON()
    ON_WM_PAINT()
    ON_WM_SIZE()
    ON_WM_SIZING()
    ON_WM_CLOSE()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_NCLBUTTONDOWN()
    ON_COMMAND(ID_FILE_EXIT                  , OnFileExit                )
    ON_COMMAND(ID_EDIT_RESET                 , OnEditReset               )
    ON_COMMAND(ID_EDIT_CIRCEL                , OnEditCircel              )
    ON_COMMAND(ID_EDIT_LINE                  , OnEditLine                )
    ON_COMMAND(ID_EDIT_N                     , OnEditN                   )
    ON_COMMAND(ID_EDIT_S                     , OnEditS                   )
    ON_COMMAND(ID_EDIT_E                     , OnEditE                   )
    ON_COMMAND(ID_EDIT_W                     , OnEditW                   )
    ON_COMMAND(ID_EDIT_NOSEARCH              , OnEditNoSearch            )
    ON_COMMAND(ID_EDIT_GO                    , OnEditGo                  )
    ON_COMMAND(ID_EDIT_TOGGLEBREAKPOINT      , OnEditToggleBreakpoint    )
    ON_COMMAND(ID_EDIT_SINGLESTEP            , OnEditSingleStep          )
    ON_COMMAND(ID_EDIT_DOFILL                , OnEditDoFill              )
    ON_COMMAND(ID_EDIT_SHOWFILLINFO          , OnEditShowFillInfo        )
    ON_COMMAND(ID_EDIT_TESTARRROW            , OnEditTestArrrow          )
    ON_COMMAND(ID_EDIT_TESTEDGEMATRIX        , OnEditTestEdgeMatrix      )
    ON_MESSAGE(ID_MSG_ENABLECONTINUETHREAD   , OnMsgEnableContinueThread )
END_MESSAGE_MAP()

BOOL CTestEdgeDetectionDlg::OnInitDialog() {
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

  m_accelTable = LoadAccelerators(theApp.m_hInstance, MAKEINTRESOURCE(IDR_MAINFRAME));

  m_layoutManager.OnInitDialog(this);
  m_layoutManager.addControl(IDC_PIXELAREA     , RELATIVE_SIZE  );
  m_layoutManager.addControl(IDC_STATICDIRTEXT , RELATIVE_Y_POS );
  m_layoutManager.addControl(IDC_STATICDIRIMAGE, RELATIVE_Y_POS );

  m_pixelWindow = GetDlgItem(IDC_PIXELAREA);

  m_breakPoint = isMenuItemChecked(this, ID_EDIT_TOGGLEBREAKPOINT);
  m_doFill     = isMenuItemChecked(this, ID_EDIT_DOFILL          );

  setWindowPosition(this, CPoint(0,0));
  setSearchDir(E);
  adjustPixelMatrix();
  return TRUE;
}

CDC &CTestEdgeDetectionDlg::getDC() {
  if(m_dc == NULL) {
    m_dc = new CClientDC(m_pixelWindow);
  }
  return *m_dc;
}

void CTestEdgeDetectionDlg::flush() {
  if(m_dc) {
    delete m_dc;
    m_dc = NULL;
  }
}

void CTestEdgeDetectionDlg::OnSysCommand(UINT nID, LPARAM lParam) {
  if((nID & 0xFFF0) == IDM_ABOUTBOX) {
    CAboutDlg().DoModal();
  } else {
    __super::OnSysCommand(nID, lParam);
  }
}

BOOL CTestEdgeDetectionDlg::PreTranslateMessage(MSG *pMsg) {
  if(TranslateAccelerator(m_hWnd, m_accelTable, pMsg)) {
    return true;
  }
  return __super::PreTranslateMessage(pMsg);
}

void CTestEdgeDetectionDlg::OnPaint() {
  if(IsIconic()) {
    CPaintDC dc(this);

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

void CTestEdgeDetectionDlg::OnSize(UINT nType, int cx, int cy) {
  if(isEdgeThreadActive()) return;
  __super::OnSize(nType, cx, cy);
  m_layoutManager.OnSize(nType, cx, cy);
  adjustPixelMatrix();
}

void CTestEdgeDetectionDlg::OnSizing(UINT fwSide, LPRECT pRect) {
  if(isEdgeThreadActive()) return;
  __super::OnSizing(fwSide, pRect);
}

void CTestEdgeDetectionDlg::adjustPixelMatrix() {
  if(m_pixelWindow == NULL) {
    return;
  }
  const CSize cs = getClientRect(m_pixelWindow).Size();
  m_logicalSize.cx = (cs.cx - 2*MARGIN) / PIXELSIZE;
  m_logicalSize.cy = (cs.cy - 2*MARGIN) / PIXELSIZE;
  m_pixelMatrix.setDimension(m_logicalSize.cx, m_logicalSize.cy);
}

HCURSOR CTestEdgeDetectionDlg::OnQueryDragIcon() {
  return (HCURSOR)m_hIcon;
}

void CTestEdgeDetectionDlg::showCount(int count) {
  SetWindowText(format(_T("MarkedPixels:%d"), count).cstr());
}

void CTestEdgeDetectionDlg::OnClose() {
  OnFileExit();
}

void CTestEdgeDetectionDlg::OnCancel() {
}

void CTestEdgeDetectionDlg::OnOK() {
}

void CTestEdgeDetectionDlg::OnFileExit() {
  EndDialog(IDOK);
}

#define PPOS(x) MARGIN+(x)*PIXELSIZE

CPoint CTestEdgeDetectionDlg::getPixelPointFromMousePoint(const CPoint &p) const {
  CPoint tmp = p;
  ClientToScreen(&tmp);
  m_pixelWindow->ScreenToClient(&tmp);
  return CPoint((tmp.x - MARGIN)/PIXELSIZE, (tmp.y - MARGIN)/PIXELSIZE);
}

void CTestEdgeDetectionDlg::setPixel(unsigned int x, unsigned int y, COLORREF c) {
  m_pixelMatrix(x, y).m_color = c;
  paintPixel(x, y);
}

void CTestEdgeDetectionDlg::paintPixelMatrix() {
  const CSize &sz = getWindowSize();
  for(int y = 0; y < sz.cy; y++) {
    for(int x = 0; x < sz.cx; x++) {
      paintPixel(x,y);
    }
  }
}

void CTestEdgeDetectionDlg::paintPixel(unsigned int x, unsigned int y) {
  CDC &dc = getDC();
  PixelInfo &pi = m_pixelMatrix(x, y);
  dc.FillSolidRect(PPOS(x), PPOS(y), PIXELSIZE-1, PIXELSIZE-1, pi.m_color);
  if(pi.m_marked) {
    paintPixelMark(x,y);
  }
}

void CTestEdgeDetectionDlg::paintPixelMark(unsigned int x, unsigned int y) {
  const PixelInfo &pi = m_pixelMatrix(x,y);
  const COLORREF c = pi.m_marked ? pi.m_markColor : pi.m_color;
  CDC &dc = getDC();
  dc.FillSolidRect(PPOS(x+1)-MARKSIZE-1, PPOS(y+1)-MARKSIZE-1, MARKSIZE, MARKSIZE, c);
}

void CTestEdgeDetectionDlg::unmarkAllPixels() {
  const CSize &sz = getWindowSize();
  for(int x = 0; x < sz.cx; x++) {
    for(int y = 0; y < sz.cy; y++) {
      unmarkPixel(x,y);
    }
  }
}

void CTestEdgeDetectionDlg::resetMarkMatrix() {
  const CSize &sz = getWindowSize();
  for(int x = 0; x < sz.cx; x++) {
    for(int y = 0; y < sz.cy; y++) {
      m_pixelMatrix(x,y).m_marked = false;
    }
  }
}

String toString(const PixelInfo &pi) {
  return format(_T("color:%06X; Marked:%s"), pi.m_color, pi.m_marked?"true":"false");
}

void CTestEdgeDetectionDlg::setBackground(COLORREF color) {

  CClientDC dc(GetDlgItem(IDC_PIXELAREA));
  CRect r = getClientRect(GetDlgItem(IDC_PIXELAREA));
  dc.FillSolidRect(&r, WHITE);
  const CSize &sz = getWindowSize();
  for(int y = 0; y < sz.cy; y++) {
    for(int x = 0; x < sz.cx; x++) {
      setPixel(x, y, color);
    }
  }
}

void CTestEdgeDetectionDlg::setMark(UINT x, UINT y, bool marked, COLORREF color) {
  const CSize &sz = getWindowSize();
  if(x >= (UINT)sz.cx || y >= (UINT)sz.cy) {
    return;
  }
  PixelInfo &pi = m_pixelMatrix(x,y);
  if((marked != pi.m_marked) || (marked && (color != pi.m_markColor))) {
    pi.m_marked    = marked;
    pi.m_markColor = color;
    paintPixelMark(x, y);
  }
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::paintArrow(const CPoint &p, Direction dir, COLORREF color) {
  removeArrow();
  Point2DP v = EdgeMatrix::dirStep[dir];
  v /= v.length();
  v *= 2*PIXELSIZE;
  CDC &dc = getDC();
  Point2DP start(PPOS(p.x)+PIXELSIZE/2, PPOS(p.y)+PIXELSIZE/2);
  Point2DP end = start + v;

  CPen pen(PS_SOLID, 2, color);

  CGdiObject *oldGDI = dc.SelectObject(&pen);
  dc.MoveTo(start);
  dc.LineTo(end);

  const short angle360[] = {
     270  // S
   ,   0  // E
   ,  90  // N
   , 180  // W
  };

  const double   angle = GRAD2RAD(angle360[dir]);
  const int      aLen  = 7;
  const double   s    = M_PI/6;
  const double   a1   = angle + s;
  const double   a2   = angle - s;
  const Point2DP v1   = end - Point2D(aLen*cos(a1),-aLen*sin(a1));
  const Point2DP v2   = end - Point2D(aLen*cos(a2),-aLen*sin(a2));

  dc.MoveTo(end);
  dc.LineTo(v1);
  dc.MoveTo(end);
  dc.LineTo(v2);

  dc.SelectObject(oldGDI);
  m_arrowDirection = dir;
  m_arrowPoint     = p;
  return *this;
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::removeArrow() {
  if(m_arrowDirection == NODIR) {
    return *this;
  }
  const CSize &sz = getWindowSize();
  const CRect  r(0,0,sz.cx,sz.cy);
  for(int dy = -2; dy <= 2; dy++) {
    for(int dx = -2; dx <= 2; dx++) {
      const CPoint p(m_arrowPoint.x+dx,m_arrowPoint.y+dy);
      if(r.PtInRect(p)) {
        setPixel(p,m_pixelMatrix(p.x,p.y).m_color);
      }
    }
  }
  m_arrowDirection = NODIR;
  return *this;
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::paintEdgeMatrix(const CPoint &center) {
  removeEdgeMatrix();
  drawEdgeMatrix(center, BLACK);
  m_edgeMatrixCenter = center;
  return *this;
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::removeEdgeMatrix() {
  if(m_edgeMatrixCenter.x >= 0) {
    drawEdgeMatrix(m_edgeMatrixCenter, ::GetSysColor(COLOR_BTNFACE));
    m_edgeMatrixCenter = CPoint(-1,-1);
  }
  return *this;
}

void CTestEdgeDetectionDlg::drawEdgeMatrix(const CPoint &center, COLORREF color) {
  CDC &dc = getDC();

  CBrush brush;
  brush.CreateSolidBrush(color);
  CRect r(PPOS(center.x-1)-1,PPOS(center.y-1)-1, PPOS(center.x+2), PPOS(center.y+2));
  dc.FrameRect(r, &brush);
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::paintLine(const CPoint &p1, const CPoint &p2, COLORREF color) {
  if(p1.x == p2.x) {
    int fromY = min(p1.y, p2.y);
    int toY   = max(p1.y, p2.y);
    for(int y = fromY; y <= toY; y++) {
      setPixel(p1.x, y, color);
    }
  } else if(p1.y == p2.y) {
    int fromX = min(p1.x, p2.x);
    int toX   = max(p1.x, p2.x);
    for(int x = fromX; x <= toX; x++) {
      setPixel(x, p1.y, color);
    }
  } else {
    CPoint p = p1;
    const int vx = p2.x - p1.x;
    const int vy = p2.y - p1.y;
    const int dx = sign(vx);
    const int dy = sign(vy);
    while(p != p2) {
      setPixel(p, color);
      if(abs((p.x-p1.x+dx)*vy - (p.y-p1.y)*vx) < abs((p.x-p1.x)*vy - (p.y-p1.y+dy)*vx)) {
        p.x += dx;
      } else {
        p.y += dy;
      }
    }
  }
  return *this;
}

CTestEdgeDetectionDlg &CTestEdgeDetectionDlg::paintCircel(const CPoint &center, int r, COLORREF color, bool filled) {
  int x = r;
  int y = 0;
  int lastpy = -1;
  unsigned int r2 = r*r;
  while(x >= 0) {
    if(filled) {
      if(y != lastpy) {
        paintLine(center.x-x, center.y-y, center.x+x,center.y-y, color);
        paintLine(center.x-x, center.y+y, center.x+x,center.y+y, color);
        lastpy = y;
      }
    } else {
      setPixel(center.x + x, center.y + y, color);
      setPixel(center.x + x, center.y - y, color);
      setPixel(center.x - x, center.y + y, color);
      setPixel(center.x - x, center.y - y, color);
    }
    if(abs((int)(sqr(x-1) + sqr(y)-r2)) < abs((int)(sqr(x) + sqr(y+1)-r2))) {
      x--;
    } else {
      y++;
    }
  }
  return *this;
}

void CTestEdgeDetectionDlg::OnEditReset() {
  resetMarkMatrix();
  setBackground(BLUE);
  flush();
  redirectDebugLog();
}

void CTestEdgeDetectionDlg::OnEditCircel() {
  const CSize &sz = getWindowSize();
  int r = min(sz.cx, sz.cy);
  r = r/2 - 2;
  const CPoint center(sz.cx/2-1,sz.cy/2-1);

  paintCircel(center, r, RED).flush();
}

void CTestEdgeDetectionDlg::OnEditLine() {
  const CSize &sz = getWindowSize();
  paintLine(0,0, sz.cx-1, sz.cy-1, GREEN).flush();
}

void CTestEdgeDetectionDlg::OnEditS()        { setSearchDir( S    ); }
void CTestEdgeDetectionDlg::OnEditE()        { setSearchDir( E    ); }
void CTestEdgeDetectionDlg::OnEditN()        { setSearchDir( N    ); }
void CTestEdgeDetectionDlg::OnEditW()        { setSearchDir( W    ); }
void CTestEdgeDetectionDlg::OnEditNoSearch() { setSearchDir( NODIR); }

void CTestEdgeDetectionDlg::OnEditTestArrrow() {
  if(toggleMenuItem(this, ID_EDIT_TESTARRROW)) {
    checkMenuItem(this, ID_EDIT_TESTEDGEMATRIX, false);
    removeEdgeMatrix();
  } else {
    removeArrow();
  }
}

void CTestEdgeDetectionDlg::OnEditTestEdgeMatrix() {
  if(toggleMenuItem(this, ID_EDIT_TESTEDGEMATRIX)) {
    checkMenuItem(this, ID_EDIT_TESTARRROW, false);
    removeArrow();
  } else {
    removeEdgeMatrix();
  }
}

typedef struct {
  Direction m_dir;
  int       m_id;
  int       m_bitmapId;
} DirectionMenuItem;

void CTestEdgeDetectionDlg::setSearchDir(Direction dir) {
  const DirectionMenuItem menuItems[] = {
    S    , ID_EDIT_S       , IDB_BITMAP_S
   ,E    , ID_EDIT_E       , IDB_BITMAP_E
   ,N    , ID_EDIT_N       , IDB_BITMAP_N
   ,W    , ID_EDIT_W       , IDB_BITMAP_W
   ,NODIR, ID_EDIT_NOSEARCH, IDB_BITMAP_NODIR
  };
  for(int i = 0; i < ARRAYSIZE(menuItems); i++) {
    const DirectionMenuItem &dm = menuItems[i];
    checkMenuItem(this, dm.m_id, dm.m_dir == dir);
    if(dm.m_dir == dir) {
      if(m_currentDirBitmap != NULL) {
        DeleteObject(m_currentDirBitmap);
        m_currentDirBitmap = NULL;
      }
      m_currentDirBitmap = ::LoadBitmap(theApp.m_hInstance, MAKEINTRESOURCE(dm.m_bitmapId));
      ((CStatic*)GetDlgItem(IDC_STATICDIRIMAGE))->SetBitmap(m_currentDirBitmap);
;   }
  }
  m_currentSearchDir = dir;
}

void CTestEdgeDetectionDlg::OnEditGo() {
  if(m_edgeThread) {
    const bool oldBreakPoint = m_breakPoint;
    m_breakPoint = false;
    m_edgeThread->resume();
    Sleep(500);
    m_breakPoint = oldBreakPoint;
  }
}

void CTestEdgeDetectionDlg::OnEditToggleBreakpoint() {
  m_breakPoint = toggleMenuItem(this, ID_EDIT_TOGGLEBREAKPOINT);
}

void CTestEdgeDetectionDlg::OnEditSingleStep() {
  if(m_edgeThread) {
    const bool oldBreakPoint = m_breakPoint;
    m_breakPoint = true;
    m_edgeThread->resume();
    Sleep(20);
    m_breakPoint = oldBreakPoint;
  }
}

void CTestEdgeDetectionDlg::OnEditDoFill() {
  m_doFill = toggleMenuItem(this, ID_EDIT_DOFILL);
}

void CTestEdgeDetectionDlg::OnEditShowFillInfo() {
  const FillInfo &info = getFillInfo();
  savePixelMatrix();
  paintPointSet(info.getEdgeSet() , RGB(255,255,255));
  paintPointSet(info.getInnerSet(), RGB(255,255,0));
  flush();
  showInformation(info.toString());
  restorePixelMatrix();
}

void CTestEdgeDetectionDlg::paintPointSet(const PointSet &ps, COLORREF color) {
  for(Iterator<size_t> it = ((PointSet&)ps).getIterator(); it.hasNext();) {
    const CPoint p = ps.next(it);
    setPixel(p, color);
  }
}

void CTestEdgeDetectionDlg::savePixelMatrix() {
  m_savedCopy = m_pixelMatrix;
}

void CTestEdgeDetectionDlg::restorePixelMatrix() {
  if(m_savedCopy.hasSameDimension(m_pixelMatrix)) {
    m_pixelMatrix = m_savedCopy;
    m_savedCopy.setDimension(1);
    paintPixelMatrix();
    flush();
  }
}

static Direction currentArrowDir = N;
void CTestEdgeDetectionDlg::OnLButtonDown(UINT nFlags, CPoint point) {
  __super::OnLButtonDown(nFlags, point);

  const CPoint p = getPixelPointFromMousePoint(point);
  if(!isPointInside(p)) return;
  if(isMenuItemChecked(this, ID_EDIT_TESTARRROW)) {
    paintArrow(p, currentArrowDir);
    currentArrowDir = turnRight(currentArrowDir);
  } else if(isMenuItemChecked(this, ID_EDIT_TESTEDGEMATRIX)) {
    paintEdgeMatrix(p);
  } else {
    m_drawColor = (getPixel(p) == BLUE) ? RED : BLUE;
    setPixel(p, m_drawColor); flush();
    m_lastMousePoint = p;
  }
}

void CTestEdgeDetectionDlg::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    const CPoint p = getPixelPointFromMousePoint(point);
    if((p != m_lastMousePoint) && isPointInside(p)) {
      paintLine(m_lastMousePoint, p, m_drawColor);
      m_lastMousePoint = p;
    }
  }
  __super::OnMouseMove(nFlags, point);
}

void CTestEdgeDetectionDlg::OnRButtonDown(UINT nFlags, CPoint point) {
  __super::OnRButtonDown(nFlags, point);
  unmarkAllPixels(); flush();
  redirectDebugLog();
  point = getPixelPointFromMousePoint(point);
  if(!isPointInside(point)) return;
  if(getSearchDir() == NODIR) {
    followEdge(point, S);
  } else {
    const CPoint ep = findEdge(point);
    if(ep.x >= 0) {
      followEdge(ep, getSearchDir());
    } else {
      showInformation(_T("No object found in %s direction"), directionName[getSearchDir()]);
      return;
    }
  }
}

void CTestEdgeDetectionDlg::OnNcLButtonDown(UINT nHitTest, CPoint point) {
  if(isBorderHit(nHitTest) && isEdgeThreadActive()) {
    return;
  }
  __super::OnNcLButtonDown(nHitTest, point);
}


CPoint CTestEdgeDetectionDlg::findEdge(const CPoint &p) {
  const CSize    size  = getWindowSize();
  const COLORREF c0    = getPixel(p);
  const CPoint  &step  = EdgeMatrix::dirStep[getSearchDir()];
  CRect r(0,0,size.cx, size.cy);
  for(CPoint sp = p; r.PtInRect(sp); sp += step) {
    if(getPixel(sp) != c0) {
      return sp;
    }
  }
  return CPoint(-1,-1);
}


void CTestEdgeDetectionDlg::followEdge(const CPoint &p, Direction searchDir) {
  killEdgeThread();
  unmarkAllPixels();
  removeEdgeMatrix();
  removeArrow();
  flush();
  enableContinueInternal(false);
  m_edgeThread = new FollowEdgeThread(this, p);
  m_edgeThread->start();
}

void CTestEdgeDetectionDlg::killEdgeThread() {
  if(m_edgeThread != NULL) {
    delete m_edgeThread;
    m_edgeThread = NULL;
    enableContinueInternal(false);
  }
}

void CTestEdgeDetectionDlg::enableContinueThread(bool enabled) {
  PostMessage(ID_MSG_ENABLECONTINUETHREAD, enabled);
}

LRESULT CTestEdgeDetectionDlg::OnMsgEnableContinueThread(WPARAM wp, LPARAM lp) {
  enableContinueInternal(wp?true:false);
  return 0;
}

void CTestEdgeDetectionDlg::enableContinueInternal(bool enabled) {
  enableMenuItem(this, ID_EDIT_GO        , enabled);
  enableMenuItem(this, ID_EDIT_SINGLESTEP, enabled);
}

void CTestEdgeDetectionDlg::releaseFillInfo() {
  delete m_fillInfo;
  m_fillInfo = NULL;
}

FillInfo &CTestEdgeDetectionDlg::getFillInfo() {
  if((m_fillInfo != NULL) && (m_fillInfo->getSize() != m_logicalSize)) {
    releaseFillInfo();
  }
  if(m_fillInfo == NULL) {
    const CRect rect(0,0,m_logicalSize.cx, m_logicalSize.cy);
    m_fillInfo = new FillInfo(rect);
  }
  return *m_fillInfo;
}

