#pragma once

#include <MFCUtil/DirectXDeviceFactory.h>

class AbstractMeshFactory {
public:
  virtual LPD3DXMESH allocateMesh(DWORD fvf, UINT faceCount, UINT vertexCount, DWORD options) = 0;
};

LPD3DXMESH        createMeshFromVertexFile(     AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
LPD3DXMESH        createMeshFromObjFile(        AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
