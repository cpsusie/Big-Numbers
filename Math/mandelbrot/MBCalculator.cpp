#include "stdafx.h"
#include "MBCalculator.h"

MBCalculator::MBCalculator(CalculatorPool *pool, UINT id)
: Thread(format(_T("MBCalc(%d)"),id))
, m_pool(*pool)
, m_id(id)
, m_mbc(pool->getMBContainer())
, m_wakeup(0)
, m_pendingMask(pool->getPendingMask(id))
, m_orbitPoints(NULL)
, m_doneCount(0)
, m_startTime(0)
, m_threadTime(0)
{
  setDeamon(true);
  setWithOrbit();
#ifdef SAVE_CALCULATORINFO
  m_info = NULL;
#endif
}

void MBCalculator::setPoolState(CalculatorState state) {
  m_pool.setState(m_id, state);
}

void MBCalculator::setWithOrbit() {
  const bool enableOrbit = m_mbc.calculateWithOrbit();
  if(enableOrbit != isWithOrbit()) {
    if(enableOrbit) {
      allocateOrbitPoints();
    } else {
      releaseOrbitPoints();
    }
  }
}

// assume thread is suspended
void MBCalculator::allocateOrbitPoints() {
  releaseOrbitPoints();
  const UINT maxCount = m_mbc.getMaxCount();
  m_orbitPoints  = new OrbitPoint[maxCount]; TRACE_NEW(m_orbitPoints);
}

// assume thread is suspended
void MBCalculator::releaseOrbitPoints() {
  m_gate.wait();
  SAFEDELETEARRAY(m_orbitPoints);
  m_gate.signal();
}

CellCountAccessor *MBCalculator::handlePending() {
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
  setWithOrbit();
  return m_mbc.getCCA();
}

Semaphore MBCalculator::s_followBlackEdgeGate;

bool MBCalculator::enterFollowBlackEdge(const CPoint &p, CellCountAccessor *cca) {
  m_pool.setState(m_id, CALC_SUSPENDED);
  s_followBlackEdgeGate.wait();
  m_pool.setState(m_id, CALC_RUNNING  );
  if(!cca->isEmptyCell(p)) {
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


#define CHECKCOUNTPP                          \
{ if(!cca->isEmptyCell(pp)) break;            \
  cca->setCount(pp, maxCount); m_doneCount++; \
  ADDPPTOINFO;                                \
}

#define CHECKNEIGHBOURCOUNT(i)                \
{ const UINT c = cca->getCount(pp.x, y##i);   \
  if(!stacked##i) {                           \
    if(c == EMPTYCELLVALUE) {                 \
      stack.push(CPoint(pp.x, y##i));         \
      stacked##i = true;                      \
    }                                         \
  } else if(c != EMPTYCELLVALUE) {            \
    stacked##i = false;                       \
  }                                           \
}

CellCountAccessor *MBCalculator::fillInnerArea(PointSet &innerSet, CellCountAccessor *cca, UINT maxCount) {
  SETPHASE(_T("FILLINNERAREA"))

  for(Iterator<CPoint> it = innerSet.getIterator(); it.hasNext();) {
    const CPoint start = it.next();
    if(!cca->isEmptyCell(start)) {
      continue;
    }

    CompactStack<CPoint> stack;
    stack.push(start);
    while(!stack.isEmpty()) {
      CHECKPENDING();
      const CPoint np = stack.pop();
      const int    y1 = np.y-1;
      const int    y2 = np.y+1;
      if(y1 >= m_currentRect.top) {
        if(y2 < m_currentRect.bottom) {
          bool stacked1 = false, stacked2 = false;
          for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) { // go left
            CHECKCOUNTPP
            CHECKNEIGHBOURCOUNT(1)
            CHECKNEIGHBOURCOUNT(2)
          }
          stacked1 = stacked2 = false;
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {      // go right
            CHECKCOUNTPP
            CHECKNEIGHBOURCOUNT(1)
            CHECKNEIGHBOURCOUNT(2)
          }
        } else {                                                    // dont check y2
          bool stacked1 = false;
          for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) { // go left
            CHECKCOUNTPP
            CHECKNEIGHBOURCOUNT(1)
          }
          stacked1 = false;
          for(CPoint pp = np; ++pp.x < m_currentRect.right;) {      // go right
            CHECKCOUNTPP
            CHECKNEIGHBOURCOUNT(1)
          }
        }
      } else if(y2 < m_currentRect.bottom) {                        // dont check y1
        bool stacked2 = false;
        for(CPoint pp = np; pp.x >= m_currentRect.left; pp.x--) {   // go left
          CHECKCOUNTPP
          CHECKNEIGHBOURCOUNT(2)
        }
        stacked2 = false;
        for(CPoint pp = np; ++pp.x < m_currentRect.right;) {        // go right
          CHECKCOUNTPP
          CHECKNEIGHBOURCOUNT(2)
        }
      }
    }
  }
  return cca;
}
