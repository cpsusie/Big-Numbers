#include "stdafx.h"
#include "MeshBuilder.h"

class Triangle {
public:
  int v1, v2, v3;
  D3DXVECTOR3 m_normal;
  float       m_area;
};

class VertexWithFaceArray : public Point3DP {
public:
  CompactIntArray m_faceArray;
  D3DXVECTOR3     m_normal;
};

LPD3DXMESH createMeshFromVertexFile(DIRECT3DDEVICE device, const String &fileName, bool doubleSided) {
  FILE *f = NULL;
  try {
    f = FOPEN("c:\\mytools2015\\Math\\triangle\\x64\\Release\\fisk.node", "r");
    String line;
    readLine(f, line);
    Tokenizer tok(line, " ");
    const int vertexCount = tok.getInt();
    Array<VertexWithFaceArray> vertexArray(vertexCount);
    for(int i = 0; i < vertexCount; i++) {
      readLine(f, line);
      Tokenizer tok(line, " ");
      VertexWithFaceArray v;
      int index = tok.getInt();
      v.x = tok.getDouble();
      v.y = tok.getDouble();
      v.z = tok.getDouble();
      vertexArray.add(v);
    }
    fclose(f); f = NULL;
    f = FOPEN("c:\\mytools2015\\Math\\triangle\\x64\\Release\\fisk.1.ele", "r");

    readLine(f, line);
    Tokenizer tok1(line, " ");
    const int faceCount = tok1.getInt();
    CompactArray<Triangle> faceArray(faceCount);
    for(int i = 0; i < faceCount; i++) {
      readLine(f, line);
      Triangle face;
      Tokenizer tok(line, " ");
      const int index = tok.getInt();
      assert(index == faceArray.size());
      face.v1 = tok.getInt();
      face.v2 = tok.getInt();
      face.v3 = tok.getInt();
      vertexArray[face.v1].m_faceArray.add(index);
      vertexArray[face.v2].m_faceArray.add(index);
      vertexArray[face.v3].m_faceArray.add(index);
      const Point3DP    &v1 = vertexArray[face.v1];
      const Point3DP    e12 = vertexArray[face.v2] - v1;
      const Point3DP    e13 = vertexArray[face.v3] - v1;
      const D3DXVECTOR3 c   = crossProduct(e13, e12);
      face.m_normal = unitVector(c);
      face.m_area = length(c)/2;
      faceArray.add(face);
    }
    fclose(f); f = NULL;
    for (int i = 0; i < vertexCount; i++) {
      VertexWithFaceArray   &v  = vertexArray[i];
      const CompactIntArray &fa = v.m_faceArray;
      D3DXVECTOR3 normal(0.0f,0.0f,0.0f);
      for(int f = 0; f < fa.size(); f++) {
        const Triangle &face = faceArray[fa[f]];
        normal += face.m_area * face.m_normal;
      }
      v.m_normal = unitVector(normal);
    }

    MeshBuilder mb;
    mb.clear(vertexCount);
    for(int i = 0; i < vertexCount; i++) {
      const VertexWithFaceArray &v = vertexArray[i];
      mb.addVertex(v);
      mb.addNormal(v.m_normal);
    }

    for(int i = 0; i < faceCount; i++) {
      const Triangle &triangle = faceArray[i];
      Face &face = mb.addFace();
      face.addVertexAndNormalIndex(triangle.v1, triangle.v1);
      face.addVertexAndNormalIndex(triangle.v2, triangle.v2);
      face.addVertexAndNormalIndex(triangle.v3, triangle.v3);
    }
    return mb.createMesh(device, doubleSided);

  } catch(Exception e) {
    if (f) {
      fclose(f); f = NULL;
    }
    throw;
  }
}
