#include "pch.h"

#ifdef __NEVER__

#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/IsoSurface.h>

class IsoSurfaceDebugObject : public SceneObjectWithMesh {
private:
  D3LineArray *m_debugLines;
public:
  IsoSurfaceDebugObject(D3Scene &scene, LPD3DXMESH mesh) : SceneObjectWithMesh(scene, mesh) {
    m_debugLines = NULL;
  }
  ~IsoSurfaceDebugObject() {
    SAFEDELETE(m_debugLines);
  }
  void createDebugLines(IsoSurface &surface);
  void draw();
};

void IsoSurfaceDebugObject::draw() {
  SceneObjectWithMesh::draw();
  if(m_debugLines) {
    m_debugLines->draw();
  }
}

void IsoSurfaceDebugObject::createDebugLines(IsoSurface &surface) {
  const CompactArray<Point3D> &pointArray = surface.getDebugPoints();
  if(pointArray.size() > 0) {
    const double  u = surface.getParam().m_cellSize / 8;
    const Point3D e1(u,0,0),e2(0,u,0),e3(0,0,u);
    CompactArray<Line> lines;
    for(size_t i = 0; i < pointArray.size(); i++) {
      const Point3D &p = pointArray[i];
      lines.add(Line(p-e1,p+e1));
      lines.add(Line(p-e2,p+e2));
      lines.add(Line(p-e3,p+e3));
    }
    m_debugLines = new D3LineArray(m_scene, lines.getBuffer(), (int)lines.size()); TRACE_NEW(m_debugLines);
  }
}

D3SceneObject *createIsoSurfaceDebugObject(D3Scene &scene, const IsoSurfaceParameters &param) {
  IsoSurface surface(param);
  surface.createData();
  LPD3DXMESH mesh = surface.getMeshbuilder().createMesh(scene.getDevice(), param.m_doubleSided);
  IsoSurfaceDebugObject *object = new IsoSurfaceDebugObject(scene, mesh); TRACE_NEW(object);
  object->createDebugLines(surface);
  return object;
}

#endif
