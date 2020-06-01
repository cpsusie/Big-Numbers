#include "pch.h"
#include <TinyBitSet.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/Profile.h>

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
    const size_t n = src.size();
    setCapacity(n);
    for(const VertexNormal *srcp = src.getBuffer(), *endp = srcp + n; srcp < endp;) {
      add(*(srcp++));
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
  D3DXVECTOR3 s = (*this)[0]->getNormal();
  for(UINT i = 1; i < size(); i++) {
    s += (*this)[i]->getNormal();
  }
  return s;
}

CompactArray<VertexNormal> rotate(const CompactArray<VertexNormal> &a, D3DXQUATERNION &rot) {
  const size_t n = a.size();
  CompactArray<VertexNormal> result(n);
  for(size_t i = 0; i < n; i++) {
    const VertexNormal &v = a[i];
    result.add(VertexNormal().setPos(rotate(v.getPos(), rot)).setNormal(rotate(v.getNormal(),rot)));
  }
  return result;
}

D3Cube getBoundingBox(const Array<VertexNormalIndexArray> &a) {
  const size_t n = a.size();
  if(n == 0) return D3Cube();
  D3Cube result = getBoundingBox(a[0]);
  for(size_t i = 1; i < n; i++) {
    result = result + getBoundingBox(a[i]);
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

// Return true, if normals point in same direction as the visible side of the face
static bool hasCorrectOrientation(const MeshBuilder &mb, const Face &f) {
  const VNTIArray   &fa = f.getIndices();
  const size_t       n  = fa.size();
  const VertexArray &va = mb.getVertexArray();
  const VertexArray &na = mb.getNormalArray();
  const Vertex      &p1 = va[fa[0].m_vIndex];
  const Vertex      &p2 = va[fa[1].m_vIndex];
  const Vertex      &p3 = va[fa[2].m_vIndex];
  const Vertex      &n1 = na[fa[0].m_nIndex];
  const Vertex      &n2 = na[fa[1].m_nIndex];
  const Vertex      &n3 = na[fa[2].m_nIndex];
  const D3DXVECTOR3 c = cross((D3DXVECTOR3&)p3 - (D3DXVECTOR3&)p1, (D3DXVECTOR3&)p2 - (D3DXVECTOR3&)p1);
  const float s1 = c * n1, s2 = c * n2, s3 = c * n3;
  return s1 > 0;
}

LPD3DXMESH rotateProfile(AbstractMeshFactory &amf, const Profile &profile, const ProfileRotationParameters &param, bool doubleSided) {
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
      if(!hasCorrectOrientation(mb, face)) {
        face.invertOrientation();
      }
    }
  }
  return mb.optimize().createMesh(amf, doubleSided);
}
