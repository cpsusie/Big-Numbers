#include "stdafx.h"
#include <Thread.h>
#include <Random.h>
#include <Console.h>


class ScreenPoint {
public:
  int m_x,m_y;
  ScreenPoint(int x, int y) { m_x = x, m_y = y; }
  ScreenPoint() { m_x = m_y = 0; }
};

class DinnerTable;

class Philosof : public Thread {
  int   m_count;
public:
  ScreenPoint    m_position;
  int            m_id;
  DinnerTable   &m_table;
  void newMeal() { m_count++; }
  Philosof(DinnerTable &table, int id );
  unsigned int run();
  void drawMyState(const TCHAR *status);
};

class Spoon {
  ScreenPoint m_releasedPosition;
  ScreenPoint m_currentPosition;
  int         m_id;
  bool        m_inuse;
  void clearCurrentPosition();
  void drawMyId();
public:
  Spoon(const DinnerTable &table, int id);
  void useby(const Philosof &filo);
  void release();
  bool inuse() const { return m_inuse; }
};

class DinnerTable {
  Array<Spoon>             m_spoonArray;
  Array<Philosof*>         m_PhilosofArray;
  Semaphore                m_gate;
  int                      m_count;
public:
  int rand(int max);
  ScreenPoint m_center;
  DinnerTable(const ScreenPoint &center, int count = 5);
  bool fetchSpoon(int filosofid, int spoonIndex);
  void releaseSpoon(int spoonIndex);
  int size() const { return m_count; }
};

#define PI 3.1415926

double interpolate(double x1, double x2, double factor) { 
  return x1 * (1.0-factor) + x2 * factor;
}

ScreenPoint interpolate(const ScreenPoint &p1, const ScreenPoint &p2, double factor) {
  return ScreenPoint((int)interpolate(p1.m_x,p2.m_x,factor),
                     (int)interpolate(p1.m_y,p2.m_y,factor)
                    );
}

void Spoon::clearCurrentPosition() {
  Console::printf(m_currentPosition.m_x,m_currentPosition.m_y,_T("  "));
}

void Spoon::drawMyId() {
  Console::printf(m_currentPosition.m_x,m_currentPosition.m_y,_T("%d"),m_id);
}

void Spoon::useby(const Philosof &filo) {
  clearCurrentPosition();
  m_currentPosition = interpolate(m_releasedPosition,filo.m_position,0.65);
  drawMyId();
  m_inuse = true;
}

void Spoon::release() {
  clearCurrentPosition();
  m_currentPosition = m_releasedPosition;
  drawMyId();
  m_inuse = false;
}

Spoon::Spoon(const DinnerTable &table, int id) {
  m_id = id;
  m_releasedPosition.m_x = (int)(5.0 * cos((double)(id-0.5) / table.size() * 2 * PI) + table.m_center.m_x);
  m_releasedPosition.m_y = (int)(3.0 * sin((double)(id-0.5) / table.size() * 2 * PI) + table.m_center.m_y);
  m_currentPosition = m_releasedPosition;
  drawMyId();
  m_inuse = false;
}

// ----------------------------------------------------

Philosof::Philosof(DinnerTable &table, int id ) : m_table(table) {
  m_id = id;
  m_count = 0;
  m_position.m_x = (int)(30.0 * cos((double)id / m_table.size() * 2 * PI) + m_table.m_center.m_x);
  m_position.m_y = (int)(10.0 * sin((double)id / m_table.size() * 2 * PI) + m_table.m_center.m_y);
  resume();
};

unsigned int Philosof::run() {
  int leftSpoon  = m_id;
  int rightSpoon = (leftSpoon + 1) % m_table.size();
  drawMyState(_T("Tænker"));
  randomize();
  for(;;) {
    Sleep(m_table.rand(3000));
    drawMyState(_T("sulten"));
    bool takeLeftFirst = (m_table.rand(2)) ? true : false;
    if(takeLeftFirst) {
      if(!m_table.fetchSpoon(m_id,leftSpoon)) continue;
      Sleep(m_table.rand(1000)); // fører til udsultning
      if(!m_table.fetchSpoon(m_id,rightSpoon)) {
        m_table.releaseSpoon(leftSpoon);
        continue;
      }
    }
    else {
      if(!m_table.fetchSpoon(m_id,rightSpoon)) continue;
      Sleep(m_table.rand(1000)); // fører til udsultning
      if(!m_table.fetchSpoon(m_id,leftSpoon)) {
        m_table.releaseSpoon(rightSpoon);
        continue;
      }
    }
    newMeal();
    drawMyState(_T("spiser"));
    Sleep(m_table.rand(3000));
    m_table.releaseSpoon(leftSpoon);
    m_table.releaseSpoon(rightSpoon);
    drawMyState(_T("mæt   "));
    Sleep(m_table.rand(3000));
    drawMyState(_T("Tænker"));
  }
  return 0;
}

void Philosof::drawMyState(const TCHAR *status) {
  const String tmpstr = format(_T("%d %s (%d)"), m_id, status, m_count);
  Console::printf((m_position.m_x - (int)tmpstr.length())/2,m_position.m_y,tmpstr.cstr());
}

// ----------------------------------------------------

DinnerTable::DinnerTable(const ScreenPoint &center, int count) {
  int i;
  m_gate.wait();

  m_count  = count;
  m_center = center;

  for(i = 0; i < count; i++)
    m_spoonArray.add(Spoon(*this,i));

  for(i = 0; i < count; i++)
    m_PhilosofArray.add(new Philosof(*this,i));

  m_gate.signal();
}

bool DinnerTable::fetchSpoon(int filosofid, int spoonIndex) {
  m_gate.wait();
  bool ret;
  if(m_spoonArray[spoonIndex].inuse())
    ret = false;
  else {
    m_spoonArray[spoonIndex].useby(*m_PhilosofArray[filosofid]);
    ret = true;
  }
  m_gate.signal();
  return ret;
}

void DinnerTable::releaseSpoon(int spoonIndex) {
  m_gate.wait();
  m_spoonArray[spoonIndex].release();
  m_gate.signal();
}

int DinnerTable::rand(int max) {
  m_gate.wait();
  int res = ::randInt() % max;
  m_gate.signal();
  return res;
}

static void usage() {
  _ftprintf(stderr,_T("Usage:filosoffer [antal]\n"));
  exit(-1);
}

int _tmain(int argc, TCHAR **argv) {
  int philosofCount = 5;
  argv++;
  if(*argv) {
    if(_stscanf(*argv,_T("%d"),&philosofCount) != 1) usage();
    if(philosofCount < 2) {
      _ftprintf(stderr,_T("Antallet af filosoffer skal være mindst 2\n"));
      exit(-1);
    }
  }
  Console::clear();
  randomize();
  DinnerTable thetable(ScreenPoint(40,12),philosofCount);

  for(;;) {
    int ch = getchar();
    exit(0);
  }
  return 0;
}
