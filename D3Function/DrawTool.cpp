#include "stdafx.h"
#include <Tokenizer.h>
#include <MFCUtil/Clipboard.h>
#include <MFCUtil/ColorSpace.h>
#include "DrawTool.h"
#include "ProfilePainter.h"

static TCHAR *stateString(ProfileDialogState state) {
  switch(state) {
  case IDLE       : return _T("idle"         );
  case DRAGGING   : return _T("dragging"     );
  case MOVING     : return _T("moving"       );
  case STRETCHING : return _T("stretching"   );
  case ROTATING   : return _T("rotating"     );
  default         : return _T("unknown state");
  }
}

void PolygonSet::repaint(Viewport2D &vp, ProfileDialogState state) {
  if(isEmpty()) {
    return;
  }

  switch(state) {
  case IDLE       :
  case DRAGGING   :
    break;

  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    m_boundingBox.repaint(vp, state);
    break;
  }
}

Point2DRefArray PolygonSet::getPointRefArray() {
  Point2DRefArray result;
  for(size_t i = 0; i < size(); i++) {
    result.addAll((*this)[i]->getAllPointsRef());
  }
  return result;
}

void PolygonSet::move(const Point2D &dp) {
  m_boundingBox.move(dp, getPointRefArray());
}

void PolygonSet::stretch(const Point2D &dp) {
  m_boundingBox.stretch(dp, getPointRefArray());
}

void PolygonSet::rotate(const Point2D &dp) {
  Point2DRefArray pa = getPointRefArray();
  m_boundingBox.rotate(dp, pa);
}

void PolygonSet::mirror(bool horizontal) {
  m_boundingBox.mirror(horizontal, getPointRefArray());
  invertNormals();
}

void PolygonSet::invertNormals() {
  for(size_t i = 0; i < size(); i++) {
    (*this)[i]->reverseOrder();
  }
}

void PolygonSet::evaluateBox() {
  if(!isEmpty()) {
    Rectangle2D box = first()->getBoundingBox();
    for(size_t i = 1; i < size(); i++) {
      box = getUnion(box,(*this)[i]->getBoundingBox());
    }
    m_boundingBox = box;
//    m_boundingBox.scale(1.2);
  }
}

bool PolygonSet::pointOnMarkRect(const CPoint &p) {
  return m_boundingBox.pointOnMarkRect(p);
}

TCHAR infostr[400];

DrawTool::DrawTool(ViewportContainer *container) : m_container(*container) {
  initState();
  m_blackBrush.CreateSolidBrush(0);
  m_redBrush.CreateSolidBrush(RED);
  infostr[0] = '\0';
}

DrawTool::~DrawTool() {
}

bool DrawTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  _stprintf(infostr,_T("nchar:%c (#%x), nFlags:%08x     "),nChar,nChar,nFlags);
  switch(nChar) {
  case VK_DELETE:
    deleteSelected();
    return TRUE;

  case 'C':
    if((nFlags & MK_CONTROL) && canCopy()) {
      copySelected();
    }
    return true;

  case 'V':
    if((nFlags & MK_CONTROL)) {
      try {
        paste();
      } catch(Exception e) {
        showException(e);
      }
    }
    return true;

  case 'X':
    if((nFlags & MK_CONTROL) && canCut()) {
      cutSelected();
    }
    return true;

  default:
    drawState();
    repaintScreen();
    return FALSE;
  }
}

#define BOXSIZE 2

DrawTool &DrawTool::paintBox(const Point2D &point, bool selected) {
  Viewport2D  &vp = m_container.getViewport();
  const CPoint  p = vp.forwardTransform(point);
  CRect rect(p.x-BOXSIZE, p.y-BOXSIZE, p.x+BOXSIZE, p.y+BOXSIZE);
  vp.getDC()->FillRect(&rect,(selected || isSelected(&point)) ? &m_redBrush : &m_blackBrush);
  return *this;
}

DrawTool &DrawTool::paintPoints(const ProfilePolygon &p, bool selected) {
  const Point2D *currentPoint = &p.m_start;
  Viewport2D &vp = m_container.getViewport();
  paintBox(*currentPoint, selected);
  for(size_t c = 0; c < p.m_curveArray.size(); c++) {
    const ProfileCurve &curve = p.m_curveArray[c];
    switch(curve.m_type) {
    case TT_PRIM_LINE   :
      { for(size_t j = 0; j < curve.m_points.size(); j++) {
          const Point2D &nextPoint = curve.m_points[j];
          paintBox(nextPoint, selected);
          currentPoint = &nextPoint;
        }
      }
      break;
    case TT_PRIM_QSPLINE:
      { String str = curve.toString();
        int f = 1;
      }
      break;
    case TT_PRIM_CSPLINE:
      { for(size_t j = 0; j < curve.m_points.size(); j+=3) {
          const Point2D &beginBezier = *currentPoint;
          const Point2D &p1          = curve.m_points[j];
          const Point2D &p2          = curve.m_points[j+1];
          const Point2D &endBezier   = curve.m_points[j+2];

          paintBox(beginBezier, selected);
          paintBox(p1         , selected);
          paintBox(p2         , selected);
          paintBox(endBezier  , selected);
          vp.MoveTo(beginBezier);
          vp.LineTo(p1);
          vp.MoveTo(endBezier);
          vp.LineTo(p2);
          currentPoint = &endBezier;
        }
      }
      break;
    }
  }
  return *this;
}

DrawTool &DrawTool::repaintPolygon(const ProfilePolygon &pp, bool selected) {
  paintProfilePolygon(pp, m_container.getViewport(), selected ? RED : BLACK);
  if(m_container.getShowPoints()) {
    paintPoints(pp, selected);
  }
  return *this;
}

DrawTool &DrawTool::repaintProfile() {
  m_container.getViewport().clear(WHITE);
  Profile &p = m_container.getProfile();
  for(size_t i = 0; i < p.m_polygonArray.size(); i++) {
    ProfilePolygon &polygon = p.m_polygonArray[i];
    repaintPolygon(polygon, isSelected(&polygon));
  }
  switch(m_container.getNormalsMode()) {
  case NORMALS_INVISIBLE:
    break;
  case NORMALS_FLAT     :
    paintProfileNormals(p, m_container.getViewport(), BLUE, false);
    break;
  case NORMALS_SMOOTH   :
    paintProfileNormals(p, m_container.getViewport(), BLUE, true);
    break;
  }
  m_polygonSet.repaint(m_container.getViewport(), m_state);
  return drawState();

  // dont call repaintScreen here.
}

DrawTool &DrawTool::repaintScreen() {
  m_container.repaintViewport();
  return *this;
}

DrawTool &DrawTool::repaintAll() {
  return repaintProfile().repaintScreen();
}

DrawTool &DrawTool::drawState() {
  m_container.getViewport().getDC()->TextOut(10,10,stateString(m_state));
  Viewport2D &vp = m_container.getViewport();
  Tokenizer tok(infostr,_T("\n"));
  int y = 20;
  while(tok.hasNext()) {
    m_container.getViewport().getDC()->TextOut(10,y+=20,tok.next().cstr());
  }
  return *this;
}

DrawTool &DrawTool::selectPolygonsInRect(const Rectangle2D &r) {
  Profile &profile = m_container.getProfile();
  for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
    ProfilePolygon &p = profile.m_polygonArray[i];
    if(r.contains(p.getBoundingBox())) {
      select(&p);
    }
  }
  return *this;
}

DrawTool &DrawTool::selectPointsInRect(const Rectangle2D &r) {
  CompactArray<Point2D*> points = m_container.getProfile().getAllPointsRef();
  for(size_t i = 0; i < points.size(); i++) {
    Point2D *p = points[i];
    if(r.contains(*p)) {
      select(p);
    }
  }
  return *this;
}

DrawTool &DrawTool::unselectAll() {
  m_polygonSet.clear();
  m_selectedPoints.clear();
  return *this;
}

DrawTool &DrawTool::select(ProfilePolygon *p) {
  m_polygonSet.add(p);
  return *this;
}

DrawTool &DrawTool::unselect(ProfilePolygon *p) {
  const intptr_t index = m_polygonSet.getFirstIndex(p);
  if(index >= 0) {
    m_polygonSet.remove(index);
  }
  return *this;
}

DrawTool &DrawTool::select(Point2D *p) {
  m_selectedPoints.add(p);
  return *this;
}

DrawTool &DrawTool::unselect(Point2D *p) {
  const intptr_t index = m_selectedPoints.getFirstIndex(p);
  if(index >= 0) {
    m_selectedPoints.remove(index);
  }
  return *this;
}

bool DrawTool::isSelected(ProfilePolygon *p) const {
  return m_polygonSet.contains(p);
}

bool DrawTool::isSelected(const Point2D *p) const {
  for(size_t i = 0; i < m_selectedPoints.size(); i++) {
    if(m_selectedPoints[i] == p) {
      return true;
    }
  }
  return false;
}

bool DrawTool::canDelete() {
  return !m_polygonSet.isEmpty();
}

DrawTool &DrawTool::deleteSelected() {
  Profile &profile = m_container.getProfile();
  for(size_t i = 0; i < m_polygonSet.size(); i++) {
    profile.m_polygonArray.remove(*m_polygonSet[i]);
  }
  return unselectAll().repaintAll();
}

bool DrawTool::canCopy() {
  return !m_polygonSet.isEmpty();
}

DrawTool &DrawTool::copySelected() {
#if !defined(__TODO__)
  showWarning(_T("%s:Function not implemented"), __TFUNCTION__);
#else
  Profile copy;
  for(size_t i = 0; i < m_polygonSet.size(); i++) {
    copy.addPolygon(*m_polygonSet[i]);
  }
  putClipboard(m_container.getWindow(),copy.toXML());
#endif
  return *this;
}

bool DrawTool::canCut() {
  return !m_polygonSet.isEmpty();
}

DrawTool &DrawTool::cutSelected() {
  return copySelected().deleteSelected();
}

DrawTool &DrawTool::paste() {
#if !defined(__TODO__)
  showWarning(_T("%s:Function not implemented"), __TFUNCTION__);
#else
  const String str = getClipboardText();
  Profile pasted(str);
  Rectangle2D rect = m_container.getViewport().getFromRectangle();
  Profile &profile = m_container.getProfile();
  pasted.move(Point2D(rect.getWidth()/10,rect.getHeight()/10));
  for(size_t i = 0; i < pasted.m_polygonArray.size(); i++) {
    profile.addPolygon(pasted.m_polygonArray[i]);
  }
  unselectAll();
  repaintProfile();
  repaintScreen();
#endif
  return *this;
}

bool DrawTool::canConnect() const {
  if(m_selectedPoints.size() != 2) {
    return false;
  }
  return m_container.getProfile().canConnect(m_selectedPoints[0],m_selectedPoints[1]);
}

DrawTool &DrawTool::connect() {
  if(!canConnect()) {
    return *this;
  }
  m_container.getProfile().connect(m_selectedPoints[0],m_selectedPoints[1]);
  return unselectAll().repaintAll();
}

bool DrawTool::canInvertNormals() const {
  return !m_polygonSet.isEmpty();
}

bool DrawTool::canMirror() const {
  return !m_polygonSet.isEmpty();
}

DrawTool &DrawTool::invertNormals() {
  m_polygonSet.invertNormals();
  return repaintAll();
}

DrawTool &DrawTool::mirror(bool horizontal) {
  if(!canMirror()) return *this;

  m_polygonSet.mirror(horizontal);
  return repaintAll();
}

bool LineTool::OnLButtonDown(UINT nFlags, CPoint point) {
  Profile &profile = m_container.getProfile();
  Point2D p = m_container.getViewport().backwardTransform(point);
  profile.addLine(p,p);
  ProfilePolygon &gp = profile.m_polygonArray.last();
  m_p0 = &gp.m_start;
  m_p1 = &gp.m_curveArray.last().m_points.last();
  unselectAll();
  select(&gp).repaintAll();
  return true;
}

bool LineTool::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON && m_p1 != NULL) {
    *m_p1 = m_container.getViewport().backwardTransform(point);
    repaintAll();
    return true;
  }
  return false;
}

bool LineTool::OnLButtonUp(UINT nFlags, CPoint point) {
  m_p0 = m_p1 = NULL;
  return true;
}

bool BezierCurveTool::OnLButtonDown(  UINT nFlags, CPoint point) {
  Profile &profile = m_container.getProfile();
  Point2D p = m_container.getViewport().backwardTransform(point);
  ProfilePolygon polygon;
  ProfileCurve   curve(TT_PRIM_CSPLINE);
  polygon.m_start  = p;
  polygon.m_closed = false;
  p.x += 0.1;
  p.y += 0.1;
  curve.addPoint(p);
  p.x += 0.1;
  curve.addPoint(p);
  p.x += 0.1;
  p.y -= 0.1;
  curve.addPoint(p);
  polygon.addCurve(curve);
  profile.addPolygon(polygon);

  unselectAll();
  select(&profile.m_polygonArray.last());
  repaintAll();
  return true;
}

bool BezierCurveTool::OnMouseMove(    UINT nFlags, CPoint point) {
  return true;
}

bool BezierCurveTool::OnLButtonUp(    UINT nFlags, CPoint point) {
  return true;
}

bool RectangleTool::OnLButtonDown(UINT nFlags, CPoint point) {
  Profile &profile = m_container.getProfile();
  Point2D p = m_container.getViewport().backwardTransform(point);
  ProfilePolygon polygon;
  ProfileCurve   curve(TT_PRIM_LINE);
  polygon.m_start  = p;
  polygon.m_closed = true;
  curve.addPoint(p);
  curve.addPoint(p);
  curve.addPoint(p);
  polygon.addCurve(curve);
  profile.addPolygon(polygon);

  ProfilePolygon &gp = profile.m_polygonArray.last();
  ProfileCurve   &pc = gp.m_curveArray.last();
  m_ul = &gp.m_start;
  m_ur = &pc.m_points[0];
  m_lr = &pc.m_points[1];
  m_ll = &pc.m_points[2];

  unselectAll();
  select(&gp);
  repaintAll();
  return true;
}

bool RectangleTool::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    Point2D p = m_container.getViewport().backwardTransform(point);
    m_ur->x = p.x;
    *m_lr = p;
    m_ll->y = p.y;
    repaintAll();
    return true;
  }
  return false;
}

bool RectangleTool::OnLButtonUp(UINT nFlags, CPoint point) {
  repaintScreen();
  return true;
}

bool PolygonTool::OnLButtonDown(UINT nFlags, CPoint point) {
  Point2D p        = m_container.getViewport().backwardTransform(point);
  Profile &profile = m_container.getProfile();
  if(m_pp == NULL) {
    ProfilePolygon pp;
    pp.m_start  = p;
    pp.m_closed = false;
    ProfileCurve curve(TT_PRIM_LINE);
    curve.addPoint(p);
    pp.addCurve(curve);
    profile.addPolygon(pp);
    m_pp = &profile.getLastPolygon();
  } else {
    m_pp->getLastCurve().addPoint(p);
    repaintAll();
  }
  return true;
}

bool PolygonTool::OnLButtonDblClk(UINT nFlags, CPoint point) {
  if(m_pp != NULL) {
    m_pp->m_closed = true;
    m_pp->removeLastPoint();
  }
  m_pp = NULL;
  repaintAll();
  return true;
}

bool PolygonTool::OnMouseMove(UINT nFlags, CPoint point) {
  Point2D p = m_container.getViewport().backwardTransform(point);
  if(m_pp != NULL) {
    m_pp->getLastPoint() = p;
    repaintAll();
    return true;
  }
  return false;
}

bool PolygonTool::OnLButtonUp(UINT nFlags, CPoint point) {
  return true;
}

bool EllipseTool::OnLButtonDown(UINT nFlags, CPoint point) {
  m_p0 = m_p1 = point;
  repaintScreen();
  return true;
}

bool EllipseTool::OnMouseMove(UINT nFlags, CPoint point) {
  if(nFlags & MK_LBUTTON) {
    m_p1 = point;
    repaintScreen();
    return true;
  }
  return false;
}

bool EllipseTool::OnLButtonUp(UINT nFlags, CPoint point) {
  repaintScreen();
  return true;
}


SelectTool::SelectTool(ViewportContainer *container) : DrawTool(container) {
}

void SelectTool::startDragRect() {
  m_dragRect = CRect(m_mouseDownPoint.x,m_mouseDownPoint.y,m_mouseDownPoint.x,m_mouseDownPoint.y);
  SIZE size;
  size.cx = size.cy = 1;
  m_container.getViewport().getDC()->DrawDragRect(&m_dragRect,size,m_dragRect,size);
  drawState();
}

void SelectTool::redrawDragRect(const CPoint &point) {
  CRect newRect(m_mouseDownPoint,point);
  if(newRect.left > newRect.right ) std::swap(newRect.left,newRect.right);
  if(newRect.top  > newRect.bottom) std::swap(newRect.top,newRect.bottom);
  SIZE size;
  size.cx = size.cy = 1;
  m_container.getViewport().getDC()->DrawDragRect(&newRect,size,m_dragRect,size);
  drawState();

  m_dragRect = newRect;
}

void SelectTool::endDragRect() {
  SIZE size,newSize;
  size.cx = size.cy = 1;
  newSize.cx = newSize.cy = 0;
  m_container.getViewport().getDC()->DrawDragRect(m_dragRect,newSize,m_dragRect,size);
  drawState();
}

class DistanceFinder: public CurveOperator {
private:
  Viewport2D &m_vp;
  CPoint    m_point;
public:
  double    m_minDist;
  DistanceFinder(Viewport2D &vp, const CPoint &p);
  void line(const Point2D &from, const Point2D &to);
};

DistanceFinder::DistanceFinder(Viewport2D &vp, const CPoint &p) : m_vp(vp) {
  m_minDist = -1;
  m_point   = p;
}

void DistanceFinder::line(const Point2D &from, const Point2D &to) {
  double dist = distanceFromLineSegment(m_vp.forwardTransform(from),m_vp.forwardTransform(to),Point2D(m_point));
  if(m_minDist < 0 || dist < m_minDist) {
    m_minDist = dist;
  }
}

#define MAXDIST 8

ProfilePolygon *SelectTool::findNearestPolygon(const CPoint &p) {
  Profile        &profile        = m_container.getProfile();
  double          minDist        = -1;
  ProfilePolygon *nearestPolygon = NULL;

  for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
    ProfilePolygon &polygon = profile.m_polygonArray[i];
    DistanceFinder df(m_container.getViewport(),p);
    polygon.apply(df);

    if((minDist < 0) || (df.m_minDist < minDist)) {
      minDist        = df.m_minDist;
      nearestPolygon = &polygon;
    }
  }
  return minDist < MAXDIST ? nearestPolygon : NULL;
}

Point2D *SelectTool::findNearestPoint(const CPoint &p) {
  CompactArray<Point2D*> points = m_container.getProfile().getAllPointsRef();
  double minDist        = -1;
  Point2D *nearestPoint = NULL;

  for(size_t i = 0; i < points.size(); i++) {
    Point2D     *point = points[i];
    const double dist  = distance(Point2D(p),m_container.getViewport().forwardTransform(*point));
    if((minDist < 0) || (dist < minDist)) {
      minDist      = dist;
      nearestPoint = point;
    }
  }
  return minDist < MAXDIST ? nearestPoint : NULL;
}

void SelectTool::moveSelectedPoints(const Point2D &dp) {
  for(size_t i = 0; i < m_selectedPoints.size(); i++) {
    *m_selectedPoints[i] += dp;
  }
}

bool SelectTool::OnLButtonDown(UINT nFlags, CPoint point) {
  m_mouseDownPoint = point;
  ProfilePolygon *np = findNearestPolygon(point);
  switch(m_state) {
  case IDLE       :
  case MOVING    :
    if(np != NULL) { // click on polygon
      if(nFlags & MK_SHIFT) { // shift pressed
        if(isSelected(np)) {
          unselect(np);
        } else {
          select(np);
        }
      } else { // shift not pressed
        if(!isSelected(np)) {
          unselectAll();
          select(np);
        }
      }
      if(!m_polygonSet.isEmpty()) {
        m_polygonSet.evaluateBox();
        m_state = MOVING;
      } else {
        m_state = IDLE;
      }
      repaintAll();
    } else if(m_polygonSet.pointInRect(m_container.getViewport().backwardTransform(point)) || m_polygonSet.pointOnMarkRect(point)) {
//      m_polygonSet.evaluateBox();
      m_state = MOVING;
      repaintAll();
    } else { // click outside everything
      unselectAll();
      m_state = DRAGGING;
      repaintProfile();
      startDragRect();
      repaintScreen();
    }
    break;

  case STRETCHING :
    if(!m_polygonSet.pointOnMarkRect(point)) {
      m_state = IDLE;
    }
    repaintAll();
    break;

  case ROTATING   :
    if(!m_polygonSet.pointOnMarkRect(point)) {
      m_state = IDLE;
    }
    repaintAll();
    break;

  case DRAGGING   :
    break;
  }
  m_lastMousePoint = point;
  return true;
}

bool SelectTool::OnLButtonDblClk(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
    break;

  case MOVING     :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = STRETCHING;
      repaintAll();
    }
    break;
  case STRETCHING :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = ROTATING;
      repaintAll();
    }
    break;
  case ROTATING   :
    if(m_polygonSet.pointOnMarkRect(point)) {
      m_state = MOVING;
      repaintAll();
    }
    break;

  case DRAGGING   :
    break;
  }
  return true;
}

bool SelectTool::OnLButtonUp(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    break;
  case DRAGGING   :
    selectPolygonsInRect(m_container.getViewport().backwardTransform(m_dragRect));
    m_state = m_polygonSet.isEmpty() ? IDLE : MOVING;
    endDragRect();
    m_polygonSet.evaluateBox();
    repaintAll();
    break;
  }
  return true;
}

bool SelectTool::OnRButtonDown(UINT nFlags, CPoint point) {
  m_mouseDownPoint = point;
  Point2D *np = findNearestPoint(point);
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    if(np != NULL) { // click on point
      if(nFlags & MK_SHIFT) { // shift pressed
        if(isSelected(np)) {
          unselect(np);
        } else {
          select(np);
        }
      } else { // shift not pressed
        if(!isSelected(np)) {
          unselectAll();
          select(np);
        }
      }
      if(!m_selectedPoints.isEmpty()) {
        m_state = MOVING;
      }
      repaintAll();
    } else {
      m_state = DRAGGING;
      unselectAll();
      repaintProfile();
      startDragRect();
      repaintScreen();
    }
    break;
  case DRAGGING   :
    break;
  }
  m_lastMousePoint = point;
  return true;
}

bool SelectTool::OnRButtonUp(UINT nFlags, CPoint point) {
  switch(m_state) {
  case IDLE       :
  case MOVING     :
  case STRETCHING :
  case ROTATING   :
    break;
  case DRAGGING   :
    selectPointsInRect(m_container.getViewport().backwardTransform(m_dragRect));
    endDragRect();
    m_state = m_selectedPoints.isEmpty() ? IDLE : MOVING;
    repaintAll();
    break;
  }
  return true;
}

void SelectTool::adjustMousePosition() {
//  m_container.setMousePosition(m_polygonSet.getSelectedMarkPoint());
}

bool SelectTool::OnMouseMove(UINT nFlags, CPoint point) {
  Point2D dp = m_container.getViewport().backwardTransform(point) - m_container.getViewport().backwardTransform(m_lastMousePoint);
  switch(m_state) {
  case IDLE       :
    break;

  case MOVING     :
    if(nFlags & MK_LBUTTON && !m_polygonSet.isEmpty()) {
      m_polygonSet.move(dp);
      repaintAll();
    } else if(nFlags & MK_RBUTTON && !m_selectedPoints.isEmpty()) {
      moveSelectedPoints(dp);
      repaintAll();
    }
    break;
  case STRETCHING :
    if(nFlags & MK_LBUTTON) {
      m_polygonSet.stretch(dp);
      adjustMousePosition();
      repaintAll();
    }
    break;
  case ROTATING   :
    if(nFlags & MK_LBUTTON) {
      m_polygonSet.rotate(dp);
      adjustMousePosition();
      repaintAll();
    }
    break;
  case DRAGGING   :
    if(nFlags & (MK_LBUTTON | MK_RBUTTON)) {
      redrawDragRect(point);
      repaintScreen();
    }
    break;

  }
  m_lastMousePoint = point;
  return true;
}
