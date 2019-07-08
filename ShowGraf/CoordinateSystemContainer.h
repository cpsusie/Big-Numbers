#pragma once

#include <MFCUtil/Coordinatesystem/CoordinateSystem.h>

class CoordinateSystemContainer {
public:
  virtual CWnd              *getWin() = 0;
  virtual CCoordinateSystem &getCoordinateSystem() = 0;
  virtual void               repaint() = 0;
};

typedef enum {
  IDLETOOL
 ,DRAGTOOL
 ,MOVEPOINTTOOL
 ,FINDZEROTOOL
 ,FINDMAXTOOL
 ,FINDMINTOOL
 ,FINDINTERSECTIONTOOL
} MouseToolType;

String toString(MouseToolType type);
