#include "stdafx.h"
#include <Random.h>
#include "PathFinder.h"
#include "resource.h"

PathFinder::PathFinder(Maze *maze, CDialog *dialog, CDC &workDC, bool keepTrack, bool slow, int choiceStrategy)
  : Thread(_T("PathFinder"))
{
  m_maze           = maze;
  m_slow           = slow;
  m_choiceStrategy = choiceStrategy;
  m_done           = false;
  m_turtle         = maze->getTurtle(dialog, workDC, keepTrack);
  m_directionCount = m_turtle->getDirectionCount();
  m_turtle->createPens();
}

PathFinder::~PathFinder() {
  delete m_turtle;
}

UINT PathFinder::run() {
  randomize();
  m_stop = false;
  try {
    m_stepCount = 0;
    m_maze->unVisitAll();
    backtrack();
  } catch(bool) {
    m_done = true;
  }
  return 0;
}

typedef struct {
  union {
    struct {
      char dir, i, firstChoice;
    } d;
    int m_dummy;
  };
} LocalData;

void PathFinder::backtrack() {
  LocalData var;
  var.d.dir = m_turtle->getCurrentDirection();
  if(m_stop || m_turtle->finished()) {
    throw true;
  }
  if(m_slow) {
    Sleep(3);
  }

  switch(m_choiceStrategy) {
  case ID_OPTIONS_DOORCHOICE_LEFTFIRST : var.d.firstChoice = 0;           break;
  case ID_OPTIONS_DOORCHOICE_RIGHTFIRST: var.d.firstChoice = 1;           break;
  case ID_OPTIONS_DOORCHOICE_MIX       : var.d.firstChoice = randInt()%2; break;
  }
  switch(var.d.firstChoice) {
  case 0:
    m_turtle->firstDirection();
    for(var.d.i = 0; var.d.i < m_directionCount; var.d.i++) {
      if(!m_turtle->isOppositeDirection(var.d.dir) && m_turtle->lookingAtDoor()) {
        if(m_turtle->walkStep()) {
          m_stepCount++;
          backtrack();
          m_turtle->walkBack();
          m_stepCount++;
        }
      }
      m_turtle->nextDirection();
    }
    break;
  case 1:
    m_turtle->lastDirection();
    for(var.d.i = 0; var.d.i < m_directionCount; var.d.i++) {
      if(!m_turtle->isOppositeDirection(var.d.dir) && m_turtle->lookingAtDoor()) {
        if(m_turtle->walkStep()) {
          m_stepCount++;
          backtrack();
          m_turtle->walkBack();
          m_stepCount++;
        }
      }
      m_turtle->prevDirection();
    }
    break;
  }
  m_turtle->turnTo(var.d.dir);
}
