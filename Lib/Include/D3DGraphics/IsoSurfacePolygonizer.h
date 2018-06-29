#pragma once

#include <Stack.h>
#include <CompactArray.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include "Cube3D.h"

typedef enum {
  LFACE // left   direction : -x, -i
 ,RFACE // right  direction : +x, +i
 ,BFACE // bottom direction : -y, -j
 ,TFACE // top    direction : +y, +j
 ,NFACE // near   direction : -z, -k
 ,FFACE // far    direction : +z, +k
} CubeFace;

typedef enum {
  LBN   // left  bottom near corner
 ,LBF   // left  bottom far  corner
 ,LTN   // left  top    near corner
 ,LTF   // left  top    far  corner
 ,RBN   // right bottom near corner
 ,RBF   // right bottom far  corner
 ,RTN   // right top    near corner
 ,RTF   // right top    far  corner
} CubeCorner;

String toString(CubeCorner cb);

// the LBN corner of cube (i, j, k), corresponds with location
// (start.x+(i-.5)*size, start.y+(j-.5)*size, start.z+(k-.5)*size)

//**** Cubical Polygonization ****
typedef enum {
  LB    // left   bottom edge
 ,LT    // left   top    edge
 ,LN    // left   near   edge
 ,LF    // left   far    edge
 ,RB    // right  bottom edge
 ,RT    // right  top    edge
 ,RN    // right  near   edge
 ,RF    // right  far    edge
 ,BN    // bottom near   edge
 ,BF    // bottom far    edge
 ,TN    // top    near   edge
 ,TF    // top    far    edge
 ,NN    // No edge
} CubeEdge;

class Point3DWithValue : public Point3D {
public:
  double m_value;    // Function value at m_point
  bool   m_positive;
  inline Point3DWithValue() {
  }
  inline Point3DWithValue(const Point3D &p) : Point3D(p) {
  }
  inline void setValue(double v) {
    m_positive = (m_value = v) > 0;
  }
  inline String toString(int precision=6) const {
    return format(_T("P:%s, V:%s")
                 ,__super::toString(precision).cstr()
                 ,::toString(m_value,precision,0,ios::showpos).cstr());
  }
};

// Test the function for a signed value
class IsoSurfaceTest : public Point3DWithValue {
public:
  bool m_ok; // True if value is of correct sign
  IsoSurfaceTest(const Point3D &p) : Point3DWithValue(p), m_ok(true) {
  }
};

// Surface vertex
class IsoSurfaceVertex {
public:
  // Position and surface normal
  Point3D m_position, m_normal;
  String toString(int dec=4) const {
    return format(_T("P:%s, N:%s"), m_position.toString(dec).cstr(), m_normal.toString(dec).cstr());
  }
};

class Point3DKey {
public:
  int i, j, k;
  inline Point3DKey() {}
  inline Point3DKey(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {
  }
  inline ULONG hashCode() const {
    return (i*2347 + j) * 2341 + k;
  }
  inline bool operator==(const Point3DKey &key) const {
    return (i==key.i) && (j==key.j) && (k==key.k);
  }
  inline String toString() const {
    return format(_T("(% 5d,% 5d,% 5d)"), i, j, k);
  }
};

//#define DUMP_STATISTICS
//#define DUMP_CUBES
//#define DUMP_CUBETABLE
//#define DUMP_CORNERMAP
//#define VALIDATE_OPPOSITESIGN
//#define VALIDATE_PUTFACE

class CubeEdgeHashKey {
private:
  Point3DKey m_key1;
  Point3DKey m_key2;
  void checkAndSwap();
public:
  inline CubeEdgeHashKey() {
  }
  inline CubeEdgeHashKey(const Point3DKey &key1, const Point3DKey &key2) : m_key1(key1), m_key2(key2) {
    checkAndSwap();
  }
  inline ULONG hashCode() const {
    return m_key1.hashCode() + m_key2.hashCode();
  }
  inline bool operator==(const CubeEdgeHashKey &k) const {
    return (m_key1 == k.m_key1) && (m_key2 == k.m_key2);
  }
};

// Corner of a cube
class HashedCubeCorner : public Point3DWithValue {
public:
  Point3DKey m_key;
  inline HashedCubeCorner() {
  }
  inline HashedCubeCorner(const Point3DKey &k, const Point3D &p)
    : m_key(k)
    , Point3DWithValue(p)
  {
  }
  inline String toString() const {
    return format(_T("HashedCubeCorner:(K:%s, %s)")
                 ,m_key.toString().cstr()
                 ,__super::toString(6).cstr(), m_value);
  }
};

class TriangleStrip {
public:
  UINT m_vertexId[10];
  char m_count;
  inline TriangleStrip() : m_count(0) {
  }
  inline TriangleStrip(UINT v0, UINT v1, UINT v2) : m_count(3) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2;
  }
  inline TriangleStrip(UINT v0, UINT v1, UINT v2, UINT v3) : m_count(4) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3;
  }
  inline TriangleStrip(UINT v0, UINT v1, UINT v2, UINT v3, UINT v4) : m_count(5) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3; m_vertexId[4] = v4;
  }
  inline TriangleStrip(UINT v0, UINT v1, UINT v2, UINT v3, UINT v4, UINT v5) : m_count(6) {
    m_vertexId[0] = v0; m_vertexId[1] = v1; m_vertexId[2] = v2; m_vertexId[3] = v3; m_vertexId[4] = v4; m_vertexId[5] = v5;
  }
  inline bool isEmpty() const {
    return m_count == 0;
  }
  String toString() const;
};

// Parameter to receiveFace
class Face3 {
public:
  // Indices into vertexArray
  UINT m_i1,m_i2,m_i3;
  D3DCOLOR m_color;
  Face3() {}
  inline Face3(UINT i1, UINT i2, UINT i3, D3DCOLOR color)
    : m_i1(i1), m_i2(i2), m_i3(i3), m_color(color)
  {
  }
};

// Partitioning cell (cube)
class StackedCube {
public:
  // Lattice location of cube
  Point3DKey              m_key;
  // Eight corners, each one in m_cornerMap
  const HashedCubeCorner *m_corners[8];
  inline StackedCube(int i, int j, int k) : m_key(i,j,k) {
    memset(m_corners, 0, sizeof(m_corners));
  }
  UINT getIndex() const;
  inline bool intersectSurface() const {
    const UINT index = getIndex();
    return index && (index < 255);
  }
  inline bool contains(const Point3D &p) const {
    return (*m_corners[LBN] <= p) && (p <= *m_corners[RTF]);
  }
  inline bool contains(const StackedCube &cube) const {
    return contains(*cube.m_corners[LBN]) && contains(*cube.m_corners[RTF]);
  }

  void validate();
  inline Point3D getSize() const {
    return *m_corners[RTF] - *m_corners[LBN];
  }
  inline Point3D getCenter() const {
    return (*m_corners[LBN] + *m_corners[RTF])/2;
  }
  String toString() const;
};

class IsoSurfaceEvaluator {
public:
  virtual double evaluate(const Point3D &p) = 0;
  virtual void   receiveFace(const Face3 &face) = 0;
  virtual void   receiveDebugVertices(int id,...) {
  };
};

class PolygonizerStatistics {
public:
  double m_threadTime;
  UINT   m_faceCount;
  UINT   m_vertexCount;
  UINT   m_cubeCount;
  UINT   m_cornerCount;
  UINT   m_edgeCount;
  UINT   m_cornerHits;
  UINT   m_edgeHits;
  UINT   m_zeroHits;
  UINT   m_evalCount;
  UINT   m_doCubeCalls;
  UINT   m_doTetraCalls;
  UINT   m_nonProduktiveCalls;
  String m_hashStat;

  PolygonizerStatistics();
  void clear();
  String toString() const;
};

class PolygonizerCubeArray : public Array<CompactArray<char> > {
private:
  const BYTE m_index;
public:
  PolygonizerCubeArray(UINT index);
#ifdef DUMP_CUBETABLE
  String toString() const;
#endif
};

class PolygonizerCubeArrayTable {
private:
  mutable PolygonizerCubeArray *m_table[256];
  void createTable() const {
    for(UINT i = 0; i < ARRAYSIZE(m_table); i++) {
      m_table[i] = new PolygonizerCubeArray(i); TRACE_NEW(m_table[i]);
    }
#ifdef DUMP_CUBETABLE
    debugLog(_T("CubeArray:\n%s"), toString().cstr());
#endif
  }
  void destroyTable() {
    for(UINT i = 0; i < ARRAYSIZE(m_table); i++) {
      SAFEDELETE(m_table[i]);
    }
  }
  inline bool isTableCreated() const {
    return m_table[0] != NULL;
  }
public:
  inline PolygonizerCubeArrayTable() {
    createTable(); // memset(m_table, 0, sizeof(m_table));
  }
  ~PolygonizerCubeArrayTable() {
    destroyTable();
  }
  inline const PolygonizerCubeArray &get(UINT index) const {
    return *m_table[index];
  }
#ifdef DUMP_CUBETABLE
  String toString() const;
#endif
};

class IsoSurfacePolygonizer {
private:
  static PolygonizerCubeArrayTable             s_cubetable;
  // Implicit surface function
  IsoSurfaceEvaluator                         &m_eval;
  // Cube size, normal delta
  double                                       m_cellSize, m_delta;
  // Bounding box. surface will be contained in this
  Cube3D                                       m_boundingBox;
  // Start point on surface
  Point3D                                      m_start;
  // Use tetrahedral decomposition
  bool                                         m_tetrahedralMode;
  // Active cubes
  Stack<StackedCube>                           m_cubeStack;
  // Surface vertices
  CompactArray<IsoSurfaceVertex>               m_vertexArray;
  // Cubes done so far
  CompactHashSet<Point3DKey>                   m_cubesDoneSet;
  // Corners of cubes
  CompactHashMap<Point3DKey, HashedCubeCorner> m_cornerMap;
  // Edge -> vertex id Map
  CompactHashMap<CubeEdgeHashKey, int>         m_edgeMap;
  CompactArray<Face3>                          m_face3Buffer;
  PolygonizerStatistics                        m_statistics;
  D3DCOLOR                                     m_color;

  Point3D             findStartPoint(const Point3D &start);
  IsoSurfaceTest      findStartPoint(bool positive, const Point3D &start);
  void                putInitialCube();
  void                addSurfaceVertices(const StackedCube &cube);
  inline void         doTetra(const StackedCube &cube, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
    doTetra(*cube.m_corners[c1], *cube.m_corners[c2], *cube.m_corners[c3], *cube.m_corners[c4]);
  }

  void                doTetra(const HashedCubeCorner &a, const HashedCubeCorner &b, const HashedCubeCorner &c, const HashedCubeCorner &d);
  void                doCube(const StackedCube &cube);
  bool                addToDoneSet(const Point3DKey &key);
  void                testFace (int i, int j, int k, const StackedCube &oldCube, CubeFace face, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4);
  void                resetTables();

  inline double       evaluate(const Point3D &p) {
    m_statistics.m_evalCount++;
    return m_eval.evaluate(p);
  }
  inline void         putFace3(UINT i1, UINT i2, UINT i3) {
#ifdef VALIDATE_PUTFACE
    if(i1 >= m_vertexArray.size() || i2 >= m_vertexArray.size() || i3 >= m_vertexArray.size()) {
      throwException(_T("Invalid face(%u,%d,%d). vertexArray.size==%u"), i1,i2,i3, m_vertexArray.size());
    }
#endif
    m_face3Buffer.add(Face3(i1,i2,i3,m_color));
  }
  inline void         putFace3R(UINT i1, UINT i2, UINT i3) {
#ifdef VALIDATE_PUTFACE
    if(i1 >= m_vertexArray.size() || i2 >= m_vertexArray.size() || i3 >= m_vertexArray.size()) {
      throwException(_T("Invalid face(%u,%d,%d). vertexArray.size==%u"), i1,i2,i3, m_vertexArray.size());
    }
#endif
    m_face3Buffer.add(Face3(i1,i3,i2,m_color));
  }
  void                putTriangleStrip( const TriangleStrip &face);
  void                putTriangleStripR(const TriangleStrip &face);
  void                flushFaceBuffer();
  inline bool         hasActiveCubes() const {
    return !m_cubeStack.isEmpty();
  }
  inline StackedCube      getActiveCube() {
    return m_cubeStack.pop();
  }
  void                    pushCube(const StackedCube &cube);

  UINT                    getVertexId(const HashedCubeCorner &c1, const HashedCubeCorner &c2);
  Point3D                 getNormal(const Point3D &point);
  const HashedCubeCorner *getCorner(int i, int j, int k);

  inline Point3D          getCornerPoint(const Point3DKey &key) const {
    return getCornerPoint(key.i,key.j,key.k);
  }
  Point3D                 getCornerPoint(int i, int j, int k) const;
  Point3D                 converge(const Point3DWithValue &p1, const Point3DWithValue &p2, int itCount = 0);
  void                    saveStatistics(double startTime);
  void                    dumpCornerMap();
public:
  IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval);
  ~IsoSurfacePolygonizer();
  // Polygonize eval (set of points, p, where eval.evaluate(p) == 0)
  // start                 : The point where the search for a zero-value of eval will start.
  // double size           : Width of the partitioning cube
  // int bounds            : max. range of cubes (+/- on the three axes) from first cube
  // bool tetrahedralmode  : if true, use tetrahedral decomposition of a rectangular cube. false to use cube directly
  void polygonize(const Point3D &start
                 ,double         cellSize
                 ,const Cube3D  &boundingBox
                 ,bool           tetrahedralMode
                 );
  inline const PolygonizerStatistics &getStatistics() const {
    return m_statistics;
  }
  inline const CompactArray<IsoSurfaceVertex> &getVertexArray() const {
    return m_vertexArray;
  }
};
