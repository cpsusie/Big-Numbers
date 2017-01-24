#include "stdafx.h"
#include "MBCalculator.h"
#include "EdgeMatrix.h"

#pragma check_stack(off)

#define ASMOPTIMIZED

UINT MBCalculator::findITCountFast(const Real &X, const Real &Y, UINT maxIteration) {
#ifndef ASMOPTIMIZED
  Double80 x = X;
  Double80 y = Y;
  Double80 a = x;
  Double80 b = y;;
  for(int count = 0; count < maxIteration; count++) {
    Double80 a2 = a*a;
    Double80 b2 = b*b;
    if(a2+b2 > 4) {
      return count;
    }
    Double80 c = a2-b2+x;
    b = 2*a*b+y;
    a = c;
  }
  return count;

#else

  Double80 x = X;
  Double80 y = Y;

  static const float four = 4;
  int count;
  unsigned short cw;

/*
  unsigned short sw;
  unsigned short tagsBuffer[14];
  unsigned short stackSize;
*/
  __asm {                                                   // Registers after instruction has been executed
    mov    ecx  , maxIteration                              // st0        st1        st2        st3        st4        st5        st6        st7
    fld    four         // Load 4                              4
    fld    y            // Load y                              y          4
    fld    x            // Load x                              x          y          4
    fld    st(1)        // Load y                              b=y        x          y          4
    fld    st(1)        // Load x                              a=x        b          x          y          4

forloop:                // Stacksize = 5                       a          b          x          y          4
    fld	   st(0)        // Load a.                             a          a          b          x          y           4   
    fmul   st(0), st(0) // st0*=st0                            a*a        a          b          x          y           4
    fld    st(2)        // Load b                              b          a*a        a          b          x           y         4
    fmul   st(0), st(0) // st0*=st0                            b*b        a*a        a          b          x           y         4
    fld    st(1)        // Load b^2.                           b*b        b*b        a*a        a          b           x         y          4
    fadd   st(0), st(1) // st(0) = a^2 + b^2.                  a*a+b*b    b*b        a*a        a          b           x         y          4
    fcomip st(0), st(7) // Compare st0 and st7, pop st0        b*b        a*a        a          b          x           y         4
    ja return           // if(a*a+b*b > 4) goto return

    fsub                // st0 = a*a-b*b, pop st1              a*a-b*b    a          b          x          y           4
    fadd   st(0), st(3) // st0 += x                            a*a-b*b+x  a          b          x          y           4
    fld    st(1)        // Load a                              a          a*a-b*b+x  a          b          x           y         4
    fmul   st(0), st(3) // st0 *= b                            a*b        a*a-b*b+x  a          b          x           y         4
    fadd   st(0), st(0) // st0 *= 2                            2*a*b      a*a-b*b+x  a          b          x           y         4
    fadd   st(0), st(5) // st0 += y                            2*a*b+y    a*a-b*b+x  a          b          x           y         4
    fstp   st(3)        // b = 2*a*b+y, pop st0                a*a-b*b+x  a          new b      x          y           4
    fstp   st(1)        // a = a*a-b*b+x, pop st0              new a      new b      x          y          4
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
    sub    ecx, maxIteration
    neg    ecx
    mov    count,ecx
  }
  return count;

#endif
}

UINT MBCalculator::findITCountPaintOrbit(const Real &X, const Real &Y, UINT maxIteration) {
  double x = getDouble(X);
  double y = getDouble(Y);
  double a = x;
  double b = y;;
  const RealRectangleTransformation &tr = m_mbc.getTransformation();
  OrbitPoint                        *op = m_orbitPoints;
  const CPoint p0 = tr.forwardTransform(a,b);
  if(m_edgeTracing) m_mbc.paintMark(p0);
  UINT count;
  for(count = 0; count < maxIteration; count++) {
    const CPoint p = tr.forwardTransform(a,b);
    *(op++) = OrbitPoint(p, m_mbc.getPixel(p));
    m_mbc.setPixel(p, RGB(0,0,255));
    const double a2 = a*a;
    const double b2 = b*b;
    if(a2+b2 > 4) {
      break;
    }
    const double c = a2-b2+x;
    b = 2*a*b+y;
    a = c;
  }
  if(m_edgeTracing) m_mbc.paintMark(p0);
  while(--op >= m_orbitPoints) {
    m_mbc.setPixel(*op,op->m_oldColor);
  }
  return count;
}

MBCalculator::MBCalculator(CalculatorPool *pool, int id)
: Thread(format(_T("MBCalc(%d)")))
, m_pool(*pool)
, m_id(id)
, m_mbc(pool->getMBContainer())
, m_wakeup(0)
, m_pendingMask(pool->getPendingMask(id))
{
  setDeamon(true);
  if(m_mbc.paintOrbit()) {
    m_itCount      = &MBCalculator::findITCountPaintOrbit;
    m_orbitPoints  = new OrbitPoint[m_mbc.getMaxIteration()];
  } else {
    m_itCount      = &MBCalculator::findITCountFast;
    m_orbitPoints  =  NULL;
  }
#ifdef SAVE_CALCULATORINFO
  m_info = NULL;
#endif
}

void MBCalculator::releaseOrbitPoints() {
  m_gate.wait();
  if(m_orbitPoints) {
    delete[] m_orbitPoints;
    m_orbitPoints = NULL;
  }
  m_gate.signal();
}

PixelAccessor *MBCalculator::handlePending() {
  UINT pendingFlags;
  while(pendingFlags = m_pool.getPendingFlags(m_id)) {
    if(pendingFlags & CALC_KILL_PENDING) {
      throw true;
    }
    if(pendingFlags & CALC_SUSPEND_PENDING) {
      DLOG((_T("calc(%d) suspending\n"), m_id));
      m_pool.setState(m_id, CALC_SUSPENDED);
      m_wakeup.wait();
      m_pool.setState(m_id, CALC_RUNNING  );
      DLOG((_T("calc(%d) resuming\n"), m_id));
    }
  }
  return m_mbc.getPixelAccessor();
}

UINT MBCalculator::run() {
  m_pool.setState(m_id, CALC_RUNNING);
  const UINT                         maxIteration     = m_mbc.getMaxIteration();
  const bool                         useEdgeDetection = m_mbc.useEdgeDetection();
  PixelAccessor                     &pa               = *m_mbc.getPixelAccessor();
  const D3DCOLOR                    *colorMap         = m_mbc.getColorMap();
  const RealRectangleTransformation &tr               = m_mbc.getTransformation();
  const RealIntervalTransformation  &xtr              = tr.getXTransformation();
  const RealIntervalTransformation  &ytr              = tr.getYTransformation();
  const Real                         xStep            = xtr.backwardTransform(1) - xtr.backwardTransform(0);
  const Real                         yStep            = ytr.backwardTransform(1) - ytr.backwardTransform(0);

  SETPHASE(_T("RUN"))

  FPU::setPrecisionMode(m_mbc.getPrecisionMode());
  try {
    while(m_mbc.getJobToDo(m_currentRect)) {

//      DLOG((_T("calc(%d) got rect (%d,%d,%d,%d)\n"), m_id, m_currentRect.left,m_currentRect.top,m_currentRect.right,m_currentRect.bottom));

      m_edgeTracing = false;
      Real        xt, yt = ytr.backwardTransform(m_currentRect.top);
      const Real  xLeft  = xtr.backwardTransform(m_currentRect.left);
      CPoint      p;
      D3DCOLOR    emptyColor = EMPTY_COLOR;

      for(p.y = m_currentRect.top; p.y < m_currentRect.bottom; p.y++, yt += yStep) {
        if(m_pool.isPending(m_pendingMask)) handlePending();
        for(p.x = m_currentRect.left, xt = xLeft; p.x < m_currentRect.right; p.x++, xt += xStep) {
          if(pa.getPixel(p) != EMPTY_COLOR) continue;
          const Real xt         = xtr.backwardTransform(p.x);
          const int  iterations = findItCount(xt, yt, maxIteration);
          pa.setPixel(p, colorMap[iterations]);

          if((iterations == maxIteration) && useEdgeDetection) {
//            DLOG((_T("calc(%d) found black point (%d,%d)\n"), m_id, p.x,p.y));
            m_edgeTracing = true;
            followBlackEdge(p);
            SETPHASE(_T("RUN"));
            m_edgeTracing = false;
          }
        }
      }
    }
    DLOG((_T("calc(%d) done\n"), m_id));
  } catch(bool) {
#ifdef SAVE_CALCULATORINFO
    DLOG((_T("calc(%d) killed in phase %s\n"), m_id, m_phase));
#else
    DLOG((_T("calc(%d) killed\n"), m_id));
#endif
  } catch(Exception e) {
    DLOG((_T("calc(%d) caught Exception:%s\n"), m_id, e.what()));
  } catch(...) {
    DLOG((_T("calc(%d) caught unknown Exception\n"), m_id));
  }
  m_pool.setState(m_id, CALC_TERMINATED);
  return 0;
}

void MBCalculator::followBlackEdge(const CPoint &p) {
  PixelAccessor                  &pa       = *m_mbc.getPixelAccessor();
  const D3DCOLOR                 *colorMap =  m_mbc.getColorMap();
  CPoint                          q        = p;
  Direction                       dir      = S, firstDir = NODIR;
  PointSet                        edgeSet(m_currentRect), innerSet(m_currentRect);
  edgeSet.add(p);
  int                             edgeCount     = 1; // p assumed to be set to black
  bool                            innerDetected = false;

  SETPHASE(_T("FOLLOWEDGE"))

  const RealRectangleTransformation&tr    = m_mbc.getTransformation();
  const RealIntervalTransformation &xtr   = tr.getXTransformation();
  const RealIntervalTransformation &ytr   = tr.getYTransformation();
  const int                         maxIt = m_mbc.getMaxIteration();

#ifdef SAVE_CALCULATORINFO
  m_info = new CalculatorInfo(m_id, m_currentRect);
#endif
//  DLOG((_T("Follow black edge starting at (%d,%d)\n"), p.x,p.y));

  EdgeMatrix                      edgeMatrix;
  for(;;) {
    for(int dy = -1; dy <= 1; dy++) {
      const int qy = q.y + dy;
      if(qy < m_currentRect.top) {
        edgeMatrix.setRowOutside(0);
        continue;
      } else if(qy >= m_currentRect.bottom) {
        edgeMatrix.setRowOutside(2);
        continue;
      }
      for(int dx = -1; dx <= 1; dx++) {
        if(!edgeMatrix.isDirty(dy+1, dx+1)) {
          continue;
        }
        const int qx = q.x+dx;
        if((qx < m_currentRect.left) || (qx >= m_currentRect.right)) {
          edgeMatrix.setOutside(dy+1, dx+1);
        } else {
          const D3DCOLOR c = pa.getPixel(qx, qy);
          if(c == BLACK) {
            edgeMatrix.setInside(dy+1, dx+1);
          } else if(c != EMPTY_COLOR) {
            edgeMatrix.setOutside(dy+1, dx+1);
          } else {
            const Real X          = xtr.backwardTransform(qx);
            const Real Y          = ytr.backwardTransform(qy);
            const int  iterations = findItCount(X, Y, maxIt);
            if(iterations == maxIt) {
              edgeMatrix.setInside(dy+1, dx+1);
            } else {
              edgeMatrix.setOutside(dy+1, dx+1);
              pa.setPixel(qx,qy, colorMap[iterations]);
            }
          }
        }
      }
    }
    if((dir = edgeMatrix.findStepDirection(dir)) == NODIR) {
      if(innerDetected) {
        DLOG((_T("dir == NODIR and has innerpoints\n")));
        innerSet -= edgeSet;
      }
#ifdef SAVE_CALCULATORINFO
      m_info->setEdgeAndInnerSet(edgeSet, innerSet);
      addInfoToPool();
#endif
      return;
    } else if(firstDir == NODIR) {
      firstDir = dir;
    } else if((q == p) && dir == firstDir) {
      break;
    }

    if(edgeMatrix.getLeftAttr(dir)) {
      innerSet.add(q + EdgeMatrix::leftStep[dir]);
      innerDetected = true;
      if(m_pool.isPending(m_pendingMask)) handlePending();
    }
    q += EdgeMatrix::dirStep[dir];
    if(!edgeSet.contains(q)) {
      pa.setPixel(q, BLACK);
      edgeSet.add(q);
      edgeCount++;
    }

    edgeMatrix.adjustAttributes(dir);
  }

  bool edgeIsSubtracted = false;
  if((edgeCount >= 8) && innerDetected) {
    innerSet -= edgeSet; edgeIsSubtracted = true;
    fillInnerArea(innerSet);
  }

#ifdef SAVE_CALCULATORINFO
  if(!edgeIsSubtracted) {
    innerSet -= edgeSet; edgeIsSubtracted = true;
  }
  m_info->setEdgeAndInnerSet(edgeSet, innerSet);
  addInfoToPool();
#endif
}

#ifdef SAVE_CALCULATORINFO

void MBCalculator::addInfoToPool() {
  m_info->addEdgeSetToBlackSet();
  m_pool.addCalculatorInfo(*m_info);  delete m_info;  m_info = NULL;
}

#define ADDPPTOINFO m_info->addBlack(pp)
#else
#define ADDPPTOINFO
#endif


#define ISCOLORTOFILL(c) ((c) == EMPTY_COLOR)
#define FILLCOLOR        FILL_COLOR

#define CHECKPIXELPP                          \
{ const D3DCOLOR c = pa.getPixel(pp);         \
  if(!ISCOLORTOFILL(c)) break;                \
  pa.setPixel(pp, FILLCOLOR);                 \
  ADDPPTOINFO;                                \
}

#define CHECKNEIGHBOURPIXEL(i)                \
{ const D3DCOLOR c = pa.getPixel(pp.x, y##i); \
  if(!stacked##i) {                           \
    if(ISCOLORTOFILL(c)) {                    \
      stack.push(CPoint(pp.x, y##i));         \
      stacked##i = true;                      \
    }                                         \
  } else if(!ISCOLORTOFILL(c)) {              \
    stacked##i = false;                       \
  }                                           \
}

void MBCalculator::fillInnerArea(PointSet &innerSet) {
  PixelAccessor &pa = *m_mbc.getPixelAccessor();

  SETPHASE(_T("FILLINNERAREA"))

  for(Iterator<size_t> it = innerSet.getIterator(); it.hasNext();) {
    const CPoint start = innerSet.next(it);
    if(!ISCOLORTOFILL(pa.getPixel(start))) {
      continue;
    }

    CompactStack<CPoint> stack;
    stack.push(start);
    while(!stack.isEmpty()) {
      if(m_pool.isPending(m_pendingMask)) handlePending();
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
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {      // go right
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
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {      // go right
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
        for(CPoint pp = np; ++pp.x < m_currentRect.right;) {        // go right
          CHECKPIXELPP
          CHECKNEIGHBOURPIXEL(2)
        }
      }
    }
  }
}
