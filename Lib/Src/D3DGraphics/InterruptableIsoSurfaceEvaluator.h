#include "pch.h"
#include <InterruptableRunnable.h>
#include <D3DGraphics/MeshBuilder.h>
#include <D3DGraphics/IsoSurfaceParameters.h>
#include <D3DGraphics/IsosurfacePolygonizer.h>

class InterruptableIsoSurfaceEvaluator : public IsoSurfaceEvaluator {
private:
  IsoSurfaceParameters          m_param;
  Function3D                   *m_f;
  void checkUserAction() const {
    if(m_interruptable) {
      m_interruptable->checkInterruptAndSuspendFlags();
    }
  }
protected:
  bool                          m_reverseSign;
  size_t                        m_lastVertexCount;
  MeshBuilder                   m_mb;
  PolygonizerStatistics         m_statistics;
  const IsoSurfaceVertexArray  *m_vertexArray;
  InterruptableRunnable        *m_interruptable;
public:
  InterruptableIsoSurfaceEvaluator(const IsoSurfaceParameters &param);
  InterruptableIsoSurfaceEvaluator &createData(Function3D &f, InterruptableRunnable *ir = NULL);
  InterruptableIsoSurfaceEvaluator &createData(FunctionWithTimeTemplate<Function3D> &f, const Real &time, InterruptableRunnable *ir = NULL);
  double evaluate(const Point3D &p);
  void   receiveFace(const Face3 &face);
  String getInfoMessage() const {
    return m_statistics.toString();
  }
  const IsoSurfaceParameters &getParam() const {
    return m_param;
  }
  const MeshBuilder &getMeshbuilder() const {
    return m_mb;
  }
};
