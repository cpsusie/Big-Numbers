#pragma once

#include <Thread.h>
#include "RectMaze.h"
#include "Turtle.h"

class PathFinder : public Thread {
private:
  Maze   *m_maze;
  bool    m_slow, m_stop;
  int     m_choiceStrategy;
  Turtle *m_turtle;
  int     m_directionCount;
  int     m_stepCount;
  bool    m_done;
  void backtrack();
public:
  PathFinder(Maze *maze, CDialog *dialog, CDC &workDC, bool keepTrack, bool slow, int choiceStrategy);
  ~PathFinder();
  bool done() const {
    return m_done;
  }
  int getStepCount() const {
    return m_stepCount;
  }
  void stop() {
    m_stop = true;
  }
  void setSlowSpeed(bool slow) {
    m_slow = slow;
  }
  UINT run();
};
