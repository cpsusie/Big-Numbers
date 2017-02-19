#pragma once

#include "Turtle.h"
#include "CellMaze.h"

class CellMazeTurtle : public Turtle {
private:
  const MazeCell  &m_endCell;
protected:
  Stack<MazeCell*> m_cellStack;

  CellMazeTurtle(const CellMaze &maze, CDialog *dialog, CDC &workDC, bool keepTrack)
  : Turtle(maze, dialog, workDC, keepTrack)
  , m_endCell(maze.getEndCell())
  {
    m_cellStack.push(&maze.getStartCell());
  }
public:
  void createPens();
  bool lookingAtDoor() const;
  bool walkStep();
  void walkBack();
  bool finished() const {
    return m_cellStack.top() == &m_endCell;
  }
};
