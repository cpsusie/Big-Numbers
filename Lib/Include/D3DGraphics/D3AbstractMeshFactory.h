#pragma once

#include <MFCUtil/D3DeviceFactory.h>

class AbstractMeshFactory {
public:
  virtual LPD3DXMESH allocateMesh(DWORD fvf, UINT faceCount, UINT vertexCount, DWORD options) = 0;
};
