#include "pch.h"
#include <D3DGraphics/D3SceneObjectWireFrameBox.h>

void D3SceneObjectWireFrameBox::init(const Vertex &p1, const Vertex &p2) {
  Line3D lines[12], *lp = lines, *ll;
  lp->m_p1 = p1;                           lp->m_p2 = Vertex(p1.x, p2.y, p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x, p2.y, p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x, p1.y, p1.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p1.x, p1.y, p1.z); ll = lp++;

  lp->m_p1 = Vertex(p1.x, p1.y, p2.z);     lp->m_p2 = Vertex(p1.x, p2.y, p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x, p2.y, p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p2.x, p1.y, p2.z); ll = lp++;
  lp->m_p1 = ll->m_p2;                     lp->m_p2 = Vertex(p1.x, p1.y, p2.z); ll = lp++;
  for (int i = 0; i < 4; i++) {
    lines[i + 8].m_p1 = lines[i].m_p1;
    lines[i + 8].m_p2 = lines[i + 4].m_p1;
  }
  initBuffer(lines, 12);
}

