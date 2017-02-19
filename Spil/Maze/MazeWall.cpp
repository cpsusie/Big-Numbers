#include "StdAfx.h"
#include "MazeWall.h"

CPen solidPen[7];

class InitSolidPens {
public:
  InitSolidPens();
};

InitSolidPens::InitSolidPens() {
  solidPen[Black].CreatePen(PS_SOLID, 1, _BLACK);
  solidPen[White].CreatePen(PS_SOLID, 1, _WHITE);
  solidPen[Red  ].CreatePen(PS_SOLID, 1, _RED  );
  solidPen[Green].CreatePen(PS_SOLID, 1, _GREEN);
  solidPen[Blue ].CreatePen(PS_SOLID, 1, _BLUE );
  solidPen[Gray ].CreatePen(PS_SOLID, 1, _GRAY );
  solidPen[Cyan ].CreatePen(PS_SOLID, 1, _CYAN );
}

static InitSolidPens penInitialize;
