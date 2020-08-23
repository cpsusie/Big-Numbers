#include "pch.h"
#include <TinyBitSet.h>
#include <D3DGraphics/D3Math.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/D3AbstractMeshFactory.h>
#include <D3DGraphics/Profile3D.h>

ProfileRotationParameters::ProfileRotationParameters() {
  m_rad          = D3DX_PI*2.0f;
  m_edgeCount    = 20;
  m_flags        = 0;
  m_color        = 0;
  checkIsValid();
}

ProfileRotationParameters::ProfileRotationParameters(const Point2DTo3DConverter &converter, float rad, UINT edgeCount, BYTE flags, D3DCOLOR color) {
  m_converter    = converter;
  m_rad          = rad;
  m_edgeCount    = edgeCount;
  m_flags        = flags;
  m_color        = color;
  checkIsValid();
}

const IntInterval ProfileRotationParameters::s_legalEdgeCountInterval(3,100);
void ProfileRotationParameters::validateEdgeCount(UINT edgeCount) { // static
  if(!s_legalEdgeCountInterval.contains(edgeCount)) {
    throwException(_T("EdgeCount must be in interval %s. (=%u")
                  , s_legalEdgeCountInterval.toString().cstr()
                  , edgeCount);
  }
}

void ProfileRotationParameters::checkIsValid() const { // throws Exception if not valid
  validateEdgeCount(m_edgeCount);
  if(m_rad == 0) {
    throwException(_T("Rad == 0"));
  }
}

bool operator==(const ProfileRotationParameters &p1, const ProfileRotationParameters &p2) {
  return p1.m_converter == p2.m_converter
      && p1.m_edgeCount == p2.m_edgeCount
      && p1.m_rad       == p2.m_rad
      && p1.m_flags     == p2.m_flags
      && p1.m_color     == p2.m_color;
}


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

D3Cube getBoundingBox(const Array<VertexProfile3D> &a) {
  const size_t n = a.size();
  if(n == 0) return D3Cube();
  D3Cube result = a[0].getBoundingBox();
  for(size_t i = 1; i < n; i++) {
    result += a[i].getBoundingBox();
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

  VertexProfile2D va2D = profile.getVertexProfile(param.isSet(PRROT_NORMALSMOOTH));
  if(param.isSet(PRROT_INVERTNORMALS)) {
    va2D.invertNormals();
  }

  VertexProfile3D         vp3D(va2D, param.m_converter);
  BitSet                  centerVertexSet(10); // = getVerticesOnRotationAxis(param.m_rotateAxis, va3D);
  const UINT              curveCount      = (UINT)vp3D.size();
  const UINT              edgeCount       = param.m_edgeCount;
  const float             step            = (float)param.m_rad / edgeCount;
  const D3DXVECTOR3       rotaxis         = createUnitVector(param.m_converter.getRotateAxisIndex());
  D3DXQUATERNION          rotStep         = createRotation(rotaxis,  step  );
  D3DXQUATERNION          rothalfP        = createRotation(rotaxis,  step/2);
  D3DXQUATERNION          rothalfN        = createRotation(rotaxis, -step/2);
  D3DXQUATERNION          currentRot      = rotStep;
  Array<VertexProfile3D>  edgeArray(edgeCount);

  edgeArray.add(vp3D);
  for(UINT i = 1; i < edgeCount; i++, currentRot *= rotStep) {
    edgeArray.add(vp3D.rotate(currentRot));
  }

  D3Cube boundingBox = getBoundingBox(edgeArray);

  MeshBuilder mb;
  for(UINT e = 1; e <= edgeCount; e++) {
    VertexProfile3D &p1 = edgeArray[e - 1];
    VertexProfile3D &p2 = edgeArray[e % edgeCount];
    for(UINT c = 0; c < curveCount; c++) {
      VertexCurve3D &c1         = p1[c];
      VertexCurve3D &c2         = p2[c];
      const size_t   pointCount = c1.size();
      for(UINT j1 = 0, j2 = 1; j2 < pointCount; j1++, j2++) {
        const bool isVj1Center = centerVertexSet.contains(j1), isVj2Center = centerVertexSet.contains(j2);
        if(isVj1Center && isVj2Center) {
          continue;
        }
        VertexNormalWithIndex   &v11 = c1[j1];  // lower left  v11=ll - asumming rotating around z-axis and starting from bottom
        VertexNormalWithIndex   &v12 = c1[j2];  // upper left  v12=ul
        VertexNormalWithIndex   &v21 = c2[j1];  // lower right v21=lr
        VertexNormalWithIndex   &v22 = c2[j2];  // upper right v22=ur
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
      } // for(UINT j1 = 0, j2 = 1; j2 < pointCount; j1++, j2++) {
    } // for(UINT c = 0; c < curveCount; c++) {
  } // for(UINT e = 1; e <= edgeCount; e++)
  return mb.optimize().createMesh(amf, doubleSided);
}
