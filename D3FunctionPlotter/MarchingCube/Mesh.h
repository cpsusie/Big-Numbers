#pragma once

#include "Plane.h"
#include <map>
#include <list>
#include "..\MeshBuilder.h"

namespace ThreeD {

class MeshFace;

/* Mesh, a three-dimesional object composed of individual mesh faces. */
class Mesh {
public:
  typedef std::list<MeshFace *> FacesList;

  // A MeshPoint may also be addressed as D3DXVECTOR3[2],
  // so keep the order of the first two fields

  static String toString(const FacesList &faceList);
  struct MeshPoint {
    D3DXVECTOR3    point;
    D3DXVECTOR3    normal;
    FacesList faces;
    String toString() const;
  };
  struct MeshPlane {
    Plane     p;
    FacesList faces;
    String toString() const;
  };
  typedef std::multimap<unsigned long, MeshPoint> PointsMap;
  typedef std::list<MeshPlane> PlanesList;

  static String toString(const PointsMap  &points);
  static String toString(const PlanesList &planes);

protected:
  PointsMap  m_points;
  PlanesList m_planes;

  D3DXVECTOR3     m_bounds[2];              // top-left and bottom-right
  bool       m_boundsCached;           // if _bounds is ok to use
public:

  Mesh();
  virtual ~Mesh();

  /* Defines a new point in this mesh.  This creates a new
   * point element unless such a point already exists in
   * the mesh.
   *
   * @return the index for the point.
   */
  MeshPoint *addPoint(const D3DXVECTOR3 &v);

  /* Adds a plane to the mesh. */
  MeshPlane *addPlane(const Plane &plane1);

  /* Associates a mesh face with this mesh.
   * IMPORTANT:  The vertices that make up the mesh face
   * should be those returned by Mesh::addPoint().
   */
  void addFace(MeshFace *face, MeshPlane *mplane = 0);

  /* Apply the transform @p trans into this object. */
  virtual void transform(const D3DXMATRIX &trans);
  
  /* Returns the bounding box for this object.  The origin of the box is
   * at the origin of the object, (0,0,0).  The negative bounding vertex
   * is place in @p v1, and the positive bounding vertex in @p v2.
   */
  virtual void getBoundingBox(D3DXVECTOR3 *v1, D3DXVECTOR3 *v2);
  
  /* Returns the radius of the bounding sphere for this object.  The
   * origin of the sphere is at the origin of the object, (0,0,0).
   *
   * @return the radius of the bounding sphere.
   */
  virtual float getBoundingRadius();
  
  /* Centralizes all vertices around (0,0,0), by computing the offset
   * of the center of the object to (0,0,0) and shifting all the
   * vertices by that offset.
   *
   * @return the offset from the original center to (0,0,0).
   */
  virtual D3DXVECTOR3 centralize();

  /* Compute vertex normals. */
  void computeVertexNormals();

  MeshBuilder getMeshBuilder() const;

#ifdef DEBUG
  void dump(FILE *f = stdout);
  void dump(const String &fileName);
#endif

};

} // namespace ThreeD
