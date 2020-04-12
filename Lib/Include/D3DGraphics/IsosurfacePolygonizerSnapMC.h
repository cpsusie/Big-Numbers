#pragma once

#include <Stack.h>
#include <CompactArray.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include <Random.h>
#include <Math/Cube3D.h>
#include <DebugLog.h>

namespace ISOSURFACE_POLYGONIZER_SNAPMC {

typedef enum {                        // x y z  x={left,right}, y={near,far}, z={bottom,top}
  LBN   // left  bottom near corner  <c> 0 0 0 </c> 0
 ,RBN   // right bottom near corner  <c> 2 0 0 </c> 1
 ,LBF   // left  bottom far  corner  <c> 0 2 0 </c> 2
 ,RBF   // right bottom far  corner  <c> 2 2 0 </c> 3
 ,LTN   // left  top    near corner  <c> 0 0 2 </c> 4
 ,RTN   // right top    near corner  <c> 2 0 2 </c> 5
 ,LTF   // left  top    far  corner  <c> 0 2 2 </c> 6
 ,RTF   // right top    far  corner  <c> 2 2 2 </c> 7
} CubeCorner;

typedef enum {
  LFACE // left   direction : -x, -i <f> 0 2 6 4 </f> LBN LBF LTF LTN  0
 ,RFACE // right  direction : +x, +i <f> 1 3 7 5 </f> RBN RBF RTF RTN  1
 ,NFACE // near   direction : -y, -j <f> 0 1 5 4 </f> LBN RBN RTN LTN  2
 ,FFACE // far    direction : +y, +j <f> 2 3 7 6 </f> LBF RBF RTF LTF  3
 ,BFACE // bottom direction : -z, -k <f> 0 1 3 2 </f> LBN RBN RBF LBF  4
 ,TFACE // top    direction : +z, +k <f> 4 5 7 6 </f> LTN RTN RTF LTF  5
} CubeFace;


String toString(CubeCorner cb);

// the LBN corner of cube (i, k, j), corresponds with location
// (start.x+(i-.5)*size, start.y+(j-.5)*size, start.z+(k-.5)*size)

//**** Cubical Polygonization ****
typedef enum {
  BN    // bottom near   edge  0 <v> 0 1 </v> LBN RBN = BN
 ,LB    // left   bottom edge  1 <v> 0 2 </v> LBN LBF = LB
 ,RB    // right  bottom edge  2 <v> 1 3 </v> RBN RBF = RB
 ,BF    // bottom far    edge  3 <v> 2 3 </v> LBF RBF = BF
 ,LN    // left   near   edge  4 <v> 0 4 </v> LBN LTN = LN
 ,RN    // right  near   edge  5 <v> 1 5 </v> RBN RTN = RN
 ,LF    // left   far    edge  6 <v> 2 6 </v> LBF LTF = LF
 ,RF    // right  far    edge  7 <v> 3 7 </v> RBF RTF = RF
 ,TN    // top    near   edge  8 <v> 4 5 </v> LTN RTN = TN
 ,LT    // left   top    edge  9 <v> 4 6 </v> LTN LTF = LT
 ,RT    // right  top    edge 10 <v> 5 7 </v> RTN RTF = RT
 ,TF    // top    far    edge 11 <v> 6 7 </v> LTF RTF = TF
 ,NN    // No edge
} CubeEdge;

typedef enum {
  CVLBN   // corner vertex 0
 ,CVRBN   // corner vertex 1
 ,CVLBF   // corner vertex 2
 ,CVRBF   // corner vertex 3
 ,CVLTN   // corner vertex 4
 ,CVRTN   // corner vertex 5
 ,CVLTF   // corner vertex 6
 ,CVRTF   // corner vertex 7
 ,EVBN    // edge vertex   8   midpoint edge BN
 ,EVLB    // edge vertex   9   midpoint edge LB
 ,EVRB    // edge vertex   10  midpoint edge RB
 ,EVBF    // edge vertex   11  midpoint edge BF
 ,EVLN    // edge vertex   12  midpoint edge LN
 ,EVRN    // edge vertex   13  midpoint edge RN
 ,EVLF    // edge vertex   14  midpoint edge LF
 ,EVRF    // edge vertex   15  midpoint edge RF
 ,EVTN    // edge vertex   16  midpoint edge TN
 ,EVLT    // edge vertex   17  midpoint edge LT
 ,EVRT    // edge vertex   18  midpoint edge RN
 ,EVTF    // edge vertex   19  midpoint edge TF
} IsoVertex;

inline bool isCubeCorner(IsoVertex iv) {
  return iv < 8;
}

inline bool isCubeEdge(IsoVertex iv) {
  return iv >= 8;
}

// assume iv = [CVLBN..CVRTF] i.e. < 8
inline CubeCorner isoVertexToCubeCorner(IsoVertex iv) {
  assert(isCubeCorner(iv));
  return (CubeCorner)iv;
}

// assume iv = [EVBN..EVTF] i.e. >= 8
inline CubeEdge isoVertexToCubeEdge(IsoVertex iv) {
  assert(isCubeEdge(iv));
  return (CubeEdge)(iv - 8);
}

typedef enum {
  V_NEGATIVE
 ,V_ZERO
 ,V_POSITIVE
} GridLabel;

extern const TCHAR *labelChars;

inline TCHAR getLabelChar(GridLabel l) {
  return labelChars[l];
}

inline bool hasOppositeSign(GridLabel l1, GridLabel l2) {
  return (l1 ^ l2) == 2;
}

class Point3DWithValue : public Point3D {
public:
  double    m_value;    // Function value at m_point
  GridLabel m_label;
  inline Point3DWithValue() {
  }
  inline Point3DWithValue(const Point3D &p) : Point3D(p) {
  }
  inline void setValue(double v) {
    m_label = (m_value = v) < 0 ? V_NEGATIVE : (v > 0) ? V_POSITIVE : V_ZERO;
  }
  inline bool isPositive() const {
    return m_label == V_POSITIVE;
  }
  inline String toString(int precision=6) const {
    return format(_T("P:(%+.*le,%+.*le,%+.*le), V:%+.*le (%c)")
                 ,precision,x,precision,y,precision,z,precision,m_value, getLabelChar(m_label));
  }
};

inline bool hasOppositeSign(const Point3DWithValue &p1, const Point3DWithValue &p2) {
  return hasOppositeSign(p1.m_label, p2.m_label);
}

// Test the function for a signed value
class IsoSurfaceTest : public Point3DWithValue {
public:
  bool m_ok; // True if value is of correct sign
  IsoSurfaceTest(const Point3D &p) : Point3DWithValue(p), m_ok(true) {
  }
};

// Parameter to receiveFace
class Face3 {
public:
  // Indices into vertexArray
  UINT     m_i1,m_i2,m_i3;
  D3DCOLOR m_color;
  Face3() {}
  inline Face3(UINT i1, UINT i2, UINT i3, D3DCOLOR color)
    : m_i1(i1), m_i2(i2), m_i3(i3), m_color(color)
  {
  }
#ifdef __NEVER__
  Face3 &reverseOrientation() {
    std::swap(m_i1, m_i2);
    return *this;
  }
#endif
  inline void reset() {
    m_i1 = m_i2 = m_i3 = 0;
    m_color = 0;
  }
  inline String toString() const {
    return format(_T("(%5u,%5u,%5u)"),m_i1,m_i2,m_i3);
  }
};

// Surface vertex
class IsoSurfaceVertex {
public:
  // Position and surface normal
  Point3DP m_position, m_normal;
  inline void reset() {
    m_position = m_normal = D3DXORIGIN;
  }
  String toString(int precision=6) const {
    return format(_T("P:(%+.*le,%+.*le,%+.*le), N:(%+.*le,%+.*le,%+.*le)")
                 ,precision,m_position.x,precision,m_position.y,precision,m_position.z
                 ,precision,m_normal.x  ,precision,m_normal.y  ,precision,m_normal.z);
  }
};

typedef CompactArray<IsoSurfaceVertex> IsoSurfaceVertexArray;

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
  inline bool operator!=(const Point3DKey &key) const {
    return !(*this == key);
  }
  inline String toString() const {
    return format(_T("(% 5d,% 5d,% 5d)"), i, j, k);
  }
};

//#define DUMP_STATISTICS
//#define DUMP_CUBES
//#define DUMP_CUBETABLE
//#define DUMP_CORNERMAP
//#define DUMP_EDGEMAP
//#define DUMP_VERTEXARRAY
//#define DUMP_FACEARRAY
#define VALIDATE_OPPOSITESIGN
//#define VALIDATE_PUTFACE
//#define VALIDATE_CUBES
//#define DEBUG_POLYGONIZER // should be defined in compile-options, to make it defined in D3FunctionPlotter too

class CubeEdgeHashKey {
private:
  Point3DKey m_key1, m_key2;
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
  String toString() const {
    return format(_T("EdgeKey:(%s,%s)"), m_key1.toString().cstr(), m_key2.toString().cstr());
  }
  inline const Point3DKey &getKey1() const {
    return m_key1;
  }
  inline const Point3DKey &getKey2() const {
    return m_key2;
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
    return format(_T("CubeCorner:(Key:%s, %s)")
                 ,m_key.toString().cstr()
                 ,__super::toString().cstr()
                 );
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

// Partitioning cell (cube)
class StackedCube {
private:
  mutable int m_index;
  UINT calculateIndex() const;
#ifdef VALIDATE_CUBES
  void verifyCubeConstraint(int line, const TCHAR *expr) const;
#endif // VALIDATE_CUBES
public:
  // Lattice location of cube
  Point3DKey              m_key;
  // Eight corners, each one in m_cornerMap
  const HashedCubeCorner *m_corners[8];
  inline StackedCube(int i, int j, int k) : m_key(i,j,k), m_index(-1) {
    memset(m_corners, 0, sizeof(m_corners));
  }
  inline UINT getIndex() const {
    if(m_index < 0) m_index = calculateIndex();
    return m_index;
  }
  inline bool intersectSurface() const {
    const UINT index = getIndex();
    return index && (index < 6561);
  }
  inline bool contains(const Point3D &p) const {
    return (*m_corners[LBN] <= p) && (p <= *m_corners[RTF]);
  }
  inline bool contains(const StackedCube &cube) const {
    return contains(*cube.m_corners[LBN]) && contains(*cube.m_corners[RTF]);
  }
  inline Point3D getSize() const {
    return *m_corners[RTF] - *m_corners[LBN];
  }
  inline Point3DP getCenter() const {
    return (*m_corners[LBN] + *m_corners[RTF])/2;
  }
#ifdef VALIDATE_CUBES
  void validate() const;
#endif // VALIDATE_CUBES

  String toString() const;
};

#ifdef DEBUG_POLYGONIZER
class Octagon {
private:
  const StackedCube *m_cube;
public:
  inline Octagon() : m_cube(NULL) {
  }
  inline Octagon(const StackedCube &cube) : m_cube(&cube) {
  }
  inline const StackedCube &getCube() const {
    return *m_cube;
  }
  inline BYTE getLevel() const {
    return 0;
  }
  inline const D3DXVECTOR3 getCenter() const {
    return Point3DP(getCube().getCenter());
  }
  inline UINT getCornerCount() const {
    return 8;
  }
  inline const HashedCubeCorner &getHashedCorner(UINT index) const {
    return *getCube().m_corners[index];
  }
  inline bool isEmpty() const {
    return m_cube == NULL;
  }
  String toString() const {
    return isEmpty() ? _T("Octa:---")
                     : format(_T("Octa:%s"), m_cube->toString().cstr());
  }
};

class Tetrahedron {
private:
  CubeCorner m_corner[4];
public:
  inline Tetrahedron() {
    m_corner[0] = m_corner[1] = m_corner[2] = m_corner[3] = LBN;
  }
  inline Tetrahedron(CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
    m_corner[0] = c1;  m_corner[1] = c2; m_corner[2] = c3; m_corner[3] = c4;
  }
  CubeCorner getCorner(UINT index) const {
    return m_corner[index];
  }
  inline UINT getCornerCount() const {
    return 4;
  }
  inline bool isEmpty() const {
    return m_corner[0] == m_corner[1];
  }
  inline bool operator==(const Tetrahedron &t) const {
    return memcmp(m_corner, t.m_corner, sizeof(m_corner)) == 0;
  }
  inline bool operator!=(const Tetrahedron &t) const {
    return !(*this == t);
  }
  ULONG hashCode() const {
    ULONG c = m_corner[0];
    for(int i = 1; i < 4; i++) {
      c *= 13;
      c += m_corner[i];
    }
    return c;
  }
  String toString() const {
    return isEmpty() ? _T("Tetra:---")
                     : format(_T("Tetra:Corners:[%s,%s,%s,%s]")
                             ,::toString(m_corner[0]).cstr()
                             ,::toString(m_corner[1]).cstr()
                             ,::toString(m_corner[2]).cstr()
                             ,::toString(m_corner[3]).cstr());
  }
};

#endif // DEBUG_POLYGONIZER

class IsoSurfaceEvaluator {
public:
  virtual double evaluate(const Point3D &p) = 0;
  virtual void   receiveFace(const Face3 &face) = 0;
  virtual void   receiveDebugVertices(int id,...) {
  }
#ifdef DEBUG_POLYGONIZER
  virtual void   markCurrentOcta(  const Octagon          &octa  ) {
  }
  virtual void   markCurrentTetra( const Tetrahedron      &tetra ) {
  }
  virtual void   markCurrentFace(  const Face3            &face  ) {
  }
  virtual void   markCurrentVertex(const IsoSurfaceVertex &vertex) {
  }
#endif // DEBUG_POLYGONIZER
};

class PolygonizerStatistics {
public:
  double m_threadTime;
  UINT   m_cubeCount;
  UINT   m_faceCount;
  UINT   m_vertexCount;
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

class Simplex {
public:
  IsoVertex m_v[3];
  inline Simplex(const BYTE *vp) {
    m_v[0] = (IsoVertex)(*(vp++) & 0x1f);
    m_v[1] = (IsoVertex)*(vp++);
    m_v[2] = (IsoVertex)*(vp++);
  }
};

class SimplexArray {
private:
  const BYTE  m_n;
  const BYTE *m_triangles;
public:
  inline SimplexArray(const BYTE *lookupEntry)
    : m_n(*lookupEntry >> 5)
    , m_triangles((BYTE*)lookupEntry)
  {
  }
  inline UINT size() const {
    return m_n;
  }
  inline bool isEmpty() const {
    return size() == 0;
  }
  inline Simplex operator[](UINT i) const {
    if(i >= size()) {
      throwInvalidArgumentException(__TFUNCTION__, _T("index %u out of range, size=%u"), i, size());
    }
    return Simplex(m_triangles + 3 * i);
  }
};

class PolygonizerCubeArrayTable {
private:
  static const BYTE  s_isosurfaceLookup[];
  CompactUshortArray m_vlistStart; // index by cubes index and initialized in constructor
public:
  PolygonizerCubeArrayTable();
  inline SimplexArray get(UINT index) const {
    return SimplexArray(s_isosurfaceLookup + m_vlistStart[index]);
  }
};

class IsoSurfacePolygonizer {
private:
  static const PolygonizerCubeArrayTable       s_cubetable;
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
  bool                                         m_tetraOptimize4;
  // Active cubes
  Stack<StackedCube>                           m_cubeStack;
  // Surface vertices
  IsoSurfaceVertexArray                        m_vertexArray;
  // Cubes done so far
  CompactHashSet<Point3DKey>                   m_cubesDoneSet;
  // Corners of cubes
  CompactHashMap<Point3DKey, HashedCubeCorner> m_cornerMap;
  // Edge -> index into m_vertexArray
  CompactHashMap<CubeEdgeHashKey, UINT>        m_edgeMap;
  CompactArray<Face3>                          m_face3Buffer, m_faceArray;
  PolygonizerStatistics                        m_statistics;
  D3DCOLOR                                     m_color;
  JavaRandom                                   m_rnd;

  Point3D             findStartPoint(const Point3D &start);
  IsoSurfaceTest      findStartPoint(bool positive, const Point3D &start);
  bool                putInitialCube();
  void                addSurfaceVertices(const StackedCube &cube);
  inline void         doTetra(const StackedCube &cube, CubeCorner c1, CubeCorner c2, CubeCorner c3, CubeCorner c4) {
#ifdef DEBUG_POLYGONIZER
    m_eval.markCurrentTetra(Tetrahedron(c1,c2,c3,c4));
#endif // DEBUG_POLYGONIZER
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
#ifdef __NEVER__
  bool checkOrientation(const Face3 &f) const;
#endif
  inline void         putFace3(UINT i1, UINT i2, UINT i3) {
#ifdef VALIDATE_PUTFACE
    if(i1 >= m_vertexArray.size() || i2 >= m_vertexArray.size() || i3 >= m_vertexArray.size()) {
      throwException(_T("Invalid face(%u,%d,%d). vertexArray.size==%u"), i1,i2,i3, m_vertexArray.size());
    }
#endif
    Face3 f(i1, i2, i3, m_color);
//    if(!checkOrientation(f)) {
//      f.reverseOrientation();
//    }
    m_face3Buffer.add(f);
#ifdef DEBUG_POLYGONIZER
    m_eval.markCurrentFace(f);
#endif // DEBUG_POLYGONIZER
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
  inline void         clearFaceBuffer() {
    m_face3Buffer.clear(-1);
  }
  void                flushFaceArray();
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
  Point3D                 convergeStartPoint(const Point3DWithValue &p1, const Point3DWithValue &p2, int itCount);
  Point3D                 converge(          const Point3DWithValue &p1, const Point3DWithValue &p2, int itCount = 0);
  void                    saveStatistics(double startTime);
  void                    dumpCornerMap()   const;
  void                    dumpEdgeMap()     const;
  void                    dumpVertexArray() const;
  void                    dumpFaceArray()   const;
public:
  IsoSurfacePolygonizer(IsoSurfaceEvaluator &eval);
  ~IsoSurfacePolygonizer();
  // Polygonize eval (set of points, p, where eval.evaluate(p) == 0)
  // start                 : The point where the search for a zero-value of eval will start.
  // double size           : Width of the partitioning cube
  // int bounds            : max. range of cubes (+/- on the three axes) from first cube
  // bool tetrahedralmode  : if true, use tetrahedral decomposition of a rectangular cube. false to use cube directly
  // bool tetraOptimize4   : only active if tetrahedralmode==true
  void polygonize(const Point3D &start
                 ,double         cellSize
                 ,const Cube3D  &boundingBox
                 ,bool           tetrahedralMode
                 ,bool           tetraOptimize4
                 );
  inline BYTE getCurrentLevel() const {
    return 0;
  }
  inline const PolygonizerStatistics &getStatistics() const {
    return m_statistics;
  }
  inline const IsoSurfaceVertexArray &getVertexArray() const {
    return m_vertexArray;
  }
};

}; // namespace
