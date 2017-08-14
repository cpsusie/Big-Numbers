#include "stdafx.h"
#include "MBCalculator.h"
#include <CPUInfo.h>

int           CalculatorPool::s_CPUCount = 0;
CalculatorSet CalculatorPool::s_maxSet;

const TCHAR  *CalculatorPool::s_stateName[] = {
  _T("SUSPENDED")
 ,_T("RUNNING")
 ,_T("TERMINATED")
};

CalculatorPool::CalculatorPool(MBContainer &mbc) : m_mbc(mbc) {
  if(s_CPUCount == 0) {
    s_CPUCount = getProcessorCount();
    s_maxSet.add(0,s_CPUCount-1);
  }
  m_pendingFlags = 0;
}

CalculatorPool::~CalculatorPool() {
  m_gate.wait();
  killAllInternal();
  m_gate.signal();
}

void CalculatorPool::startCalculators(int count) {
  m_gate.wait();

  killAllInternal();

#ifdef SAVE_CALCULATORINFO
  m_infoArray.clear();
#endif

  count = min(count, s_CPUCount);

  for(int i = 0; i < count; i++) {
    MBCalculator *calculator = new MBCalculator(this, i); TRACE_NEW(calculator);
    add(calculator);
    addToExistingInternal(i);
    setStateInternal(i, CALC_SUSPENDED);
  }
  createAllPendinglMasks();
  m_pendingFlags = 0;
  for(int i = 0; i < s_CPUCount; i++) {
    if(m_existing.contains(i)) {
      (*this)[i]->start();
    }
  }
  m_gate.signal();
}

void CalculatorPool::killAll() {
  m_gate.wait();
  killAllInternal();
  m_gate.signal();
}

void CalculatorPool::killAllInternal() {
  m_pendingFlags |= m_killAllPendingFlags;

  wakeAllInternal();
  waitUntilAllTerminatedInternal();

  if(m_calculatorsInState[CALC_TERMINATED] != m_existing) {
    DLOG((_T("Cannot kill all calculators. Still running:%s. Delete them anyway.\n")
          ,(m_calculatorsInState[CALC_RUNNING] | m_calculatorsInState[CALC_SUSPENDED]).toString().cstr()));
  }

  for(size_t i = 0; i < size(); i++) {
    MBCalculator *calculator = (*this)[i];
    SAFEDELETE(calculator);
  }
  CompactArray<MBCalculator*>::clear();

  const CalculatorSet oldRunningSet = m_calculatorsInState[CALC_RUNNING];

  clearExistingInternal();
  m_calculatorsInState[CALC_SUSPENDED ].clear();
  m_calculatorsInState[CALC_RUNNING   ].clear();
  m_calculatorsInState[CALC_TERMINATED].clear();

  notifyIfChanged(oldRunningSet);
}

void CalculatorPool::waitUntilNoRunning() {
  m_gate.wait();
  waitUntilNoRunningInternal();
  m_gate.signal();
}

void CalculatorPool::waitUntilNoRunningInternal() {
  const int maxWait = 200;
  for(int i = 0; i < maxWait; i++) {
    if(m_calculatorsInState[CALC_RUNNING].isEmpty()) {
      break;
    }
    m_gate.signal();
    Sleep(20);
    m_gate.wait();
  }
}

void CalculatorPool::waitUntilAllTerminated() {
  m_gate.wait();
  waitUntilAllTerminatedInternal();
  m_gate.signal();
}

void CalculatorPool::waitUntilAllTerminatedInternal() {
  const int maxWait = 200;
  for(int i = 0; i < maxWait; i++) {
    if(m_calculatorsInState[CALC_TERMINATED] == m_existing) {
      break;
    }
    m_gate.signal();
    Sleep(20);
    m_gate.wait();
  }
}

void CalculatorPool::wakeAllInternal() {
  const CalculatorSet suspendedSet = m_calculatorsInState[CALC_SUSPENDED];
  for(UINT i = 0; i < size(); i++) {
    if(suspendedSet.contains(i)) {
      (*this)[i]->wakeUp();
    }
  }
}

void CalculatorPool::createAllPendinglMasks() {
  ULONG mask = 0;
  for(int i = 0; i < s_CPUCount; i++) {
    if(m_existing.contains(i)) {
      mask |= CALC_SUSPEND_PENDING << (2*i);
    }
  }
  m_suspendAllPendingFlags = mask;

  mask = 0;
  for(int i = 0; i < s_CPUCount; i++) {
    if(m_existing.contains(i)) {
      mask |= CALC_KILL_PENDING << (2*i);
    }
  }
  m_killAllPendingFlags = mask;
}

void CalculatorPool::suspendCalculation() {
  m_gate.wait();
  m_pendingFlags |= m_suspendAllPendingFlags;
  m_gate.signal();
}

void CalculatorPool::resumeCalculation() {
  m_gate.wait();
  m_pendingFlags &= ~m_suspendAllPendingFlags;
  wakeAllInternal();
  m_gate.signal();
}

CalculatorState CalculatorPool::getState(int id) {
  m_gate.wait();
  const CalculatorState state = getStateInternal(id);
  m_gate.signal();
  return state;
}

void CalculatorPool::setState(int id, CalculatorState state) {
  m_gate.wait();
  setStateInternal(id, state);
  m_gate.signal();
}

CalculatorState CalculatorPool::getStateInternal(int id) const {
         if(m_calculatorsInState[CALC_SUSPENDED ].contains(id)) {
    return CALC_SUSPENDED;
  } else if(m_calculatorsInState[CALC_RUNNING   ].contains(id)) {
    return CALC_RUNNING;
  } else if(m_calculatorsInState[CALC_TERMINATED].contains(id)) {
    return CALC_TERMINATED;
  } else if(m_existing.contains(id)) {
    DLOG((_T("Undefined state for calculator %d. Existing:%s, Suspended:%s, Running:%s, Terminated:%s\n")
              ,id
              ,m_existing.toString().cstr()
              ,m_calculatorsInState[CALC_SUSPENDED ].toString().cstr()
              ,m_calculatorsInState[CALC_RUNNING   ].toString().cstr()
              ,m_calculatorsInState[CALC_TERMINATED].toString().cstr()
              ));
  } else {
    DLOG((_T("Trying to get state for non-existing calculator:%d\n"), id));
  }
  return CALC_TERMINATED;
}

void CalculatorPool::setStateInternal(int id, CalculatorState state) {
  assert(m_existing.contains(id));

  const CalculatorSet oldRunningSet = m_calculatorsInState[CALC_RUNNING];
  const bool          oldIsActive   = isCalculationActive();

  switch(state) {
  case CALC_SUSPENDED :
    if(m_calculatorsInState[CALC_TERMINATED].contains(id)) {
      DLOG((_T("Trying to set terminated calulator %d to state SUSPENDED. Skipped\n"), id));
      break;
    }
    m_calculatorsInState[CALC_RUNNING   ].remove(id);
    m_calculatorsInState[CALC_SUSPENDED ].add(id);
    break;
  case CALC_RUNNING   :
    if(m_calculatorsInState[CALC_TERMINATED].contains(id)) {
      DLOG((_T("Trying to set terminated calulator %d to state RUNNING. Skipped\n"), id));
      break;
    }
    m_calculatorsInState[CALC_SUSPENDED ].remove(id);
    m_calculatorsInState[CALC_RUNNING   ].add(id);
    break;
  case CALC_TERMINATED:
    m_calculatorsInState[CALC_SUSPENDED ].remove(id);
    m_calculatorsInState[CALC_RUNNING   ].remove(id);
    m_calculatorsInState[CALC_TERMINATED].add(id);
    break;
  }

  notifyIfChanged(oldRunningSet);
  notifyIfChanged(oldIsActive);
}

void CalculatorPool::addToExistingInternal(int id) {
  const bool oldIsActive = isCalculationActive();
  m_existing.add(id);
  notifyIfChanged(oldIsActive);
}

void CalculatorPool::clearExistingInternal() {
  const bool oldIsActive = isCalculationActive();
  m_existing.clear();
  notifyIfChanged(oldIsActive);
}

void CalculatorPool::notifyIfChanged(bool oldIsActive) {            // assume in critical section
  setProperty(CALCULATIONACTIVE, oldIsActive, isCalculationActive());
}

void CalculatorPool::notifyIfChanged(CalculatorSet oldRunningSet) { // assume in critical section
  setProperty(RUNNINGSET, oldRunningSet, m_calculatorsInState[CALC_RUNNING]);
}

String CalculatorPool::getStatesString() const {
  m_gate.wait();
  String result;
  for(UINT i = 0; i < size(); i++) {
    result += format(_T("(%u):%s "), i, getStateName(getStateInternal(i)));
  }
  m_gate.signal();
  return result;
}

#ifdef SAVE_CALCULATORINFO
void CalculatorPool::addCalculatorInfo(const CalculatorInfo &info) {
  m_gate.wait();
  m_infoArray.add(info);
  m_gate.signal();
}

const CalculatorInfo *CalculatorPool::findInfo(const CPoint &p) const {
  m_gate.wait();
  const CalculatorInfo *result = NULL;
  for(int i = 0; i < m_infoArray.size(); i++) {
    if(m_infoArray[i].contains(p)) {
      result = &m_infoArray[i];
      break;
    }
  }
  m_gate.signal();
  return result;
}

String CalculatorInfo::toString() const {
  String result;
  const CRect &r = m_blackSet.getRect();
  result = format(_T("R:(%d,%d,%d,%d)\n"), r.left,r.top,r.right,r.bottom);
  result += format(_T("Calc(%d)\n"), m_calculatorId);
  result += format(_T("#Black points:%d\n"), m_blackSet.size());
  result += format(_T("#Edge  points:%d\n"), m_edgeSet.size());
  result += format(_T("#Inner points:%d\n"), m_innerSet.size());
  String tmp;
  Iterator<size_t> it = ((CalculatorInfo*)this)->m_edgeSet.getIterator();
  for(int i = 0; (i < 20) && it.hasNext(); i++) {
    const CPoint p = m_edgeSet.next(it);
    tmp += format(_T("(%d,%d) "), p.x,p.y);
  }
  result += format(_T("First %d edgePoints:(%s)\n"), i, tmp.cstr());
  tmp = EMPTYSTRING;
  it = ((CalculatorInfo*)this)->m_innerSet.getIterator();
  for(i = 0; (i < 20) && it.hasNext(); i++) {
    const CPoint p = m_innerSet.next(it);
    tmp += format(_T("(%d,%d) "), p.x,p.y);
  }
  result += format(_T("First %d innerPoints:(%s)\n"), i, tmp.cstr());
  return result;
}

#endif
