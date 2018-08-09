#include "stdafx.h"
#include "MBBigRealCalculator.h"

UINT MBBigRealCalculator::findITCountFast(const BigReal &X, const BigReal &Y, UINT maxIteration) {
  UINT          count = 0;
  BigReal       a     = X;
  BigReal       b     = Y;
  const BigInt &_2 = getDigitPool()->get2();
  for(;count < maxIteration; count++) {
    const BigReal a2 = rProd(a,a,m_digits);
    const BigReal b2 = rProd(b,b,m_digits);
    if(a2+b2 > _4) {
      break;
    }
    const BigReal c = a2-b2+X;
    b = rProd(a*_2,b,m_digits)+Y;
    a = c;
  }
  return count;
}

UINT MBBigRealCalculator::findITCountPaintOrbit(const BigReal &X, const BigReal &Y, UINT maxIteration) {
  MBContainer  &mbc   = getMBContainer();
  UINT          count = 0;
  BigReal       a     = X;
  BigReal       b     = Y;
  const BigInt &_2 = getDigitPool()->get2();
  OrbitPoint   *startOp = getOrbitPoints(), *op = startOp;
  const CPoint  p0(toCPoint(a,b));
  if(isEdgeTracing()) mbc.paintMark(p0);
  for(; count < maxIteration; count++) {
    const CPoint p(toCPoint(a,b));
    *(op++) = OrbitPoint(p, mbc.getPixel(p.x,p.y));
    mbc.setPixel(p.x,p.y, RGB(0,0,255));
    const BigReal a2 = rProd(a,a,m_digits);
    const BigReal b2 = rProd(b,b,m_digits);
    if(a2+b2 > _4) {
      break;
    }
    const BigReal c = a2-b2+X;
    b = rProd(a*_2,b,m_digits)+Y;
    a = c;
  }
  if(isEdgeTracing()) mbc.paintMark(p0);
  while(--op >= startOp) {
    mbc.setPixel(op->x,op->y,op->m_oldColor);
  }
  return count;
}

UINT MBBigRealCalculator::run() {
  setPoolState(CALC_RUNNING);
  MBContainer                          &mbc              =  getMBContainer();
  const UINT                            maxIteration     =  mbc.getMaxIteration();
  const bool                            useEdgeDetection =  mbc.useEdgeDetection();
  PixelAccessor                        *pa               =  mbc.getPixelAccessor();
  const D3DCOLOR                       *colorMap         =  mbc.getColorMap();
  m_digits                                               =  mbc.getDigits();
  const BigRealRectangleTransformation  tr(mbc.getBigRealTransformation(), getDigitPool());
  m_xtr                                                  =  &tr.getXTransformation();
  m_ytr                                                  =  &tr.getYTransformation();

  SETPHASE(_T("RUN"))

  try {
    BigReal xt(getDigitPool()), yt(getDigitPool());
    while(mbc.getJobToDo(m_currentRect)) {

//      DLOG(_T("calc(%d) got rect (%d,%d,%d,%d)\n"), getId(), m_currentRect.left,m_currentRect.top,m_currentRect.right,m_currentRect.bottom);

      enableEdgeTracing(false);
      CPoint p;
      for(p.y = m_currentRect.top; p.y < m_currentRect.bottom; p.y++) {
        CHECKPENDING();
        yt = m_ytr->backwardTransform(BigReal(p.y,getDigitPool()));
        for(p.x = m_currentRect.left; p.x < m_currentRect.right; p.x++) {
          if(pa->getPixel(p) != EMPTY_COLOR) continue;
          xt = m_xtr->backwardTransform(BigReal(p.x,getDigitPool()));
          const UINT iterations = findItCount(xt, yt, maxIteration);

          if((iterations == maxIteration) && useEdgeDetection) {
//            DLOG(_T("calc(%d) found black point (%d,%d)\n"), getId(), p.x,p.y);
            enableEdgeTracing(true);
            pa = followBlackEdge(p, pa, maxIteration);
            SETPHASE(_T("RUN"));
            enableEdgeTracing(false);
          } else {
            pa->setPixel(p, colorMap[iterations]); m_doneCount++;
          }
        }
      }
    }
    DLOG(_T("calc(%d) done\n"), getId());
  } catch(bool) {
#ifdef SAVE_CALCULATORINFO
    DLOG(_T("calc(%d) killed in phase %s\n"), getId(), m_phase);
#else
    DLOG(_T("calc(%d) killed\n"), getId());
#endif
  } catch(Exception e) {
    DLOG(_T("calc(%d) caught Exception:%s\n"), getId(), e.what());
  } catch(...) {
    DLOG(_T("calc(%d) caught unknown Exception\n"), getId());
  }
  setPoolState(CALC_TERMINATED);
  return 0;
}

PixelAccessor *MBBigRealCalculator::followBlackEdge(const CPoint &p, PixelAccessor *pa, UINT maxIteration) {
  if(!enterFollowBlackEdge(p)) return pa;
  MBContainer       &mbc           =  getMBContainer();
  const CSize        sz            =  mbc.getWindowSize();
  const CRect        rect(m_currentRect.left,m_currentRect.top, sz.cx, sz.cy);
  const D3DCOLOR    *colorMap      =  mbc.getColorMap();
  CPoint             q             =  p;
  Direction          dir           =  S, firstDir = NODIR;
  int                edgeCount     =  1; // p assumed to be set to black
  bool               innerSetEmpty =  true;
  PointSet           edgeSet(rect), innerSet(rect);
  edgeSet.add(p);
  pa->setPixel(p, colorMap[maxIteration]); m_doneCount++;

  SETPHASE(_T("FOLLOWEDGE"))

#ifdef SAVE_CALCULATORINFO
  m_info = new CalculatorInfo(getId(), rect);
#endif
//  DLOG(_T("Follow black edge starting at (%d,%d)\n"), p.x,p.y);

  EdgeMatrix edgeMatrix;
  BigReal xt(getDigitPool()), yt(getDigitPool());
  for(;;) {
    for(int dy = -1; dy <= 1; dy++) {
      const int qy = q.y + dy;
      if(qy < rect.top) {
        edgeMatrix.setRowOutside(0);
        continue;
      } else if(qy >= rect.bottom) {
        edgeMatrix.setRowOutside(2);
        continue;
      }
      for(int dx = -1; dx <= 1; dx++) {
        if(!edgeMatrix.isDirty(dy+1, dx+1)) {
          continue;
        }
        const int qx = q.x+dx;
        if((qx < rect.left) || (qx >= rect.right)) {
          edgeMatrix.setOutside(dy+1, dx+1);
        } else {
          const D3DCOLOR c = pa->getPixel(qx, qy);
          if(c == BLACK) {
            edgeMatrix.setInside(dy+1, dx+1);
          } else if(c != EMPTY_COLOR) {
            edgeMatrix.setOutside(dy+1, dx+1);
          } else {
            xt = m_xtr->backwardTransform(BigReal(qx, getDigitPool()));
            yt = m_ytr->backwardTransform(BigReal(qy, getDigitPool()));
            const UINT iterations = findItCount(xt, yt, maxIteration);
            if(iterations == maxIteration) {
              edgeMatrix.setInside(dy+1, dx+1);
            } else {
              edgeMatrix.setOutside(dy+1, dx+1);
              pa->setPixel(qx,qy, colorMap[iterations]); m_doneCount++;
            }
          }
        }
      }
    }
    if((dir = edgeMatrix.findStepDirection(dir)) == NODIR) {
      if(!innerSetEmpty) {
        DLOG(_T("dir == NODIR and has innerpoints\n"));
        innerSet -= edgeSet;
      }
#ifdef SAVE_CALCULATORINFO
      m_info->setEdgeAndInnerSet(edgeSet, innerSet);
      addInfoToPool();
#endif
      goto Return;
    } else if(firstDir == NODIR) {
      firstDir = dir;
    } else if((q == p) && dir == firstDir) {
      break;
    }

    if(edgeMatrix.getLeftAttr(dir)) {
      innerSet.add(q + EdgeMatrix::s_leftStep[dir]);
      innerSetEmpty = false;
      CHECKPENDING();
    }
    q += EdgeMatrix::s_dirStep[dir];
    if(!edgeSet.contains(q)) {
      pa->setPixel(q, BLACK); m_doneCount++;
      edgeSet.add(q);
      edgeCount++;
    }
    edgeMatrix.adjustAttributes(dir);
  }

  bool edgeIsSubtracted = false;
  if((edgeCount >= 8) && !innerSetEmpty) {
    innerSet -= edgeSet; edgeIsSubtracted = true;
    pa = fillInnerArea(innerSet, pa);
  }

#ifdef SAVE_CALCULATORINFO
  if(!edgeIsSubtracted) {
    innerSet -= edgeSet; edgeIsSubtracted = true;
  }
  m_info->setEdgeAndInnerSet(edgeSet, innerSet);
  addInfoToPool();
#endif
Return:
  leaveFollowBlackEdge();
  return pa;
}
