//
//
// An implicit surface polygonizer, translated from Mesa
// applications should call polygonize()
//
// Authored by Jules Bloomenthal, Xerox PARC.
// Copyright (c) Xerox Corporation, 1991. All rights reserved.
// Permission is granted to reproduce, use and distribute this code for
// any and all purposes, provided that this notice appears in all
// copies.

#include "pch.h"
#include <Thread.h>
#include <DebugLog.h>
#include <TinyBitSet.h>
#include <MFCUtil/ColorSpace.h>
#include <D3DGraphics/IsoSurfacePolygonizerSnapMC.h>

namespace ISOSURFACE_POLYGONIZER_SNAPMC {

#define RES 10 // # converge iterations

const TCHAR *labelChars = _T("-=+");

typedef struct {
  const CubeCorner corner[2];
  const BYTE       coordIndex; // = [0..2], 0=x, 1=y, 2=z
} CubeEdgeInfo;

static const CubeEdgeInfo cubeEdgeTable[12] = {
  { LBN,RBN, 0}  // bottom near   edge BN
 ,{ LBN,LBF, 1}  // left   bottom edge LB
 ,{ RBN,RBF, 1}  // right  bottom edge RB
 ,{ LBF,RBF, 0}  // bottom far    edge BF
 ,{ LBN,LTN, 2}  // left   near   edge LN
 ,{ RBN,RTN, 2}  // right  near   edge RN
 ,{ LBF,LTF, 2}  // left   far    edge LF
 ,{ RBF,RTF, 2}  // right  far    edge RF
 ,{ LTN,RTN, 0}  // top    near   edge TN
 ,{ LTN,LTF, 1}  // left   top    edge LT
 ,{ RTN,RTF, 1}  // right  top    edge RT
 ,{ LTF,RTF, 0}  // top    far    edge TF
};

typedef struct {
  const CubeCorner c1, c2, c3, c4;
  const BYTE       vertex[6];
} CubeFaceInfo;

// order clockwise looked from outside (visible side)
static const CubeFaceInfo cubeFaceTable[6] = {
  LBN, LBF, LTF, LTN, 64, 2, 6, 0, 6, 4  // LFACE direction : -x, -i
 ,RBN, RTN, RTF, RBF, 65, 7, 5, 1, 3, 7  // RFACE direction : +x, +i
 ,LBN, LTN, RTN, RBN, 64, 5, 4, 0, 1, 5  // NFACE direction : -y, -j
 ,LBF, RBF, RTF, LTF, 66, 7, 3, 2, 6, 7  // FFACE direction : +y, +j
 ,LBN, RBN, RBF, LBF, 64, 3, 1, 0, 2, 3  // BFACE direction : -z, -k
 ,LTN, LTF, RTF, RTN, 68, 7, 6, 4, 5, 7  // TFACE direction : +z, +k
};

#define HASHSIZE 100001
#define BIT(i, bit) (((i)>>(bit))&1)
#define FLIP(i,bit) ((i)^(1<<(bit))) // flip the given bit of i

const PolygonizerCubeArrayTable IsoSurfacePolygonizer::s_cubetable;

PolygonizerBase::PolygonizerBase() : m_cornerMap(HASHSIZE) {
}

PolygonizerBase::PolygonizerBase(const PolygonizerBase &src) {
  copy(src);
}

PolygonizerBase &PolygonizerBase::operator=(const PolygonizerBase &src) {
  return cleanup().copy(src);
}

PolygonizerBase::~PolygonizerBase() {
  cleanup();
}

PolygonizerBase &PolygonizerBase::copy(const PolygonizerBase &src) {
  m_cellSize     = src.m_cellSize;
  m_start        = src.m_start;
  m_cornerMap    = src.m_cornerMap;
  m_cubesDoneSet = src.m_cubesDoneSet;
  m_edgeMap      = src.m_edgeMap;
  m_faceArray    = src.m_faceArray;
  m_vertexArray  = src.m_vertexArray;
  const size_t n = src.m_intersectingCubeTable.size();
  m_intersectingCubeTable.setCapacity(n);
  for(size_t i = 0; i < n; i++) {
    m_intersectingCubeTable.add(StackedCube().copy(src.m_intersectingCubeTable[i], m_cornerMap));
  }
  return *this;
}

PolygonizerBase &PolygonizerBase::cleanup() {
  m_cellSize = 0;
  m_start    = Point3D(0, 0, 0);
  m_cornerMap.clear();
  m_cubesDoneSet.clear();
  m_edgeMap.clear();
  m_faceArray.clear();
  m_vertexArray.clear();
  m_intersectingCubeTable.clear();
  return *this;
}

Point3D PolygonizerBase::getCornerPoint(int i, int j, int k) const {
  Point3D result;
  result.x = m_start.x + ((double)i)*m_cellSize;
  result.y = m_start.y + ((double)j)*m_cellSize;
  result.z = m_start.z + ((double)k)*m_cellSize;
  return result;
}

IsoSurfacePolygonizer::IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval) : m_eval(eval) {
}

IsoSurfacePolygonizer::~IsoSurfacePolygonizer() {
}

static int cubeCenterCompare(const StackedCube &c1, const StackedCube &c2) {
  return Point3DKey::compare(c1.m_key, c2.m_key);
}

class CubeCenterDistanceComparator : public Comparator<StackedCube> {
private:
  const Point3DKey m_key0;
public:
  CubeCenterDistanceComparator(const StackedCube &c0) : m_key0(c0.m_key) {
  }
  AbstractComparator *clone() const {
    return new CubeCenterDistanceComparator(*this);
  }
  int compare(const StackedCube &c1, const StackedCube &c2) {
    return sign(Point3DKey::distance(c1.m_key, m_key0) - Point3DKey::distance(c2.m_key, m_key0));
  }
};

void IsoSurfacePolygonizer::polygonize(const Point3D &start
                                      ,double         cellSize
                                      ,double         lambda
                                      ,const Cube3D  &boundingBox
                                      ,bool           tetrahedralMode
                                      ,bool           tetraOptimize4
                                      )
{

  const double startTime = getThreadTime();

#ifndef _DEBUG
  m_rnd.randomize();
#endif // _DEBUG

  m_statistics.clear();
  cleanup();

  m_cellSize         = cellSize;
  m_lambda           = lambda;
  m_maxSnapDistance  = m_cellSize * m_lambda;
  m_boundingBox      = boundingBox;
  m_delta            = cellSize/(double)(RES*RES);
  m_tetrahedralMode  = tetrahedralMode;
  m_tetraOptimize4   = tetraOptimize4;

  m_start = start;
  for(int i = 0; i < 10; i++) {
    m_start = findStartPoint(m_start);
    if(putInitialCube()) {
      break;
    }
  }

  m_vertexArray.setCapacity( HASHSIZE);
  m_cubesDoneSet.setCapacity(HASHSIZE);
  m_edgeMap.setCapacity(     HASHSIZE);
  while(hasActiveCubes()) { // process active cubes until none left
    const StackedCube cube = getActiveCube();
    addSurfaceVertices(cube);
    // test six face directions, maybe add to stack:
    testFace(cube.m_key.i-1 , cube.m_key.j   , cube.m_key.k   , cube, LFACE, LBN, LBF, LTN, LTF);
    testFace(cube.m_key.i+1 , cube.m_key.j   , cube.m_key.k   , cube, RFACE, RBN, RBF, RTN, RTF);
    testFace(cube.m_key.i   , cube.m_key.j-1 , cube.m_key.k   , cube, NFACE, LBN, LTN, RBN, RTN);
    testFace(cube.m_key.i   , cube.m_key.j+1 , cube.m_key.k   , cube, FFACE, LBF, LTF, RBF, RTF);
    testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k-1 , cube, BFACE, LBN, LBF, RBN, RBF);
    testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k+1 , cube, TFACE, LTN, LTF, RTN, RTF);
  }

  for(Iterator<Entry< Point3DKey, HashedCubeCorner> > it = m_cornerMap.getEntryIterator(); it.hasNext();) {
    HashedCubeCorner &c = it.next().getValue();
    if(c.hasSnapVertex()) {
      c.setLabel(V_ZERO);
    }
  }

  const size_t cubeCount = m_intersectingCubeTable.size();
  for(size_t i = 0; i < cubeCount; i++) {
    doCube2(i);
    flushFaceBuffer();
  }
  saveStatistics(startTime);
  flushFaceArray();

#ifdef  DUMP_STATISTICS
  debugLog(_T("%s\n"), m_statistics.toString().cstr());
#endif
#ifdef DUMP_CORNERMAP
  dumpCornerMap();
#endif
#ifdef DUMP_EDGEMAP
  dumpEdgeMap();
#endif
#ifdef DUMP_VERTEXARRAY
  dumpVertexArray();
#endif
#ifdef DUMP_FACEARRAY
  dumpFaceArray();
#endif
}

bool IsoSurfacePolygonizer::putInitialCube() {
  StackedCube cube(0,0,0);
  m_cornerMap.clear();
  // set corners of initial cube:
  for(int i = 0; i < ARRAYSIZE(cube.m_corners); i++) {
    cube.m_corners[i] = getCorner(BIT(i,0), BIT(i,1), BIT(i,2));
  }
  if(cube.intersectSurface()) {
    addToDoneSet(cube.m_key);
    pushCube(cube);
    return true;
  }
  return false;
}

void IsoSurfacePolygonizer::addSurfaceVertices(const StackedCube &cube) {
  if(m_tetrahedralMode) { // either decompose into tetrahedra and polygonize each:
    doTetra(cube, LBN, LTN, RBN, LBF);
    doTetra(cube, RTN, LTN, LBF, RBN);
    doTetra(cube, RTN, LTN, LTF, LBF);
    doTetra(cube, RTN, RBN, LBF, RBF);
    doTetra(cube, RTN, LBF, LTF, RBF);
    doTetra(cube, RTN, LTF, RTF, RBF);
    flushFaceBuffer();
  } else {
    m_intersectingCubeTable.add(cube);
    doCube1(m_intersectingCubeTable.last());
  }
}

// testface: given cube at lattice (i, j, k), and four corners of face,
// if surface crosses face, compute other four corners of adjacent cube
// and add new cube to cube stack
void IsoSurfacePolygonizer::testFace(int i, int j, int k, const StackedCube &oldCube, CubeFace face, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
  static BYTE      facebit[6] = {0, 0, 1, 1, 2, 2};
  const  BYTE      bit        = facebit[face];
  const  GridLabel c1Label    = oldCube.m_corners[c1]->m_label;

  // test if no surface crossing, cube out of bounds, or already visited:
  if(oldCube.m_corners[c2]->m_label == c1Label
  && oldCube.m_corners[c3]->m_label == c1Label
  && oldCube.m_corners[c4]->m_label == c1Label) {
    return;
  }

  // create new cube:
  StackedCube newCube(i, j, k);
  if(!addToDoneSet(newCube.m_key)) {
    return;
  }
  newCube.m_corners[FLIP(c1, bit)] = oldCube.m_corners[c1];
  newCube.m_corners[FLIP(c2, bit)] = oldCube.m_corners[c2];
  newCube.m_corners[FLIP(c3, bit)] = oldCube.m_corners[c3];
  newCube.m_corners[FLIP(c4, bit)] = oldCube.m_corners[c4];
  for(int n = 0; n < ARRAYSIZE(newCube.m_corners); n++) {
    if(newCube.m_corners[n] == NULL) {
      newCube.m_corners[n] = getCorner(i+BIT(n,0), j+BIT(n,1), k+BIT(n,2));
    }
  }

  if(  !m_boundingBox.contains(*newCube.m_corners[LBN])
    || !m_boundingBox.contains(*newCube.m_corners[RTF])) {
    return;
  }
  pushCube(newCube);
}

// doCube: Identify all bipolar edges for all cubes, intersecting the isosurface, and save them in m_edgeMap
void IsoSurfacePolygonizer::doCube1(StackedCube &cube) {
  m_statistics.m_doCubeCalls++;
  const SimplexArray sa = s_cubetable.get(cube.getIndex());
  const UINT         n  = sa.size();
  for(UINT i = 0; i < n; i++) {
    const Simplex s = sa[i];
    UINT v[3];
    for(BYTE j = 0; j < 3; j++) {
      const IsoVertex iv = s[j];
      if(isCubeEdge(iv)) {
        v[j] = getCubeEdgeVertexId(cube, isoVertexToCubeEdge(iv));
      } else {
        showWarning(_T("vertex not on CubeEdge"));
      }
    }
  }
}

void IsoSurfacePolygonizer::doCube2(size_t index) {
  StackedCube &cube = m_intersectingCubeTable[index];
  cube.resetIndex();

  const SimplexArray sa = s_cubetable.get(cube.getIndex());
  const UINT         n  = sa.size();

#ifdef ISODEBUGGER
  m_eval.markCurrentOcta(Octagon(this, (UINT)index));
#endif // ISODEBUGGER

  for(UINT i = 0; i < n; i++) {
    const Simplex s = sa[i];
    UINT v[3];
    for(BYTE j = 0; j < 3; j++) {
      const IsoVertex iv = s[j];
      if(isCubeEdge(iv)) {
        v[j] = getCubeEdgeVertexId(cube, isoVertexToCubeEdge(iv));
      } else {
        v[j] = cube.m_corners[isoVertexToCubeCorner(iv)]->getSnapIndex();
      }
#ifdef ISODEBUGGER
      m_eval.markCurrentVertex(m_vertexArray[v[j]]);
#endif // ISODEBUGGER
    }
    putFace3(v[0], v[1], v[2]);
  }
}

// **** Tetrahedral Polygonization ****
// doTetra: triangulate the tetrahedron
// b, c, d should appear clockwise when viewed from a
void IsoSurfacePolygonizer::doTetra(const HashedCubeCorner &a, const HashedCubeCorner &b, const HashedCubeCorner &c, const HashedCubeCorner &d) {
  m_statistics.m_doTetraCalls++;

  BYTE index = 0;
  if(a.isPositive()) index |= 8;
  if(b.isPositive()) index |= 4;
  if(c.isPositive()) index |= 2;
  if(d.isPositive()) index |= 1;
  // index is now 4-bit number representing one of the 16 possible cases

  int ab, ac, ad, bc, bd, cd;
  if(hasOppositeSign(a, b)) ab = getVertexId(a, b, -1);
  if(hasOppositeSign(a, c)) ac = getVertexId(a, c, -1);
  if(hasOppositeSign(a, d)) ad = getVertexId(a, d, -1);
  if(hasOppositeSign(b, c)) bc = getVertexId(b, c, -1);
  if(hasOppositeSign(b, d)) bd = getVertexId(b, d, -1);
  if(hasOppositeSign(c, d)) cd = getVertexId(c, d, -1);
  // 14 productive tetrahedral cases (0000 and 1111 do not yield polygons

  TriangleStrip ts;

  switch(index) {
  case  0:                                         //----
  case 15:                                         //++++
    m_statistics.m_nonProduktiveCalls++;
    return;
  case  1: ts = TriangleStrip(ad,bd,cd   ); break; //---+
  case  2: ts = TriangleStrip(ac,cd,bc   ); break; //--+-
  case  3: ts = TriangleStrip(ad,bd,bc,ac); break; //--++
  case  4: ts = TriangleStrip(ab,bc,bd   ); break; //-+--
  case  5: ts = TriangleStrip(ad,ab,bc,cd); break; //-+-+
  case  6: ts = TriangleStrip(ab,ac,cd,bd); break; //-++-
  case  7: ts = TriangleStrip(ab,ac,ad   ); break; //-+++
  case  8: ts = TriangleStrip(ab,ad,ac   ); break; //+---
  case  9: ts = TriangleStrip(ab,bd,cd,ac); break; //+--+
  case 10: ts = TriangleStrip(ab,ad,cd,bc); break; //+-+-
  case 11: ts = TriangleStrip(ab,bd,bc   ); break; //+-++
  case 12: ts = TriangleStrip(ad,ac,bc,bd); break; //++--
  case 13: ts = TriangleStrip(ac,bc,cd   ); break; //++-+
  case 14: ts = TriangleStrip(ad,cd,bd   ); break; //+++-
  default: throwException(_T("%s:Invalid index:%d"), __TFUNCTION__,index);
  }
  putTriangleStrip(ts);
}

#ifdef VALIDATE_PUTFACE
bool IsoSurfacePolygonizer::checkOrientation(const Face3 &f) const {
  const Point3D &p1 = m_vertexArray[f.m_i1].m_position;
  const Point3D &p2 = m_vertexArray[f.m_i2].m_position;
  const Point3D &p3 = m_vertexArray[f.m_i3].m_position;
  const Point3D &n1 = m_vertexArray[f.m_i1].m_normal;
  const Point3D &n2 = m_vertexArray[f.m_i2].m_normal;
  const Point3D &n3 = m_vertexArray[f.m_i3].m_normal;
  Point3D c = cross(Point3D(p3 - p1), Point3D(p2 - p1));
  double s1 = c * n1, s2 = c * n2, s3 = c * n3;
  return s1 > 0;
}
#endif // VALIDATE_PUTFACE

void IsoSurfacePolygonizer::putTriangleStrip(const TriangleStrip &ts) {
  if(ts.m_count == 3) {
    putFace3(ts.m_vertexId[0], ts.m_vertexId[1], ts.m_vertexId[2]);
  } else if(!m_tetraOptimize4) {
    for(int i = 2; i < ts.m_count; i++) {
      putFace3(ts.m_vertexId[0], ts.m_vertexId[i-1], ts.m_vertexId[i]);
    }
  } else {
    const Point3D &p0    = m_vertexArray[ts.m_vertexId[0]].m_position;
    const Point3D &p1    = m_vertexArray[ts.m_vertexId[1]].m_position;
    const Point3D &p2    = m_vertexArray[ts.m_vertexId[2]].m_position;
    const Point3D &p3    = m_vertexArray[ts.m_vertexId[3]].m_position;

    if(distance(p0,p2) < distance(p1,p3)) {
      putFace3(ts.m_vertexId[0], ts.m_vertexId[1], ts.m_vertexId[2]);
      putFace3(ts.m_vertexId[0], ts.m_vertexId[2], ts.m_vertexId[3]);
    } else {
      putFace3(ts.m_vertexId[1], ts.m_vertexId[2], ts.m_vertexId[3]);
      putFace3(ts.m_vertexId[1], ts.m_vertexId[3], ts.m_vertexId[0]);
    }
  }
}

void IsoSurfacePolygonizer::putTriangleStripR(const TriangleStrip &ts) {
  for(int i = 2; i < ts.m_count; i++) {
    putFace3R(ts.m_vertexId[0], ts.m_vertexId[i-1], ts.m_vertexId[i]);
  }
}

void IsoSurfacePolygonizer::flushFaceBuffer() {
  for(size_t i = 0; i < m_face3Buffer.size(); i++) {
    const Face3 &f      = m_face3Buffer[i];
    const UINT   findex = (UINT)m_faceArray.size();
    m_faceArray.add(f);
#ifdef ISODEBUGGER
    m_eval.receiveFace(f);
#endif // ISODEBUGGER
  }
  clearFaceBuffer();
}

void IsoSurfacePolygonizer::flushFaceArray() {
#ifndef ISODEBUGGER
  const size_t n = m_faceArray.size();
  for(size_t i = 0;i < n; i++) {
    m_eval.receiveFace(m_faceArray[i]);
  }
#endif // ISODEBUGGER
}

void IsoSurfacePolygonizer::cleanup() {
  __super::cleanup();
  m_cubeStack.clear();
  m_face3Buffer.clear();
}

void IsoSurfacePolygonizer::pushCube(const StackedCube &cube) {
#ifdef VALIDATE_CUBES
  cube.validate();
#endif //  VALIDATE_CUBES

  m_cubeStack.push(cube);
  m_statistics.m_cubeCount++;
#ifdef DUMP_CUBES
  debugLog(_T("pushCube():%s"), cube.toString().cstr());
#endif
}

// getCorner: return cached/new corner with the given lattice location
const HashedCubeCorner *IsoSurfacePolygonizer::getCorner(int i, int j, int k) {
  const Point3DKey key(i, j, k);
  const HashedCubeCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedCubeCorner corner(key, getCornerPoint(key));
    corner.setValue(evaluate(corner));
    m_cornerMap.put(key, corner);
    return m_cornerMap.get(key);
  }
}

// find point on surface, beginning search at start
Point3D IsoSurfacePolygonizer::findStartPoint(const Point3D &start) {
  const IsoSurfaceTest in  = findStartPoint(true , start);
  const IsoSurfaceTest out = findStartPoint(false, start);
  if(!in.m_ok || !out.m_ok) {
    throwException(_T("Cannot find starting point1"));
  }
  return convergeStartPoint(in, out, RES + 20);
}

// findStartPoint: search for point with value of sign specified by positive-parameter
IsoSurfaceTest IsoSurfacePolygonizer::findStartPoint(bool positive, const Point3D &p) {
  IsoSurfaceTest result(p);
#define STEPCOUNT 200000
  const double step  = root(1000,STEPCOUNT); // multiply range by this STEPCOUNT times
                                             // range will end up with value 10000*cellSize

  const GridLabel label = positive ? V_POSITIVE : V_NEGATIVE;
  double       range = m_cellSize;
  for(int i = 0; i < STEPCOUNT; i++) {
    result.x = p.x + randDouble(-range, range, m_rnd);
    result.y = p.y + randDouble(-range, range, m_rnd);
    result.z = p.z + randDouble(-range, range, m_rnd);
    result.setValue(evaluate(result));
    if(result.m_label == label) {
      return result;
    }
    range *= step; // slowly expand search outwards
  }
  result.m_ok = false;
  return result;
}

UINT IsoSurfacePolygonizer::findLeftBottomFarCube(const CompactArray<StackedCube> &a) const {
  const size_t n = a.size();
  if(n == 0) {
    return -1;
  }
  UINT bestIndex = 0;
  Point3DKey bestKey = a[bestIndex].m_key;
  for(UINT i = 1; i < n; i++) {
    const Point3DKey &key = a[i].m_key;
    if(Point3DKey::compare(key, bestKey) < 0) {
      bestIndex = i;
      bestKey   = key;
    }
  }
  return bestIndex;
}

// return false if already set; otherwise add to m_centerSet and return true
bool IsoSurfacePolygonizer::addToDoneSet(const Point3DKey &key) {
  if(m_cubesDoneSet.contains(key)) {
    return false;
  } else {
    m_cubesDoneSet.add(key);
    return true;
  }
}

UINT IsoSurfacePolygonizer::getCubeEdgeVertexId(StackedCube &cube, CubeEdge ce) {
  const CubeEdgeInfo     &edge = cubeEdgeTable[ce];
  const HashedCubeCorner &c0   = *cube.m_corners[edge.corner[0]];
  const HashedCubeCorner &c1   = *cube.m_corners[edge.corner[1]];
  return getVertexId(c0, c1, edge.coordIndex);
}

// getVertexId: return index for vertex on edge:
// c1.m_value and c2.m_value are presumed of different sign
// return saved index if any; else calculate and save vertex for later use
UINT IsoSurfacePolygonizer::getVertexId(const HashedCubeCorner &c0, const HashedCubeCorner &c1, BYTE coordIndex) {
  const CubeEdgeHashKey edgeKey(c0.m_key, c1.m_key);
  const UINT *p = m_edgeMap.get(edgeKey);
  if(p != NULL) {
    m_statistics.m_edgeHits++;
    return *p; // previously computed
  }
#ifdef VALIDATE_OPPOSITESIGN
  if(!hasOppositeSign(c0, c1)) {
    throwException(_T("%s:Corners doesn't have opposite sign. c0:%s, c1:%s")
      , __TFUNCTION__
      , c0.toString().cstr(), c1.toString().cstr());
  }
#endif // VALIDATE_OPPOSITESIGN
#define CHECKSNAPDIM(coord)                                             \
  if((dist = fabs(zp.coord - c0.coord)) <= m_maxSnapDistance) {         \
    snapCorner = &c0;                                                   \
  } else if((dist = fabs(zp.coord - c1.coord)) <= m_maxSnapDistance) {  \
    snapCorner = &c1;                                                   \
  } else {                                                              \
    snapCorner = NULL;                                                  \
  }

  const Point3D           zp = converge(c0, c1); // position;
  double                  dist;
  const HashedCubeCorner *snapCorner;
  switch(coordIndex) {
  case 0 : CHECKSNAPDIM(x); break;
  case 1 : CHECKSNAPDIM(y); break;
  case 2 : CHECKSNAPDIM(z); break;
  default: snapCorner = NULL; break;
  }
  UINT result;
  if(snapCorner == NULL) {
    result = (UINT)m_vertexArray.size();
    m_vertexArray.add(IsoSurfaceVertex().setPosition(zp, getNormal(zp)));
  } else if(snapCorner->hasSnapVertex()) {
    result = snapCorner->getSnapIndex();
    if(dist < snapCorner->m_shortestSnapDistance) {
      snapCorner->setSnapDistance(dist);
      m_vertexArray[result].setPosition(zp, getNormal(zp));
    }
  } else { // no isoVertex snapped to this corner yet. Create one
    result = (UINT)m_vertexArray.size();
    m_vertexArray.add(IsoSurfaceVertex().setPosition(zp, getNormal(zp)));
    snapCorner->setSnapDistance(dist, result);
  }
  m_edgeMap.put(edgeKey, result);
  return result;
}

// getNormal: calculate unit length surface normal at point
Point3D IsoSurfacePolygonizer::getNormal(const Point3D &point) {
  const double f0 = evaluate(point);
  Point3D p=point, result;
                 p.x += m_delta; result.x = evaluate(p) - f0;
  p.x = point.x; p.y += m_delta; result.y = evaluate(p) - f0;
  p.y = point.y; p.z += m_delta; result.z = evaluate(p) - f0;
  return result.normalize();
}

// converge: from two points of differing sign, converge to zero crossing
// Assume sign(v1) = -sign(v2)
Point3D IsoSurfacePolygonizer::converge(const Point3DWithValue &p1, const Point3DWithValue &p2, int itCount) {
#ifdef _DEBUG
  if(!hasOppositeSign(p1, p2)) {
    throwInvalidArgumentException(__TFUNCTION__, _T("%s has same sign as %s"), p1.toString().cstr(), p2.toString().cstr());
  }
#endif // _DEBUG
  Point3DWithValue x1, x2;
  if(p1.isPositive()) {
    x1 = p1;
    x2 = p2;
  } else {
    x1 = p2;
    x2 = p1;
  }
  for(int i = itCount = max(itCount, RES);;) {
    const Point3D x = x2 - (x2-x1) * (x2.m_value/(x2.m_value-x1.m_value));
    if(i-- == 0) {
      return x;
    }
    const double y = evaluate(x);
    if(y > 0) {
      (x1 = x).setValue(y);
    } else if(y < 0) {
      (x2 = x).setValue(y);
    } else {
      m_statistics.m_zeroHits++;
      return x;
    }
  }
}

// convergeStartPoint: from two points of differing sign, converge to zero crossing
Point3D IsoSurfacePolygonizer::convergeStartPoint(const Point3DWithValue &p1, const Point3DWithValue &p2, int itCount) {
  Point3D pos, neg;
  if(p1.isPositive()) {
    pos = p1;
    neg = p2;
  } else {
    pos = p2;
    neg = p1;
  }
  for(int i = itCount = itCount;;) {
    const Point3D result = (pos + neg)/2;
    if(i-- == 0) {
      return result;
    }
    const double v = evaluate(result);
    if(v > 0.0) {
      pos = result;
    } else if(v < 0) {
      neg = result;
    } else {
      m_statistics.m_zeroHits++;
      return result;
    }
  }
}

#ifdef DUMP_CORNERMAP
typedef Entry<Point3DKey, HashedCubeCorner> CornerMapEntry;

static int cubeCornerCmp(const HashedCubeCorner * const &c1, const HashedCubeCorner * const &c2) {
  return point3DKeyCmp(c1->m_key,c2->m_key);
}

void PolygonizerBase::dumpCornerMap() const {
  debugLog(_T("CornerMap\n"));
  CompactArray<const HashedCubeCorner*> tmpArray(m_cornerMap.size());
  for(Iterator<CornerMapEntry> it = m_cornerMap.getEntryIterator(); it.hasNext();) {
    const CornerMapEntry &e = it.next();
    tmpArray.add(&e.getValue());
  }
  tmpArray.sort(cubeCornerCmp);

  const size_t n = tmpArray.size();
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%s\n"), tmpArray[i]->toString().cstr());
  }
}
#endif // DUMP_CORNERMAP

#ifdef DUMP_EDGEMAP
typedef Entry<CubeEdgeHashKey, UINT> EdgeMapEntry;
class SortedCubeEdge : public CubeEdgeHashKey {
public:
  int m_index;
  SortedCubeEdge(const EdgeMapEntry &e) : CubeEdgeHashKey(e.getKey()), m_index(e.getValue()) {
  }
};

static int sortedCubeEdgeCmp(const SortedCubeEdge &e1, const SortedCubeEdge &e2) {
  int c;
  if(c = Point3DKey::compare(e1.getKey1(),e2.getKey1())) return c;
  return Point3DKey::compare(e1.getKey2(),e2.getKey2());
}

void PolygonizerBase::dumpEdgeMap() const {
  debugLog(_T("EdgeMap\n"));
  Array<SortedCubeEdge> tmpArray(m_edgeMap.size());
  for(Iterator<EdgeMapEntry> it = m_edgeMap.getEntryIterator(); it.hasNext();) {
    tmpArray.add(it.next());
  }
  tmpArray.sort(sortedCubeEdgeCmp);
  const size_t n = tmpArray.size();
  for(size_t i = 0; i < n; i++) {
    const SortedCubeEdge &e = tmpArray[i];
    debugLog(_T("%s:%6d\n"), e.toString().cstr(), e.m_index);
  }
}
#endif

#ifdef DUMP_VERTEXARRAY
void PolygonizerBase::dumpVertexArray() const {
  debugLog(_T("VertexArray\n"));
  const size_t n = m_vertexArray.size();
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%6zu:%s\n"), i, m_vertexArray[i].toString().cstr());
  }
}
#endif // DUMP_VERTEXARRAY

#ifdef DUMP_FACEARRAY
void PolygonizerBase::dumpFaceArray() const {
  debugLog(_T("FaceArray\n"));
  const size_t n = m_faceArray.size();
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%6zu:%s\n"), i, m_faceArray[i].toString().cstr());
  }
}
#endif // DUMP_FACEARRAY

String cubeCornerToString(CubeCorner cb) {
#define caseStr(c) case c: return #c
  switch(cb) {
  caseStr(LBN);
  caseStr(RBN);
  caseStr(LBF);
  caseStr(RBF);
  caseStr(LTN);
  caseStr(RTN);
  caseStr(LTF);
  caseStr(RTF);
  default: return format(_T("unknown cubeCorner:%d"), cb);
  }
}

String cubeEdgeToString(CubeEdge ce) {
  switch(ce) {
  caseStr(BN);
  caseStr(LB);
  caseStr(RB);
  caseStr(BF);
  caseStr(LN);
  caseStr(RN);
  caseStr(LF);
  caseStr(RF);
  caseStr(TN);
  caseStr(LT);
  caseStr(RT);
  caseStr(TF);
  default: return format(_T("unknown cubeEdge:%d"), ce);
  }
}

String ivToString(IsoVertex iv) {
  return isCubeCorner(iv) ? cubeCornerToString(isoVertexToCubeCorner(iv)) : cubeEdgeToString(isoVertexToCubeEdge(iv));
}

// ------------------------------------- Point3DWithValue --------------------------------------
String Point3DWithValue::toString(int precision) const {
  return format(_T("P:(%+.*le,%+.*le,%+.*le), V:%+.*le (%c)")
               ,precision, x
               ,precision, y
               ,precision, z
               ,precision, m_value
               ,getLabelChar(m_label)
               );
}

String Face3::toString() const {
  return format(_T("(%5u,%5u,%5u)"), m_i1, m_i2, m_i3);
}

String IsoSurfaceVertex::toString(int precision) const {
  return format(_T("P:(%+.*le,%+.*le,%+.*le), N:(%+.*le,%+.*le,%+.*le)")
               ,precision,m_position.x,precision,m_position.y,precision,m_position.z
               ,precision,m_normal.x  ,precision,m_normal.y  ,precision,m_normal.z
               );
}

int Point3DKey::compare(const Point3DKey &k1, const Point3DKey &k2) { // static
  int c;
  if(c = k1.i - k2.i) return c;
  if(c = k1.j - k2.j) return c;
  return k1.k - k2.k;
}

double Point3DKey::distance(const Point3DKey &k1, const Point3DKey &k2) { // static
  return sqrt((sqr((double)k1.i - k2.i) + sqr((double)k1.j - k2.j) + sqr((double)k1.k - k2.k)));
}

String Point3DKey::toString() const {
  return format(_T("(% 5d,% 5d,% 5d)"), i, j, k);
}

void CubeEdgeHashKey::checkAndSwap() {
  if(Point3DKey::compare(m_key1, m_key2)) {
    std::swap(m_key1, m_key2);
  }
}

String CubeEdgeHashKey::toString() const {
  return format(_T("EdgeKey:(%s,%s)"), m_key1.toString().cstr(), m_key2.toString().cstr());
}

void HashedCubeCorner::setSnapDistance(double dist, int vertexIndex) const {
  if(dist < 0) {
    throwInvalidArgumentException(__TFUNCTION__, _T("dist=%le"), dist);
  }
  if(hasSnapVertex()) {
    if(vertexIndex >= 0) {
      throwInvalidArgumentException(__TFUNCTION__, _T("vertexIndex=%d"), vertexIndex);
    }
    if(dist > getSnapDistance()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("dist=%le"), dist);
    }
    m_shortestSnapDistance = dist;
  } else {
    if(vertexIndex < 0) {
      throwInvalidArgumentException(__TFUNCTION__, _T("vertexIndex=%d"), vertexIndex);
    }
    m_snappedIsoVertexIndex = vertexIndex;
    m_shortestSnapDistance  = dist;
  }
}

String HashedCubeCorner::toString(int precision) const {
  String result = format(_T("CubeCorner:(Key:%s, %s)")
                        ,m_key.toString().cstr()
                        ,__super::toString(precision).cstr()
                        );
  if(hasSnapVertex()) {
    result += format(_T("->%4u, dist:%.*le"), getSnapIndex(), precision, getSnapDistance());
  }
  return result;
}

String TriangleStrip::toString() const {
  String result;
  for(int i = 0; i < m_count; i++) {
    if(i) {
      result += _T(",");
    }
    result += format(_T("%4u"), m_vertexId[i]);
  }
  return result;
}

String Simplex::toString() const {
  const Simplex &s = *this;
  return format(_T("(%s,%s,%s)"), ivToString(s[0]).cstr(), ivToString(s[1]).cstr(), ivToString(s[2]).cstr());
}

String SimplexArray::toString() const {
  const UINT   n         = size();
  const TCHAR *delimiter = NULL;
  String       result;
  for(UINT i = 0; i < n; i++) {
    if(delimiter) result += delimiter; else delimiter = _T(",");
    result += (*this)[i].toString();
  }
  return format(_T("{%s}"), result.cstr());
}

class CubeCornerStringifier : public AbstractStringifier<UINT> {
public:
  String toString(const UINT &e) {
    return cubeCornerToString((CubeCorner)e);
  }
};

UINT StackedCube::calculateIndex() const {
  const HashedCubeCorner * const *first = m_corners, * const *ccp = &LASTVALUE(m_corners);
  UINT index = (*ccp)->m_label;
  while(--ccp >= first) {
    index *= 3;
    index += (*ccp)->m_label;
  }
  return index;
}

SimplexArray StackedCube::getSimplexArray(UINT index) { // static
  return PolygonizerBase::getExtendedLookupTable().get(index);
}

#ifdef VALIDATE_CUBES
#define TOLERANCE 1e-10
void StackedCube::verifyCubeConstraint(int line, const TCHAR *expr) const {
  MessageBox(0, format(_T("Verification\r\n\r\n%s\r\n\r\nat %s\r\nline %d failed.\nCube:\n%s")
            ,expr, __TFILE__, line, toString().cstr()).cstr()
            ,_T("Verification failed")
            ,MB_ICONSTOP);
  abort();
}

#ifdef verify
#undef verify
#endif
#define verify(exp) (void)( (exp) || (verifyCubeConstraint(__LINE__, _T(#exp)), 0) )

void StackedCube::validate() const {
  const HashedCubeCorner *lbn = m_corners[LBN];
  const HashedCubeCorner *rbn = m_corners[RBN];
  const HashedCubeCorner *lbf = m_corners[LBF];
  const HashedCubeCorner *rbf = m_corners[RBF];
  const HashedCubeCorner *ltn = m_corners[LTN];
  const HashedCubeCorner *rtn = m_corners[RTN];
  const HashedCubeCorner *ltf = m_corners[LTF];
  const HashedCubeCorner *rtf = m_corners[RTF];

  verify(lbn != NULL);
  verify(rbn != NULL);
  verify(lbf != NULL);
  verify(rbf != NULL);
  verify(ltn != NULL);
  verify(rtn != NULL);
  verify(ltf != NULL);
  verify(rtf != NULL);
  verify(fabs(lbn->x - ltn->x) <= TOLERANCE); // left   same x
  verify(fabs(lbn->x - lbf->x) <= TOLERANCE);
  verify(fabs(lbn->x - ltf->x) <= TOLERANCE);
  verify(     lbn->m_key.i == ltn->m_key.i ); //        same i
  verify(     lbn->m_key.i == lbf->m_key.i );
  verify(     lbn->m_key.i == ltf->m_key.i );

  verify(fabs(rbn->x - rtn->x) <= TOLERANCE); // right  same x
  verify(fabs(rbn->x - rbf->x) <= TOLERANCE);
  verify(fabs(rbn->x - rtf->x) <= TOLERANCE);
  verify(     rbn->m_key.i == rtn->m_key.i ); //        same i
  verify(     rbn->m_key.i == rbf->m_key.i );
  verify(     rbn->m_key.i == rtf->m_key.i );

  verify(fabs(lbn->z - rbn->z) <= TOLERANCE); // bottom same z
  verify(fabs(lbn->z - rbf->z) <= TOLERANCE);
  verify(fabs(lbn->z - lbf->z) <= TOLERANCE);
  verify(     lbn->m_key.k == rbn->m_key.k ); //        same k
  verify(     lbn->m_key.k == rbf->m_key.k );
  verify(     lbn->m_key.k == lbf->m_key.k );

  verify(fabs(ltn->z - rtn->z) <= TOLERANCE); // top    same z
  verify(fabs(ltn->z - rtf->z) <= TOLERANCE);
  verify(fabs(ltn->z - ltf->z) <= TOLERANCE);
  verify(     ltn->m_key.k == rtn->m_key.k ); //        same k
  verify(     ltn->m_key.k == rtf->m_key.k );
  verify(     ltn->m_key.k == ltf->m_key.k );

  verify(fabs(lbn->y - rbn->y) <= TOLERANCE); // near   same y
  verify(fabs(lbn->y - ltn->y) <= TOLERANCE);
  verify(fabs(lbn->y - rtn->y) <= TOLERANCE);
  verify(     lbn->m_key.j == rbn->m_key.j ); //        same j
  verify(     lbn->m_key.j == ltn->m_key.j );
  verify(     lbn->m_key.j == rtn->m_key.j );

  verify(fabs(lbf->y - rbf->y) <= TOLERANCE); // far    same y
  verify(fabs(lbf->y - ltf->y) <= TOLERANCE);
  verify(fabs(lbf->y - rtf->y) <= TOLERANCE);
  verify(     lbf->m_key.j == rbf->m_key.j ); //        same j
  verify(     lbf->m_key.j == ltf->m_key.j );
  verify(     lbf->m_key.j == rtf->m_key.j );

  verify(lbn->x < rtf->x);
  verify(lbn->y < rtf->y);
  verify(lbn->z < rtf->z);
  verify(lbn->m_key.i + 1 == rtf->m_key.i);
  verify(lbn->m_key.j + 1 == rtf->m_key.j);
  verify(lbn->m_key.k + 1 == rtf->m_key.k);
}

#endif // VALIDATE_CUBES

SimplexArray StackedCube::has4ZeroCorners(CubeFace cf) const {
  const CubeFaceInfo &info = cubeFaceTable[cf];
  static const BYTE empty[] = { 0 };
  return (m_corners[info.c1]->m_label == V_ZERO)
      && (m_corners[info.c2]->m_label == V_ZERO)
      && (m_corners[info.c3]->m_label == V_ZERO)
      && (m_corners[info.c4]->m_label == V_ZERO)
    ? SimplexArray(info.vertex)
    : SimplexArray(empty);
}

StackedCube &StackedCube::copy(const StackedCube &src, const HashedCubeCornerMap &cornerMap) {
  *this = src;
  for(UINT i = 0; i < ARRAYSIZE(m_corners); i++) {
    m_corners[i] = cornerMap.get(src.m_corners[i]->m_key);
  }
  return *this;
}

String StackedCube::toString(int precision) const {
  String result = format(_T("Cube:Key:%s,Size:%s, Index0:%4d, Index:%4d\n")
                        ,m_key.toString().cstr()
                        ,getSize().toString(precision).cstr()
                        ,m_index0, m_index
                        );
  if(m_index >= 0) {
    result += getSimplexArray(m_index).toString();
    result += _T("\n");
  }
  for(int i = 0; i < ARRAYSIZE(m_corners); i++) {
    const HashedCubeCorner *c = m_corners[i];
    result += format(_T("  %d:(%s):%s\n")
                    ,i, cubeCornerToString((CubeCorner)i).cstr()
                    ,c ? c->toString(precision).cstr():_T("NULL")
                    );
  }
  return result;
}

static String intArrayToString(const CompactIntArray &a) {
  if(a.size() == 0) {
    return EMPTYSTRING;
  } else {
    String result = format(_T("%d:%s"), (int)0, format1000(a[0]).cstr());
    for(size_t i = 1; i < a.size(); i++) {
      result += format(_T(", %zd:%s"), i, format1000(a[i]).cstr());
    }
    return result;
  }
}

void IsoSurfacePolygonizer::saveStatistics(double startTime) {
  m_statistics.m_threadTime   = getThreadTime() - startTime;
  m_statistics.m_vertexCount  = (UINT)m_vertexArray.size();
  m_statistics.m_cornerCount  = (UINT)m_cornerMap.size();
  m_statistics.m_edgeCount    = (UINT)m_edgeMap.size();
  m_statistics.m_hashStat     = _T("  CornerMap:") + intArrayToString(m_cornerMap.getLength())    + _T("\n")
                              + _T("  EdgeMap  :") + intArrayToString(m_edgeMap.getLength())      + _T("\n")
                              + _T("  DoneSet  :") + intArrayToString(m_cubesDoneSet.getLength()) + _T("\n");
  m_statistics.m_faceCount    = (UINT)m_faceArray.size();
}

PolygonizerStatistics::PolygonizerStatistics() {
  clear();
}

void PolygonizerStatistics::clear() {
  m_threadTime         = 0;
  m_cubeCount          = 0;
  m_faceCount          = 0;
  m_vertexCount        = 0;
  m_cornerCount        = 0;
  m_edgeCount          = 0;
  m_cornerHits         = 0;
  m_edgeHits           = 0;
  m_zeroHits           = 0;
  m_evalCount          = 0;
  m_doCubeCalls        = 0;
  m_doTetraCalls       = 0;
  m_nonProduktiveCalls = 0;
  m_hashStat           = EMPTYSTRING;
}

String PolygonizerStatistics::toString() const {
  return format(_T("Polygonizer statistics\n"
                   "Time                : %.2lf sec.\n"
                   "CubeCount           : %s\n"
                   "FaceCount           : %s\n"
                   "#Vertices           : %s\n"
                   "#Corners            : %s\n"
                   "#Edges              : %s\n"
                   "#Corner hits        : %s\n"
                   "#Edge hits          : %s\n"
                   "#ZeroHits           : %s\n"
                   "#Functionevaluations: %s\n"
                   "#doCube calls       : %s\n"
                   "#doTetra calls      : %s\n"
                   "#Nonproduktive calls: %s\n"
                   "HashStatistics:\n"
                   "%s")
              ,m_threadTime / 1000000
              ,format1000(m_cubeCount         ).cstr()
              ,format1000(m_faceCount         ).cstr()
              ,format1000(m_vertexCount       ).cstr()
              ,format1000(m_cornerCount       ).cstr()
              ,format1000(m_edgeCount         ).cstr()
              ,format1000(m_cornerHits        ).cstr()
              ,format1000(m_edgeHits          ).cstr()
              ,format1000(m_zeroHits          ).cstr()
              ,format1000(m_evalCount         ).cstr()
              ,format1000(m_doCubeCalls       ).cstr()
              ,format1000(m_doTetraCalls      ).cstr()
              ,format1000(m_nonProduktiveCalls).cstr()
              ,m_hashStat.cstr()
              );
}

#ifdef ISODEBUGGER
String Octagon::toString(int precision) const {
  return isEmpty() ? _T("Octa:---")
                   : format(_T("Octa:%s"), getCube()->toString(precision).cstr());
}

String Tetrahedron::toString() const {
  return isEmpty() ? _T("Tetra:---")
                   : format(_T("Tetra:Corners:[%s,%s,%s,%s]")
                           , cubeCornerToString(m_corner[0]).cstr()
                           , cubeCornerToString(m_corner[1]).cstr()
                           , cubeCornerToString(m_corner[2]).cstr()
                           , cubeCornerToString(m_corner[3]).cstr());
}
#endif // ISODEBUGGER

};
