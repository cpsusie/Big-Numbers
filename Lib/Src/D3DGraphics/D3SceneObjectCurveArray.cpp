#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3ToString.h>

DECLARE_THISFILE;

D3SceneObjectCurveArray::D3SceneObjectCurveArray(D3Scene &scene, const CurveArray &curves) : D3SceneObjectWithVertexBuffer(scene) {
  int totalVertexCount = 0;

  for(size_t i = 0; i < curves.size(); i++) {
    const int vertexCount = (int)curves[i].size();
    m_curveSize.add(vertexCount);
    totalVertexCount += vertexCount;
  }
  Vertex *vertices = allocateVertexArray<Vertex>(totalVertexCount);

  int startIndex = 0;
  for(size_t i = 0; i < curves.size(); i++) {
    const VertexArray &a = curves[i];
    const int          count = (int)a.size();
    memcpy(vertices + startIndex, a.getBuffer(), sizeof(Vertex)*count);
    startIndex += count;
  }
  unlockVertexArray();
}

void D3SceneObjectCurveArray::draw() {
  if(hasVertexBuffer()) {
    setStreamSource();
    setLightingEnable(true);
    int startIndex = 0;
    for (size_t i = 0; i < m_curveSize.size(); i++) {
      const int vertexCount = m_curveSize[i];
      drawPrimitive(D3DPT_LINESTRIP, startIndex, vertexCount - 1);
      startIndex += vertexCount;
    }
  }
}

#define SINCOS(degree,c,s) double c = radians(degree), s; sincos(c,s)
#define RSINCOS(degree,r,c,s) SINCOS(degree,c,s); c*=r; s*=r

CurveArray CurveArray::createSphereObject(double r) {
  CurveArray curves;
  for(int fi = 0; fi < 180; fi += 45) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  for(int fi = -180+30; fi < 180; fi += 30) {
    RSINCOS(fi, r, RcosFi, RsinFi);
    VertexArray va;
    for(int theta = 0; theta < 360; theta += 5) {
      RSINCOS(theta, RsinFi, RcosTheta, RsinTheta);
      va.add(Vertex(RcosTheta, RsinTheta, RcosFi));
    }
    curves.add(va);
  }
  return curves;
}
