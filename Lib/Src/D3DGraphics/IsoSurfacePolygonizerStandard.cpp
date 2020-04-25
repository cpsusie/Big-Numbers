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
#include <D3DGraphics/IsoSurfacePolygonizerStandard.h>

namespace ISOSURFACE_POLYGONIZER_STANDARD {

#define RES 10 // # converge iterations

typedef struct {
  CubeCorner corner1;
  CubeCorner corner2;
  CubeFace   leftface;
  CubeFace   rightface;
  CubeEdge   nextClockwiseEdge[6]; // indexed by face
} CubeEdgeInfo;

static const CubeEdgeInfo cubeEdgeTable[12] = {
  { LBN,LBF, BFACE, LFACE, { LF, NN, BN, NN, NN, NN } }  // left   bottom edge
 ,{ LTN,LTF, LFACE, TFACE, { LN, NN, NN, TF, NN, NN } }  // left   top    edge
 ,{ LBN,LTN, LFACE, NFACE, { LB, NN, NN, NN, TN, NN } }  // left   near   edge
 ,{ LBF,LTF, FFACE, LFACE, { LT, NN, NN, NN, NN, BF } }  // left   far    edge
 ,{ RBN,RBF, RFACE, BFACE, { NN, RN, BF, NN, NN, NN } }  // right  bottom edge
 ,{ RTN,RTF, TFACE, RFACE, { NN, RF, NN, TN, NN, NN } }  // right  top    edge
 ,{ RBN,RTN, NFACE, RFACE, { NN, RT, NN, NN, BN, NN } }  // right  near   edge
 ,{ RBF,RTF, RFACE, FFACE, { NN, RB, NN, NN, NN, TF } }  // right  far    edge
 ,{ LBN,RBN, NFACE, BFACE, { NN, NN, RB, NN, LN, NN } }  // bottom near   edge
 ,{ LBF,RBF, BFACE, FFACE, { NN, NN, LB, NN, NN, RF } }  // bottom far    edge
 ,{ LTN,RTN, TFACE, NFACE, { NN, NN, NN, LT, RN, NN } }  // top    near   edge
 ,{ LTF,RTF, FFACE, TFACE, { NN, NN, NN, RT, NN, LF } }  // top    far    edge
};

#define HASHSIZE 100001
#define BIT(i, bit) (((i)>>(bit))&1)
#define FLIP(i,bit) ((i)^(1<<(bit))) // flip the given bit of i

PolygonizerCubeArrayTable IsoSurfacePolygonizer::s_cubetable;

IsoSurfacePolygonizer::IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval)
: m_eval(eval)
, m_cornerMap(HASHSIZE)
{
}

IsoSurfacePolygonizer::~IsoSurfacePolygonizer() {
}

void IsoSurfacePolygonizer::polygonize(const Point3D &start
                                      ,double         cellSize
                                      ,const Cube3D  &boundingBox
                                      ,bool           tetrahedralMode
                                      ,bool           tetraOptimize4
                                      ) {

  const double startTime = getThreadTime();

#ifndef _DEBUG
  m_rnd.randomize();
#endif // _DEBUG

  m_cellSize         = cellSize;
  m_boundingBox      = boundingBox;
  m_delta            = cellSize/(double)(RES*RES);
  m_tetrahedralMode  = tetrahedralMode;
  m_tetraOptimize4   = tetraOptimize4;

  m_statistics.clear();
  resetTables();

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
#ifdef ISODEBUGGER
    m_eval.markCurrentOcta(cube);
#endif // ISODEBUGGER
    addSurfaceVertices(cube);

    // test six face directions, maybe add to stack:
    testFace(cube.m_key.i-1 , cube.m_key.j   , cube.m_key.k   , cube, LFACE, LBN, LBF, LTN, LTF);
    testFace(cube.m_key.i+1 , cube.m_key.j   , cube.m_key.k   , cube, RFACE, RBN, RBF, RTN, RTF);
    testFace(cube.m_key.i   , cube.m_key.j-1 , cube.m_key.k   , cube, BFACE, LBN, LBF, RBN, RBF);
    testFace(cube.m_key.i   , cube.m_key.j+1 , cube.m_key.k   , cube, TFACE, LTN, LTF, RTN, RTF);
    testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k-1 , cube, NFACE, LBN, LTN, RBN, RTN);
    testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k+1 , cube, FFACE, LBF, LTF, RBF, RTF);
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
    cube.m_corners[i] = getCorner(BIT(i,2), BIT(i,1), BIT(i,0));
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
  } else {
    doCube(cube);         // or polygonize the cube directly:
  }
  flushFaceBuffer();
}

// testface: given cube at lattice (i, j, k), and four corners of face,
// if surface crosses face, compute other four corners of adjacent cube
// and add new cube to cube stack
void IsoSurfacePolygonizer::testFace(int i, int j, int k, const StackedCube &oldCube, CubeFace face, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
  static int facebit[6]    = {2, 2, 1, 1, 0, 0};
  int        bit           = facebit[face];
  const  GridLabel c1Label = oldCube.m_corners[c1]->m_label;

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
      newCube.m_corners[n] = getCorner(i+BIT(n,2), j+BIT(n,1), k+BIT(n,0));
    }
  }

  if(  !m_boundingBox.contains(*newCube.m_corners[LBN])
    || !m_boundingBox.contains(*newCube.m_corners[RTF])) {
    return;
  }
  pushCube(newCube);
}

// doCube: triangulate the cube directly, without decomposition
void IsoSurfacePolygonizer::doCube(const StackedCube &cube) {
  m_statistics.m_doCubeCalls++;
  const PolygonizerCubeArray &polys = s_cubetable.get(cube.getIndex());
  for(size_t i = 0; i < polys.size(); i++) {
    int a = -1, b = -1;
    const CompactArray<char> &vertexEdges = polys[i];
    for(size_t j = 0; j < vertexEdges.size(); j++) {
      const CubeEdgeInfo     &edge = cubeEdgeTable[vertexEdges[j]];
      const HashedCubeCorner &c1   = *cube.m_corners[edge.corner1];
      const HashedCubeCorner &c2   = *cube.m_corners[edge.corner2];
      const int c = getVertexId(c1, c2);
      if(j >= 2) {
        putFace3(c, b, a);
      } else {
        a = b;
      }
      b = c;
    }
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
  if(hasOppositeSign(a, b)) ab = getVertexId(a, b);
  if(hasOppositeSign(a, c)) ac = getVertexId(a, c);
  if(hasOppositeSign(a, d)) ad = getVertexId(a, d);
  if(hasOppositeSign(b, c)) bc = getVertexId(b, c);
  if(hasOppositeSign(b, d)) bd = getVertexId(b, d);
  if(hasOppositeSign(c, d)) cd = getVertexId(c, d);
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
  default: throwException(_T("doTetra:Invalid index:%d"), index);
  }
  putTriangleStrip(ts);
}

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

void IsoSurfacePolygonizer::resetTables() {
  m_cubeStack.clear();
  m_vertexArray.clear();
  m_cubesDoneSet.clear();
  m_cornerMap.clear();
  m_edgeMap.clear();
  m_face3Buffer.clear();
  m_faceArray.clear();
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

Point3D IsoSurfacePolygonizer::getCornerPoint(int i, int j, int k) const {
  Point3D result;
  result.x = m_start.x+((double)i)*m_cellSize;
  result.y = m_start.y+((double)j)*m_cellSize;
  result.z = m_start.z+((double)k)*m_cellSize;
  return result;
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
  double       range = m_cellSize;
  for(int i = 0; i < STEPCOUNT; i++) {
    result.x = p.x + randDouble(-range, range, m_rnd);
    result.y = p.y + randDouble(-range, range, m_rnd);
    result.z = p.z + randDouble(-range, range, m_rnd);
    result.setValue(evaluate(result));
    if(result.isPositive() == positive) {
      return result;
    }
    range *= step; // slowly expand search outwards
  }
  result.m_ok = false;
  return result;
}

// otherFace: Return face adjoining edge that is not the given face
static CubeFace otherFace(CubeEdge edge, CubeFace face) {
  const CubeEdgeInfo &cedge = cubeEdgeTable[edge];
  CubeFace other = cedge.leftface;
  return face == other ? cedge.rightface : other;
}

// makeCubeTable: Create the 256 entry table for cubical polygonization
PolygonizerCubeArray::PolygonizerCubeArray(UINT index) : m_index((BYTE)index) {
  BitSet16 done;

#define OPPOSITE_SIGN(i,cep) ((((i)>>cep->corner1)&1) != (((i)>>cep->corner2)&1))

  for(int e = 0; e < ARRAYSIZE(cubeEdgeTable); e++) {
    const CubeEdgeInfo *cep = &cubeEdgeTable[e];
    if(!done.contains(e) && OPPOSITE_SIGN(index,cep)) {
      CompactArray<char> vertexEdges;
      // get face that is to right of edge from pos to neg corner:
      CubeFace face = (index&(1<<cep->corner1)) ? cep->rightface : cep->leftface;
      for(;;) {
        const CubeEdge edge = cep->nextClockwiseEdge[face];
        done.add(edge);
        cep = &cubeEdgeTable[edge];
        if(OPPOSITE_SIGN(index, cep)) {
          vertexEdges.add(edge);
          if(edge == e) {
            break;
          }
          face = otherFace(edge, face);
        }
      }
      add(vertexEdges);
    }
  }
}

#ifdef DUMP_CUBETABLE
String PolygonizerCubeArray::toString() const {
  const PolygonizerCubeArray &a = *this;
  String result;
  for(size_t j = 0; j < a.size(); j++) {
    result += format(_T("%2d:"), j);
    const CompactArray<char> &b = a[j];
    for(int k = 0; k < b.size(); k++) {
      result += format(_T("%d "), b[k]);
    }
    result += _T("\n");
  }
  return result;
}

String PolygonizerCubeArrayTable::toString() const {
  String result;
  for(UINT i = 0; i < ARRAYSIZE(m_table); i++) {
    result += format(_T("%3d,%s\n"), i, sprintbin((BYTE)i).cstr());
    result += indentString(get(i).toString(),2);
  }
  return result;
}
#endif

// return false if already set; otherwise add to m_centerSet and return true
bool IsoSurfacePolygonizer::addToDoneSet(const Point3DKey &key) {
  if(m_cubesDoneSet.contains(key)) {
    return false;
  } else {
    m_cubesDoneSet.add(key);
    return true;
  }
}

// getVertexId: return index for vertex on edge:
// c1.m_value and c2.m_value are presumed of different sign
// return saved index if any; else calculate and save vertex for later use
UINT IsoSurfacePolygonizer::getVertexId(const HashedCubeCorner &c1, const HashedCubeCorner &c2) {
#ifdef VALIDATE_OPPOSITESIGN
  if(!hasOppositeSign(c1, c2)) {
    throwException(_T("%s:Corners doesn't have opposite sign. c1:%s, c2:%s")
                  ,__TFUNCTION__
                  ,c1.toString().cstr(), c2.toString().cstr());
  }
#endif
  const CubeEdgeHashKey edgeKey(c1.m_key, c2.m_key);
  const UINT *p = m_edgeMap.get(edgeKey);
  if(p != NULL) {
    m_statistics.m_edgeHits++;
#ifdef ISODEBUGGER
    m_eval.markCurrentVertex(m_vertexArray[*p]);
#endif // ISODEBUGGER
    return *p; // previously computed
  }

  IsoSurfaceVertex vertex;
  vertex.m_position   = converge(c1, c2);             // position;
  vertex.m_normal     = getNormal(vertex.m_position); // normal

  const UINT result = (UINT)m_vertexArray.size();
  m_vertexArray.add(vertex);
  m_edgeMap.put(edgeKey, result);
#ifdef ISODEBUGGER
  m_eval.markCurrentVertex(m_vertexArray[result]);
#endif // ISODEBUGGER
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

static inline int point3DKeyCmp(const Point3DKey &k1, const Point3DKey &k2) {
  int c;
  if(c = k1.i - k2.i) return c;
  if(c = k1.j - k2.j) return c;
  return k1.k - k2.k;
}

#ifdef DUMP_CORNERMAP
typedef Entry<Point3DKey, HashedCubeCorner> CornerMapEntry;

static int cubeCornerCmp(const HashedCubeCorner * const &c1, const HashedCubeCorner * const &c2) {
  return point3DKeyCmp(c1->m_key,c2->m_key);
}

void IsoSurfacePolygonizer::dumpCornerMap() const {
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

#define DUMP_EDGEMAP

typedef Entry<CubeEdgeHashKey, UINT> EdgeMapEntry;
class SortedCubeEdge : public CubeEdgeHashKey {
public:
  int m_index;
  SortedCubeEdge(const EdgeMapEntry &e) : CubeEdgeHashKey(e.getKey()), m_index(e.getValue()) {
  }
};

static int sortedCubeEdgeCmp(const SortedCubeEdge &e1, const SortedCubeEdge &e2) {
  int c;
  if(c = point3DKeyCmp(e1.getKey1(),e2.getKey1())) return c;
  return point3DKeyCmp(e1.getKey2(),e2.getKey2());
}

void IsoSurfacePolygonizer::dumpEdgeMap() const {
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

#ifdef DUMP_VERTEXARRAY
void IsoSurfacePolygonizer::dumpVertexArray() const {
  debugLog(_T("VertexArray\n"));
  const size_t n = m_vertexArray.size();
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%6zu:%s\n"), i, m_vertexArray[i].toString().cstr());
  }
}
#endif // DUMP_VERTEXARRAY

#ifdef DUMP_FACEARRAY
void IsoSurfacePolygonizer::dumpFaceArray() const {
  debugLog(_T("FaceArray\n"));
  const size_t n = m_faceArray.size();
  for(size_t i = 0; i < n; i++) {
    debugLog(_T("%6zu:%s\n"), i, m_faceArray[i].toString().cstr());
  }
}
#endif // DUMP_FACEARRAY

void CubeEdgeHashKey::checkAndSwap() {
  if(point3DKeyCmp(m_key1, m_key2)) {
    std::swap(m_key1, m_key2);
  }
}

String TriangleStrip::toString() const {
  String result;
  for(int i = 0; i < m_count; i++) {
    if(i) {
      result += _T(",");
    }
    result += format(_T("%4d"), m_vertexId[i]);
  }
  return result;
}

String toString(CubeCorner cb) {
#define caseStr(c) case c: return #c
  switch(cb) {
  caseStr(LBN);
  caseStr(LBF);
  caseStr(LTN);
  caseStr(LTF);
  caseStr(RBN);
  caseStr(RBF);
  caseStr(RTN);
  caseStr(RTF);
  default: return format(_T("unknown cubeCorner:%d"), cb);
  }
}


UINT StackedCube::calculateIndex() const {
  UINT index = 0;
  for(int i = 0; i < ARRAYSIZE(m_corners); i++)  {
    if(m_corners[i]->isPositive()) {
      index |= (1<<i);
    }
  }
  return index;
}

#ifdef VALIDATE_CUBES

#define TOLERANCE 1e-10

void StackedCube::validate() {
  if((fabs(m_corners[LBN]->x - m_corners[LTN]->x) > TOLERANCE)
  || (fabs(m_corners[LBN]->x - m_corners[LBF]->x) > TOLERANCE)
  || (fabs(m_corners[LBN]->x - m_corners[LTF]->x) > TOLERANCE)
  || (fabs(m_corners[RBN]->x - m_corners[RTN]->x) > TOLERANCE)
  || (fabs(m_corners[RBN]->x - m_corners[RBF]->x) > TOLERANCE)
  || (fabs(m_corners[RBN]->x - m_corners[RTF]->x) > TOLERANCE)

  || (fabs(m_corners[LBN]->y - m_corners[RBN]->y) > TOLERANCE)
  || (fabs(m_corners[LBN]->y - m_corners[RBF]->y) > TOLERANCE)
  || (fabs(m_corners[LBN]->y - m_corners[LBF]->y) > TOLERANCE)
  || (fabs(m_corners[LTN]->y - m_corners[RTN]->y) > TOLERANCE)
  || (fabs(m_corners[LTN]->y - m_corners[RTF]->y) > TOLERANCE)
  || (fabs(m_corners[LTN]->y - m_corners[LTF]->y) > TOLERANCE)

  || (fabs(m_corners[LBN]->z - m_corners[RBN]->z) > TOLERANCE)
  || (fabs(m_corners[LBN]->z - m_corners[LTN]->z) > TOLERANCE)
  || (fabs(m_corners[LBN]->z - m_corners[RTN]->z) > TOLERANCE)
  || (fabs(m_corners[LBF]->z - m_corners[RBF]->z) > TOLERANCE)
  || (fabs(m_corners[LBF]->z - m_corners[LTF]->z) > TOLERANCE)
  || (fabs(m_corners[LBF]->z - m_corners[RTF]->z) > TOLERANCE)
  ) {
    throwException(_T("Cube not valid\n%s"), toString().cstr());
  }
  if((m_corners[LBN]->x >= m_corners[RBN]->x)
    || (m_corners[LBN]->y >= m_corners[LTN]->y)
    || (m_corners[LBN]->z >= m_corners[LBF]->z))
  {
    throwException(_T("Cube not valid\n%s"), toString().cstr());
  }
}
#endif // VALIDATE_CUBES

String StackedCube::toString() const {
  String result = format(_T("Cube:Key:%s. Size:%s\n"), m_key.toString().cstr(), getSize().toString(5).cstr());
  for(int i = 0; i < ARRAYSIZE(m_corners); i++) {
    result += format(_T("  %s:%s\n"), ::toString((CubeCorner)i).cstr(), m_corners[i]->toString().cstr());
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

};
