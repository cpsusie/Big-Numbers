#include "stdafx.h"
#include "MBCalculator.h"

MBCalculator::MBCalculator(CalculatorPool *pool, int id)
: Thread(format(_T("MBCalc(%d)"),id))
, m_pool(*pool)
, m_id(id)
, m_mbc(pool->getMBContainer())
, m_wakeup(0)
, m_pendingMask(pool->getPendingMask(id))
, m_orbitPoints(NULL)
{
  setDeamon(true);
  if(m_mbc.calculateWithOrbit()) {
    m_orbitPoints  = new OrbitPoint[m_mbc.getMaxIteration()]; TRACE_NEW(m_orbitPoints);
  }
#ifdef SAVE_CALCULATORINFO
  m_info = NULL;
#endif
}

void MBCalculator::releaseOrbitPoints() {
  m_gate.wait();
  SAFEDELETEARRAY(m_orbitPoints);
  m_gate.signal();
}

bool MBCalculator::isPending() const {
  return m_pool.isPending(m_pendingMask);
}

void MBCalculator::setPoolState(CalculatorState state) {
  m_pool.setState(m_id, state);
}

PixelAccessor *MBCalculator::handlePending() {
  UINT pendingFlags;
  while(pendingFlags = m_pool.getPendingFlags(m_id)) {
    if(pendingFlags & CALC_KILL_PENDING) {
      throw true;
    }
    if(pendingFlags & CALC_SUSPEND_PENDING) {
      DLOG(_T("calc(%d) suspending\n"), m_id);
      m_pool.setState(m_id, CALC_SUSPENDED);
      m_wakeup.wait();
      m_pool.setState(m_id, CALC_RUNNING  );
      DLOG(_T("calc(%d) resuming\n"), m_id);
    }
  }
  return m_mbc.getPixelAccessor();
}

Semaphore MBCalculator::s_followBlackEdgeGate;

bool MBCalculator::enterFollowBlackEdge(const CPoint &p) {
  s_followBlackEdgeGate.wait();
  if(m_mbc.getPixelAccessor()->getPixel(p) != EMPTY_COLOR) {
    s_followBlackEdgeGate.signal();
    return false;
  }
  return true;
}

void MBCalculator::leaveFollowBlackEdge() {
  s_followBlackEdgeGate.signal();
}

#ifdef SAVE_CALCULATORINFO

void MBCalculator::addInfoToPool() {
  m_info->addEdgeSetToBlackSet();
  m_pool.addCalculatorInfo(*m_info);  SAFEDELETE(m_info);
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

  for(Iterator<CPoint> it = innerSet.getIterator(); it.hasNext();) {
    const CPoint start = it.next();
    if(!ISCOLORTOFILL(pa.getPixel(start))) {
      continue;
    }

    CompactStack<CPoint> stack;
    stack.push(start);
    while(!stack.isEmpty()) {
      if(isPending()) handlePending();
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
