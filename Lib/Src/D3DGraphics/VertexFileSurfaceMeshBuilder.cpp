#include "pch.h"
#include <FileNameSplitter.h>
#include <ExternProcess.h>
#include <D3DGraphics/MeshBuilder.h>

class Triangle {
public:
  int v1, v2, v3;
  D3DXVECTOR3 m_normal;
  float       m_area;
};

class VertexWithFaceArray : public Point3D {
public:
  CompactIntArray m_faceArray;
  D3DXVECTOR3     m_normal;
};

static String convertVertexFile(const String &fileName) {
  FILE *f = NULL;
  try {
    f = FOPEN(fileName, _T("r"));
    String line;
    int lineno = 0;
    CompactArray<Point3D> pointArray;
    while(readLine(f, line)) {
      lineno++;
      Point3D p;
      if(_stscanf(line.cstr(), _T("%le %le %le"), &p.x, &p.y, &p.z) != 3) {
        throwException(_T("Invalid input in file %s line %d"), fileName.cstr(), lineno);
      }
      pointArray.add(p);
    }
    fclose(f); f = NULL;
    String nodeFileName = _T("C:\\temp\\cXXXXXX");
    _tmktemp(nodeFileName.cstr());
    nodeFileName += _T(".node");
    f = MKFOPEN(nodeFileName, _T("w"));
    const UINT n = (UINT)pointArray.size();
    _ftprintf(f, _T("%d 2 1 0\n"), n);
    for(UINT i = 0; i < n; i++) {
      const Point3D &p = pointArray[i];
      _ftprintf(f, _T("%4d %lf %lf %lf\n"), i, p.x, p.y, p.z);
    }
    fclose(f); f = NULL;
    return nodeFileName;
  } catch(...) {
    if(f) fclose(f);
    throw;
  }
}

static void runTriangle(const String &nodeFileName) {
  const String triangleExeName = _T("C:\\mytools2015\\Math\\Triangle\\x64\\Release\\triangle.exe");
  const TCHAR *options         = _T("-QN");
  const TCHAR *fileName        = nodeFileName.cstr();
  ExternProcess::run(true, triangleExeName, options, fileName, NULL);
}

static MeshBuilder &createMeshBuilderFromNodefile(const String &nodeFileName, MeshBuilder &mb) {
  FILE *f = NULL;

  runTriangle(nodeFileName);
  FileNameSplitter info(nodeFileName);
  const String eleFileName = info.setFileName(info.getFileName() + _T(".1")).setExtension(_T("ele")).getAbsolutePath();
  try {
    f = FOPEN(nodeFileName, _T("r"));
    String line;
    readLine(f, line);
    Tokenizer tok(line, _T(" "));
    const int vertexCount = tok.getInt();
    Array<VertexWithFaceArray> vertexArray(vertexCount);
    for(int i = 0; i < vertexCount; i++) {
      readLine(f, line);
      VertexWithFaceArray v;
      int index;
      if(_stscanf(line.cstr(), _T("%d %le %le %le"), &index, &v.x, &v.y, &v.z) != 4) {
        throwException(_T("Invalid input in file %s line %d"), nodeFileName.cstr(), i+1);
      }
      vertexArray.add(v);
    }
    fclose(f); f = NULL;

    f = FOPEN(eleFileName, _T("r"));
    readLine(f, line);
    Tokenizer tok1(line, _T(" "));
    const int faceCount = tok1.getInt();
    CompactArray<Triangle> faceArray(faceCount);
    for(int i = 0; i < faceCount; i++) {
      readLine(f, line);
      int      index;
      Triangle face;
      if(_stscanf(line.cstr(), _T("%d %d %d %d"), &index, &face.v1, &face.v2, &face.v3) != 4) {
        throwException(_T("Invalid input in file %s line %d"), eleFileName.cstr(), i+1);
      }
      assert(index == faceArray.size());
      vertexArray[face.v1].m_faceArray.add(index);
      vertexArray[face.v2].m_faceArray.add(index);
      vertexArray[face.v3].m_faceArray.add(index);
      const D3DXVECTOR3 v1  = vertexArray[face.v1];
      const D3DXVECTOR3 e12 = (D3DXVECTOR3)vertexArray[face.v2] - v1;
      const D3DXVECTOR3 e13 = (D3DXVECTOR3)vertexArray[face.v3] - v1;
      const D3DXVECTOR3 c   = cross(e13, e12);
      face.m_normal = unitVector(c);
      face.m_area   = length(c)/2;
      faceArray.add(face);
    }
    fclose(f); f = NULL;
    for(int i = 0; i < vertexCount; i++) {
      VertexWithFaceArray   &v  = vertexArray[i];
      const CompactIntArray &fa = v.m_faceArray;
      D3DXVECTOR3 normal(0.0f,0.0f,0.0f);
      for(size_t f = 0; f < fa.size(); f++) {
        const Triangle &face = faceArray[fa[f]];
        normal += face.m_area * face.m_normal;
      }
      v.m_normal = unitVector(normal);
    }

    mb.clear(vertexCount);
    for(int i = 0; i < vertexCount; i++) {
      const VertexWithFaceArray &v = vertexArray[i];
      mb.addVertex(v);
      mb.addNormal(v.m_normal);
    }

    for(int i = 0; i < faceCount; i++) {
      const Triangle &triangle = faceArray[i];
      Face           &face     = mb.addFace();
      face.addVertexNormalIndex(triangle.v1, triangle.v1);
      face.addVertexNormalIndex(triangle.v2, triangle.v2);
      face.addVertexNormalIndex(triangle.v3, triangle.v3);
    }

    UNLINK(nodeFileName);
    UNLINK(eleFileName );
    return mb;
  } catch(Exception e) {
    if(f) {
      fclose(f); f = NULL;
    }
    throw;
  }
}

LPD3DXMESH createMeshFromVertexFile(AbstractMeshFactory &amf, const String &fileName, bool doubleSided) {
  MeshBuilder mb;
  return createMeshBuilderFromNodefile(convertVertexFile(fileName), mb).createMesh(amf, doubleSided);
}
