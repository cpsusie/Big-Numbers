#include "stdafx.h"
#include "TestEdgeDetectionDlg.h"

FollowEdgeThread::FollowEdgeThread(CTestEdgeDetectionDlg *dlg, const CPoint &startPoint)
: Thread(_T("FollowEdgeThread"))
, m_dlg(*dlg)
, m_currentRect(CPoint(0,0), dlg->getWindowSize())
, m_startPoint(startPoint)

{
  setDemon(true);
}

unsigned int FollowEdgeThread::run() {
  m_fillInfo = &m_dlg.getFillInfo();
  m_fillInfo->clear();

  const COLORREF c0 = m_dlg.getPixel(m_startPoint);
  m_dlg.setPixel(m_startPoint, COMPLEMENTCOLOR(c0));

  followBlackEdge(m_startPoint);
  return 0;
}

bool FollowEdgeThread::followBlackEdge(const CPoint &p) {
  CTestEdgeDetectionDlg &pa       = m_dlg;
  const COLORREF         newColor = pa.getPixel(p);
  const COLORREF         c0       = COMPLEMENTCOLOR(newColor);
  CPoint                 q        = p;
  Direction              dir      = turnRight(m_dlg.getSearchDir());
  Direction              firstDir = NODIR;

  PointSet edgeSet( m_fillInfo->getRect());
  PointSet innerSet(m_fillInfo->getRect());

  edgeSet.add(p);

  EdgeMatrix             edgeMatrix;
  for(;;) {

//    debugLog(_T("Før søgning:\n%s"), edgeMatrix.toString().cstr());

    for(int dy = -1; dy <= 1; dy++) {
      const int qy = q.y+dy;
      if(qy < m_currentRect.top) {
        edgeMatrix.setRowOutside(0);
        continue;
      } else if(qy >= m_currentRect.bottom) {
        edgeMatrix.setRowOutside(2);
        continue;
      }
      for(int dx = -1; dx <= 1; dx++) {
        if(!edgeMatrix.isDirty(dy+1,dx+1)) {
          continue;
        }
        const int qx = q.x+dx;
        if((qx < m_currentRect.left) || (qx >= m_currentRect.right)) {
          edgeMatrix.setOutside(dy+1,dx+1);
        } else if(pa.getPixel(qx,qy) == newColor) {
          edgeMatrix.setInside(dy+1,dx+1);
        } else if(pa.getPixel(qx,qy) == c0) {
          edgeMatrix.setInside(dy+1,dx+1);
        } else {
          edgeMatrix.setOutside(dy+1,dx+1);
        }
      }
    }

    if((dir = edgeMatrix.findStepDirection(dir)) == NODIR) {
      break;
    } else if(firstDir == NODIR) {
      firstDir = dir;
    } else if((q == p) && (dir == firstDir)) {
      break;
    }

    if(m_dlg.hasBreakPoint()) {
      m_dlg.paintEdgeMatrix(q).paintArrow(q, dir).flush(); pause();
    }

    if(edgeMatrix.getLeftAttr(dir)) {
      const CPoint ql = q + EdgeMatrix::leftStep[dir];
      innerSet.add(ql);
    }
    q += EdgeMatrix::dirStep[dir];
    pa.setPixel(q, newColor);// m_dlg.flush();
    edgeSet.add(q);

    edgeMatrix.adjustAttributes(dir);
  }

  m_dlg.removeEdgeMatrix().removeArrow().flush();

  innerSet -= edgeSet;
  m_fillInfo->setEdgeAndInnerSet(edgeSet, innerSet);
  m_fillInfo->addEdgeSetToFilledSet();
  if(m_dlg.doFill()) {
    fillInnerArea(innerSet);
  }
  return 0;
}

#define ISCOLORTOFILL(c) ((c) == c0)
#define FILLCOLOR newColor

#define CHECKPIXELPP                    \
  COLORREF c = pa.getPixel(pp);         \
  if(!ISCOLORTOFILL(c)) break;          \
  pa.setPixel(pp, FILLCOLOR);           \
  m_fillInfo->addFilled(pp);

#define CHECKNEIGHBOURPIXEL(i)          \
{ c = pa.getPixel(pp.x, y##i);          \
  if(!stacked##i) {                     \
    if(ISCOLORTOFILL(c)) {              \
      stack.push(CPoint(pp.x, y##i));   \
      stacked##i = true;                \
    }                                   \
  } else if(!ISCOLORTOFILL(c)) {        \
    stacked##i = false;                 \
  }                                     \
}

void FollowEdgeThread::fillInnerArea(PointSet &innerSet) {
  CTestEdgeDetectionDlg &pa       = m_dlg;
  const COLORREF         newColor = pa.getPixel(m_startPoint);
  const COLORREF         c0       = COMPLEMENTCOLOR(newColor);

  for(Iterator<CPoint> it = innerSet.getIterator(); it.hasNext();) {
    const CPoint &start = it.next();
    if(!ISCOLORTOFILL(pa.getPixel(start))) {
      continue;
    }

    CompactStack<CPoint> stack;
    stack.push(start);
    while(!stack.isEmpty()) {
      const CPoint np = stack.pop();
      const int    y1 = np.y-1;
      const int    y2 = np.y+1;
      if(y1 >= m_currentRect.top) {
        if(y2 < m_currentRect.bottom) {
          bool stacked1 = false, stacked2 = false;
          for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) { // go left
            CHECKPIXELPP
            CHECKNEIGHBOURPIXEL(1)
            CHECKNEIGHBOURPIXEL(2)
          }
          stacked1 = stacked2 = false;
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {             // go right
            CHECKPIXELPP
            CHECKNEIGHBOURPIXEL(1)
            CHECKNEIGHBOURPIXEL(2)
          }
        } else {                                                    // dont check y2
          bool stacked1 = false;
          for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) { // go left
            CHECKPIXELPP
            CHECKNEIGHBOURPIXEL(1)
          }
          stacked1 = false;
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {             // go right
            CHECKPIXELPP
            CHECKNEIGHBOURPIXEL(1)
          }
        }
      } else if(y2 < m_currentRect.bottom) {                        // dont check y1
        bool stacked2 = false;
        for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) {   // go left
          CHECKPIXELPP
          CHECKNEIGHBOURPIXEL(2)
        }
        stacked2 = false;
        for(CPoint pp = np; ++pp.x < m_currentRect.right;) {               // go right
          CHECKPIXELPP
          CHECKNEIGHBOURPIXEL(2)
        }
      }
    }
  }
  m_dlg.flush();
}

void FollowEdgeThread::pause() {
  m_dlg.enableContinueThread(true);
  suspend();
  m_dlg.enableContinueThread(false);
}

String FillInfo::toString() const {
  String result;
  const CRect &r = m_filledSet.getRect();
  result = format(_T("R:(%d,%d,%d,%d)\n"), r.left,r.top,r.right,r.bottom);
  result += format(_T("#Filled points:%d\n"), m_filledSet.size());
  result += format(_T("#Edge   points:%d\n"), m_edgeSet.size()) ;
  result += format(_T("#Inner  points:%d\n"), m_innerSet.size() );
  result += format(_T("First edgePoints :(%s)\n"), m_edgeSet.toString().cstr());
  result += format(_T("First innerPoints:(%s)\n"), m_innerSet.toString().cstr());
  return result;
}
