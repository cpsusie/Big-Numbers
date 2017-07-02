#include "stdafx.h"

using namespace ThreeD;

#define TOLERANCE_DENSITY 1e-5
#define TOLERANCE_COORD   1e-5

IsoMesher::IsoMesher(Isosurface &iso) : m_iso(iso) {
  setVoxelSize(0,0,0);
  m_progressFunc = NULL;
}

void IsoMesher::setVoxelSize(float x, float y, float z) {
  m_voxelSize = D3DXVECTOR3(x, y, z);
}

void IsoMesher::setProgressFunc(bool (*func)(void *, int), void *parm) {
  m_progressFunc    = func;
  m_progressParm    = parm;
  m_progressPercent = 0;
  m_progressTime    = GetTickCount();
}

bool IsoMesher::invokeProgressFunc() {
  bool cancel = false;
  if(m_progressFunc) {
    long time = GetTickCount();
    if(time - m_progressTime > 500) {
      cancel = m_progressFunc(m_progressParm, m_progressPercent);
      m_progressTime = time;
    }
  }
  return cancel;
}

// select root solver

#define NEW_BISECTION  0
#define USE_BISECTION  1
#define FALSE_POSITION 2

#define ROOT_SOLVER BISECTION

#if(ROOT_SOLVER == NEW_BISECTION)

#define MAX_ITER 10

// Bisection (midpoint) root solver
void IsoMesher::intersectXaxis(const Point &p0, const Point &p1, Point &out) const {
  float xa, xb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    xa = p0.v->x;
    xb = p1.v->x;
  } else {                      // p1 < 0,  p0 > 0
    xa = p1.v->x;
    xb = p0.v->x;
  }
  const float y = p0.v->y;
  const float z = p0.v->z;
  float density;

  for(int i = MAX_ITER;;) {
    const float xm = (xa + xb) / 2;
    if(i-- == 0) {
      out.density = density;
      *out.v = D3DXVECTOR3(xm, y, z);
      return;
    }
    m_iso.fDensity(xm, y, z, 0, 1, &density);
    if(fsign(density)) {        // pm < 0
      xa = xm;
    } else {                    // pm > 0
      xb = xm;
    }
  }
}

























void IsoMesher::intersectYaxis(const Point &p0, const Point &p1, Point &out) const {
  float ya, yb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    ya = p0.v->y;
    yb = p1.v->y;
  } else {                      // p1 < 0,  p0 > 0
    ya = p1.v->y;
    yb = p0.v->y;
  }
  const float x = p0.v->x;
  const float z = p0.v->z;
  float density;

  for(int i = MAX_ITER;;) {
    const float ym = (ya + yb) / 2;
    if(i-- == 0) {
      out.density = density;
      *out.v = D3DXVECTOR3(x, ym, z);
      return;
    }
    m_iso.fDensity(x, ym, z, 0, 1, &density);
    if(fsign(density)) {        // pm < 0
      ya = ym;
    } else {                    // pm > 0
      yb = ym;
    }
  }
}

























void IsoMesher::intersectZaxis(const Point &p0, const Point &p1, Point &out) const {
  float za, zb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    za = p0.v->z;
    zb = p1.v->z;
  } else {                      // p1 < 0,  p0 > 0
    za = p1.v->z;
    zb = p0.v->z;
  }
  const float x = p0.v->x;
  const float y = p0.v->y;
  float density;

  for(int i = MAX_ITER;;) {
    const float zm = (za + zb) / 2;
    if(i-- == 0) {
      out.density = density;
      *out.v = D3DXVECTOR3(x, y, zm);
      return;
    }
    m_iso.fDensity(x, y, zm, 0, 1, &density);
    if(fsign(density)) {        // pm < 0
      za = zm;
    } else {                    // pm > 0
      zb = zm;
    }
  }

}

#elif(ROOT_SOLVER == USE_BISECTION)

// Bisection (midpoint) root solver
void IsoMesher::intersectXaxis(const Point &p0, const Point &p1, Point &out) const {
  float xa, xb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    xa = p0.v->x;
    xb = p1.v->x;
  } else {                      // p1 < 0,  p0 > 0
    xa = p1.v->x;
    xb = p0.v->x;
  }
  float y = p0.v->y;
  float z = p0.v->z;
  float xm;
  float density;

  for(;;) {
    xm = (xa + xb) / 2;
    m_iso.fDensity(xm, y, z, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(xa - xb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm < 0
      xa = xm;
    } else {                    // pm > 0
      xb = xm;
    }
  }

  out.density = density;
  *out.v = D3DXVECTOR3(xm, y, z);
}


















void IsoMesher::intersectYaxis(const Point &p0, const Point &p1, Point &out) const {
  float ya, yb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    ya = p0.v->y;
    yb = p1.v->y;
  } else {                      // p1 < 0,  p0 > 0
    ya = p1.v->y;
    yb = p0.v->y;
  }
  float x = p0.v->x;
  float z = p0.v->z;
  float ym;
  float density;

  for(;;) {
    ym = (ya + yb) / 2;
    m_iso.fDensity(x, ym, z, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(ya - yb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm < 0
      ya = ym;
    } else {                    // pm > 0
      yb = ym;
    }
  }

  out.density = density;
  *out.v = D3DXVECTOR3(x, ym, z);
}


















void IsoMesher::intersectZaxis(const Point &p0, const Point &p1, Point &out) const {
  float za, zb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    za = p0.v->z;
    zb = p1.v->z;
  } else {                      // p1 < 0,  p0 > 0
    za = p1.v->z;
    zb = p0.v->z;
  }
  float x = p0.v->x;
  float y = p0.v->y;
  float zm;
  float density;

  for(;;) {
    zm = (za + zb) / 2;
    m_iso.fDensity(x, y, zm, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(za - zb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm < 0
      za = zm;
    } else {                    // pm > 0
      zb = zm;
    }
  }

  out.density = density;
  *out.v = D3DXVECTOR3(x, y, zm);
}

#elif(ROOT_SOLVER == FALSE_POSITION)















// False-position root solver
void IsoMesher::intersectXaxis(const Point &p0, const Point &p1, Point &out) const {
  float fa, fb;
  float xa, xb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    fa = p0.density;
    fb = p1.density;
    xa = p0.v->x;
    xb = p1.v->x;
  } else {                      // p1 < 0,  p0 > 0
    fa = p1.density;
    fb = p0.density;
    xa = p1.v->x;
    xb = p0.v->x;
  }
  float y = p0.v->y;
  float z = p0.v->z;
  float xm;
  float density;

  for(;;) {
    xm = xb - (fb * (xb - xa) / (fb - fa));
    m_iso.fDensity(xm, y, z, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(xa - xb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm is negative
      xa = xm;
      fa = density;
    } else {                    // pm is positive
      xb = xm;
      fb = density;
    }

    xm = (xa + xb) * 0.5f;
    m_iso.fDensity(xm, y, z, 0, 1, &density);
    density += 1e-4f;
    if(fsign(density)) {        // pm < 0
      xa = xm;
      fa = density;
    } else {                    // pm > 0
      xb = xm;
      fb = density;
    }
  }
  out.density = density;
  *out.v = D3DXVECTOR3(xm, y, z);
}

void IsoMesher::intersectYaxis(const Point &p0, const Point &p1, Point &out) const {
  float fa, fb;
  float ya, yb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    fa = p0.density;
    fb = p1.density;
    ya = p0.v->y;
    yb = p1.v->y;
  } else {                      // p1 < 0,  p0 > 0
    fa = p1.density;
    fb = p0.density;
    ya = p1.v->y;
    yb = p0.v->y;
  }
  float x = p0.v->x;
  float z = p0.v->z;
  float ym;
  float density;

  for(;;) {
    ym = yb - (fb * (yb - ya) / (fb - fa));
    m_iso.fDensity(x, ym, z, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(ya - yb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm is negative
      ya = ym;
      fa = density;
    } else {                    // pm is positive
      yb = ym;
      fb = density;
    }

    ym = (ya + yb) * 0.5f;
    m_iso.fDensity(x, ym, z, 0, 1, &density);
    density += 1e-4f;
    if(fsign(density)) {        // pm < 0
      ya = ym;
      fa = density;
    } else {                    // pm > 0
      yb = ym;
      fb = density;
    }
  }
  out.density = density;
  *out.v = D3DXVECTOR3(x, ym, z);
}

void IsoMesher::intersectZaxis(const Point &p0, const Point &p1, Point &out) const {
  float fa, fb;
  float za, zb;
  if(fsign(p0.density)) {       // p0 < 0,  p1 > 0
    fa = p0.density;
    fb = p1.density;
    za = p0.v->z;
    zb = p1.v->z;
  } else {                      // p1 < 0,  p0 > 0
    fa = p1.density;
    fb = p0.density;
    za = p1.v->z;
    zb = p0.v->z;
  }
  float x = p0.v->x;
  float y = p0.v->y;
  float zm;
  float density;

  for(;;) {
    zm = zb - (fb * (zb - za) / (fb - fa));
    m_iso.fDensity(x, y, zm, 0, 1, &density);
    density += 1e-4f;
    if(fabs(density) < TOLERANCE_DENSITY) {
      break;
    }
    if(fabs(za - zb) < TOLERANCE_COORD) {
      break;
    }

    if(fsign(density)) {        // pm is negative
      za = zm;
      fa = density;
    } else {                    // pm is positive
      zb = zm;
      fb = density;
    }

    zm = (za + zb) * 0.5f;
    m_iso.fDensity(x, y, zm, 0, 1, &density);
    density += 1e-4f;
    if(fsign(density)) {        // pm < 0
      za = zm;
      fa = density;
    } else {                    // pm > 0
      zb = zm;
      fb = density;
    }
  }
  out.density = density;
  *out.v = D3DXVECTOR3(x, y, zm);
}

#endif
