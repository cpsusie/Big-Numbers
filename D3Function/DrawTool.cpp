#include "stdafx.h"
#include <Tokenizer.h>
#include <MFCUtil/Clipboard.h>
#include <MFCUtil/ColorSpace.h>
#include "DrawTool.h"
#include "ProfilePainter.h"

DrawTool::DrawTool(ProfileEditor *editor) : m_editor(*editor) {
  initState();
  m_blackBrush.CreateSolidBrush(0);
  m_redBrush.CreateSolidBrush(RED);
  m_infostr[0] = '\0';
}

bool DrawTool::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  _stprintf(m_infostr,_T("nchar:%c (#%x), nFlags:%08x     "),nChar,nChar,nFlags);
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

DrawTool &DrawTool::paintBox(Point2D *point, bool selected) {
  Viewport2D  &vp = m_editor.getViewport();
  const CPoint  p = (CPoint)vp.forwardTransform(*point);
  CRect rect(p.x-BOXSIZE, p.y-BOXSIZE, p.x+BOXSIZE, p.y+BOXSIZE);
  vp.getDC()->FillRect(&rect,(selected || isSelected(point)) ? &m_redBrush : &m_blackBrush);
  return *this;
}

DrawTool &DrawTool::paintPoints(ProfilePolygon2D *p, bool selected) {
  Viewport2D &vp           = m_editor.getViewport();
  Point2D    *currentPoint = &p->m_start;
  paintBox(currentPoint, selected);
  for(auto it = p->m_curveArray.getIterator(); it.hasNext();) {
    ProfileCurve2D &curve       = it.next();
    Point2DArray   &curvePoints = curve.m_points;
    switch(curve.m_type) {
    case TT_PRIM_LINE   :
      { for(Point2D *p = curvePoints.begin(), *endp = curvePoints.end(); p < endp;) {
          paintBox(p, selected);
          currentPoint = p++;
        }
      }
      break;
    case TT_PRIM_QSPLINE:
      { String str = curve.toString();
        int f = 1;
      }
      break;
    case TT_PRIM_CSPLINE:
      { for(Point2D *p = curvePoints.begin(), *endp = curvePoints.end(); p < endp;) {
          Point2D *beginBezier = currentPoint;
          Point2D *p1          =  p++;
          Point2D *p2          =  p++;
          Point2D *endBezier   =  p++;

          paintBox(beginBezier, selected);
          paintBox(p1         , selected);
          paintBox(p2         , selected);
          paintBox(endBezier  , selected);
          vp.MoveTo(*beginBezier);
          vp.LineTo(*p1);
          vp.MoveTo(*endBezier);
          vp.LineTo(*p2);
          currentPoint = endBezier;
        }
      }
      break;
    }
  }
  return *this;
}

DrawTool &DrawTool::repaintPolygon(ProfilePolygon2D *pp, bool selected) {
  paintProfilePolygon(*pp, m_editor.getViewport(), selected ? RED : BLACK);
  if(m_editor.getShowPoints()) {
    paintPoints(pp, selected);
  }
  return *this;
}

DrawTool &DrawTool::repaintProfile() {
  m_editor.getViewport().clear(WHITE);
  Profile2D &p = m_editor.getProfile();
  for(auto it = p.m_polygonArray.getIterator(); it.hasNext();) {
    ProfilePolygon2D &polygon = it.next();
    repaintPolygon(&polygon, isSelected(&polygon));
  }
  switch(m_editor.getNormalsMode()) {
  case NORMALS_INVISIBLE:
    break;
  case NORMALS_FLAT     :
    paintProfileNormals(p, m_editor.getViewport(), BLUE, false);
    break;
  case NORMALS_SMOOTH   :
    paintProfileNormals(p, m_editor.getViewport(), BLUE, true);
    break;
  }
  m_polygonSet.repaint(m_editor.getViewport(), m_state);
  return drawState();

  // dont call repaintScreen here.
}

DrawTool &DrawTool::repaintScreen() {
  m_editor.repaintViewport();
  return *this;
}

DrawTool &DrawTool::repaintAll() {
  return repaintProfile().repaintScreen();
}

DrawTool &DrawTool::drawState() {
  m_editor.getViewport().getDC()->TextOut(10,10,stateToString(m_state));
  Viewport2D &vp = m_editor.getViewport();
  int y = 20;
  for(Tokenizer tok(m_infostr,_T("\n")); tok.hasNext();) {
    m_editor.getViewport().getDC()->TextOut(10,y+=20,tok.next().cstr());
  }
  return *this;
}

DrawTool &DrawTool::selectPolygonsInRect(const Rectangle2D &r) {
  Profile2D &profile = m_editor.getProfile();
  for(auto it = profile.m_polygonArray.getIterator(); it.hasNext();) {
    ProfilePolygon2D &p = it.next();
    if(r.contains(p.getBoundingBox())) {
      select(&p);
    }
  }
  return *this;
}

DrawTool &DrawTool::selectPointsInRect(const Rectangle2D &r) {
  Point2DRefArray points = m_editor.getProfile().getAllPointsRef();
  for(Point2D *p : points) {
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

DrawTool &DrawTool::select(ProfilePolygon2D *p) {
  m_polygonSet.add(p);
  return *this;
}

DrawTool &DrawTool::unselect(ProfilePolygon2D *p) {
  const intptr_t index = m_polygonSet.getFirstIndex(p);
  if(index >= 0) {
    m_polygonSet.remove(index);
  }
  return *this;
}

bool DrawTool::isSelected(ProfilePolygon2D *p) const {
  return m_polygonSet.contains(p);
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

bool DrawTool::isSelected(Point2D *p) const {
  return m_selectedPoints.contains(p);
}

bool DrawTool::canDelete() {
  return !m_polygonSet.isEmpty();
}

DrawTool &DrawTool::deleteSelected() {
  Profile2D &profile = m_editor.getProfile();
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
  Profile2D copy;
  for(size_t i = 0; i < m_polygonSet.size(); i++) {
    copy.addPolygon(*m_polygonSet[i]);
  }
  putClipboard(m_editor.getWindow(),copy.toXML());
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
  Profile2D pasted(str);
  Rectangle2D rect = m_editor.getViewport().getFromRectangle();
  Profile2D &profile = m_editor.getProfile();
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
  return m_editor.getProfile().canConnect(m_selectedPoints[0], m_selectedPoints[1]);
}

DrawTool &DrawTool::connect() {
  if(!canConnect()) {
    return *this;
  }
  m_editor.getProfile().connect(m_selectedPoints[0], m_selectedPoints[1]);
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
  if(!canMirror()) {
    return *this;
  }

  m_polygonSet.mirror(horizontal);
  return repaintAll();
}
