#include "stdafx.h"

using namespace ThreeD;

void MeshFace::getMinMax(D3DXVECTOR3 *vmin, D3DXVECTOR3 *vmax) const {
  float vmin_x, vmax_x;
  float vmin_y, vmax_y;
  float vmin_z, vmax_z;
  vmin_x = vmax_x = vertex(0).x;
  vmin_y = vmax_y = vertex(0).y;
  vmin_z = vmax_z = vertex(0).z;

  for(int n = 1; n < 3; n++) {
    const D3DXVECTOR3 &v = vertex(n);
    if(v.x < vmin_x) vmin_x = v.x;
    if(v.x > vmax_x) vmax_x = v.x;
    if(v.y < vmin_y) vmin_y = v.y;
    if(v.y > vmax_y) vmax_y = v.y;
    if(v.z < vmin_z) vmin_z = v.z;
    if(v.z > vmax_z) vmax_z = v.z;
  }

  *vmin = D3DXVECTOR3(vmin_x, vmin_y, vmin_z);
  *vmax = D3DXVECTOR3(vmax_x, vmax_y, vmax_z);
}

String MeshFace::toString() const {
  return format("v0:%s v1:%s v2:%s N:%s"
               ,::toString(*m_v[0]).cstr()
               ,::toString(*m_v[1]).cstr() 
               ,::toString(*m_v[2]).cstr() 
               ,::toString(m_planeNormal).cstr()
               );
}
