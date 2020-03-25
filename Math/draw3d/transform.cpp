#include "stdafx.h"
#include <Math.h>
#include <Math/MathLib.h>
#include "Transform.h"

#define EPSILON 0.001
#define ABS(x) ((x) < 0 ? -(x) : (x))

Camera::Camera() {
  m_angleh = 60;
  m_anglev = 50;
  m_zoom   = 1;
  m_projection = PERSPECTIVE; // ORTHOGRAPHIC;
  m_front = 0.5;
  m_back  = 100;
  transInitialize();
}

void Camera::setRect(const CRect &r) {
  m_screen.m_size.x = r.Width();
  m_screen.m_size.y = r.Height();
  m_screen.m_center.x = r.left + m_screen.m_size.x/2;
  m_screen.m_center.y = r.top  + m_screen.m_size.y/2;
}
#define LEFTHANDED
bool D3DFrame::calcw2e() {
  const float l1 = length(m_dir);
  if(l1 < EPSILON) {  // is the dir vector legal
    return false;
  }
  D3DXVECTOR3 baseA(m_dir);
  baseA /= l1;

  D3DXVECTOR3 baseB(cross(m_up,baseA));
  const float l2 = length(baseB);
  if(l2 < EPSILON) {
    return false;
  }
  baseB /= l2;

  D3DXVECTOR3 baseC(cross(baseA,baseB));
  m_w2e(0,0) = baseB.x; m_w2e(0,1) = baseB.y; m_w2e(0,2) = baseB.z;
  m_w2e(1,0) = baseA.x; m_w2e(1,1) = baseA.y; m_w2e(1,2) = baseA.z;
  m_w2e(2,0) = baseC.x; m_w2e(2,1) = baseC.y; m_w2e(2,2) = baseC.z;
  return true;
}

void D3DFrame::setOrientation(const D3DXVECTOR3 &newDir, const D3DXVECTOR3 &newUp) {
  m_dir = newDir;
  m_up  = newUp;
  calcw2e();
}

void D3DFrame::getOrientation(D3DXVECTOR3 &dir, D3DXVECTOR3 &up) {
  dir = m_dir;
  up  = m_up;
}

void D3DFrame::getOrientation(D3DXVECTOR3 &dir, D3DXVECTOR3 &up, D3DXVECTOR3 &right) {
  getOrientation(dir,up);
  right = cross(up,dir);
}

/*
   transInit() initialises various variables and performs checks
   on some of the camera and screen parameters. It is left up to a
   particular implementation to handle the different error conditions.
   It should be called whenever the screen or camera variables change.
*/
bool Camera::transInitialize() {

  /* Do we have legal camera apertures ? */
  if(m_angleh < EPSILON || m_anglev < EPSILON)
    return false;

  /* Calculate camera aperture statics, note: angles in degrees */
  m_tanthetah = (float)tan(GRAD2RAD(m_angleh) / 2);
  m_tanthetav = (float)tan(GRAD2RAD(m_anglev) / 2);

  /* Do we have a legal camera zoom ? */
  if(m_zoom < EPSILON)
    return false;

  /* Are the clipping planes legal ? */
  if(m_front < 0 || m_back < 0 || m_back <= m_front)
    return false;

  return true;
}

/*
   Take a point in world coordinates and transform it to
   a point in the eye coordinate system.
*/
D3DXVECTOR3 Camera::transWorld2Eye(const D3DXVECTOR3 &w) {
  return m_w2e * (w-m_pos);
}

/*
   Clip a line segment in eye coordinates to the camera front
   and back clipping planes. Return FALSE if the line segment
   is entirely before or after the clipping planes.
*/
bool Camera::transClipEye(D3DXVECTOR3 &e1, D3DXVECTOR3 &e2) {
   float mu;

   /* Is the vector totally in front of the front cutting plane ? */
  if(e1.y <= m_front && e2.y <= m_front)
    return false;

   /* Is the vector totally behind the back cutting plane ? */
  if(e1.y >= m_back && e2.y >= m_back)
    return false;

   /* Is the vector partly in front of the front cutting plane ? */
  if((e1.y < m_front && e2.y > m_front) ||
     (e1.y > m_front && e2.y < m_front)) {
    mu = (m_front - e1.y) / (e2.y - e1.y);
    if(e1.y < m_front) {
      e1.x = e1.x + mu * (e2.x - e1.x);
      e1.z = e1.z + mu * (e2.z - e1.z);
      e1.y = m_front;
    } else {
      e2.x = e1.x + mu * (e2.x - e1.x);
      e2.z = e1.z + mu * (e2.z - e1.z);
      e2.y = m_front;
    }
  }

   /* Is the vector partly behind the back cutting plane ? */
  if((e1.y < m_back && e2.y > m_back) ||
     (e1.y > m_back && e2.y < m_back)) {
    mu = (m_back - e1.y) / (e2.y - e1.y);
    if(e1.y < m_back) {
      e2.x = e1.x + mu * (e2.x - e1.x);
      e2.z = e1.z + mu * (e2.z - e1.z);
      e2.y = m_back;
    } else {
      e1.x = e1.x + mu * (e2.x - e1.x);
      e1.z = e1.z + mu * (e2.z - e1.z);
      e1.y = m_back;
    }
  }
  return true;
}

/*
   Take a vector in eye coordinates and transform it into
   normalized coordinates for a perspective view. No normalisation
   is performed for an orthographic projection. Note that although
   the y component of the normalised vector is copied from the eye
   coordinate system, it is generally no longer needed. It can
   however still be used externally for vector sorting.
*/
D3DXVECTOR3 Camera::transEye2Norm(const D3DXVECTOR3 &e) {
  float d;
  D3DXVECTOR3 n;
  if(m_projection == PERSPECTIVE) {
    d = m_zoom / e.y;
    n.x = d * e.x / m_tanthetah;
    n.y = e.y;
    n.z = d * e.z / m_tanthetav;
  } else {
    n.x = m_zoom * e.x / m_tanthetah;
    n.y = e.y;
    n.z = m_zoom * e.z / m_tanthetav;
  }
  return n;
}

/*
   Clip a line segment to the normalised coordinate +- square.
   The y component is not touched.
*/
bool Camera::transClipNorm(D3DXVECTOR3 &n1, D3DXVECTOR3 &n2) {
  float mu;

   /* Is the line segment totally right of x = 1 ? */
  if(n1.x >= 1 && n2.x >= 1)
    return false;

   /* Is the line segment totally left of x = -1 ? */
  if(n1.x <= -1 && n2.x <= -1)
    return false;

   /* Does the vector cross x = 1 ? */
  if((n1.x > 1 && n2.x < 1) || (n1.x < 1 && n2.x > 1)) {
    mu = (1 - n1.x) / (n2.x - n1.x);
    if(n1.x < 1) {
      n2.z = n1.z + mu * (n2.z - n1.z);
      n2.x = 1;
    } else {
      n1.z = n1.z + mu * (n2.z - n1.z);
      n1.x = 1;
    }
  }

   /* Does the vector cross x = -1 ? */
  if((n1.x < -1 && n2.x > -1) || (n1.x > -1 && n2.x < -1)) {
    mu = (-1 - n1.x) / (n2.x - n1.x);
    if (n1.x > -1) {
      n2.z = n1.z + mu * (n2.z - n1.z);
      n2.x = -1;
    } else {
      n1.z = n1.z + mu * (n2.z - n1.z);
      n1.x = -1;
    }
  }

   /* Is the line segment totally above z = 1 ? */
  if(n1.z >= 1 && n2.z >= 1)
    return false;

   /* Is the line segment totally below z = -1 ? */
  if(n1.z <= -1 && n2.z <= -1)
    return false;

   /* Does the vector cross z = 1 ? */
  if((n1.z > 1 && n2.z < 1) || (n1.z < 1 && n2.z > 1)) {
    mu = (1 - n1.z) / (n2.z - n1.z);
    if(n1.z < 1) {
      n2.x = n1.x + mu * (n2.x - n1.x);
      n2.z = 1;
    } else {
      n1.x = n1.x + mu * (n2.x - n1.x);
      n1.z = 1;
    }
  }

   /* Does the vector cross z = -1 ? */
  if((n1.z < -1 && n2.z > -1) || (n1.z > -1 && n2.z < -1)) {
    mu = (-1 - n1.z) / (n2.z - n1.z);
    if(n1.z > -1) {
      n2.x = n1.x + mu * (n2.x - n1.x);
      n2.z = -1;
    } else {
      n1.x = n1.x + mu * (n2.x - n1.x);
      n1.z = -1;
    }
  }
  return true;
}

/*
   Take a vector in normalised Coordinates and transform it into
   screen coordinates.
*/
CPoint Camera::transNorm2Screen(const D3DXVECTOR3 &norm) {
  CPoint projected;
  projected.x = m_screen.m_center.x + (int)(m_screen.m_size.x * norm.x / 2);
  projected.y = m_screen.m_center.y - (int)(m_screen.m_size.y * norm.z / 2);
  return projected;
}

/*
   Transform a point from world to screen coordinates. Return TRUE
   if the point is visible, the point in screen coordinates is p.
   Assumes transInitialize() has been called
*/
bool Camera::transPoint(const D3DXVECTOR3 &w, CPoint &p) {
  D3DXVECTOR3 e,n;

  e = transWorld2Eye(w);
  if(e.y >= m_front && e.y <= m_back) {
    n = transEye2Norm(e);
    if(n.x >= -1 && n.x <= 1 && n.z >= -1 && n.z <= 1) {
      p = transNorm2Screen(n);
      return true;
    }
  }
  return false;
}

/*
   Transform and appropriately clip a line segment from
   world to screen coordinates. Return TRUE if something
   is visible and needs to be drawn, namely a line between
   screen coordinates p1 and p2.
   Assumes transInitialize() has been called
*/
bool Camera::transLine(const D3DXVECTOR3 &w1, const D3DXVECTOR3 &w2, CPoint &p1, CPoint &p2) {
  D3DXVECTOR3 e1,e2,n1,n2;

  e1 = transWorld2Eye(w1);
  e2 = transWorld2Eye(w2);
  if(transClipEye(e1,e2)) {
    n1 = transEye2Norm(e1);
    n2 = transEye2Norm(e2);
    if(transClipNorm(n1,n2)) {
      p1 = transNorm2Screen(n1);
      p2 = transNorm2Screen(n2);
      return true;
    }
  }
  return false;
}

void Camera::render(CDC &dc, const D3DObject &obj) {
  CPoint p1,p2;
  const MeshBuilder &mesh = obj.m_mesh;
  for(size_t i = 0; i < mesh.m_faces.size(); i++) {
    const Face  &face = mesh.m_faces[i];
    const short *i1   = face.getBuffer();
    const short *i2   = i1+1;
    for(size_t n = face.size(); n--; i1=i2, i2=(n>1)?(i2+1):face.getBuffer()) {
      if(transLine(mesh.m_points[*i1],mesh.m_points[*i2],p1,p2)) {
        dc.MoveTo(p1.x,p1.y);
        dc.LineTo(p2.x,p2.y);
      }
    }
  }
}

void D3DObject::addPoint(double x, double y, double z) {
  D3DXVECTOR3 v;
  v.x = (float)x;
  v.y = (float)y;
  v.z = (float)z;
  m_mesh.m_points.add(v);
}

void D3DObject::addPoint(const D3DXVECTOR3 &v) {
  m_mesh.m_points.add(v);
}

void D3DObject::addFace(const Face &f) {
  m_mesh.m_faces.add(f);
}

void Scene::render(CDC &dc) {
  for(size_t i = 0; i < m_objects.size(); i++) {
    m_camera.render(dc,m_objects[i]);
  }
  D3DXVECTOR3 pos, dir, up, right;
  getCameraPos(pos);
  getCameraOrientation(dir, up, right);
  String str = format(_T("Pos:(%.2f,%.2f,%.2f) Dir:(%.2f,%.2f,%.2f) Up:(%.2f,%.2f,%.2f) Right:(%.2f,%.2f,%.2f)")
                     , pos.x  , pos.y  , pos.z
                     , dir.x  , dir.y  , dir.z
                     , up.x   , up.y   , up.z
                     , right.x, right.y, right.z);
  textOut(dc,10,15, str);
}

void Scene::setCameraOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  m_camera.setOrientation(dir, up);
}

void Scene::getCameraOrientation( D3DXVECTOR3 &dir, D3DXVECTOR3 &up) {
  m_camera.getOrientation(dir, up);
}

void Scene::getCameraOrientation( D3DXVECTOR3 &dir, D3DXVECTOR3 &up, D3DXVECTOR3 &right) {
  m_camera.getOrientation(dir, up, right);
}

void Scene::setCameraPos(const D3DXVECTOR3 &pos) {
  m_camera.setPos(pos);
}

void Scene::getCameraPos(D3DXVECTOR3 &pos) {
  m_camera.getPos(pos);
}

void Scene::setRect(const CRect &r) {
  m_camera.setRect(r);
}
