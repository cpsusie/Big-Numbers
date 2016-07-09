#pragma once

#include "ExpressionWrapper.h"
#include "MeshBuilder.h"

class VariableMeshCreator {
public:
  virtual LPD3DXMESH createMesh(double t) const = 0;
  virtual ~VariableMeshCreator() {
  }
};

class MeshArrayJobParameter {
public:
  virtual const DoubleInterval &getTimeInterval()    const = 0;
  virtual int                   getTimeCount()       const = 0;
  virtual VariableMeshCreator  *fetchMeshCreator()   const = 0;
  MeshArray createMeshArray(CWnd *wnd);

};

