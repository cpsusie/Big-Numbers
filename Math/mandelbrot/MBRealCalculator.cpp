#include "stdafx.h"
#include "MBRealCalculator.h"

#pragma check_stack(off)

#define ASMOPTIMIZED

#ifdef ASMOPTIMIZED
#ifdef IS64BIT
extern "C" UINT64 mbloop(const Double80 &x, const Double80 &y, UINT64 maxCount);
#endif
#endif

UINT MBRealCalculator::findCountFast(const Real &X, const Real &Y, UINT maxCount) {
#ifndef ASMOPTIMIZED
  UINT count = 0;
  Real a     = X;
  Real b     = Y;
  for(; count < maxCount; count++) {
    const Real a2 = a*a;
    const Real b2 = b*b;
    if(a2+b2 > 4) {
      break;
    }
    const Real c = a2-b2+X;
    b = 2*a*b+Y;
    a = c;
  }
  return count;

#else

#ifdef IS64BIT
  return (UINT)mbloop(X,Y, maxCount);
#else // IS32BIT
  UINT     count;
  Double80 x     = X;
  Double80 y     = Y;

  static const float _4 = 4;
  unsigned short cw;

/*
  unsigned short sw;
  unsigned short tagsBuffer[14];
  unsigned short stackSize;
*/
  __asm {                                                   // Registers after instruction has been executed
    mov    ecx  , maxCount                                  // st0        st1        st2        st3        st4        st5        st6        st7
    fld    _4           // Load 4                              4
    fld    y            // Load y                              y          4
    fld    x            // Load x                              x          y          4
    fld    st(1)        // Load y                              b=y        x          y          4
    fld    st(1)        // Load x                              a=x        b          x          y          4

forloop:                // Stacksize = 5                       a          b          x          y          4
    fld	   st(0)        // Load a.                             a          a          b          x          y           4
    fmul   st(0), st(0) // st0*=st0                            a^2        a          b          x          y           4
    fld    st(2)        // Load b                              b          a^2        a          b          x           y         4
    fmul   st(0), st(0) // st0*=st0                            b^2        a^2        a          b          x           y         4
    fld    st(1)        // Load a^2.                           a^2        b^2        a^2        a          b           x         y          4
    fadd   st(0), st(1) // st0 += st1                          a^2+b^2    b^2        a^2        a          b           x         y          4
    fcomip st(0), st(7) // Compare st0 and st7, pop st0        b^2        a^2        a          b          x           y         4
    ja return           // if(a^2+b^2 > 4) goto return

    fsub                // st0 = a^2-b^2, pop st1              a^2-b^2    a          b          x          y           4
    fadd   st(0), st(3) // st0 += x                            a^2-b^2+x  a          b          x          y           4
    fld    st(1)        // Load a                              a          a^2-b^2+x  a          b          x           y         4
    fmul   st(0), st(3) // st0 *= b                            ab         a^2-b^2+x  a          b          x           y         4
    fadd   st(0), st(0) // st0 *= 2                            2ab        a^2-b^2+x  a          b          x           y         4
    fadd   st(0), st(5) // st0 += y                            2ab+y      a^2-b^2+x  a          b          x           y         4
    fstp   st(3)        // b = 2ab+y, pop st0                  a^2-b^2+x  a          new b      x          y           4
    fstp   st(1)        // a = a^2-b^2+x, pop st0              new a      new b      x          y          4
    loop   forloop      // Stacksize = 5. if(--ecx) goto forloop

/*
getStackSize:
    fnstsw sw
    mov ax,sw
    shr ax,11
    and ax,7
    cmp ax,0
    jnz normalHeight
    fstenv tagsBuffer
    mov ax, WORD PTR tagsBuffer[4]
    cmp ax, 0xffff
    je stackSizeZero
    mov stackSize,8
    ret
stackSizeZero:
    mov stackSize,0
    ret
normalHeight:
    neg ax
    add ax, 8
    mov stackSize, ax
    ret
*/
return:
    fnstcw cw
    fninit
    fldcw  cw
    sub    ecx, maxCount
    neg    ecx
    mov    count,ecx
  }
  return count;
#endif // IS32BIT

#endif // ASMOPTIMIZED
}

UINT MBRealCalculator::findCountPaintOrbit(const Real &X, const Real &Y, UINT maxCount) {
  MBContainer  &mbc     = getMBContainer();
  UINT          count   = 0;
  Real          a       = X;
  Real          b       = Y;
  OrbitPoint   *startOp = getOrbitPoints(), *op = startOp;
  const CPoint  p0(toCPoint(a,b));
  if(isEdgeTracing()) mbc.paintMark(p0);
  for(; count < maxCount; count++) {
    const CPoint p(toCPoint(a,b));
    if(m_currentRect.PtInRect(p)) {
      *(op++) = OrbitPoint(p, mbc.setOrbitPixel(p, ORBITCOLOR));
    }
    const Real a2 = a*a;
    const Real b2 = b*b;
    if(a2+b2 > 4) {
      break;
    }
    const Real c = a2-b2+X;
    b = 2*a*b+Y;
    a = c;
  }
  if(isEdgeTracing()) mbc.paintMark(p0);
  mbc.resetOrbitPixels(startOp, op-startOp);
  return count;
}

UINT MBRealCalculator::run() {
  setPoolState(CALC_RUNNING);
  MBContainer                       &mbc              =  getMBContainer();
  const UINT                         maxCount         =  mbc.getMaxCount();
  const bool                         useEdgeDetection =  mbc.useEdgeDetection();
  CellCountAccessor                 *cca              =  mbc.getCCA();
  const RealRectangleTransformation &tr               =  mbc.getRealTransformation();
  m_xtr                                               = &tr.getXTransformation();
  m_ytr                                               = &tr.getYTransformation();

  SETPHASE(_T("RUN"))

  FPU::setPrecisionMode(mbc.getPrecisionMode());
  try {
    Real xt, yt;
    while(mbc.getJobToDo(m_currentRect)) {

//      DLOG(_T("calc(%d) got rect (%d,%d,%d,%d)\n"), getId(), m_currentRect.left,m_currentRect.top,m_currentRect.right,m_currentRect.bottom);

      enableEdgeTracing(false);
      CPoint p;
      for(p.y = m_currentRect.top; p.y < m_currentRect.bottom; p.y++) {
        CHECKPENDING();
        yt = m_ytr->backwardTransform(p.y);
        for(p.x = m_currentRect.left; p.x < m_currentRect.right; p.x++) {
          if(!cca->isEmptyCell(p)) continue;
          xt = m_xtr->backwardTransform(p.x);
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

CellCountAccessor *MBRealCalculator::followBlackEdge(const CPoint &p, CellCountAccessor *cca, UINT maxCount) {
  if(!enterFollowBlackEdge(p)) return cca;
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
  Real       xt, yt;
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
            xt = m_xtr->backwardTransform(qx);
            yt = m_ytr->backwardTransform(qy);
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
