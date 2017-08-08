#pragma once

#include <NumberInterval.h>
#include <InterruptableRunnable.h>
#include <D3DGraphics/MeshArray.h>

class AbstractVariableMeshCreator {
public:
  virtual LPD3DXMESH createMesh(double time, InterruptableRunnable *ir = NULL) const = 0;
  virtual ~AbstractVariableMeshCreator() {
  }
};

class AbstractMeshArrayJobParameter {
public:
  virtual const DoubleInterval &getTimeInterval()    const = 0;
  virtual UINT                  getFrameCount()      const = 0;
  virtual AbstractVariableMeshCreator  *fetchMeshCreator()   const = 0;
  MeshArray createMeshArray(CWnd *wnd);

};

