#include "stdafx.h"
#include "MeshBuilder.h"
#include "Profile.h"

ProfileRotationParameters::ProfileRotationParameters(int rotateAxis, int alignx, int aligny, double rad, int edgeCount, int smoothness) {
  m_rotateAxis = rotateAxis;
  m_alignx     = alignx;
  m_aligny     = aligny;
  m_rad        = rad;
  m_edgeCount  = edgeCount;
  m_smoothness = smoothness;
}

class Point2DTo3DConverter {
protected:
  int m_alignx, m_aligny;
public:
  Vertex convertPoint(const Point2D &p);
  VertexArray  convertPoints(const Point2DArray &p);
  Point2DTo3DConverter(int alignx, int aligny) { m_alignx = alignx; m_aligny = aligny; }
  int getAlignx() const { return m_alignx; }
  int getAligny() const { return m_aligny; }
};

Vertex Point2DTo3DConverter::convertPoint(const Point2D &p) {
  Vertex result = createVertex(0,0,0);
  switch(m_alignx) {
  case 0:
    result.x = (float)p.x;
    break;
  case 1:
    result.y = (float)p.x;
    break;
  case 2:
    result.z = (float)p.x;
    break;
  }
  switch(m_aligny) {
  case 0:
    result.x = (float)p.y;
    break;
  case 1:
    result.y = (float)p.y;
    break;
  case 2:
    result.z = (float)p.y;
    break;
  }
  return result;
}

VertexArray Point2DTo3DConverter::convertPoints(const Point2DArray &p) {
  VertexArray result;
  for(size_t i = 0; i < p.size(); i++) {
    result.add(convertPoint(p[i]));
  }
  return result;
}

class ProfileRotator : public CurveOperator {
private:
  const ProfileRotationParameters &m_param;
  Point2DTo3DConverter             m_converter;
  const D3DXMATRIX                &m_pointRotation;
  const D3DXMATRIX                &m_normalRotation;
  const VertexArray               &m_normals;
  MeshBuilder                     &m_meshBuilder;
  int                              m_normalIndex;
  int                              m_pointsInOneSlice;
  int                              m_normalsInOneSlice;
  int                              m_meshPointIndex;
  int                              m_meshNormalIndex;
public:
  ProfileRotator(const ProfileRotationParameters &param, int pointsInOneSlice, int normalsInOneSlice, const VertexArray &normals, const D3DXMATRIX &pointRotation, const D3DXMATRIX &normalRotation, MeshBuilder &meshBuilder);
  void apply(const Point2D &p);
  void line(const Point2D &from, const Point2D &to);
};

ProfileRotator::ProfileRotator(const ProfileRotationParameters &param, int pointsInOneSlice, int normalsInOneSlice, const VertexArray &normals, const D3DXMATRIX &pointRotation, const D3DXMATRIX &normalRotation, MeshBuilder &meshBuilder)
: m_param(param)
, m_converter(param.m_alignx, param.m_aligny)
, m_pointRotation(pointRotation)
, m_normalRotation(normalRotation)
, m_normals(normals)
, m_meshBuilder(meshBuilder) {

  m_pointsInOneSlice  = pointsInOneSlice;
  m_normalsInOneSlice = normalsInOneSlice;
  m_normalIndex       = 0;
}

void ProfileRotator::apply(const Point2D &p) {
  D3DXVECTOR3 point = m_pointRotation * m_converter.convertPoint(p);
  m_meshPointIndex = m_meshBuilder.addVertex(point.x,point.y,point.z);
  if(m_param.m_smoothness & NORMALSMOOTH) {
    D3DXVECTOR3 normal  = -m_normalRotation * m_normals[m_normalIndex++ % m_normals.size()];
    m_meshNormalIndex = m_meshBuilder.addNormal(normal.x,normal.y,normal.z);
  }
  CurveOperator::apply(p);
}

void ProfileRotator::line(const Point2D &from, const Point2D &to) {
  if(!(m_param.m_smoothness & NORMALSMOOTH)) {
    D3DXVECTOR3 normal = -m_normalRotation * m_normals[m_normalIndex++ % m_normals.size()];
    m_meshNormalIndex  = m_meshBuilder.addNormal(normal.x,normal.y,normal.z);
  }
  if(m_pointsInOneSlice > 0) {
    Face &face = m_meshBuilder.addFace();
    int pIndex[4],nIndex[4];
    pIndex[0] = m_meshPointIndex - m_pointsInOneSlice;
    pIndex[1] = m_meshPointIndex - m_pointsInOneSlice - 1;
    pIndex[2] = m_meshPointIndex - 1;
    pIndex[3] = m_meshPointIndex;

    if(m_param.m_smoothness & NORMALSMOOTH) {   // noOfNormals = noOfPoints
      if(m_param.m_smoothness & ROTATESMOOTH) { // NORMALSMOOTH && ROTATESMOOTH
        nIndex[0] = m_meshPointIndex - m_normalsInOneSlice;
        nIndex[1] = m_meshPointIndex - m_normalsInOneSlice - 1;
        nIndex[2] = m_meshPointIndex - 1;
        nIndex[3] = m_meshPointIndex;
      } else {                        // NORMALSMOOTH && !ROTATESMOOTH
        nIndex[0] = m_meshPointIndex - m_normalsInOneSlice;
        nIndex[1] = m_meshPointIndex - m_normalsInOneSlice - 1;
        nIndex[2] = m_meshPointIndex - m_normalsInOneSlice - 1;
        nIndex[3] = m_meshPointIndex - m_normalsInOneSlice;
      }
    } else {                          // noOfNormals == noOfLines
      if(m_param.m_smoothness & ROTATESMOOTH) { // !NORMALSMOOTH && ROTATESMOOTH
        nIndex[0] = m_meshNormalIndex - m_normalsInOneSlice;
        nIndex[1] = m_meshNormalIndex - m_normalsInOneSlice;
        nIndex[2] = m_meshNormalIndex;
        nIndex[3] = m_meshNormalIndex;
      } else {                       // !NORMALSMOOTH && !ROTATESMOOTH
        int normalIndex = m_meshNormalIndex - m_normalsInOneSlice;
        nIndex[0] = normalIndex;
        nIndex[1] = normalIndex;
        nIndex[2] = normalIndex;
        nIndex[3] = normalIndex;
      }
    }
    if(m_param.m_rotateAxis == m_param.m_alignx) {
      for(int i = 0; i < 4; i++) {
        face.addVertexAndNormalIndex(pIndex[i],nIndex[i]);
      }
    } else {
      for(int i = 3; i >= 0; i--) {
        face.addVertexAndNormalIndex(pIndex[i],nIndex[i]);
      }
    }
  }
}

LPD3DXMESH rotateProfile(LPDIRECT3DDEVICE9EX device, const Profile &profile, const ProfileRotationParameters &param, bool doubleSided) {
  if(param.m_alignx == param.m_aligny) {
    throwException(_T("alignx == aligny (=%d) in rotateProfile. Must be different "),param.m_alignx);
  }

  MeshBuilder meshBuilder;
  const double step = param.m_rad / param.m_edgeCount;

  D3DXVECTOR3 rotaxis = createUnitVector(param.m_rotateAxis);

  const double nrotOffset  = (param.m_smoothness & ROTATESMOOTH)?0.5:0;

  int pointsInOneSlice  = 0;
  int normalsInOneSlice = 0;

  for(int edgeIndex = 0; edgeIndex <= param.m_edgeCount; edgeIndex++) {
    const D3DXMATRIX vRotation = createRotationMatrix(rotaxis, step * edgeIndex);
    const D3DXMATRIX nRotation = createRotationMatrix(rotaxis, step * (edgeIndex + nrotOffset));
    for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
      const ProfilePolygon &pp = profile.m_polygonArray[i];
      Point2DArray normals2D   = (param.m_smoothness & NORMALSMOOTH) ? pp.getSmoothNormals() : pp.getFlatNormals();
      VertexArray  normals3D   = Point2DTo3DConverter(param.m_alignx,param.m_aligny).convertPoints(normals2D);
      pp.apply(ProfileRotator(param,pointsInOneSlice,normalsInOneSlice,normals3D,vRotation,nRotation, meshBuilder));
    }
    if(pointsInOneSlice == 0) {
      pointsInOneSlice  = meshBuilder.getVertexArray().size();
      normalsInOneSlice = meshBuilder.getNormalCount();
    }
  }
  return meshBuilder.createMesh(device, doubleSided);
}

/*
class FaceMaker : public CurveOperator {
private:
  double       m_width;
  MeshBuilder &m_meshBuilder;
public:
  FaceMaker(MeshBuilder &meshBuilder, double width);
  void line(const Point2D &from, const Point2D &to);
};

FaceMaker::FaceMaker(MeshBuilder &meshBuilder, double width) : m_meshBuilder(meshBuilder) {
  m_width       = width;
}

void FaceMaker::line(const Point2D &from, const Point2D &to) {
  Face &face = m_meshBuilder.addFace();
  face.addVertex(from.x, from.y, 0);
  face.addVertex(to.x  , to.y  , 0);
  face.addVertex(to.x  , to.y  , m_width);
  face.addVertex(from.x, from.y, m_width);
}

LPD3DXMESH createMesh(LPDIRECT3DDEVICE9EX device, const Profile &profile, double width) {
  MeshBuilder meshBuilder
  apply(FaceMaker(meshBuilder, width));
  return meshBuilder.createMesh(device);
}
*/
