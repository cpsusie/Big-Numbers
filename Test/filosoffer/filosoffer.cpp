#include "stdafx.h"
#include <ThreadPool.h>
#include <Thread.h>
#include <InterruptableRunnable.h>
#include <MathUtil.h>
#include <Random.h>
#include <Console.h>

class ScreenPoint {
public:
  int m_x,m_y;
  ScreenPoint(int x, int y) { m_x = x, m_y = y; }
  ScreenPoint() { m_x = m_y = 0; }
};

double interpolate(double x1, double x2, double factor) {
  return x1 * (1.0 - factor) + x2 * factor;
}

ScreenPoint interpolate(const ScreenPoint &p1, const ScreenPoint &p2, double factor) {
  return ScreenPoint((int)interpolate(p1.m_x, p2.m_x, factor),
                     (int)interpolate(p1.m_y, p2.m_y, factor)
                    );
}

class DinnerTable;

class Philosof : public InterruptableRunnable {
private:
  const UINT     m_id;
  UINT           m_count;
  ScreenPoint    m_position;
  DinnerTable   &m_table;
  inline void newMeal() {
    m_count++;
  }
public:
  Philosof(DinnerTable &table, UINT id);
  ~Philosof();
  UINT safeRun();
  void drawMyState(const TCHAR *status);
  inline const ScreenPoint &getPosition() const {
    return m_position;
  }
};

class Spoon {
private:
  const UINT  m_id;
  ScreenPoint m_releasedPosition;
  ScreenPoint m_currentPosition;
  bool        m_inuse;
  void clearCurrentPosition();
  void drawMyId();
public:
  Spoon(const DinnerTable &table, UINT id);
  ~Spoon();
  void useby(const Philosof &filo);
  void release();
  inline bool inuse() const {
    return m_inuse;
  }
};

#define _2PI (2.0*M_PI)

class DinnerTable {
  CompactArray<Spoon*>     m_spoonArray;
  CompactArray<Philosof*>  m_philosofArray;
  FastSemaphore            m_gate;
  UINT                     m_count;
  const ScreenPoint        m_center;
  DinnerTable(           const DinnerTable &src); // not implemented
  DinnerTable &operator=(const DinnerTable &src); // not implemented
public:
  DinnerTable(const ScreenPoint &center, UINT count = 5);
  ~DinnerTable();
  void stopDinner();
  bool fetchSpoon(UINT filosofid, UINT spoonIndex);
  void releaseSpoon(UINT spoonIndex);
  inline UINT size() const {
    return m_count;
  }
  const inline ScreenPoint &getTableCenter() const {
    return m_center;
  }
};

Spoon::Spoon(const DinnerTable &table, UINT id) : m_id(id) {
  const ScreenPoint &center = table.getTableCenter();
  m_releasedPosition.m_x = (int)((center.m_x/4) * cos((double)(id-0.5) / table.size() * _2PI) + center.m_x);
  m_releasedPosition.m_y = (int)((center.m_y/2) * sin((double)(id-0.5) / table.size() * _2PI) + center.m_y);
  m_currentPosition = m_releasedPosition;
  drawMyId();
  m_inuse = false;
}

Spoon::~Spoon() {
  Console::printf(m_currentPosition.m_x,m_currentPosition.m_y,_T("  "));
}

void Spoon::clearCurrentPosition() {
  Console::printf(m_currentPosition.m_x,m_currentPosition.m_y,_T("  "));
}

void Spoon::drawMyId() {
  Console::printf(m_currentPosition.m_x,m_currentPosition.m_y,_T("%u"),m_id);
}

void Spoon::useby(const Philosof &filo) {
  clearCurrentPosition();
  m_currentPosition = interpolate(m_releasedPosition,filo.getPosition(),0.65);
  drawMyId();
  m_inuse = true;
}

void Spoon::release() {
  clearCurrentPosition();
  m_currentPosition = m_releasedPosition;
  drawMyId();
  m_inuse = false;
}

// ----------------------------------------------------

Philosof::Philosof(DinnerTable &table, UINT id ) : m_id(id), m_table(table) {
  const ScreenPoint &center = table.getTableCenter();
  m_position.m_x = center.m_x + (int)((center.m_x-10) * cos((double)id / m_table.size() * _2PI));
  m_position.m_y = center.m_y + (int)((center.m_y-3 ) * sin((double)id / m_table.size() * _2PI));
  m_count = 0;
};

Philosof::~Philosof() {
  waitUntilJobDone();
  drawMyState(_T("gået           "));
}

UINT Philosof::safeRun() {
  SETTHREADDESCRIPTION(format(_T("Philosof %u"), m_id));
  const UINT leftSpoon  = m_id, rightSpoon = (leftSpoon + 1) % m_table.size();
  drawMyState(_T("Tænker"));
  JavaRandom rnd;
  rnd.randomize();
  for(;;) {
    checkInterruptAndSuspendFlags();
    Sleep(rnd.nextInt(3000));
    drawMyState(_T("sulten"));
    bool takeLeftFirst = rnd.nextBool();
    if(takeLeftFirst) {
      if(!m_table.fetchSpoon(m_id,leftSpoon)) continue;
      Sleep(rnd.nextInt(1000)); // fører til udsultning
      if(!m_table.fetchSpoon(m_id,rightSpoon)) {
        m_table.releaseSpoon(leftSpoon);
        continue;
      }
    } else {
      if(!m_table.fetchSpoon(m_id,rightSpoon)) continue;
      Sleep(rnd.nextInt(1000)); // fører til udsultning
      if(!m_table.fetchSpoon(m_id,leftSpoon)) {
        m_table.releaseSpoon(rightSpoon);
        continue;
      }
    }
    newMeal();
    drawMyState(_T("spiser"));
    Sleep(rnd.nextInt(3000));
    m_table.releaseSpoon(leftSpoon);
    m_table.releaseSpoon(rightSpoon);
    drawMyState(_T("mæt   "));
    Sleep(rnd.nextInt(3000));
    drawMyState(_T("Tænker"));
  }
  return 0;
}

void Philosof::drawMyState(const TCHAR *status) {
  const String tmpstr = format(_T("%u %s (%u)"), m_id, status, m_count);
  Console::printf(m_position.m_x - (int)tmpstr.length()/2,m_position.m_y,tmpstr.cstr());
}

// ----------------------------------------------------

DinnerTable::DinnerTable(const ScreenPoint &center, UINT count) : m_center(center), m_count(count) {
  m_gate.wait();

  for(UINT i = 0; i < m_count; i++) {
    Spoon *sp = new Spoon(*this, i); TRACE_NEW(sp);
    m_spoonArray.add(sp); 
  }

  for(UINT i = 0; i < m_count; i++) {
    Philosof *ph = new Philosof(*this, i); TRACE_NEW(ph);
    m_philosofArray.add(ph);
    ThreadPool::executeNoWait(*ph);
  }
  m_gate.notify();
}

DinnerTable::~DinnerTable() {
  stopDinner();
}

void DinnerTable::stopDinner() {
  for(UINT i = 0; i < m_philosofArray.size(); i++) {
    Philosof *ph = m_philosofArray[i];
    ph->setInterrupted();
  }
  for(UINT i = 0; i < m_philosofArray.size(); i++) {
    Philosof *ph = m_philosofArray[i];
    SAFEDELETE(ph);
  }
  for(UINT i = 0; i < m_spoonArray.size(); i++) {
    Spoon *sp = m_spoonArray[i];
    SAFEDELETE(sp);
  }
  m_philosofArray.clear();
  m_spoonArray.clear();
}

bool DinnerTable::fetchSpoon(UINT filosofid, UINT spoonIndex) {
  m_gate.wait();
  bool ret;
  if(m_spoonArray[spoonIndex]->inuse()) {
    ret = false;
  } else {
    m_spoonArray[spoonIndex]->useby(*m_philosofArray[filosofid]);
    ret = true;
  }
  m_gate.notify();
  return ret;
}

void DinnerTable::releaseSpoon(UINT spoonIndex) {
  m_gate.wait();
  m_spoonArray[spoonIndex]->release();
  m_gate.notify();
}

static void usage() {
  _ftprintf(stderr,_T("Usage:filosoffer [antal]\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  UINT philosofCount = 5;
  argv++;
  if(*argv) {
    if(_stscanf(*argv, _T("%u"), &philosofCount) != 1) {
      usage();
    }
    if(philosofCount < 2) {
      _ftprintf(stderr,_T("Antallet af filosoffer skal være mindst 2\n"));
      exit(-1);
    }
  }
  const COORD sz = Console::getWindowSize();
  ScreenPoint screenCenter(sz.X / 2, sz.Y / 2);
  Console::clear();
  DinnerTable *dinnerTable = new DinnerTable(screenCenter, philosofCount); TRACE_NEW(dinnerTable);

  for(;;) {
    char ch = getchar();
    if(ch == 'q') {
      SAFEDELETE(dinnerTable);
      break;
    }
  }
  return 0;
}
