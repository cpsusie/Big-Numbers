#pragma once

#include <NumberInterval.h>
#include <D3DGraphics/MeshArray.h>

class VariableMeshCreator {
public:
  virtual LPD3DXMESH createMesh(double time) const = 0;
  virtual ~VariableMeshCreator() {
  }
};

class MeshArrayJobParameter {
public:
  virtual const DoubleInterval &getTimeInterval()    const = 0;
  virtual UINT                  getFrameCount()      const = 0;
  virtual VariableMeshCreator  *fetchMeshCreator()   const = 0;
  MeshArray createMeshArray(CWnd *wnd);

};

