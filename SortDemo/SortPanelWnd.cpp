#include "stdafx.h"
#include "SortDemoDlg.h"
#include "SortPanelWnd.h"

#define BLACK RGB(  0,  0,  0)
#define WHITE RGB(255,255,255)
#define RED   RGB(255,  0,  0)

BEGIN_MESSAGE_MAP(SortPanelWnd, CStatic)
  ON_WM_PAINT()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

SortPanelWnd::SortPanelWnd(CSortDemoDlg *parent, int methodId)
: m_parent(parent)
, m_sortMethod(SortMethodId::getMethodById(methodId))
, m_fast(parent->getFast())
, m_dataArray(parent->getInitParameters())
, m_savedArray(parent->getInitParameters())
, m_modifiedSet(parent->getInitParameters().m_elementCount)
, m_resume(0)
{
  m_sortThread     = NULL;

  const CRect r(0,0,100,100);
  BOOL ok = Create( EMPTYSTRING, WS_VISIBLE|WS_CHILD|SS_WHITERECT, r, parent, parent->getNextCtrlId());
  if(!ok) {
    throwException(_T("cannot cerate panel"));
  }
  ModifyStyleEx(0, WS_EX_CLIENTEDGE);
  m_font.CreateFont(10, 6, 0, 0, 400, FALSE, FALSE, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS
                   ,CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY
                   ,DEFAULT_PITCH | FF_MODERN
                   ,_T("Courier") );

  m_whitePen.CreatePen(PS_SOLID, 1, WHITE);
  m_blackPen.CreatePen(PS_SOLID, 1, BLACK);
  m_redPen.CreatePen(  PS_SOLID, 1, RED  );

  m_threadState  = STATE_IDLE;
  m_sortThread   = new SortThread(this);

  m_oldIndex1    = m_oldIndex2 = -1;
  initArray();
}

void SortPanelWnd::setRect(const CRect &r) {
  setWindowRect(this, r);
}

void SortPanelWnd::OnPaint() {
  CStatic::OnPaint();
  CClientDC dc(this);
  GetClientRect(&m_rect);
  m_elementCount = m_dataArray.size();
  for(UINT i = 0; i < m_elementCount; i++) {
    drawElement(dc, i);
  }
  printCompareCount(dc, true);
}

void SortPanelWnd::doSort() {
  CClientDC dc(this);
  updateScreen(dc, m_fast);
  m_threadSignal = 0;
  m_sortThread->resume();
}

void SortPanelWnd::resumeSort() {
  setThreadState(STATE_RUNNING);
}

bool SortPanelWnd::invalidStateTransition(SortThreadState newState) {
  errorMessage(_T("Invalid statetransition:%d->%d (%s -> %s)"), m_threadState, newState, getStateString(m_threadState), getStateString(newState));
  return false;
}

static const bool legalTransitions[5][5] = {
  false,  true, false,  true, false
 ,true , false,  true,  true,  true
 ,false,  true, false,  true, false
 ,false, false, false, false, false
 , true,  true, false, false, false
};

bool SortPanelWnd::setThreadState(SortThreadState newState) {
  if(!legalTransitions[m_threadState][newState]) {
    return invalidStateTransition(newState);
  }

  switch(m_threadState) {
  case STATE_PAUSED :
    if(newState == STATE_RUNNING) {
      m_resume.notify();
    }
    break;
  }

  const SortThreadState oldState = m_threadState;
  m_threadState = newState;
  notifyStateChange(oldState, newState);
  Invalidate();
  return true;
}

void SortPanelWnd::waitForResume() {
  setThreadState(STATE_PAUSED);
  m_resume.wait();
}

void SortPanelWnd::notifyStateChange(SortThreadState oldState, SortThreadState newState) {
  m_parent->postStateShift(this, oldState, newState);
}

const InitializeParameters &SortPanelWnd::getInitParameters() const {
  return m_parent->getInitParameters();
}

bool SortPanelWnd::isAnimatedSort() const {
  return m_parent->isAnimatedSort();
}

void SortPanelWnd::initArray() {
  m_dataArray.init();;
  m_savedArray.reset();
  m_elementCount   = m_dataArray.size();
  m_maxElementSize = m_dataArray.getMaxValue();
  m_compareCount   = 0;
  m_modifiedSet.setCapacity(m_elementCount);
  m_startTime      = m_sortThread->getThreadTime(); // just to print 0 in updateScreen
}

static void line(CDC &dc, int x1, int y1, int x2, int y2) {
  dc.MoveTo(x1,y1);
  dc.LineTo(x2,y2);
}

#define MARG 20
#define INDEXTOX(index)  ((int)(MARG + (index) * (m_rect.Width()-2*MARG) / m_elementCount))

void SortPanelWnd::drawElement(CDC &dc, size_t index) {
  const int x  = INDEXTOX(index);
  const int y0 = m_rect.Height()-MARG;
  const int y  = (int)(y0 - m_dataArray[index] * (m_rect.Height()-2*MARG) / m_maxElementSize);
  CPen *oldPen = dc.SelectObject(&m_whitePen);
  line(dc,x,MARG, x, y);
  dc.SelectObject(m_blackPen);
  line(dc,x,y0  , x, y);
  dc.SelectObject(oldPen);
}

void SortPanelWnd::markElement(CDC &dc, size_t index, CPen &pen) {
  const int x  = INDEXTOX(index);
  const int y0 = m_rect.Height()-MARG+1;
  CPen *oldPen = dc.SelectObject(&pen);
  line(dc,x  , y0, x  , y0+4);
  line(dc,x+1, y0, x+1, y0+4);
  line(dc,x+1, y0, x+3, y0+1);
  line(dc,x  , y0, x-2, y0+1);
  dc.SelectObject(oldPen);
}

void SortPanelWnd::updateMarks(CDC &dc, size_t index1, size_t index2) {
  if(index1 != m_oldIndex1) {
    if(m_oldIndex1 >= 0) {
      markElement(dc, m_oldIndex1, m_whitePen);
      m_oldIndex1 = -1;
    }
    if(index1 < m_elementCount) {
      markElement(dc, index1, m_redPen);
      m_oldIndex1 = index1;
    }
  }
  if(index2 != m_oldIndex2) {
    if(m_oldIndex2 >= 0) {
      markElement(dc, m_oldIndex2, m_whitePen);
      m_oldIndex2 = -1;
    }
    if(index2 < m_elementCount) {
      markElement(dc, index2, m_redPen);
      m_oldIndex2 = index2;
    }
  }
}

void SortPanelWnd::updateScreen(CDC &dc, bool showTime) {
  printCompareCount(dc, showTime);
  m_dataArray.getModified(m_modifiedSet, m_savedArray);
  for(Iterator<size_t> it = m_modifiedSet.getIterator(); it.hasNext();) {
    drawElement(dc, it.next());
  }
  saveArray();
}

void SortPanelWnd::saveArray() {
  m_savedArray = m_dataArray;
}

const TCHAR *SortPanelWnd::stateStringTable[] = {
  _T("IDLE")
 ,_T("RUN")
 ,_T("PAUSE")
 ,_T("KILLED")
 ,_T("ERROR")
};

void SortPanelWnd::printCompareCount(CDC &dc, bool showTime) {
  CFont *oldFont = dc.SelectObject(&m_font);
  if(showTime) {
    dc.TextOut(5,6
              ,format(_T("%s:State:%-5s #Compares:%7d %7.0lf msec.")
                     ,m_sortMethod.getName().cstr()
                     ,getStateStr()
                     ,m_compareCount
                     ,(m_sortThread->getThreadTime()-m_startTime)/1000.0).cstr());
  } else {
    dc.TextOut(5,6
              ,format(_T("%s State:%-5s #Compares:%7d              ")
                     ,m_sortMethod.getName().cstr()
                     ,getStateStr()
                     ,m_compareCount).cstr());
  }
  dc.SelectObject(oldFont);
}


void SortPanelWnd::OnDestroy() {
  if(m_sortThread) {
    m_threadSignal = TERMINATE_SORT | KILL_THREAD;
    if(getThreadState() != STATE_RUNNING) {
      m_sortThread->resume();
    }
    for(int emergencyCount = 0; (emergencyCount < 10) && m_sortThread->stillActive(); emergencyCount++) {
      Sleep(50);
    }
    if(m_sortThread->stillActive()) {
      errorMessage(_T("Cannot stop sortThread!!"));
    } else {
      delete m_sortThread;
      m_sortThread = NULL;
    }
  }
  CStatic::OnDestroy();
}
