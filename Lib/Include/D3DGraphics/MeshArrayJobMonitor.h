#pragma once

#include <NumberInterval.h>
#include <InterruptableRunnable.h>
#include <MFCUtil/AnimationParameters.h>
#include "MeshArray.h"

class AbstractMeshFactory;

class AbstractVariableMeshCreator {
protected:
  AbstractMeshFactory &m_amf;
  AbstractVariableMeshCreator(AbstractMeshFactory &amf)
    : m_amf(amf)
  {
  }
public:
  virtual LPD3DXMESH createMesh(double time, InterruptableRunnable *ir = nullptr) const = 0;
  virtual ~AbstractVariableMeshCreator() {
  }
};

class AbstractMeshArrayJobParameter {
private:
  const AnimationParameters &m_animation;
protected:
  AbstractMeshFactory &m_amf;
  AbstractMeshArrayJobParameter(AbstractMeshFactory &amf, const AnimationParameters &animation)
    : m_amf(amf)
    , m_animation(animation)
  {
  }
public:
  const DoubleInterval &getTimeInterval() const {
    return m_animation.getTimeInterval();
  }
  UINT getFrameCount() const {
    return m_animation.getFrameCount();
  }
  virtual AbstractVariableMeshCreator  *fetchMeshCreator() const = 0;
  MeshArray createMeshArray(CWnd *wnd);
};

