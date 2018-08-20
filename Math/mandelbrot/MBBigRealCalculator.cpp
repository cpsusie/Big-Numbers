#include "stdafx.h"
#include "MBBigRealCalculator.h"

UINT MBBigRealCalculator::findCountFast(const BigReal &X, const BigReal &Y, UINT maxCount) {
  UINT          count = 0;
  BigReal       a     = X;
  BigReal       b     = Y;
  const BigInt &_2 = getDigitPool()->get2();
  for(;count < maxCount; count++) {
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

UINT MBBigRealCalculator::findCountPaintOrbit(const BigReal &X, const BigReal &Y, UINT maxCount) {
  MBContainer  &mbc   = getMBContainer();
  UINT          count = 0;
  BigReal       a     = X;
  BigReal       b     = Y;
  const BigInt &_2 = getDigitPool()->get2();
  OrbitPoint   *startOp = getOrbitPoints(), *op = startOp;
  const CPoint  p0(toCPoint(a,b));
  if(isEdgeTracing()) mbc.paintMark(p0);
  for(; count < maxCount; count++) {
    const CPoint p(toCPoint(a,b));
    if(m_currentRect.PtInRect(p)) {
      *(op++) = OrbitPoint(p, mbc.setOrbitPixel(p, ORBITCOLOR));
    }
    const BigReal a2 = rProd(a,a,m_digits);
    const BigReal b2 = rProd(b,b,m_digits);
    if(a2+b2 > _4) {
      break;
    }
    const BigReal c = a2-b2+X;
    b = rProd(a*_2,b,m_digits)+Y;
    a = c;
  }
  mbc.resetOrbitPixels(startOp, op-startOp);
  if(isEdgeTracing()) mbc.paintMark(p0);
  return count;
}

UINT MBBigRealCalculator::run() {
  setPoolState(CALC_RUNNING);
  MBContainer                          &mbc             =  getMBContainer();
  const UINT                           maxCount         =  mbc.getMaxCount();
  const bool                           useEdgeDetection =  mbc.useEdgeDetection();
  m_digits                                              =  mbc.getDigits();
  CellCountAccessor                   *cca              =  mbc.getCCA();
  const BigRealRectangleTransformation tr(mbc.getBigRealTransformation(), getDigitPool());
  m_xtr                                                 =  &tr.getXTransformation();
  m_ytr                                                 =  &tr.getYTransformation();

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
          if(!cca->isEmptyCell(p)) continue;
          xt = m_xtr->backwardTransform(BigReal(p.x,getDigitPool()));
          const UINT count = findCount(xt, yt, maxCount);

          if((count == maxCount) && useEdgeDetection) {
//            DLOG(_T("calc(%d) found black point (%d,%d)\n"), getId(), p.x,p.y);
            enableEdgeTracing(true);
            cca = followBlackEdge(p, cca, maxCount);
            SETPHASE(_T("RUN"));
            enableEdgeTracing(false);
          } else {
            cca->setCount(p, count); m_doneCount++;
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

CellCountAccessor *MBBigRealCalculator::followBlackEdge(const CPoint &p, CellCountAccessor *cca, UINT maxCount) {
  if(!enterFollowBlackEdge(p, cca)) return cca;
  MBContainer       &mbc           =  getMBContainer();
  const CSize        sz            =  mbc.getWindowSize();
  const CRect        rect(m_currentRect.left,m_currentRect.top, sz.cx, sz.cy);
  CPoint             q             =  p;
  Direction          dir           =  S, firstDir = NODIR;
  int                edgeCount     =  1; // p assumed to be set to black
  bool               innerSetEmpty =  true;
  PointSet           edgeSet(rect), innerSet(rect);
  edgeSet.add(p);
  cca->setCount(p, maxCount); m_doneCount++;

  SETPHASE(_T("FOLLOWEDGE"))

#ifdef SAVE_CALCULATORINFO
  m_info = new CalculatorInfo(getId(), rect);
#endif
//  DLOG(_T("Follow black edge starting at (%d,%d)\n"), p.x,p.y);

  EdgeMatrix edgeMatrix;
  BigReal    xt(getDigitPool()), yt(getDigitPool());
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
          const UINT c = cca->getCount(qx, qy);
          if(c == maxCount) {
            edgeMatrix.setInside(dy+1, dx+1);
          } else if(c != EMPTYCELLVALUE) {
            edgeMatrix.setOutside(dy+1, dx+1);
          } else {
            xt = m_xtr->backwardTransform(BigReal(qx, getDigitPool()));
            yt = m_ytr->backwardTransform(BigReal(qy, getDigitPool()));
            const UINT count = findCount(xt, yt, maxCount);
            if(count == maxCount) {
              edgeMatrix.setInside(dy+1, dx+1);
            } else {
              edgeMatrix.setOutside(dy+1, dx+1);
              cca->setCount(qx,qy, count); m_doneCount++;
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
      cca->setCount(q, maxCount); m_doneCount++;
      edgeSet.add(q);
      edgeCount++;
    }
    edgeMatrix.adjustAttributes(dir);
  }

  bool edgeIsSubtracted = false;
  if((edgeCount >= 8) && !innerSetEmpty) {
    innerSet -= edgeSet; edgeIsSubtracted = true;
    cca = fillInnerArea(innerSet, cca, maxCount);
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
  return cca;
}
