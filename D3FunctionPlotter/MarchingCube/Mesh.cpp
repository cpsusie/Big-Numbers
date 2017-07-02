#pragma warning(disable : 4786)

#include "stdafx.h"
#include <CompactHashMap.h>

using namespace ThreeD;

#define TOLERANCE 1e-3

Mesh::Mesh() {
  m_boundsCached = false;
}

Mesh::~Mesh() {
  // delete faces throughout all planes

  PlanesList::iterator itPlanes = m_planes.begin();
  while(itPlanes != m_planes.end()) {
    FacesList &faces = (*itPlanes).faces;
    while(!faces.empty()) {
      MeshFace *face = faces.front();
      delete face;
      faces.pop_front();
    }
    itPlanes = m_planes.erase(itPlanes);
  }

  // delete points

  m_points.clear();
}

Mesh::MeshPoint *Mesh::addPoint(const D3DXVECTOR3 &v) {
  // points are held in a sorted map, where the key of
  // a point is its vector distance from (0,0,0) times 10.
  float x = v.x;
  float y = v.y;
  float z = v.z;
  unsigned long key = (unsigned long)(10.0f * ((double)x * (double)x + (double)y * (double)y + (double)z * (double)z));

  // look at points in the vicinity of the point we're adding
  // and see if any of them is close enough to the new point
  PointsMap::iterator it0 = m_points.lower_bound(key - 1);
  if(it0 == m_points.end()) {
    it0 = m_points.lower_bound(key);
  }
  PointsMap::iterator it1 = m_points.upper_bound(key + 1);

  PointsMap::iterator it = it0;
  while(it != it1) {
    const MeshPoint &mpoint = (*it).second;
    const D3DXVECTOR3 *v2 = &mpoint.point;
    if(fabs(x - v2->x) < TOLERANCE
    && fabs(y - v2->y) < TOLERANCE
    && fabs(z - v2->z) < TOLERANCE) {
      return (MeshPoint *)&mpoint; // v2
    }
    ++it;
  }

  // if match not found, create a new point
  MeshPoint mpoint;
  mpoint.point = v;
  PointsMap::value_type pair(key, mpoint);
  if(it0 != m_points.end()) {
    it = m_points.insert(it0, pair);
  } else {
    it = m_points.insert(pair);
  }
  const MeshPoint &newmpoint = (*it).second;
  //return &newmpoint.point;
  return (MeshPoint *)&newmpoint;
}

Mesh::MeshPlane *Mesh::addPlane(const Plane &plane1) {
  Plane planeToAdd = plane1.normalized();

  PlanesList::iterator itPlanes = m_planes.begin();
  if(itPlanes != m_planes.end()) {
    const Plane *plane2 = &(*itPlanes).p;
    if(fabs(planeToAdd.a() - plane2->a()) < TOLERANCE
    && fabs(planeToAdd.b() - plane2->b()) < TOLERANCE
    && fabs(planeToAdd.c() - plane2->c()) < TOLERANCE
    && fabs(planeToAdd.d() - plane2->d()) < TOLERANCE) {
      return &(*itPlanes);
    }
    for(;;) {
      ++itPlanes;
      if(itPlanes == m_planes.end()) {
        break;
      }
      plane2 = &(*itPlanes).p;
      if(fabs(planeToAdd.a() - plane2->a()) < TOLERANCE
      && fabs(planeToAdd.b() - plane2->b()) < TOLERANCE
      && fabs(planeToAdd.c() - plane2->c()) < TOLERANCE
      && fabs(planeToAdd.d() - plane2->d()) < TOLERANCE) {
        return &(*itPlanes);
      }
    }
  }

  MeshPlane mpNew;
  mpNew.p = planeToAdd;
  m_planes.push_back(mpNew);
  return &(m_planes.back());
}

void Mesh::addFace(MeshFace *face, MeshPlane *mplane) {
/*
  // compute the area of the face, and drop it if the area is too small
  double face_a = length(*face->m_v[1] - *face->m_v[0]);
  double face_b = length(*face->m_v[2] - *face->m_v[1]);
  double face_c = length(*face->m_v[0] - *face->m_v[2]);
  double face_s = (face_a + face_b + face_c) / 2.0f;
  double face_area2 = face_s * (face_s - face_a) * (face_s - face_b) * (face_s - face_c);
  if(face_area2 < 1e-7) {
    return;
  }
*/
  // find the plane for the face
  if(!mplane) {
    Plane p(face->vertex(0), face->vertex(1), face->vertex(2));
    mplane = addPlane(p);
  }

  // make sure the same face hasn't been added before
  const D3DXVECTOR3 *newVertex0 = face->vertexPtr(0);
  const D3DXVECTOR3 *newVertex1 = face->vertexPtr(1);
  const D3DXVECTOR3 *newVertex2 = face->vertexPtr(2);

#if 0
  const FacesList &mplane_faces = mplane->faces;
  FacesList::const_iterator it = mplane_faces.begin();
  while(it != mplane_faces.end()) {
    MeshFace *oldface = (*it);
    const D3DXVECTOR3 *oldVertex0 = oldface->vertexPtr(0);
    const D3DXVECTOR3 *oldVertex1 = oldface->vertexPtr(1);
    const D3DXVECTOR3 *oldVertex2 = oldface->vertexPtr(2);

    bool same0 = (oldVertex0 == newVertex0) || (oldVertex0 == newVertex1) || (oldVertex0 == newVertex2);
    bool same1 = (oldVertex1 == newVertex0) || (oldVertex1 == newVertex1) || (oldVertex1 == newVertex2);
    bool same2 = (oldVertex2 == newVertex0) || (oldVertex2 == newVertex1) || (oldVertex2 == newVertex2);

    if(same0 && same1 && same2) {
      return;
    }
    ++it;
  }
#endif

  // the face hasn't been added before so add it now
  mplane->faces.push_back(face);

  // add a reference to the face from each of its vertices
  MeshPoint *mpoint0 = (MeshPoint *)newVertex0;
  MeshPoint *mpoint1 = (MeshPoint *)newVertex1;
  MeshPoint *mpoint2 = (MeshPoint *)newVertex2;
  mpoint0->faces.push_back(face);
  mpoint1->faces.push_back(face);
  mpoint2->faces.push_back(face);

  m_boundsCached = false;
}

void Mesh::transform(const D3DXMATRIX &trans) {
  PointsMap::iterator it = m_points.begin();
  while(it != m_points.end()) {
    MeshPoint &mpoint = (*it).second;
    D3DXVECTOR3 *v = &mpoint.point;
    *v = trans * *v;
    ++it;
  }
  m_boundsCached = false;
}

void Mesh::getBoundingBox(D3DXVECTOR3 *v1, D3DXVECTOR3 *v2) {
  if(m_boundsCached) {
    *v1 = m_bounds[0];
    *v2 = m_bounds[1];
    return;
  }

  float vmin_x = 99999999.0f;
  float vmin_y = 99999999.0f;
  float vmin_z = 99999999.0f;
  float vmax_x = -99999999.0f;
  float vmax_y = -99999999.0f;
  float vmax_z = -99999999.0f;

  PlanesList::const_iterator itPlanes = m_planes.begin();
  while(itPlanes != m_planes.end()) {
    const FacesList &faces = (*itPlanes).faces;
    FacesList::const_iterator itFaces = faces.begin();
    while(itFaces != faces.end()) {
      D3DXVECTOR3 vmin, vmax;
      (*itFaces)->getMinMax(&vmin, &vmax);
      if(vmin.x < vmin_x) vmin_x = vmin.x;
      if(vmin.y < vmin_y) vmin_y = vmin.y;
      if(vmin.z < vmin_z) vmin_z = vmin.z;
      if(vmax.x > vmax_x) vmax_x = vmax.x;
      if(vmax.y > vmax_y) vmax_y = vmax.y;
      if(vmax.z > vmax_z) vmax_z = vmax.z;
      ++itFaces;
    }
    ++itPlanes;
  }

  m_bounds[0] = D3DXVECTOR3(vmin_x, vmin_y, vmin_z);
  m_bounds[1] = D3DXVECTOR3(vmax_x, vmax_y, vmax_z);
  m_boundsCached = true;

  *v1 = m_bounds[0];
  *v2 = m_bounds[1];
}

float Mesh::getBoundingRadius() {
  D3DXVECTOR3 vmin, vmax;
  getBoundingBox(&vmin, &vmax);
  return length(vmin);
}

D3DXVECTOR3 Mesh::centralize() {
  D3DXVECTOR3 vmin, vmax;
  getBoundingBox(&vmin, &vmax);
  D3DXVECTOR3 vcenter = (vmin + vmax) / 2.0f;
  D3DXMATRIX t = createTranslateMatrix(-vcenter);
  transform(t);
  return vcenter;
}

void Mesh::computeVertexNormals() {
  for(PointsMap::iterator itPoints = m_points.begin(); itPoints != m_points.end(); ++itPoints) {
    MeshPoint &mpoint = (*itPoints).second;

    D3DXVECTOR3 sum;
    int count = 0;

    FacesList &faces = mpoint.faces;
    if(!faces.empty()) {
      FacesList::iterator itFaces = faces.begin();
      MeshFace *face1 = (*itFaces);
      sum += face1->planeNormal();

      ++itFaces;
      while(itFaces != faces.end()) {
        const MeshFace *face2 = (*itFaces);
        const float dotprod = face1->planeNormal() * face2->planeNormal();
        if(dotprod > -0.1 && dotprod < 0.1) {
          count = 0;
          break;
        } else {
          sum += face2->planeNormal();
          ++count;
          ++itFaces;
        }
      }
    }

    //count = 0;

    if(count > 0) {
      sum /= (float)count;
      mpoint.normal = unitVector(sum);
    } else {
      mpoint.normal = D3DXVECTOR3(0,0,0);
    }
  }
}

#ifdef DEBUG

String Mesh::toString(const FacesList &faceList) { // static
  String result = "FaceList:\n";
  for(FacesList::const_iterator it = faceList.begin(); it != faceList.end(); ++it) {
    const MeshFace *face = (*it);
    result += "  ";
    result += face->toString();
    result += "\n";
  }
  return result;
}


String Mesh::MeshPoint::toString() const {
  return format("MeshPoint:P:%s, N:%s\n%s", ::toString(point).cstr(), ::toString(normal).cstr(), Mesh::toString(faces).cstr());
}

String Mesh::MeshPlane::toString() const {
  return format("MeshPlane:%s\n%s", p.toString().cstr(), Mesh::toString(faces).cstr());
}

String Mesh::toString(const Mesh::PointsMap &points) { // static
  String result = "Points:\n";
  for(PointsMap::const_iterator it = points.begin(); it != points.end(); ++it) {
    const MeshPoint &point = (*it).second;
    result += point.toString();
    result += "\n";
  }
  return result;
}

String Mesh::toString(const Mesh::PlanesList &planes) { // static
  String result = "PlaneList:\n";
  for(PlanesList::const_iterator it = planes.begin(); it != planes.end(); ++it) {
    result += (*it).toString();
  }
  return result;
}

void Mesh::dump(FILE *f) {
  fprintf(f, "Generated mesh:\n");
  fprintf(f, toString(m_points).cstr());
  fprintf(f, toString(m_planes).cstr());
}

void Mesh::dump(const String &fileName) {
  FILE *f = MKFOPEN(fileName, "w");
  try {
    dump(f);
    fclose(f);
  } catch(...) {
    fclose(f);
    throw;
  }
}

#endif

class VertexKey {
public:
  const D3DXVECTOR3 *m_vertex;

  inline VertexKey() : m_vertex(0) {
  }
  inline VertexKey(const D3DXVECTOR3 *vertex) : m_vertex(vertex) {
  }
  inline unsigned long hashCode() const {
    return (unsigned long)m_vertex;
  }
  bool operator==(const VertexKey &k) const {
    return m_vertex == k.m_vertex;
  }
};

MeshBuilder Mesh::getMeshBuilder() const {
  CompactHashMap<VertexKey, int> vertexMap;
  int vertexCount = 0;

  MeshBuilder mb;
  for(PlanesList::const_iterator itPlanes = m_planes.begin(); itPlanes != m_planes.end(); ++itPlanes) {
    const FacesList &faces = (*itPlanes).faces;
    for(FacesList::const_iterator itFaces = faces.begin(); itFaces != faces.end(); ++itFaces) {
      Face &mbFace = mb.addFace();
      const MeshFace &face = *(*itFaces);
      for(int i = 0; i < 3; i++) {
        const D3DXVECTOR3 *vertexp = face.m_v[i];
        int *pIndex = vertexMap.get(vertexp);
        int vertexIndex = pIndex ? *pIndex : vertexCount++;
        if(pIndex == NULL) {
          vertexMap.put(vertexp, vertexIndex);
          const D3DXVECTOR3 &n       = face.m_v[i][1];
          D3DXVECTOR3 normal         = isNullVector(n) ? face.m_planeNormal : n;
//          normal.z              = -normal.z;
          mb.addVertex(*vertexp);
          mb.addNormal(normal);
        }
        mbFace.addVertexNormalIndex(vertexIndex, vertexIndex);
      }
    }
  }
  return mb;
}
