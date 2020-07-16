#include "pch.h"
#include <TinyBitSet.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/Profile2D.h>

ProfileRotationParameters::ProfileRotationParameters(char rotateAxis, char rotateAxisAlignsTo, float rad, unsigned int edgeCount, BYTE flags, D3DCOLOR color) {
  m_rotateAxis         = rotateAxis;         // ['x','y','z'] - rotationaxis in 3D space
  m_rotateAxisAlignsTo = rotateAxisAlignsTo; // ['x','y']     - axis in 2D space aligned with rotationAxis
  m_rad                = rad;
  m_edgeCount          = edgeCount;
  m_flags              = flags;
  m_color              = color;
  checkIsValid();
}

void ProfileRotationParameters::checkIsValid() const { // throws Exception if not valid
  if(strchr("xyz",m_rotateAxis) == NULL) {
    throwException(_T("RotateAxis must be 'x','y' or 'z'"));
  }
  if(strchr("xy",m_rotateAxisAlignsTo) == NULL) {
    throwException(_T("RotateAxisAlignsTo must be 'x' or 'y'"));
  }
  if(m_edgeCount < 0) {
    throwException(_T("EdgeCount must >= 2. (=%u"), m_edgeCount);
  }
  if(m_rad == 0) {
    throwException(_T("Rad == 0"));
  }
}

class Vertex2DTo3DConverter {
protected:
  char m_rotateAxis;
  char m_rotateAxisAlignsTo;
  char m_xTo3Dcoord, m_yTo3Dcoord;
public:
  Vertex                     convertPoint2D(      const Point2D       &p) const;
  CompactArray<Vertex>       convertPoint2DArray( const Point2DArray  &a) const;
  inline VertexNormal        convertVertex2D(     const Vertex2D      &v) const {
    return VertexNormal().setPos(convertPoint2D(v.m_pos)).setNormal(convertPoint2D(v.m_normal));
  }
  CompactArray<VertexNormal> convertVertex2DArray(const Vertex2DArray &a) const;
  Vertex2DTo3DConverter(char rotateAxis, char rotationAlignsTo);
};

Vertex2DTo3DConverter::Vertex2DTo3DConverter(char rotateAxis, char rotateAxisAlignsTo) {
  m_rotateAxis       = rotateAxis;
  m_rotateAxisAlignsTo = rotateAxisAlignsTo;
  switch(m_rotateAxis) {
  case 'x':
    switch(m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'y'; break;
    case 'y': m_xTo3Dcoord = 'z'; m_yTo3Dcoord = 'x'; break;
    }
    break;
  case 'y':
    switch(m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'y'; m_yTo3Dcoord = 'z'; break;
    case 'y': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'y'; break;
    }
    break;
  case 'z':
    switch(m_rotateAxisAlignsTo) {
    case 'x': m_xTo3Dcoord = 'z'; m_yTo3Dcoord = 'y'; break;
    case 'y': m_xTo3Dcoord = 'x'; m_yTo3Dcoord = 'z'; break;
    }
    break;
  }
}

Vertex Vertex2DTo3DConverter::convertPoint2D(const Point2D &p) const {
  Vertex result(0,0,0);
  switch(m_xTo3Dcoord) {
  case 'x':
    result.x = (float)p.x;
    break;
  case 'y':
    result.y = (float)p.x;
    break;
  case 'z':
    result.z = (float)p.x;
    break;
  }
  switch(m_yTo3Dcoord) {
  case 'x':
    result.x = (float)p.y;
    break;
  case 'y':
    result.y = (float)p.y;
    break;
  case 'z':
    result.z = (float)p.y;
    break;
  }
  return result;
}

CompactArray<Vertex> Vertex2DTo3DConverter::convertPoint2DArray(const Point2DArray &a) const {
  const size_t n = a.size();
  CompactArray<Vertex> result(n);
  for(size_t i = 0; i < n; i++) result.add(convertPoint2D(a[i]));
  return result;
}

CompactArray<VertexNormal> Vertex2DTo3DConverter::convertVertex2DArray(const Vertex2DArray &a) const {
  const size_t n = a.size();
  CompactArray<VertexNormal> result(n);
  for(size_t i = 0; i < n; i++) result.add(convertVertex2D(a[i]));
  return result;
}

class VertexNormalWithIndex : public VertexNormal {
public:
  int m_pindex;
  int m_nindex;
  VertexNormalWithIndex() : m_pindex(-1), m_nindex(-1) {
  }
  VertexNormalWithIndex(const VertexNormal &vn) : VertexNormal(vn), m_pindex(-1), m_nindex(-1) {
  }
  inline bool haspindex() const {
    return m_pindex >= 0;
  }
  inline bool hasnindex() const {
    return m_nindex >= 0;
  }
  inline UINT getpindex() const {
    return m_pindex;
  }
  inline UINT getnindex() const {
    return m_nindex;
  }
};

class VertexNormalIndexArray : public CompactArray<VertexNormalWithIndex> {
public:
  explicit VertexNormalIndexArray(size_t capacity = 0) : CompactArray(capacity) {
  }
  VertexNormalIndexArray(const CompactArray<VertexNormal> &src) {
    setCapacity(src.size());
    for(const VertexNormal v : src) {
      add(v);
    }
  }
};

class VertexNormalIndexPArray : public CompactArray<VertexNormalWithIndex*> {
public:
  explicit VertexNormalIndexPArray(size_t capacity = 0) : CompactArray(capacity) {
  }
  VertexNormalIndexPArray &add(VertexNormalWithIndex &v) {
    __super::add(&v);
    return *this;
  }
  D3DXVECTOR3 sumAllNormals() const;
};

D3DXVECTOR3 VertexNormalIndexPArray::sumAllNormals() const {
  D3DXVECTOR3 s(0, 0, 0);
  for(VertexNormalWithIndex *vp : *this) {
    s += vp->getNormal();
  }
  return s;
}

CompactArray<VertexNormal> rotate(const CompactArray<VertexNormal> &a, D3DXQUATERNION &rot) {
  CompactArray<VertexNormal> result(a.size());
  for(const VertexNormal v : a) {
    result.add(VertexNormal().setPos(rotate(v.getPos(), rot)).setNormal(rotate(v.getNormal(),rot)));
  }
  return result;
}

#ifdef __NEVER__
class ProfileRotator : public CurveOperator {
private:
  const ProfileRotationParameters &m_param;
  Vertex2DTo3DConverter            m_converter;
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
, m_converter(param.m_rotateAxis, param.m_rotateAxisAlignsTo)
, m_pointRotation(pointRotation)
, m_normalRotation(normalRotation)
, m_normals(normals)
, m_meshBuilder(meshBuilder) {

  m_pointsInOneSlice  = pointsInOneSlice;
  m_normalsInOneSlice = normalsInOneSlice;
  m_normalIndex       = 0;
}

void ProfileRotator::apply(const Point2D &p) {
  D3DXVECTOR3 point = m_pointRotation * m_converter.convertPoint2D(p);
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
    Face &face = m_param.m_useColor
               ? m_meshBuilder.addFace(m_param.m_color)
               : m_meshBuilder.addFace();
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
        face.addVertexNormalIndex(pIndex[i],nIndex[i]);
      }
    } else {
      for(int i = 3; i >= 0; i--) {
        face.addVertexNormalIndex(pIndex[i],nIndex[i]);
      }
    }
  }
}

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile &profile, const ProfileRotationParameters &param, bool doubleSided) {
  if(param.m_alignx == param.m_aligny) {
    throwException(_T("alignx == aligny (=%d) in rotateProfile. Must be different "),param.m_alignx);
  }

  MeshBuilder meshBuilder;
  const double      step       = param.m_rad / param.m_edgeCount;
  const D3DXVECTOR3 rotaxis    = createUnitVector(param.m_rotateAxis);
  const double      nrotOffset = (param.m_smoothness & ROTATESMOOTH)?0.5:0;

  int pointsInOneSlice  = 0;
  int normalsInOneSlice = 0;

  for(int edgeIndex = 0; edgeIndex <= param.m_edgeCount; edgeIndex++) {
    const D3DXMATRIX vRotation = createRotationMatrix(rotaxis, step * edgeIndex);
    const D3DXMATRIX nRotation = createRotationMatrix(rotaxis, step * (edgeIndex + nrotOffset));
    for(size_t i = 0; i < profile.m_polygonArray.size(); i++) {
      const ProfilePolygon &pp = profile.m_polygonArray[i];
      Point2DArray normals2D   = (param.m_smoothness & NORMALSMOOTH) ? pp.getSmoothNormals() : pp.getFlatNormals();
      VertexArray  normals3D   = Point2DTo3DConverter(param.m_alignx,param.m_aligny).convertPoints(normals2D);
      pp.apply(ProfileRotator(param, pointsInOneSlice, normalsInOneSlice, normals3D, vRotation, nRotation, meshBuilder));
    }
    if(pointsInOneSlice == 0) {
      pointsInOneSlice  = (int)meshBuilder.getVertexArray().size();
      normalsInOneSlice = meshBuilder.getNormalCount();
    }
  }
  return meshBuilder.createMesh(amf, doubleSided);
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
#endif






D3Cube getBoundingBox(const Array<VertexNormalIndexArray> &a) {
  const size_t n = a.size();
  if(n == 0) return D3Cube();
  D3Cube result = getBoundingBox(a[0]);
  for(size_t i = 1; i < n; i++) {
    result += getBoundingBox(a[i]);
  }
  return result;
}

static bool isOnRotationAxis(char rotateAxis, const D3DXVECTOR3 &v) {
  switch(rotateAxis) {
  case 'x': return (v.y == 0) && (v.z == 0);
  case 'y': return (v.x == 0) && (v.z == 0);
  case 'z': return (v.x == 0) && (v.y == 0);
  }
  return false;
}

static BitSet getVerticesOnRotationAxis(char rotateAxis, const CompactArray<VertexNormal> &va) {
  BitSet result(va.size());
  for(size_t i = 0; i < va.size(); i++) {
    if(isOnRotationAxis(rotateAxis, va[i].getPos())) {
      result.add(i);
    }
  }
  return result;
}

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile2D &profile, const ProfileRotationParameters &param, bool doubleSided) {
  param.checkIsValid();

  Vertex2DArray               va2D = profile.getAllVertices(param.isSet(PRROT_NORMALSMOOTH));
  if(param.isSet(PRROT_INVERTNORMALS)) {
    va2D.invertNormals();
  }
  CompactArray<VertexNormal>        va3D            = Vertex2DTo3DConverter(param.m_rotateAxis, param.m_rotateAxisAlignsTo).convertVertex2DArray(va2D);
  BitSet                            centerVertexSet = getVerticesOnRotationAxis(param.m_rotateAxis, va3D);
  const UINT                        pointCount      = (UINT)va3D.size();
  const UINT                        edgeCount       = param.m_edgeCount;
  const float                       step            = (float)param.m_rad / edgeCount;
  const D3DXVECTOR3                 rotaxis         = createUnitVector(param.getRotateAxisIndex());
  D3DXQUATERNION                    rotStep         = createRotation(rotaxis,  step);
  D3DXQUATERNION                    rothalfP        = createRotation(rotaxis,  step/2);
  D3DXQUATERNION                    rothalfN        = createRotation(rotaxis, -step/2);
  D3DXQUATERNION                    currentRot      = rotStep;
  Array<VertexNormalIndexArray>     edgeArray(edgeCount);

  edgeArray.add(va3D);
  for(UINT i = 1; i < edgeCount; i++) {
    edgeArray.add(rotate(va3D, currentRot));
    currentRot *= rotStep;
  }

  D3Cube boundingBox = getBoundingBox(edgeArray);

  MeshBuilder mb;
  for(UINT e = 1; e <= edgeCount; e++) {
    VertexNormalIndexArray &a1 = edgeArray[e - 1];
    VertexNormalIndexArray &a2 = edgeArray[e % edgeCount];
    for(UINT j1 = 0, j2 = 1; j2 < pointCount; j1++, j2++) {
      const bool isVj1Center = centerVertexSet.contains(j1), isVj2Center = centerVertexSet.contains(j2);
      if(isVj1Center && isVj2Center) {
        continue;
      }
      VertexNormalWithIndex &v11 = a1[j1];  // lower left  v11=ll - asumming rotating around z-axis and starting from bottom
      VertexNormalWithIndex &v12 = a1[j2];  // upper left  v12=ul
      VertexNormalWithIndex &v21 = a2[j1];  // lower right v21=lr
      VertexNormalWithIndex &v22 = a2[j2];  // upper right v22=ur
      VertexNormalIndexPArray tmp(4);
      Vertex                  avgAllNormals   = unitVector(v11.getNormal() + v12.getNormal() + v21.getNormal() + v22.getNormal());
      Vertex                  avgLowNormals   = unitVector(v11.getNormal() + v21.getNormal());
      Vertex                  avgUpNormals    = unitVector(v12.getNormal() + v22.getNormal());
      Vertex                  avgLeftNormals  = unitVector(v11.getNormal() + v12.getNormal());
      Vertex                  avgRightNormals = unitVector(v21.getNormal() + v22.getNormal());
      if(isVj1Center) {
        tmp.add(v11).add(v12).add(v22); // lower left -> upper left -> upper right. v11 == v21 (va3D[j1] fixed when rotate
      } else if(isVj2Center) {
        tmp.add(v22).add(v21).add(v11); // upper right -> lower right -> lower left. 
      } else {
        tmp.add(v11).add(v12).add(v22).add(v21); // else  make a rectangle
      }

      int pindex[4] = { 0,0,0,0 }, nindex[4] = { 0,0,0,0 };
      for(UINT i = 0; i < tmp.size(); i++) {
        VertexNormalWithIndex *vtx = tmp[i];
        if(!vtx->haspindex()) {
          vtx->m_pindex = mb.addVertex(vtx->getPos());
        }
        pindex[i] = vtx->getpindex();
      }

      switch(param.m_flags & (PRROT_NORMALSMOOTH | PRROT_ROTATESMOOTH)) {
      case 0:
        nindex[0] = nindex[1] = nindex[2] = nindex[3] = mb.addNormal(avgAllNormals);
        break;
      case PRROT_ROTATESMOOTH:
        { if(tmp.size() == 4) {
            nindex[0] = nindex[1] = mb.addNormal(rotate(avgAllNormals,rothalfN));
            nindex[2] = nindex[3] = mb.addNormal(rotate(avgAllNormals,rothalfP));
          } else {
            nindex[0]             = mb.addNormal(avgAllNormals);
            nindex[1]             = mb.addNormal(rotate(avgAllNormals,rothalfN));
            nindex[2]             = mb.addNormal(rotate(avgAllNormals,rothalfP));
          }
        }
        break;
      case PRROT_NORMALSMOOTH:
        if(tmp.size() == 4) {
          nindex[0] = nindex[3]   = mb.addNormal(avgLowNormals);
          nindex[1] = nindex[2]   = mb.addNormal(avgUpNormals );
        } else {
          nindex[0]               = mb.addNormal(avgLowNormals);
          nindex[1] = nindex[2]   = mb.addNormal(avgUpNormals );
        }
        break;
      case PRROT_NORMALSMOOTH | PRROT_ROTATESMOOTH: // 4 normals
        if(tmp.size() == 4) {
          nindex[0] = mb.addNormal(rotate(avgLowNormals,rothalfN));
          nindex[1] = mb.addNormal(rotate(avgUpNormals ,rothalfN));
          nindex[2] = mb.addNormal(rotate(avgUpNormals ,rothalfP));
          nindex[3] = mb.addNormal(rotate(avgLowNormals,rothalfP));
        } else {
          nindex[0] = mb.addNormal(avgLowNormals);
          nindex[1] = mb.addNormal(rotate(avgUpNormals, rothalfN));
          nindex[2] = mb.addNormal(rotate(avgUpNormals, rothalfP));
        }
        break;
      }
      Face &face = param.isSet(PRROT_USECOLOR)
                 ? mb.addFace(param.m_color,tmp.size())
                 : mb.addFace(tmp.size());

      for(UINT i = 0; i < tmp.size(); i++) {
        face.addVertexNormalIndex(pindex[i], nindex[i]);
      }
      if(!mb.hasCorrectOrientation(face)) {
        face.invertOrientation();
      }
    }
  }
  return mb.optimize().createMesh(amf, doubleSided);
}
