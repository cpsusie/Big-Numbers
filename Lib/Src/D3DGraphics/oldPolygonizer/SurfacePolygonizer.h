#pragma once

#include <Stack.h>
#include <Array.h>
#include <CompactArray.h>
#include <CompactHashSet.h>
#include <CompactHashMap.h>
#include <HashMap.h>
#include <Math/PragmaLib.h>
#include "D3Math.h"

class SurfaceTest {             // Test the function for a signed value
public:
  Point3D m_point;              // Location of test
  double  m_value;              // Function value at p
  bool    m_ok;                 // true if value is of correct sign
};

class SurfVertex {              // Surface vertex
public:
  Point3D m_position, m_normal; // Position and surface normal
};

class HashKey {
public:
  int i, j, k;
  inline HashKey() {}
  inline HashKey(int _i, int _j, int _k) : i(_i), j(_j), k(_k) {
  }
  inline unsigned long hashCode() const {
    return ((i*2347) + j) * 2343 + k;
  }
  inline bool operator==(const HashKey &key) const {
    return (i==key.i) && (j==key.j) && (k==key.k);
  }
};

class EdgeKey {
private:
  void checkAndSwap();
public:
  HashKey m_key1;
  HashKey m_key2;
  inline EdgeKey() {}
  inline EdgeKey(int i1, int j1, int k1, int i2, int j2, int k2) : m_key1(i1,j1,k1), m_key2(i2,j2,k2) {
    checkAndSwap();
  }
  inline EdgeKey(const HashKey &key1, const HashKey &key2) : m_key1(key1), m_key2(key2) {
    checkAndSwap();
  }
  inline unsigned long hashCode() const {
    return m_key1.hashCode() + m_key2.hashCode();
  }
  inline bool operator==(const EdgeKey &k) const {
    return (m_key1 == k.m_key1) && (m_key2 == k.m_key2);
  }
};

class HashedCorner {            // Corner of a cube
public:
  HashKey m_key;
  Point3D m_point;              // Location
  double  m_value;              // Function value (=polygonizer.m_eval.evaluate(m_point))
  inline HashedCorner(const HashKey &k, const Point3D &p) : m_key(k) {
    m_point = p;
  }
};

class HashedCube {              // Partitioning cell (cube)
public:
  HashKey       m_key;          // Lattice location of cube
  HashedCorner *m_corners[8];   // Eight corners
  inline HashedCube(int i, int j, int k) : m_key(i,j,k) {
    memset(m_corners, 0, sizeof(m_corners));
  }
  int getIndex() const;
};

class Face3 {                   // Parameter to receiveVertex
public:
  int m_i1,m_i2,m_i3;           // Indices into vertexArray
  Face3() {}
  inline Face3(int i1, int i2, int i3) : m_i1(i1), m_i2(i2), m_i3(i3) {
  }
};

class SurfaceEvaluator {
public:
  virtual double evaluate(const Point3D &p) = 0;
  virtual void receiveVertex(const Face3 &face, const CompactArray<SurfVertex> &vertexArray) = 0;
};

class PolygonizerStatistics {
public:
  unsigned int m_cornerCount;
  unsigned int m_edgeCount;
  unsigned int m_vertexCount;
  unsigned int m_faceCount;
  unsigned int m_evalCount;
  unsigned int m_cornerHits;
  unsigned int m_edgeHits;
  double       m_threadTime;
  String       m_hashStat;
  PolygonizerStatistics();
  void clear();
  String toString() const;
};

class SurfacePolygonizer {
private:
  SurfaceEvaluator             &m_eval;            // Implicit surface function
  double                        m_size, m_delta;   // Cube size, normal delta
  int                           m_bounds;          // Cube range within lattice
  Point3D                       m_start;           // Start point on surface
  bool                          m_tetrahedralMode; // Use tetrahedral decomposition
  Stack<HashedCube>             m_cubeStack;       // Active cubes
  CompactArray<SurfVertex>      m_vertexArray;     // Surface vertices
  CompactHashSet<HashKey>       m_centerSet;       // Cube center hash table
  HashMap<HashKey,HashedCorner> m_cornerMap;       // Corners of cubes
  CompactHashMap<EdgeKey,int>   m_edgeMap;         // Edge -> vertex id Map
  Array<CompactArray<char> >    m_cubetable[256];
  PolygonizerStatistics         m_statistics;

  SurfaceTest   find(bool positive, const Point3D &start);
  Point3D       converge(const Point3D &p1, const Point3D &p2, double v);
  HashedCorner *getCorner(     int i, int j, int k);
  Point3D       getCornerPoint(int i, int j, int k) const;
  void          doTetra(const HashedCube &cube, int c1, int c2, int c3, int c4);
  void          doCube(const HashedCube &cube);
  int           getVertexId(const HashedCorner &c1, const HashedCorner &c2);
  bool          setCenter(int i , int j , int k);
  Point3D       vnormal(const Point3D &point);
  void          testFace (int i, int j, int k, const HashedCube &oldCube, int face, int c1, int c2, int c3, int c4);
  void          makeCubeTable();
  void          resetTables();
  double        evaluate(const Point3D &p);
public:
  SurfacePolygonizer(SurfaceEvaluator &eval,
                     double            size,
                     int               bounds,
                     bool              tetrahedralMode);
  ~SurfacePolygonizer();
  void polygonize(const Point3D &start);
  const PolygonizerStatistics &getStatistics() const {
    return m_statistics;
  }
};
