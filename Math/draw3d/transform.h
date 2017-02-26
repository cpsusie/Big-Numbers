#pragma once

typedef enum {
  PERSPECTIVE
 ,ORTHOGRAPHIC
} Projection;

class D3DFrame {
private:
  bool calcw2e();
public:
  D3DXVECTOR3 m_pos, m_dir, m_up;
  D3DXMATRIX  m_w2e; // world->eye transformation
  void setPos(const D3DXVECTOR3 &newpos) { m_pos = newpos; }
  void getPos(D3DXVECTOR3 &pos) { pos = m_pos; }
  void setOrientation(const D3DXVECTOR3 &newDir, const D3DXVECTOR3 &newUp);
  void getOrientation(D3DXVECTOR3 &dir, D3DXVECTOR3 &up);
  void getOrientation(D3DXVECTOR3 &dir, D3DXVECTOR3 &up, D3DXVECTOR3 &right);
};

class Screen {
public:
  CPoint m_center;
  CPoint m_size;
};

typedef CompactShortArray Face;

class MeshBuilder {
public:
  CompactArray<D3DXVECTOR3> m_points;
  Array<Face>               m_faces;
};

class D3DObject : public D3DFrame {
public:
  MeshBuilder m_mesh;
  void addPoint(double x, double y, double z);
  void addPoint(const D3DXVECTOR3 &p);
  void addFace(const Face &f);
};

class Camera : public D3DFrame {
private:
  D3DXVECTOR3 transWorld2Eye(const D3DXVECTOR3 &w);
  bool        transClipEye(D3DXVECTOR3 &e1, D3DXVECTOR3 &e2);
  D3DXVECTOR3 transEye2Norm(const D3DXVECTOR3 &e);
  bool        transClipNorm(D3DXVECTOR3 &n1, D3DXVECTOR3 &n2);
  CPoint      transNorm2Screen(const D3DXVECTOR3 &norm);
  bool        transInitialize();
public:
  Screen m_screen;
  float  m_angleh, m_anglev;
  float  m_tanthetah, m_tanthetav;
  float  m_zoom;
  float  m_front, m_back;
  Projection m_projection;

  void setRect(const CRect &r);
  bool transPoint(const D3DXVECTOR3 &w, CPoint &p);
  bool transLine( const D3DXVECTOR3 &w1, const D3DXVECTOR3 &w2, CPoint &p1, CPoint &p2);
  void render(CDC &dc, const D3DObject &obj);
  Camera();
};

class Scene {
private:
  Camera m_camera;

  Array<D3DObject> m_objects;
public:
  void setRect(const CRect &r);
  void setCameraOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  void getCameraOrientation( D3DXVECTOR3 &dir, D3DXVECTOR3 &up);
  void getCameraOrientation( D3DXVECTOR3 &dir, D3DXVECTOR3 &up, D3DXVECTOR3 &right);
  void setCameraPos(const D3DXVECTOR3 &pos);
  void getCameraPos(D3DXVECTOR3 &pos);
  void addObject(const D3DObject &obj) {
    m_objects.add(obj);
  }
  void render(CDC &dc);
};
