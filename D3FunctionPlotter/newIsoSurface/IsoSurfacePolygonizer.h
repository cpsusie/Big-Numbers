#pragma once

#include <Stack.h>
#include <Array.h>
#include <CompactArray.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include <HashMap.h>
#include <Math/PragmaLib.h>
#include "Cube3D.h"

#define MAX_SPLITLEVEL 1

typedef enum {
  LFACE // left   direction : -x, -i
 ,RFACE // right  direction : +x, +i
 ,BFACE // bottom direction : -y, -j
 ,TFACE // top    direction : +y, +j
 ,NFACE // near   direction : -z, -k
 ,FFACE // far    direction : +z, +k
} CubeFace;

typedef enum {
  LBN // left  bottom near corner
 ,LBF // left  bottom far  corner
 ,LTN // left  top    near corner
 ,LTF // left  top    far  corner
 ,RBN // right bottom near corner
 ,RBF // right bottom far  corner
 ,RTN // right top    near corner
 ,RTF // right top    far  corner
} CubeCorner;

String toString(CubeCorner cb);

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
} CubeEdge;

class IsoSurfaceTest {             // Test the function for a signed value
public:
  Point3D m_point;              // Location of test
  bool    m_positive;           // Function value at m_point
  bool    m_ok;                 // true if value is of correct sign
};

class SurfaceVertex {           // Surface vertex
public:
  Point3D m_position, m_normal; // Position and surface normal
  String toString(int dec=4) const {
    return format("P:%s, N:%s", m_position.toString(dec).cstr(), m_normal.toString(dec).cstr());
  }
};

class PointKey {
public:
  int i, j, k;
  inline PointKey() {}
  inline PointKey(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {
  }
  inline unsigned long hashCode() const {
    return (i*2347 + j) * 2341 + k;
  }
  inline bool operator==(const PointKey &key) const {
    return (i==key.i) && (j==key.j) && (k==key.k);
  }
  inline String toString() const {
    return format("(% 5d,% 5d,% 5d)", i, j, k);
  }
};

//#define VALIDATE_SUBCUBES
//#define VALIDATE_AVG2
//#define VALIDATE_AVG4
//#define DUMP_CUBES
//#define DUMP_CUBETABLE
//#define VALIDATE_OPPOSITESIGN
//#define VALIDATE_PUTFACE

inline PointKey avg2(const PointKey &k1, const PointKey &k2) { // assume all integers even
#ifdef VALIDATE_AVG2
  PointKey result(k1.i+k2.i,k1.j+k2.j,k1.k+k2.k);
  if((result.i|result.j|result.k)&1) {
    throwException("avg2:result not dividable by 2. result:%s", result.toString().cstr());
  }
  result.i /= 2; result.j /= 2; result.k /= 2;
  return result;
#else
  return PointKey((k1.i+k2.i)/2,(k1.j+k2.j)/2,(k1.k+k2.k)/2);
#endif
}

inline PointKey avg4(const PointKey &k1, const PointKey &k2, const PointKey &k3, const PointKey &k4) { // assume all integers even
#ifdef VALIDATE_AVG4
  PointKey result(k1.i+k2.i+k3.i+k4.i,k1.j+k2.j+k3.j+k4.j,k1.k+k2.k+k3.k+k4.k);
  if((result.i|result.j|result.k)&3) {
    throwException("avg4:result not dividable by 4. result:%s", result.toString().cstr());
  }
  result.i /= 4; result.j /= 4; result.k /= 4;
  return result;
#else
  return PointKey((k1.i+k2.i+k3.i+k4.i)/4,(k1.j+k2.j+k3.j+k4.j)/4,(k1.k+k2.k+k3.k+k4.k)/4);
#endif
}

class EdgeHashKey {
private:
  PointKey m_key1;
  PointKey m_key2;
  void checkAndSwap();
public:
  inline EdgeHashKey() {}
  inline EdgeHashKey(const PointKey &key1, const PointKey &key2) : m_key1(key1), m_key2(key2) {
    checkAndSwap();
  }
  inline unsigned long hashCode() const {
    return m_key1.hashCode() + m_key2.hashCode();
  }
  inline bool operator==(const EdgeHashKey &k) const {
    return (m_key1 == k.m_key1) && (m_key2 == k.m_key2);
  }
};

class HashedCorner {            // Corner of a cube
public:
  PointKey m_key;
  Point3D  m_point;             // Location
  bool     m_positive;
  inline HashedCorner(const PointKey &k, const Point3D &p) : m_key(k) {
    m_point = p;
  }
  String toString() const {
    return format("HashedCorner:(K:%s, P:%s, %c)", m_key.toString().cstr(), m_point.toString(6).cstr(), m_positive?'+':'-');
  }
};

class TriangleStrip {
public:
  unsigned int  m_vertexId[10];
  char m_count;
  inline TriangleStrip() : m_count(0) {
  }
  inline TriangleStrip(unsigned int v0, unsigned int v1, unsigned int v2) : m_count(3) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2;
  }
  inline TriangleStrip(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3) : m_count(4) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3;
  }
  inline TriangleStrip(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4) : m_count(5) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3; m_vertexId[4] = v4;
  }
  inline TriangleStrip(unsigned int v0, unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4, unsigned int v5) : m_count(6) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3; m_vertexId[4] = v4; m_vertexId[5] = v5;
  }
  inline bool isEmpty() const {
    return m_count == 0;
  }
  String toString() const;
};

class Face3 {                   // Parameter to receiveFace
public:
  unsigned int m_i1,m_i2,m_i3;  // Indices into vertexArray
  unsigned int m_diffuseIndex;
  Face3() {}
  inline Face3(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int diffuseIndex) 
    : m_i1(i1), m_i2(i2), m_i3(i3)
    , m_diffuseIndex(diffuseIndex)
  {
  }
};

class StackedCube {                 // Partitioning cell (cube)
public:
  PointKey            m_key;        // Lattice location of cube
  unsigned char       m_level;
  const HashedCorner *m_corners[8]; // Eight corners, each one in m_cornerMap
  inline StackedCube(int i, int j, int k, unsigned char level) : m_key(i,j,k), m_level(level) {
    memset(m_corners, 0, sizeof(m_corners));
  }
  unsigned int getIndex() const;
  inline bool intersectSurface() const {
    const unsigned int index = getIndex();
    return index && (index < 255);
  }
  inline bool contains(const Point3D &p) const {
    return (m_corners[LBN]->m_point <= p) && (p <= m_corners[RTF]->m_point);
  }
  inline bool contains(const StackedCube &cube) const {
    return contains(cube.m_corners[LBN]->m_point) && contains(cube.m_corners[RTF]->m_point);
  }

  void validate();
  inline Point3D getSize() const {
    return m_corners[RTF]->m_point - m_corners[LBN]->m_point;
  }
  inline Point3D getCenter() const {
    return (m_corners[LBN]->m_point + m_corners[RTF]->m_point)/2;
  }
  String toString() const;
};

class SubCube {
public:
  unsigned int m_cornerIndex[8];
};

class IsoSurfaceEvaluator {
public:
  virtual double evaluate(const Point3D &p) = 0;
  virtual void   receiveFace(const Face3 &face) = 0;
  virtual void   receiveDebugVertices(int id,...) {
  };
};

class PolygonizerStatistics {
private:
  String getNonProdutiveString() const;
  String getDoTetraCallsString() const;
public:
  unsigned int m_cubeCount;
  unsigned int m_cornerCount;
  unsigned int m_nonProduktiveCount[MAX_SPLITLEVEL+1];
  unsigned int m_doTetraCallCount[MAX_SPLITLEVEL+1];
  unsigned int m_doCubeCalls;
  unsigned int m_edgeCount;
  unsigned int m_vertexCount;
  unsigned int m_faceCount;
  unsigned int m_evalCount;
  unsigned int m_cornerHits;
  unsigned int m_edgeHits;
  unsigned int m_exactZeroHits;
  double       m_threadTime;

  String       m_hashStat;

  PolygonizerStatistics();
  void clear();
  String toString() const;
};

#define PURPLE          0
#define GREEN           1
#define RED             2
#define DARKRED         3
#define LIGHT_BLUE      4 
#define MIDDLE_BLUE     5 
#define BLUE            6 
#define DARK_BLUE       7 
#define YELLOW          8 
#define PINK            9

class IsoSurfacePolygonizer {
private:
  static Array<CompactArray<char> > m_cubetable[256];
  IsoSurfaceEvaluator               &m_eval;            // Implicit surface function
  double                             m_size, m_delta;   // Cube size, normal delta
  Cube3D                             m_boundingBox;     // bounding box
  Point3D                            m_start;           // Start point on surface
  bool                               m_tetrahedralMode; // Use tetrahedral decomposition
  bool                               m_adaptSize;       // if true, cell size will adapt to sharp curve edges
  Stack<StackedCube>                 m_cubeStack;       // Active cubes and subcubes.
  CompactArray<SurfaceVertex>        m_vertexArray;     // Surface vertices
  CompactHashSet<PointKey>           m_cubesDoneSet;    // Cubes done so far
  HashMap<PointKey, HashedCorner>    m_cornerMap;       // Corners of cubes
  CompactHashMap<EdgeHashKey, int>   m_edgeMap;         // Edge -> vertex id Map
  CompactArray<Face3>                m_face3Buffer;
  PolygonizerStatistics              m_statistics;

  friend class InitPolygonizerCubeTable;
  static void         makeCubeTable();
  static void         clearCubeTable();
#ifdef DUMP_CUBETABLE
  static void         dumpCubeTable(const String &fileName="");
#endif

  Point3D             findStartPoint(const Point3D &start);
  IsoSurfaceTest      findStartPoint(bool positive, const Point3D &start);
  void                putInitialCube();
  void                addSurfaceVertices(const StackedCube &cube);
  inline void         doTetra(const StackedCube &cube, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
    doTetra(*cube.m_corners[c1], *cube.m_corners[c2], *cube.m_corners[c3], *cube.m_corners[c4], cube.m_level, GREEN);
  }

  int                 doTetra(const HashedCorner &a, const HashedCorner &b, const HashedCorner &c, const HashedCorner &d, int level, int colorIndex);
  void                splitTetraPNNN(const HashedCorner &p, const HashedCorner &n1, const HashedCorner &n2, const HashedCorner &n3, int v1, int v2, int v3, int level);
  void                splitTetraNPPP(const HashedCorner &p, const HashedCorner &n1, const HashedCorner &n2, const HashedCorner &n3, int v1, int v2, int v3, int level);
  void                splitTetraPPNN(const HashedCorner &a, const HashedCorner &b , const HashedCorner &c , const HashedCorner &d , int bc, int ac, int ad, int bd, int level);
  void                doCube(const StackedCube &cube);
  bool                addToDoneSet(const PointKey &key);
  void                testFace (int i, int j, int k, const StackedCube &oldCube, CubeFace face, CubeCorner c1, CubeCorner  c2, CubeCorner c3, CubeCorner c4);
  void                resetTables();

  inline double       evaluate(const Point3D &p) {
    m_statistics.m_evalCount++;
    return m_eval.evaluate(p);
  }
  inline void         putFace3(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int diffuseIndex) {
#ifdef VALIDATE_PUTFACE
    if(i1 >= m_vertexArray.size() || i2 >= m_vertexArray.size() || i3 >= m_vertexArray.size()) {
      throwException("Invalid face(%u,%d,%d). vertexArray.size==%u", i1,i2,i3, m_vertexArray.size());
    }
#endif
    m_face3Buffer.add(Face3(i1,i2,i3, diffuseIndex));
  }
  inline void         putFace3R(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int diffuseIndex) {
#ifdef VALIDATE_PUTFACE
    if(i1 >= m_vertexArray.size() || i2 >= m_vertexArray.size() || i3 >= m_vertexArray.size()) {
      throwException("Invalid face(%u,%d,%d). vertexArray.size==%u", i1,i2,i3, m_vertexArray.size());
    }
#endif
    m_face3Buffer.add(Face3(i1,i3,i2, diffuseIndex));
  }
  void                putTriangleStrip( const TriangleStrip &face, unsigned int diffuseIndex);
  void                putTriangleStripR(const TriangleStrip &face, unsigned int diffuseIndex);
  void                flushFaceBuffer();
  bool                needSplit(const TriangleStrip &face) const;
  inline bool         hasActiveCubes() const {
    return !m_cubeStack.isEmpty();
  }
  inline StackedCube  getActiveCube() {
    return m_cubeStack.pop();
  }
  void                pushCube(const StackedCube &cube);
  void                addSubCube(const StackedCube &oldCube, CubeCorner commonCorner);

  unsigned int        getVertexId(const HashedCorner &c1, const HashedCorner &c2);
  Point3D             getNormal(const Point3D &point);
  const HashedCorner *getCorner(     int i, int j, int k);
  const HashedCorner *avg2(const HashedCorner &a, const HashedCorner &b);
  const HashedCorner *avg4(const HashedCorner &a, const HashedCorner &b
                          ,const HashedCorner &c, const HashedCorner &d);

  inline Point3D             getCornerPoint(const PointKey &key) const {
    return getCornerPoint(key.i,key.j,key.k);
  }
  Point3D             getCornerPoint(int i, int j, int k) const;
  Point3D             converge(const Point3D &p1, const Point3D &p2, bool p1Positive, int itCount = 0);
  void                saveStatistics(double startTime);
  void                dumpCornerMap();
public:
  IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval);
  ~IsoSurfacePolygonizer();
  void polygonize(const Point3D &start
                 ,double         size
                 ,const Cube3D  &boundingBox
                 ,bool           tetrahedralMode
                 ,bool           adaptSize
                 );
  inline const PolygonizerStatistics &getStatistics() const {
    return m_statistics;
  }
  inline const CompactArray<SurfaceVertex> &getVertexArray() const {
    return m_vertexArray;
  }

};
