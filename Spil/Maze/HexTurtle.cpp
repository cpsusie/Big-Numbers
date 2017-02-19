#include "stdafx.h"
#include "resource.h"
#include "HexTurtle.h"

void HexTurtle::firstDirection() {
  const int dir = getCurrentDirection();
  if(dir < 3) {
    turnTo(dir+3);
  } else {
    turnTo(dir-3);
  }
}

void HexTurtle::lastDirection() {
  const int dir = getCurrentDirection();
  if(dir >= 3) {
    turnTo(dir-3);
  } else {
    turnTo(dir+3);
  }
}
