// 
// An implicit surface polygonizer, translated from Mesa
// applications should call polygonize()
//
// Authored by Jules Bloomenthal, Xerox PARC.
// Copyright (c) Xerox Corporation, 1991. All rights reserved.
// Permission is granted to reproduce, use and distribute this code for
// any and all purposes, provided that this notice appears in all
// copies.

#include "stdafx.h"
#include <Random.h>
#include "D3Math.h"
#include "IsoSurfacePolygonizer.h"

#define RES 10 // # converge iterations

static bool doCount;
typedef struct {
  CubeCorner corner1;
  CubeCorner corner2;
  CubeFace   leftface;
  CubeFace   rightface;
  CubeEdge   nextClockwiseEdge[6]; // indexet by face
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

#pragma check_stack(off)

class PointKeyComparator : public Comparator<PointKey> {
public:
  inline int compare(const PointKey &k1, const PointKey &k2) {
    return (k1 == k2) ? 0 : 1;
  }
  inline AbstractComparator *clone() const {
    return new PointKeyComparator();
  }
};

static inline unsigned long pointKeyHash(const PointKey &key) {
  return key.hashCode();
}

static PointKeyComparator pointKeyCmp;

Array<CompactArray<char> > IsoSurfacePolygonizer::m_cubetable[256];

class InitPolygonizerCubeTable {
public:
  InitPolygonizerCubeTable() {
    IsoSurfacePolygonizer::makeCubeTable();
  }
  ~InitPolygonizerCubeTable() {
    IsoSurfacePolygonizer::clearCubeTable();
  }
};

static InitPolygonizerCubeTable cubeTableInitializer;

#pragma check_stack(on)

IsoSurfacePolygonizer::IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval)
: m_eval(eval)
, m_cornerMap(pointKeyHash  ,pointKeyCmp, HASHSIZE)
{
}

IsoSurfacePolygonizer::~IsoSurfacePolygonizer() {
}

// polygonize: polygonize the implicit surface function
// Arguments:
// start                 : The point where the search for a zero-value of the surfacefunction will start.
// double size           : Width of the partitioning cube
// int bounds            : max. range of cubes (+/- on the three axes) from first cube
// bool tetrahedralmode  : true to use tetrahedral decomposition of a rectangular cube. false to use cube directly
void IsoSurfacePolygonizer::polygonize(const Point3D &start
                                      ,double         size
                                      ,const Cube3D  &boundingBox
                                      ,bool           tetrahedralMode
                                      ,bool           adaptSize) {

  const double startTime = getThreadTime();

  m_size            = size;
  m_boundingBox     = boundingBox;
  m_delta           = size/(double)(RES*RES);
  m_tetrahedralMode = tetrahedralMode;
  m_adaptSize       = adaptSize;

  m_statistics.clear();
  resetTables();

  _standardRandomGenerator.setSeed(87);
//  randomize();

  m_start = findStartPoint(start);

  m_vertexArray.setCapacity( HASHSIZE);
  m_cubesDoneSet.setCapacity(HASHSIZE);
  m_edgeMap.setCapacity(     HASHSIZE);
  
  putInitialCube();

  doCount = true;

  while(hasActiveCubes()) { // process active cubes until none left
    const StackedCube cube = getActiveCube();

    addSurfaceVertices(cube);

    const int dk = (1<<cube.m_level);
   // test six face directions, maybe add to stack:
	testFace(cube.m_key.i-dk, cube.m_key.j   , cube.m_key.k   , cube, LFACE, LBN, LBF, LTN, LTF);
	testFace(cube.m_key.i+dk, cube.m_key.j   , cube.m_key.k   , cube, RFACE, RBN, RBF, RTN, RTF);
	testFace(cube.m_key.i   , cube.m_key.j-dk, cube.m_key.k   , cube, BFACE, LBN, LBF, RBN, RBF);
	testFace(cube.m_key.i   , cube.m_key.j+dk, cube.m_key.k   , cube, TFACE, LTN, LTF, RTN, RTF);
	testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k-dk, cube, NFACE, LBN, LTN, RBN, RTN);
	testFace(cube.m_key.i   , cube.m_key.j   , cube.m_key.k+dk, cube, FFACE, LBF, LTF, RBF, RTF);
  }

  saveStatistics(startTime);

  debugLog("%s\n", m_statistics.toString().cstr());
//  dumpCornerMap();
}

void IsoSurfacePolygonizer::putInitialCube() {
  StackedCube cube(0, 0, 0, MAX_SPLITLEVEL);
  const int shift = cube.m_level;

  // set corners of initial cube:
  for(int i = 0; i < ARRAYSIZE(cube.m_corners); i++) {
    cube.m_corners[i] = getCorner(BIT(i,2)<<shift, BIT(i,1)<<shift, BIT(i,0)<<shift);
  }
  addToDoneSet(cube.m_key);
  pushCube(cube);
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
	doCube(cube);            // or polygonize the cube directly:
  }
  flushFaceBuffer();
}

// testface: given cube at lattice (i, j, k), and four corners of face,
// if surface crosses face, compute other four corners of adjacent cube
// and add new cube to cube stack
void IsoSurfacePolygonizer::testFace(int i, int j, int k, const StackedCube &oldCube, CubeFace face, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
  static int facebit[6] = {2, 2, 1, 1, 0, 0};
  int        bit        = facebit[face];
  bool       c1Positive = oldCube.m_corners[c1]->m_positive;

  // test if no surface crossing, cube out of bounds, or already visited:
  if(oldCube.m_corners[c2]->m_positive == c1Positive
  && oldCube.m_corners[c3]->m_positive == c1Positive
  && oldCube.m_corners[c4]->m_positive == c1Positive) {
    return;
  }

  // create new cube:
  StackedCube newCube(i, j, k, oldCube.m_level);
  if(!addToDoneSet(newCube.m_key)) {
	return;
  }
  const int shift = newCube.m_level;
  newCube.m_corners[FLIP(c1, bit)] = oldCube.m_corners[c1];
  newCube.m_corners[FLIP(c2, bit)] = oldCube.m_corners[c2];
  newCube.m_corners[FLIP(c3, bit)] = oldCube.m_corners[c3];
  newCube.m_corners[FLIP(c4, bit)] = oldCube.m_corners[c4];
  for(int n = 0; n < ARRAYSIZE(newCube.m_corners); n++) {
    if(newCube.m_corners[n] == NULL) {
      newCube.m_corners[n] = getCorner(i+(BIT(n,2)<<shift), j+(BIT(n,1)<<shift), k+(BIT(n,0)<<shift));
    }
  }

  if(  !m_boundingBox.contains(newCube.m_corners[LBN]->m_point)
    || !m_boundingBox.contains(newCube.m_corners[RTF]->m_point)) {
    return;
  }
  pushCube(newCube);
}

// doCube: triangulate the cube directly, without decomposition
void IsoSurfacePolygonizer::doCube(const StackedCube &cube) {
  m_statistics.m_doCubeCalls++;
  const Array<CompactArray<char> > &polys = m_cubetable[cube.getIndex()];
  for(int i = 0; i < polys.size(); i++) {
	int a = -1, b = -1;
    const CompactArray<char> &vertexEdges = polys[i];
	for(int j = 0; j < vertexEdges.size(); j++) {
      const CubeEdgeInfo &edge = cubeEdgeTable[vertexEdges[j]];
	  const HashedCorner &c1 = *cube.m_corners[edge.corner1];
	  const HashedCorner &c2 = *cube.m_corners[edge.corner2];
	  const int c = getVertexId(c1, c2);
      if(j >= 2) {
        putFace3(c, b, a, -1);
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
int IsoSurfacePolygonizer::doTetra(const HashedCorner &a, const HashedCorner &b, const HashedCorner &c, const HashedCorner &d, int level, int colorIndex) {
#define CANSPLIT (m_adaptSize && (level > 0))

  m_statistics.m_doTetraCallCount[level]++;

  unsigned char index = 0;
  if(a.m_positive) index += 8;
  if(b.m_positive) index += 4;
  if(c.m_positive) index += 2;
  if(d.m_positive) index += 1;
  // index is now 4-bit number representing one of the 16 possible cases

  int ab = -1, ac= -1, ad = -1, bc = -1, bd = -1, cd = -1;
  if(a.m_positive != b.m_positive) ab = getVertexId(a, b);
  if(a.m_positive != c.m_positive) ac = getVertexId(a, c);
  if(a.m_positive != d.m_positive) ad = getVertexId(a, d);
  if(b.m_positive != c.m_positive) bc = getVertexId(b, c);
  if(b.m_positive != d.m_positive) bd = getVertexId(b, d);
  if(c.m_positive != d.m_positive) cd = getVertexId(c, d);
  // 14 productive tetrahedral cases (0000 and 1111 do not yield polygons

  TriangleStrip ts;

  switch (index) {
  case  0:                                                     //----
  case 15:                                                     //++++
    m_statistics.m_nonProduktiveCount[level]++;
    return index;
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
  default: throwException("doTetra:Invalid index:%d", index);
  }

#define LEGALSPLITSET_PNNN ((1<<1) | (1<<2 ) | (1<<4 ) | (1<<8 ))

#define LEGALSPLITSET_NPPP ((1<<7) | (1<<11) | (1<<13) | (1<<14))

#define LEGALSPLITSET_PPNN ((1<<3) | (1<<5 ) | (1<< 6) | (1<<9 ) | (1<<10) | (1<<12))

#define LEGALSPLITSET (LEGALSPLITSET_PPNN | LEGALSPLITSET_PNNN | LEGALSPLITSET_NPPP)
  
//  (LEGALSPLITSET_PNNN | LEGALSPLITSET_NPPP)

  if((((1 << index) & LEGALSPLITSET)==0) || (!CANSPLIT || !needSplit(ts))) { 
    putTriangleStrip(ts, colorIndex);
  } else {
    level--;
    switch(index) {
    case  1: // d    > 0, a,b,c  < 0
      splitTetraPNNN(d, a,c,b, ad,cd,bd   , level);
      break;
    case  2:  // c   > 0, a,b,d  < 0
      splitTetraPNNN(c, b,d,a, bc,cd,ac   , level);
      break;
    case  3:  // c,d > 0, a,b    < 0
      splitTetraPPNN(c,d, a,b, ad,ac,bc,bd, level);
      break;
    case  4:  // b   > 0, a,c,d  < 0
      splitTetraPNNN(b, a,d,c, ab,bd,bc   , level);
      break;
    case  5:  // d,b > 0, a,c    < 0
      splitTetraPPNN(d,b, a,c, ab,ad,cd,bc, level);
      break;
    case  6:  // b,c > 0, a,d    < 0
      splitTetraPPNN(b,c, a,d, ac,ab,bd,cd, level);
      break;
    case  7:  // a   < 0, b,c,d  > 0
      splitTetraNPPP(a, b,c,d, ab,ac,ad   , level);
      break;
    case  8:  // a   > 0, b,c,d  < 0
      splitTetraPNNN(a, b,c,d, ab,ac,ad   , level);
      break;
    case  9:  // a,d > 0, b,c    < 0
      splitTetraPPNN(a,d, b,c, bd,ab,ac,cd, level);
      break;
    case 10:  // a,c > 0, d,b    < 0
      splitTetraPPNN(a,c, d,b, cd,ad,ab,bc, level);
      break;
    case 11:  // b   < 0, a,c,d  > 0
      splitTetraNPPP(b, a,d,c, ab,bd,bc   , level);
      break;
    case 12:  // a,b > 0, c,d    < 0
      splitTetraPPNN(a,b, c,d, bc,ac,ad,bd, level);
      break;
    case 13:  // c   < 0, a,b,d  > 0
      splitTetraNPPP(c, a,b,d, ac,bc,cd   , level);
      break;
    case 14:  // d   < 0, a,b,c  > 0
      splitTetraNPPP(d, a,c,b, ad,cd,bd   , level);
      break;
    }
  }
  return index;
}

#define NEEDSPLIT(p, q) ((p)*(q) < 0.82)

bool IsoSurfacePolygonizer::needSplit(const TriangleStrip &ts) const {
  const Point3D &n0 = m_vertexArray[ts.m_vertexId[0]].m_normal;
  const Point3D &n1 = m_vertexArray[ts.m_vertexId[1]].m_normal;
  const Point3D &n2 = m_vertexArray[ts.m_vertexId[2]].m_normal;

  switch(ts.m_count) {
  case 3:
    return NEEDSPLIT(n0, n1) || NEEDSPLIT(n1, n2) || NEEDSPLIT(n2, n0);

  case 4:
    if(NEEDSPLIT(n0, n1) || NEEDSPLIT(n1, n2) || NEEDSPLIT(n2, n0)) {
      return true;
    } else {
      const Point3D &n3 = m_vertexArray[ts.m_vertexId[3]].m_normal;
      return NEEDSPLIT(n2, n3) || NEEDSPLIT(n3, n0);
    }
  default:
    throwInvalidArgumentException("needSplit", "ts.count=%d",  ts.m_count);
    return false;
  }
}

#define SPLIT1SAMESIGN(add, a, b, c, color) { /* m(c,a) same sign as top */     \
  doTetra(top, m##a##b,  hc##b, m##b##c, level, color);                         \
  const int V1 = getVertexId(top     , m##a##b);                                \
  const int V2 = getVertexId(top     , m##b##c);                                \
  const int V3 = getVertexId(hc##c   , m##c##a);                                \
  const int V4 = getVertexId(hc##a   , m##c##a);                                \
  const int V5 = getVertexId(m##a##b , m##c##a);                                \
  const int V6 = getVertexId(m##b##c , m##c##a);                                \
  add(TriangleStrip(V5, V1  , v##a, V4, V3, V6), color);                        \
  add(TriangleStrip(V3, v##c, V2  , V6), color);                                \
}

#ifdef _OLDSPLIT1SAMESIGN
#define SPLIT1SAMESIGN(add, a, b, c, color) { /* m(c,a) same sign as top */     \
  doTetra(top, m##a##b,  hc##b, m##b##c, level, color);                         \
  const int V1 = getVertexId(top     , m##a##b);                                \
  const int V2 = getVertexId(top     , m##b##c);                                \
  const int V3 = getVertexId(hc##c   , m##c##a);                                \
  const int V4 = getVertexId(hc##a   , m##c##a);                                \
  const int V5 = getVertexId(m##a##b , m##c##a);                                \
  const int V6 = getVertexId(m##b##c , m##c##a);                                \
  add(TriangleStrip(V5, V1  , v##a, V4), color);                                \
  add(TriangleStrip(V3, v##c, V2  , V6), color);                                \
  add(TriangleStrip(V5, V4  , V3  , V6), color);                                \
}
#endif

#define SPLIT2SAMESIGN(add, a, b, c, color) { /* m(b,c) opposite sign of top */ \
  doTetra(top, m##a##b,  hc##b, m##b##c, level, color);                         \
  doTetra(top, m##b##c,  hc##c, m##c##a, level, color);                         \
  const int V1 = getVertexId(hc##a   , m##a##b);                                \
  const int V2 = getVertexId(hc##a   , m##c##a);                                \
  const int V3 = getVertexId(hc##b   , m##a##b);                                \
  const int V4 = getVertexId(hc##c   , m##c##a);                                \
  const int V5 = getVertexId(m##b##c , m##c##a);                                \
  const int V6 = getVertexId(m##a##b , m##b##c);                                \
/*  const int V7 = getVertexId(top     , m##b##c);                 */           \
  add(V1, V4  , V5, color);                                                     \
  add(V1, V5  , V6, color);                                                     \
  add(V1, V6  , V3, color);                                                     \
  add(V1, v##a, V2, color);                                                     \
  add(V1, V2  , V4, color);                                                     \
}

/*
  if((color == RED) || (color == PINK)) {                                       \
    m_eval.receiveDebugVertices(V1,V2,V3,V4,V5,V6,V7,v##a,v##b,v##c,-1);        \
  }                                                                             \
}
*/

void IsoSurfacePolygonizer::splitTetraPNNN(const HashedCorner &top, const HashedCorner &hc1, const HashedCorner &hc2, const HashedCorner &hc3, int v1, int v2, int v3, int level) {
  const HashedCorner &m12 = *avg2(hc1, hc2);
  const HashedCorner &m23 = *avg2(hc2, hc3);
  const HashedCorner &m31 = *avg2(hc3, hc1);

  const int innerIndex = doTetra(top, m12, m23, m31, level, GREEN);
  const int color      = GREEN;

  switch(innerIndex) { // top > 0 => possible values for innerIndex:[8..15]
  case  8: //+--- m12, m23, m31 < 0
    doTetra(top, m12,  hc2, m23, level, color);
    doTetra(top, m23,  hc3, m31, level, color);
    doTetra(top, m31,  hc1, m12, level, color); 
    break;
  case  9: //+--+           m31 > 0 
    SPLIT1SAMESIGN(putTriangleStrip, 1,2,3, color)
    break;
  case 10: //+-+-      m23      > 0
    SPLIT1SAMESIGN(putTriangleStrip, 3,1,2, color)
    break;
  case 12: //++-- m12           > 0
    SPLIT1SAMESIGN(putTriangleStrip, 2,3,1, color)
    break;

  case 11: //+-++      m23, m31 > 0
    SPLIT2SAMESIGN(putFace3        , 3,1,2, color)
    break;
  case 13: //++-+ m12     , m31 > 0
    SPLIT2SAMESIGN(putFace3        , 1,2,3, color)
    break;
  case 14: //+++- m12, m23      > 0
    SPLIT2SAMESIGN(putFace3        , 2,3,1, color)
    break;

  case 15: //++++ m12, m23, m31 > 0
  default:
    throwException("splitTetraPNNN:Unexpected index returned from innerTetra:%d. Must be [8..14]", innerIndex);
  }
}

void IsoSurfacePolygonizer::splitTetraNPPP(const HashedCorner &top, const HashedCorner &hc1, const HashedCorner &hc2, const HashedCorner &hc3, int v1, int v2, int v3, int level) {
  const HashedCorner &m12 = *avg2(hc1, hc2);
  const HashedCorner &m23 = *avg2(hc2, hc3);
  const HashedCorner &m31 = *avg2(hc3, hc1);

  const int innerIndex = doTetra(top, m12, m23, m31, level, GREEN);
  const int color      = GREEN;

  switch(innerIndex) { // top < 0 => possible values for innerIndex:[0..7]
  case  7: //-+++ m12, m23, m31 > 0
    doTetra(top, m12,  hc2, m23, level, color);
    doTetra(top, m23,  hc3, m31, level, color);
    doTetra(top, m31,  hc1, m12, level, color); 
    break;
  case  6: //-++-           m31 < 0 
    SPLIT1SAMESIGN(putTriangleStripR, 1,2,3, color)
    break;
  case  5: //-+-+      m23      < 0
    SPLIT1SAMESIGN(putTriangleStripR, 3,1,2, color)
    break;
  case  3: //--++ m12           < 0
    SPLIT1SAMESIGN(putTriangleStripR, 2,3,1, color)
    break;

  case  4: //-+--      m23, m31 < 0
    SPLIT2SAMESIGN(putFace3R        , 3,1,2, color)
    break;
  case  2: //--+- m12     , m31 < 0
    SPLIT2SAMESIGN(putFace3R        , 1,2,3, color)
    break;
  case  1: //---+ m12, m23      < 0
    SPLIT2SAMESIGN(putFace3R        , 2,3,1, color)
    break;

  case  0: //---- m12, m23, m31 < 0
  default:
    throwException("splitTetraNPPP:Unexpected index returned from innerTetra:%d. Must be [1..7]", innerIndex);
  }
}

void IsoSurfacePolygonizer::splitTetraPPNN(const HashedCorner &top, const HashedCorner &hc1 , const HashedCorner &hc2 , const HashedCorner &hc3 , int v1, int v2, int v3, int v4, int level) {

  assert((v1>=0)&&(v2>=0)&&(v3>=0)&&(v4>=0));
  assert(top.m_positive && hc1.m_positive && !hc2.m_positive && !hc3.m_positive);

  const HashedCorner &m12 = *avg2(hc1, hc2);
  const HashedCorner &m23 = *avg2(hc2, hc3);
  const HashedCorner &m31 = *avg2(hc3, hc1);

  int index = 0;
  if(m12.m_positive) index += 4;
  if(m23.m_positive) index += 2;
  if(m31.m_positive) index += 1;

  switch(index) {
  case 0: // m12, m23, m31 < 0
    { /*doTetra(top, m23, hc3, m31, level, RED);*/
      const int V1 = getVertexId(top, m12);
      const int V2 = getVertexId(top, m23);
      const int V3 = getVertexId(top, m31);

      putFace3R(V1,v2,V2,GREEN);
      putFace3R(V3,V2,v3,GREEN);
      putFace3R(v4,v1,V3,GREEN);
      putFace3R(v1,V1,V3,GREEN);
      putFace3R(V1,V2,V3,GREEN);
      putFace3R(v1,v2,V1,GREEN);


      m_eval.receiveDebugVertices(V1,V2,V3,v1,v2,v3,v4,-1);
/*

//                                0  1  2  3  4  5  6
      const int candidates[] = { V1,V2,V3,v1,v2,v3,v4 };

static int counter1 = -1, counter2 = 1, counter3 = 2;
if(doCount) {
  counter1++;
  if(counter1 == counter2) {
    counter1 = 0; counter2++;
    if(counter2 == counter3) {
      counter2 = 1;
      counter3++;
      if(counter3 == ARRAYSIZE(candidates)) {
        counter3 = 2;
      }
    }
  }
  doCount = false;
  debugLog("(counter1, counter2, counter3):(%d,%d,%d)\n", counter1, counter2, counter3);
}

      const int VV1 = candidates[counter1];
      const int VV2 = candidates[counter2];
      const int VV3 = candidates[counter3];
      putFace3(VV1,VV2,VV3,PURPLE);
      putFace3(VV1,VV3,VV2,PURPLE);
*/
    }
    break;
  case 1: // m12, m23 < 0, m31 > 0
    {
      const int V1 = getVertexId(top, m12);
      const int V2 = getVertexId(top, m23);
      const int V3 = getVertexId(m12, m31);
      const int V4 = getVertexId(m23, m31);

      putTriangleStrip(TriangleStrip(V1,v1,V3,V4,v4,v3), GREEN);
      putTriangleStrip(TriangleStrip(V1,v3,V2,v2), GREEN);
    }
    break;

  case 2:
  case 3:
  case 4:
  case 5:
  case 6:
    break;
  case 7:
    break;
  default:
    throwException("splitTetraPPNN:Unexpected index:%d. Must be [0..7]", index);
  }
}

void IsoSurfacePolygonizer::putTriangleStrip(const TriangleStrip &ts, unsigned int colorIndex) {
  for(int i = 2; i < ts.m_count; i++) {
    putFace3(ts.m_vertexId[0], ts.m_vertexId[i-1], ts.m_vertexId[i], colorIndex);
  }
}

void IsoSurfacePolygonizer::putTriangleStripR(const TriangleStrip &ts, unsigned int colorIndex) {
  for(int i = 2; i < ts.m_count; i++) {
    putFace3R(ts.m_vertexId[0], ts.m_vertexId[i-1], ts.m_vertexId[i], colorIndex);
  }
}

void IsoSurfacePolygonizer::flushFaceBuffer() {
  for(int i = 0; i < m_face3Buffer.size(); i++) {
    m_eval.receiveFace(m_face3Buffer[i]);
  }
  m_statistics.m_faceCount += m_face3Buffer.size();
  m_face3Buffer.clear(-1);
}

void IsoSurfacePolygonizer::resetTables() {
  m_cubeStack.clear();
  m_vertexArray.clear();
  m_cubesDoneSet.clear();
  m_cornerMap.clear();
  m_edgeMap.clear();
  m_face3Buffer.clear();
}

void IsoSurfacePolygonizer::pushCube(const StackedCube &cube) {
  m_cubeStack.push(cube);
  m_statistics.m_cubeCount++;
#ifdef DUMP_CUBES
  debugLog("pushCube():%s", cube.toString().cstr());
#endif
}

// getCorner: return cached/new corner with the given lattice location
const HashedCorner *IsoSurfacePolygonizer::getCorner(int i, int j, int k) {
  const PointKey key(i, j, k);
  const HashedCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedCorner corner(key, getCornerPoint(key));
    corner.m_positive = evaluate(corner.m_point) > 0;
    m_cornerMap.put(key, corner);
    return m_cornerMap.get(key);
  }
}

const HashedCorner *IsoSurfacePolygonizer::avg2(const HashedCorner &a, const HashedCorner &b) {
  const PointKey      key    = ::avg2(a.m_key,b.m_key);
  const HashedCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedCorner corner(key, getCornerPoint(key)); // (a.m_point+b.m_point+c.m_point+d.m_point)/4);

#ifdef VALIDATE_AVG2
    const Point3D m = (a.m_point+b.m_point)/2;
    const Point3D diff = corner.m_point - m;
    const double l = diff.length();
    if(l > 1e-6) {
      throwException("avg2(%s,%s)\n:%s\nm=%s\nl=%le"
                    ,a.toString().cstr()
                    ,b.toString().cstr()
                    ,corner.toString().cstr()
                    ,m.toString(5).cstr()
                    ,l);
    }
#endif

    corner.m_positive = evaluate(corner.m_point) > 0;
    m_cornerMap.put(key, corner);
    return m_cornerMap.get(key);
  }
}

const HashedCorner *IsoSurfacePolygonizer::avg4(const HashedCorner &a, const HashedCorner &b, const HashedCorner &c, const HashedCorner &d) {
  const PointKey      key    = ::avg4(a.m_key,b.m_key,c.m_key,d.m_key);
  const HashedCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedCorner corner(key, getCornerPoint(key)); // (a.m_point+b.m_point+c.m_point+d.m_point)/4);

#ifdef VALIDATE_AVG4
    const Point3D m = (a.m_point+b.m_point+c.m_point+d.m_point)/4;
    const Point3D diff = corner.m_point - m;
    const double l = diff.length();
    if(l > 1e-6) {
      throwException("avg4(%s,%s,%s,%s)\n:%s\nm=%s\nl=%le"
                    ,a.toString().cstr()
                    ,b.toString().cstr()
                    ,c.toString().cstr()
                    ,d.toString().cstr()
                    ,corner.toString().cstr()
                    ,m.toString(5).cstr()
                    ,l);
    }
#endif

    corner.m_positive = evaluate(corner.m_point) > 0;
    m_cornerMap.put(key, corner);
    return m_cornerMap.get(key);
  }
}

Point3D IsoSurfacePolygonizer::getCornerPoint(int i, int j, int k) const {
  Point3D result;
  result.x = m_start.x+((double)i/(1<<MAX_SPLITLEVEL))*m_size;
  result.y = m_start.y+((double)j/(1<<MAX_SPLITLEVEL))*m_size;
  result.z = m_start.z+((double)k/(1<<MAX_SPLITLEVEL))*m_size;
  return result;
}

// find point on surface, beginning search at start
Point3D IsoSurfacePolygonizer::findStartPoint(const Point3D &start) {
  IsoSurfaceTest in  = findStartPoint(true , start);
  IsoSurfaceTest out = findStartPoint(false, start);
  if(!in.m_ok || !out.m_ok) {
    throwException("Cannot find starting point1");
  }
  return converge(in.m_point, out.m_point, in.m_positive, RES + 7);
}

// findStartPoint: search for point with value of sign specified by positive-parameter
IsoSurfaceTest IsoSurfacePolygonizer::findStartPoint(bool positive, const Point3D &p) {
  IsoSurfaceTest result;
  double range = m_size;
  result.m_ok = true;
  for(int i = 0; i < 10000; i++) {
	result.m_point.x = p.x + _standardRandomGenerator.nextDouble(-range, range);
	result.m_point.y = p.y + _standardRandomGenerator.nextDouble(-range, range);
	result.m_point.z = p.z + _standardRandomGenerator.nextDouble(-range, range);
	const double value = evaluate(result.m_point);
    result.m_positive = value > 0.0;
    if(result.m_positive == positive) {
	  return result;
    }
	range = range*1.0005; // slowly expand search outwards
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
void IsoSurfacePolygonizer::makeCubeTable() { // static
  for(int i = 0; i < ARRAYSIZE(m_cubetable); i++) {
    unsigned short done = 0;

#define OPPOSITE_SIGN(i,cep) ((((i)>>cep->corner1)&1) != (((i)>>cep->corner2)&1))
#define ISDONE(e)            (done&(1<<(e)))
#define SETDONE(e)           done |= (1<<(e))

    for(int e = 0; e < ARRAYSIZE(cubeEdgeTable); e++) {
      const CubeEdgeInfo *cep = &cubeEdgeTable[e];
	  if(!ISDONE(e) && OPPOSITE_SIGN(i,cep)) {
        CompactArray<char> vertexEdges;
		// get face that is to right of edge from pos to neg corner:
		CubeFace face = (i&(1<<cep->corner1)) ? cep->rightface : cep->leftface;
		for(;;) {
		  const CubeEdge edge = cep->nextClockwiseEdge[face];
		  SETDONE(edge);
          cep = &cubeEdgeTable[edge];
		  if(OPPOSITE_SIGN(i, cep)) {
			vertexEdges.add(edge);
            if(edge == e) {
              break;
            }
			face = otherFace(edge, face);
		  }
		}
		m_cubetable[i].add(vertexEdges);
	  }
    }
  }
#ifdef DUMP_CUBETABLE
  dumpCubeTable();
#endif
}

void IsoSurfacePolygonizer::clearCubeTable() { // static
  for(int i = 0; i < ARRAYSIZE(m_cubetable); i++) {
    m_cubetable[i].clear();
  }
}

#ifdef DUMP_CUBETABLE
void IsoSurfacePolygonizer::dumpCubeTable(const String &fileName) { // static
  String name = fileName;
  if(name.length() == 0) {
    name = "c:\\temp\\surfacePolyCubeTable.txt";
  }
  FILE *f = MKFOPEN(name, "w");
  for(int i = 0; i < ARRAYSIZE(m_cubetable); i++) {
    const Array<CompactArray<char> > &a = m_cubetable[i];
    for(int j = 0; j < a.size(); j++) {
      const CompactArray<char> &b = a[j];
      fprintf(f, "(%3d,%s,%d):", i, sprintbin((unsigned char)i).cstr(),j);
      for(int k = 0; k < b.size(); k++) {
        fprintf(f,"%d ", b[k]);
      }
      fprintf(f, "\n");
    }
  }
  fclose(f);
}
#endif

// return false if already set; otherwise add to m_centerSet and return true
bool IsoSurfacePolygonizer::addToDoneSet(const PointKey &key) {
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
unsigned int IsoSurfacePolygonizer::getVertexId(const HashedCorner &c1, const HashedCorner &c2) {
#ifdef VALIDATE_OPPOSITESIGN
  if(c1.m_positive == c2.m_positive) {
    throwException("getVertexId:corners have same sign. c1:%s, c2:%s", c1.toString().cstr(), c2.toString().cstr());
  }
#endif

  const EdgeHashKey edgeKey(c1.m_key, c2.m_key);
  const int *p = m_edgeMap.get(edgeKey);
  if(p != NULL) {
    m_statistics.m_edgeHits++;
    return *p; // previously computed
  }
  SurfaceVertex v;
  v.m_position = converge(c1.m_point, c2.m_point, c1.m_positive); // position
  v.m_normal   = getNormal(v.m_position); // normal
  const unsigned int vid = m_vertexArray.size();
  m_vertexArray.add(v);
  m_edgeMap.put(edgeKey, vid);
  return vid;
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
Point3D IsoSurfacePolygonizer::converge(const Point3D &p1, const Point3D &p2, bool p1Positive, int itCount) {
  Point3D pos, neg;
  if(p1Positive) {
	pos = p1;
	neg = p2;
  } else {
	pos = p2;
	neg = p1;
  }
  double v = 1e7;
  for(int i = itCount = max(itCount, RES);;) {
	const Point3D result = (pos + neg)*0.5;
    if(i-- == 0) {
      return result;
    }
	if((v = evaluate(result)) > 0.0) {
	  pos = result;
    } else if(v < 0) {
	  neg = result;
    } else {
      m_statistics.m_exactZeroHits++;
      return result;
    }
  }
}

void IsoSurfacePolygonizer::dumpCornerMap() {
  for(Iterator<Entry<PointKey, HashedCorner> > it = m_cornerMap.entrySet().getIterator(); it.hasNext();) {
    const Entry<PointKey, HashedCorner> &e = it.next();
    debugLog("%s\n", e.getValue().toString().cstr());
  }
}

#define KEY_GT(k1,k2) (((k1).i>(k2).i) || ((k1).i==(k2).i && ((k1).j>(k2).j || ((k1).j==(k2).j && (k1).k>(k2).k))))

#define SWAPKEYS(k1,k2) { const PointKey tmp=k1; k1=k2; k2=tmp; }

void EdgeHashKey::checkAndSwap() {
  if(KEY_GT(m_key1, m_key2)) {
    SWAPKEYS(m_key1, m_key2);
  }
}

String TriangleStrip::toString() const {
  String result;
  for(int i = 0; i < m_count; i++) {
    if(i) {
      result += ",";
    }
    result += format("%4d", m_vertexId[i]);
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
  default: return format("unknown cubeCorner:%d", cb);
  }
}


#define TOLERANCE 1e-10

unsigned int StackedCube::getIndex() const {
  unsigned int index = 0;
  for(int i = 0; i < ARRAYSIZE(m_corners); i++)  {
    if(m_corners[i]->m_positive) {
      index |= (1<<i);
    }
  }
  return index;
}

void StackedCube::validate() {
  if((fabs(m_corners[LBN]->m_point.x - m_corners[LTN]->m_point.x) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.x - m_corners[LBF]->m_point.x) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.x - m_corners[LTF]->m_point.x) > TOLERANCE)
  || (fabs(m_corners[RBN]->m_point.x - m_corners[RTN]->m_point.x) > TOLERANCE)
  || (fabs(m_corners[RBN]->m_point.x - m_corners[RBF]->m_point.x) > TOLERANCE)
  || (fabs(m_corners[RBN]->m_point.x - m_corners[RTF]->m_point.x) > TOLERANCE)

  || (fabs(m_corners[LBN]->m_point.y - m_corners[RBN]->m_point.y) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.y - m_corners[RBF]->m_point.y) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.y - m_corners[LBF]->m_point.y) > TOLERANCE)
  || (fabs(m_corners[LTN]->m_point.y - m_corners[RTN]->m_point.y) > TOLERANCE)
  || (fabs(m_corners[LTN]->m_point.y - m_corners[RTF]->m_point.y) > TOLERANCE)
  || (fabs(m_corners[LTN]->m_point.y - m_corners[LTF]->m_point.y) > TOLERANCE)

  || (fabs(m_corners[LBN]->m_point.z - m_corners[RBN]->m_point.z) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.z - m_corners[LTN]->m_point.z) > TOLERANCE)
  || (fabs(m_corners[LBN]->m_point.z - m_corners[RTN]->m_point.z) > TOLERANCE)
  || (fabs(m_corners[LBF]->m_point.z - m_corners[RBF]->m_point.z) > TOLERANCE)
  || (fabs(m_corners[LBF]->m_point.z - m_corners[LTF]->m_point.z) > TOLERANCE)
  || (fabs(m_corners[LBF]->m_point.z - m_corners[RTF]->m_point.z) > TOLERANCE)
  ) {
    throwException("cube not valid\n%s",toString().cstr());
  }
}

String StackedCube::toString() const {
  String result = format("Cube:Key:%s. Size:%s\n", m_key.toString().cstr(), getSize().toString(5).cstr());
  for(int i = 0; i < ARRAYSIZE(m_corners); i++) {
    result += format("  %s:%s\n", ::toString((CubeCorner)i).cstr(), m_corners[i]->toString().cstr());
  }
  return result;
}

static String intArrayToString(const CompactIntArray &a) {
  if(a.size() == 0) {
    return "";
  } else {
    String result = format("%d:%s", 0, format1000(a[0]).cstr());
    for(int i = 1; i < a.size(); i++) {
      result += format(", %d:%s", i, format1000(a[i]).cstr());
    }
    return result;
  }
}

void IsoSurfacePolygonizer::saveStatistics(double startTime) {
  m_statistics.m_cornerCount = m_cornerMap.size();
  m_statistics.m_edgeCount   = m_edgeMap.size();
  m_statistics.m_vertexCount = m_vertexArray.size();
  m_statistics.m_threadTime  = getThreadTime() - startTime;
  m_statistics.m_hashStat    = "  CornerMap:" + intArrayToString(m_cornerMap.getLength())    + "\n"
                             + "  EdgeMap  :" + intArrayToString(m_edgeMap.getLength())      + "\n"
                             + "  DoneSet  :" + intArrayToString(m_cubesDoneSet.getLength()) + "\n";
}

PolygonizerStatistics::PolygonizerStatistics() {
  clear();
}

void PolygonizerStatistics::clear() {
  m_cubeCount      = 0;
  m_cornerCount    = 0;
  memset(m_nonProduktiveCount,0,sizeof(m_nonProduktiveCount));
  memset(m_doTetraCallCount  ,0,sizeof(m_doTetraCallCount  ));
  m_doCubeCalls    = 0;
  m_edgeCount      = 0;
  m_vertexCount    = 0;
  m_faceCount      = 0;
  m_evalCount      = 0;
  m_cornerHits     = 0;
  m_edgeHits       = 0;
  m_exactZeroHits  = 0;
  m_threadTime     = 0;
  m_hashStat       = "";
}

String PolygonizerStatistics::toString() const {
  return format("Polygonizer statistics\n"
               "Time                : %.2lf sec.\n"
               "#Vertices           : %s\n"
               "#Triangles          : %s\n"
               "#Cubes              : %s\n"
               "#Corners            : %s\n"
               "#Edges              : %s\n"
               "#Functionevaluations: %s\n"
               "#Corner hits        : %s\n"
               "#Edge hits          : %s\n"
               "#ExactZeroHits      : %d\n"
               "#doTetra calls      : %s\n"
               "#Nonproduktive tetra: %s\n"
               "#doCube calls       : %s\n"
               "HashStatistics:\n"
               "%s"
              ,m_threadTime / 1000000
              ,format1000(m_vertexCount).cstr()
              ,format1000(m_faceCount).cstr()
              ,format1000(m_cubeCount).cstr()
              ,format1000(m_cornerCount).cstr()
              ,format1000(m_edgeCount).cstr()
              ,format1000(m_evalCount).cstr()
              ,format1000(m_cornerHits).cstr()
              ,format1000(m_edgeHits).cstr()
              ,m_exactZeroHits
              ,getDoTetraCallsString().cstr()
              ,getNonProdutiveString().cstr()
              ,format1000(m_doCubeCalls).cstr()
              ,m_hashStat.cstr()

              );
}

String PolygonizerStatistics::getDoTetraCallsString() const {
  String result;
  for(int i = ARRAYSIZE(m_doTetraCallCount); i--;) {
    result += format(" L(%d):%s", i, format1000(m_doTetraCallCount[i]).cstr());
  }
  return result;
}

String PolygonizerStatistics::getNonProdutiveString() const {
  String result;
  for(int i = ARRAYSIZE(m_nonProduktiveCount); i--;) {
    result += format(" L(%d):%s", i, format1000(m_nonProduktiveCount[i]).cstr());
  }
  return result;
}
