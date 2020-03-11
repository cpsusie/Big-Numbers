#pragma once

#include <MFCUtil/DirectXDeviceFactory.h>

class AbstractMeshFactory {
public:
  virtual LPD3DXMESH allocateMesh(DWORD fvf, UINT faceCount, UINT vertexCount, DWORD options) = 0;
};
