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
#include <Math.h>
#include <Random.h>
#include <Math/MathException.h>
#include "SurfacePolygonizer.h"

#define RES 10 // # converge iterations

typedef enum {
  L // left   direction : -x, -i
 ,R // right  direction : +x, +i
 ,B // bottom direction : -y, -j
 ,T // top    direction : +y, +j
 ,N // near   direction : -z, -k
 ,F // far    direction : +z, +k
} Face;

typedef enum {
  LBN // left  bottom near corner
 ,LBF // left  bottom far  corner
 ,LTN // left  top    near corner
 ,LTF // left  top    far  corner
 ,RBN // right bottom near corner
 ,RBF // right bottom far  corner
 ,RTN // right top    near corner
 ,RTF // right top    far  corner
} Corner;

// the LBN corner of cube (i, j, k), corresponds with location
// (start.x+(i-.5)*size, start.y+(j-.5)*size, start.z+(k-.5)*size)

//**** Cubical Polygonization ****
typedef enum {
  LB  // left   bottom edge
 ,LT  // left   top    edge
 ,LN  // left   near   edge
 ,LF  // left   far    edge
 ,RB  // right  bottom edge
 ,RT  // right  top    edge
 ,RN  // right  near   edge
 ,RF  // right  far    edge
 ,BN  // bottom near   edge
 ,BF  // bottom far    edge
 ,TN  // top    near   edge
 ,TF  // top    far    edge
 ,NN  // No edge
} Edge;

typedef struct {
  Corner corner1;
  Corner corner2;
  Face leftface;
  Face rightface;
  Edge nextClockwiseEdge[6]; // indexet by face
} CubeEdge;

static const CubeEdge cubeEdge[] = {
  { LBN,LBF, B, L, { LF, NN, BN, NN, NN, NN } }, // left   bottom edge
  { LTN,LTF, L, T, { LN, NN, NN, TF, NN, NN } }, // left   top    edge
  { LBN,LTN, L, N, { LB, NN, NN, NN, TN, NN } }, // left   near   edge
  { LBF,LTF, F, L, { LT, NN, NN, NN, NN, BF } }, // left   far    edge
  { RBN,RBF, R, B, { NN, RN, BF, NN, NN, NN } }, // right  bottom edge
  { RTN,RTF, T, R, { NN, RF, NN, TN, NN, NN } }, // right  top    edge
  { RBN,RTN, N, R, { NN, RT, NN, NN, BN, NN } }, // right  near   edge
  { RBF,RTF, R, F, { NN, RB, NN, NN, NN, TF } }, // right  far    edge
  { LBN,RBN, N, B, { NN, NN, RB, NN, LN, NN } }, // bottom near   edge
  { LBF,RBF, B, F, { NN, NN, LB, NN, NN, RF } }, // bottom far    edge
  { LTN,RTN, T, N, { NN, NN, NN, LT, RN, NN } }, // top    near   edge
  { LTF,RTF, F, T, { NN, NN, NN, RT, NN, LF } }  // top    far    edge
}; 

#define RAND() ((double)(rand()%RAND_MAX)/RAND_MAX) // random number between 0 and 1
#define HASHSIZE 100001
#define SWAP(a,b) { int tmp=a; a = b; b = tmp; }
#define BIT(i, bit) (((i)>>(bit))&1)
#define FLIP(i,bit) ((i)^1<<(bit)) // flip the given bit of i

#pragma check_stack(off)

void EdgeKey::checkAndSwap() {
  if(m_key1.i>m_key2.i || (m_key1.i==m_key2.i && (m_key1.j>m_key2.j || (m_key1.j==m_key2.j && m_key1.k>m_key2.k)))) {
    SWAP(m_key1.i,m_key2.i);
    SWAP(m_key1.j,m_key2.j);
    SWAP(m_key1.k,m_key2.k);
  }
}

class HashKeyComparator : public Comparator<HashKey> {
public:
  inline int compare(const HashKey &k1, const HashKey &k2) {
    return (k1 == k2) ? 0 : 1;
  }
  inline AbstractComparator *clone() const {
    return new HashKeyComparator();
  }
};

static inline unsigned long keyHash(const HashKey &key) {
  return key.hashCode();
}

static HashKeyComparator hashKeyCmp;

int HashedCube::getIndex() const {
  int index = 0;
  for(int i = 0; i < ARRAYSIZE(m_corners); i++)  {
    if(m_corners[i]->m_value > 0.0) {
      index += (1<<i);
    }
  }
  return index;
}

static String intArrayToString(const CompactIntArray &a) {
  if(a.size() == 0) {
    return "";
  } else {
    String result = format("%d:%d",0,a[0]);
    for(int i = 1; i < a.size(); i++) {
      result += ",";
      result += format("%d:%d",i,a[i]);
    }
    return result;
  }
}

double SurfacePolygonizer::evaluate(const Point3D &p) {
  m_statistics.m_evalCount++;
  return m_eval.evaluate(p);
}

#pragma check_stack(on)

// **** Implicit Surface Polygonizer ****
// Constructor.
// Arguments are:
// SurfaceEvaluator eval : Contains the surfaceFunction, evaluate(const Point3D &p), and the function to handle
//                         the calculated vertices, receiveVertex. This function will be called for every face 
//                         on the surface. eval.evaluate should return negative values for points inside the surface,
//                         positive for outside.
// double size           : Width of the partitioning cube
// int bounds            : max. range of cubes (+/- on the three axes) from first cube
// bool tetrahedralmode  : true to use tetrahedral decomposition of a rectangular cube. false to use cube directly
SurfacePolygonizer::SurfacePolygonizer(SurfaceEvaluator &eval,
                                       double           size, 
                                       int              bounds, 
                                       bool             tetrahedralMode)
: m_eval(eval)
, m_cornerMap(keyHash  ,hashKeyCmp, HASHSIZE)
{
  m_edgeMap.setCapacity(  HASHSIZE);
  m_centerSet.setCapacity(HASHSIZE);

  m_size            = size;
  m_bounds          = bounds;
  m_delta           = size/(double)(RES*RES);
  m_tetrahedralMode = tetrahedralMode;
  makeCubeTable();
}

SurfacePolygonizer::~SurfacePolygonizer() {
}

// polygonize: polygonize the implicit surface function
// Arguments:
// start: The point where the search for a zero-value of the surfacefunction will start.
void SurfacePolygonizer::polygonize(const Point3D &start) {
  m_statistics.clear();
  const double startTime = getThreadTime();
  resetTables();

  // find point on surface, beginning search at (x, y, z):
  randomize();
  SurfaceTest in  = find(true , start);
  SurfaceTest out = find(false, start);
  if(!in.m_ok || !out.m_ok) {
    throwMathException("can’t find starting point");
  }

  m_start = converge(in.m_point, out.m_point, in.m_value);
  
  HashedCube cube(0,0,0);
  // set corners of initial cube:
  for(int i = 0; i < ARRAYSIZE(cube.m_corners); i++) {
    cube.m_corners[i] = getCorner(BIT(i,2), BIT(i,1), BIT(i,0));
  }
  m_cubeStack.push(cube); // push initial cube on stack
  setCenter(0, 0, 0);

  while(!m_cubeStack.isEmpty()) { // process active cubes till none left
	HashedCube c = m_cubeStack.pop();
    if(m_tetrahedralMode) { // either decompose into tetrahedra and polygonize:
	  doTetra(c, LBN, LTN, RBN, LBF);
	  doTetra(c, RTN, LTN, LBF, RBN);
	  doTetra(c, RTN, LTN, LTF, LBF);
      doTetra(c, RTN, RBN, LBF, RBF);
	  doTetra(c, RTN, LBF, LTF, RBF);
	  doTetra(c, RTN, LTF, RTF, RBF);
    } else {
	  doCube(c);            // or polygonize the cube directly:
    }

	// test six face directions, maybe add to stack:
	testFace(c.m_key.i-1, c.m_key.j  , c.m_key.k  , c, L, LBN, LBF, LTN, LTF);
	testFace(c.m_key.i+1, c.m_key.j  , c.m_key.k  , c, R, RBN, RBF, RTN, RTF);
	testFace(c.m_key.i  , c.m_key.j-1, c.m_key.k  , c, B, LBN, LBF, RBN, RBF);
	testFace(c.m_key.i  , c.m_key.j+1, c.m_key.k  , c, T, LTN, LTF, RTN, RTF);
	testFace(c.m_key.i  , c.m_key.j  , c.m_key.k-1, c, N, LBN, LTN, RBN, RTN);
	testFace(c.m_key.i  , c.m_key.j  , c.m_key.k+1, c, F, LBF, LTF, RBF, RTF);
  }

  m_statistics.m_cornerCount = m_cornerMap.size();
  m_statistics.m_edgeCount   = m_edgeMap.size();
  m_statistics.m_vertexCount = m_vertexArray.size();
  m_statistics.m_threadTime  = getThreadTime() - startTime;
  m_statistics.m_hashStat    = "cornerMap:" + intArrayToString(m_cornerMap.getLength()) + "\n"
                             + "edgeMap  :" + intArrayToString(m_edgeMap.getLength())   + "\n"
                             + "centerSet:" + intArrayToString(m_centerSet.getLength()) + "\n";
}

PolygonizerStatistics::PolygonizerStatistics() {
  clear();
}

void PolygonizerStatistics::clear() {
  m_cornerCount = 0;
  m_edgeCount   = 0;
  m_vertexCount = 0;
  m_faceCount   = 0;
  m_evalCount   = 0;
  m_cornerHits  = 0;
  m_edgeHits    = 0;
  m_threadTime  = 0;
  m_hashStat    = "";
}

String PolygonizerStatistics::toString() const {
  return format("Polygonizer statistics\n"
               "Time                :%.2lf\n"
               "#Points             :%d\n"
               "#Polygons           :%d\n"
               "#Corners            :%d\n"
               "#Edges              :%d\n"
               "#Functionevaluations:%d\n"
               "#Corner hits        :%d\n"
               "#Edge hits          :%d\n"
               "HashStatistics:\n"
               "%s"
              ,m_threadTime / 1000000
              ,m_vertexCount
              ,m_faceCount
              ,m_cornerCount
              ,m_edgeCount
              ,m_evalCount
              ,m_cornerHits
              ,m_edgeHits
              ,m_hashStat.cstr()
              );
}

void SurfacePolygonizer::resetTables() {
  m_vertexArray.clear();
  m_edgeMap.clear();
  m_centerSet.clear();
  m_cornerMap.clear();
}

// testface: given cube at lattice (i, j, k), and four corners of face,
// if surface crosses face, compute other four corners of adjacent cube
// and add new cube to cube stack
void SurfacePolygonizer::testFace(int i, int j, int k, const HashedCube &oldCube, int face, int c1, int c2, int c3, int c4) {
  static int facebit[6] = {2, 2, 1, 1, 0, 0};
  int        bit        = facebit[face];
  bool       positiv    = oldCube.m_corners[c1]->m_value > 0.0;
  // test if no surface crossing, cube out of bounds, or already visited:
  if((oldCube.m_corners[c2]->m_value > 0) == positiv &&
	 (oldCube.m_corners[c3]->m_value > 0) == positiv &&
     (oldCube.m_corners[c4]->m_value > 0) == positiv) {
	 return;
  }

  if(abs(i) > m_bounds || abs(j) > m_bounds || abs(k) > m_bounds) {
    return;
  }

  if(!setCenter(i, j, k)) {
	return;
  }

  // create new cube:
  HashedCube newCube(i,j,k);
  newCube.m_corners[FLIP(c1, bit)] = oldCube.m_corners[c1];
  newCube.m_corners[FLIP(c2, bit)] = oldCube.m_corners[c2];
  newCube.m_corners[FLIP(c3, bit)] = oldCube.m_corners[c3];
  newCube.m_corners[FLIP(c4, bit)] = oldCube.m_corners[c4];
  for(int n = 0; n < ARRAYSIZE(newCube.m_corners); n++) {
    if(newCube.m_corners[n] == NULL) {
      newCube.m_corners[n] = getCorner(i+BIT(n,2), j+BIT(n,1), k+BIT(n,0));
    }
  }
  m_cubeStack.push(newCube);
}

Point3D SurfacePolygonizer::getCornerPoint(int i, int j, int k) const {
  Point3D result;
  result.x = m_start.x+((double)i-0.5)*m_size;
  result.y = m_start.y+((double)j-0.5)*m_size;
  result.z = m_start.z+((double)k-0.5)*m_size;
  return result;
}

// getCorner: return cached/new corner with the given lattice location
HashedCorner *SurfacePolygonizer::getCorner(int i, int j, int k) {
  HashKey key(i,j,k);
  HashedCorner *result = m_cornerMap.get(key);
  if(result != NULL) {
    m_statistics.m_cornerHits++;
    return result;
  } else {
    HashedCorner corner(key, getCornerPoint(i,j,k));
    corner.m_value = evaluate(corner.m_point);
    m_cornerMap.put(key,corner);
    return m_cornerMap.get(key);
  }
}

// find: search for point with value of sign specified by positive-parameter
SurfaceTest SurfacePolygonizer::find(bool positive, const Point3D &p) {
  SurfaceTest result;
  double range = m_size;
  result.m_ok = true;
  for(int i = 0; i < 10000; i++) {
	result.m_point.x = p.x+range*(RAND()-0.5);
	result.m_point.y = p.y+range*(RAND()-0.5);
	result.m_point.z = p.z+range*(RAND()-0.5);
	result.m_value = evaluate(result.m_point);
    if(positive == (result.m_value > 0.0)) {
	  return result;
    }
	range = range*1.0005; // slowly expand search outwards
  }
  result.m_ok = false;
  return result;
}

// **** Tetrahedral Polygonization ****
// doTetra: triangulate the tetrahedron
// b, c, d should appear clockwise when viewed from a
// return false if client aborts, true otherwise
void SurfacePolygonizer::doTetra(const HashedCube &cube, int c1, int c2, int c3, int c4) {
  const HashedCorner &a = *cube.m_corners[c1];
  const HashedCorner &b = *cube.m_corners[c2];
  const HashedCorner &c = *cube.m_corners[c3];
  const HashedCorner &d = *cube.m_corners[c4];
  int index = 0, e1, e2, e3, e4, e5, e6;
  bool aPositiv, bPositiv, cPositiv, dPositiv;
  if(aPositiv = (a.m_value > 0.0)) index += 8;
  if(bPositiv = (b.m_value > 0.0)) index += 4;
  if(cPositiv = (c.m_value > 0.0)) index += 2;
  if(dPositiv = (d.m_value > 0.0)) index += 1;
  // index is now 4-bit number representing one of the 16 possible cases
  if(aPositiv != bPositiv) e1 = getVertexId(a, b);
  if(aPositiv != cPositiv) e2 = getVertexId(a, c);
  if(aPositiv != dPositiv) e3 = getVertexId(a, d);
  if(bPositiv != cPositiv) e4 = getVertexId(b, c);
  if(bPositiv != dPositiv) e5 = getVertexId(b, d);
  if(cPositiv != dPositiv) e6 = getVertexId(c, d);
  // 14 productive tetrahedral cases (0000 and 1111 do not yield polygons
  switch (index) {
  case  1:
    m_eval.receiveVertex(Face3(e5, e6, e3), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case  2:
    m_eval.receiveVertex(Face3(e2, e6, e4), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case  3:
    m_eval.receiveVertex(Face3(e3, e5, e4), m_vertexArray);
    m_eval.receiveVertex(Face3(e3, e4, e2), m_vertexArray); m_statistics.m_faceCount+=2;
    break;
  case  4:
    m_eval.receiveVertex(Face3(e1, e4, e5), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case  5:
    m_eval.receiveVertex(Face3(e3, e1, e4), m_vertexArray);
    m_eval.receiveVertex(Face3(e3, e4, e6), m_vertexArray); m_statistics.m_faceCount+=2;
    break;
  case  6:
    m_eval.receiveVertex(Face3(e1, e2, e6), m_vertexArray); m_statistics.m_faceCount+=2;
    m_eval.receiveVertex(Face3(e1, e6, e5), m_vertexArray);
    break;
  case  7:
    m_eval.receiveVertex(Face3(e1, e2, e3), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case  8:
    m_eval.receiveVertex(Face3(e1, e3, e2), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case  9:
    m_eval.receiveVertex(Face3(e1, e5, e6), m_vertexArray); 
    m_eval.receiveVertex(Face3(e1, e6, e2), m_vertexArray); m_statistics.m_faceCount+=2;
    break;
  case 10:
    m_eval.receiveVertex(Face3(e1, e3, e6), m_vertexArray);
    m_eval.receiveVertex(Face3(e1, e6, e4), m_vertexArray); m_statistics.m_faceCount+=2;
    break;
  case 11:
    m_eval.receiveVertex(Face3(e1, e5, e4), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case 12:
    m_eval.receiveVertex(Face3(e3, e2, e4), m_vertexArray);
    m_eval.receiveVertex(Face3(e3, e4, e5), m_vertexArray); m_statistics.m_faceCount+=2;
    break;
  case 13:
    m_eval.receiveVertex(Face3(e6, e2, e4), m_vertexArray); m_statistics.m_faceCount++;
    break;
  case 14:
    m_eval.receiveVertex(Face3(e5, e3, e6), m_vertexArray); m_statistics.m_faceCount++;
    break;
  }
}

// doCube: triangulate the cube directly, without decomposition
void SurfacePolygonizer::doCube(const HashedCube &cube) {
  const Array<CompactArray<char> > &polys = m_cubetable[cube.getIndex()];
  for(int i = 0; i < polys.size(); i++) {
	int a = -1, b = -1;
    const CompactArray<char> &vertexEdges = polys[i];
	for(int j = 0; j < vertexEdges.size(); j++) {
      const CubeEdge &edge = cubeEdge[vertexEdges[j]];
	  const HashedCorner &c1 = *cube.m_corners[edge.corner1];
	  const HashedCorner &c2 = *cube.m_corners[edge.corner2];
	  const int c = getVertexId(c1, c2);
      if(j >= 2) {
        m_eval.receiveVertex(Face3(c, b, a), m_vertexArray);
      } else {
        a = b;
      }
	  b = c;
	}
  }
}

// otherFace: Return face adjoining edge that is not the given face
static Face otherFace(int edge, Face face) {
  const CubeEdge &cedge = cubeEdge[edge];
  Face other = cedge.leftface;
  return face == other ? cedge.rightface : other;
}

// makeCubeTable: Create the 256 entry table for cubical polygonization
void SurfacePolygonizer::makeCubeTable() {
  for(int i = 0; i < ARRAYSIZE(m_cubetable); i++) {
    bool done[12];
    bool positiv[8];
    for(int j = 0; j < ARRAYSIZE(done); j++) {
	  done[j] = false;
    }

    for(int c = 0; c < ARRAYSIZE(positiv); c++)  {
      positiv[c] = BIT(i, c) ? true : false;
    }

    for(int e = 0; e < ARRAYSIZE(cubeEdge); e++) {
      const CubeEdge *ce = &cubeEdge[e];
	  if(!done[e] && (positiv[ce->corner1] != positiv[ce->corner2])) {
        CompactArray<char> vertexEdges;
		// get face that is to right of edge from pos to neg corner:
		Face face = positiv[ce->corner1] ? ce->rightface : ce->leftface;
		for(;;) {
		  Edge edge = ce->nextClockwiseEdge[face];
		  done[edge] = true;
          ce = &cubeEdge[edge];
		  if(positiv[ce->corner1] != positiv[ce->corner2]) {
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
  /*
  for(i = 0; i < ARRAYSIZE(m_cubetable); i++) {
    const Array<Array<char> > &a = m_cubetable[i];
    for(int j = 0; j < a.size(); j++) {
      const Array<char> &b = a[j];
      printf("(%3d,%d):",i,j);
      for(int k = 0; k < b.size(); k++)
        printf("%d ",b[k]);
      printf("\n");
    }
  }
  */
}

// setCenter: set (i,j,k) entry of table[]
// return false if already set; otherwise add to m_centerSet and return true
bool SurfacePolygonizer::setCenter(int i, int j, int k) {
  HashKey key(i,j,k);
  if(m_centerSet.contains(key)) {
    return false;
  } else {
    m_centerSet.add(key);
    return true;
  }
}

// getVertexId: return index for vertex on edge:
// c1.m_value and c2.m_value are presumed of different sign
// return saved index if any; else calculate and save vertex for later use
int SurfacePolygonizer::getVertexId(const HashedCorner &c1, const HashedCorner &c2) {
  EdgeKey edgeKey(c1.m_key,c2.m_key);
  int *p = m_edgeMap.get(edgeKey);
  if(p != NULL) {
    m_statistics.m_edgeHits++;
    return *p; // previously computed
  }
  SurfVertex v;
  v.m_position = converge(c1.m_point, c2.m_point, c1.m_value); // position
  v.m_normal = vnormal(v.m_position); // normal
  const int vid = m_vertexArray.size();
  m_vertexArray.add(v);
  m_edgeMap.put(edgeKey,vid);
  return vid;
}

// vnormal: calculate unit length surface normal at point
Point3D SurfacePolygonizer::vnormal(const Point3D &point) {
  Point3D result,p;
  const double f0 = evaluate(point);
  p = point; p.x += m_delta;
  result.x = evaluate(p) - f0;
  p = point; p.y += m_delta;
  result.y = evaluate(p) - f0;
  p = point; p.z += m_delta;
  result.z = evaluate(p) - f0;
  const double length = result.length();
  if(length != 0.0) {
	result /= length;
  }
  return result;
}

// converge: from two points of differing sign, converge to zero crossing
Point3D SurfacePolygonizer::converge(const Point3D &p1, const Point3D &p2, double v) {
  Point3D pos, neg;
  if(v < 0) {
	pos = p2;
	neg = p1;
  } else {
	pos = p1;
	neg = p2;
  }
  for(int i = 0;;) {
	Point3D result = (pos + neg)*0.5;
    if(i++ == RES) {
      return result;
    }
	if((evaluate(result)) > 0.0) {
	  pos = result;
    } else {
	  neg = result;
	}
  }
}
