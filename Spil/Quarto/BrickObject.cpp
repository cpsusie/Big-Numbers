#include "stdafx.h"
#include <D3DGraphics/Profile.h>
#include "GraphicObjects.h"

DECLARE_THISFILE;

static const Point2D smallProfileNoTop[] = {
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,1   ),
  Point2D( 0   ,1   )
};

static const Point2D smallProfileWithTop[] = {
  Point2D( 0    ,0  ),
  Point2D( 0.6  ,0  ),
  Point2D( 0.6  ,0.6),
  Point2D( 0.75 ,0.6),
  Point2D( 0.75 ,1  ),
  Point2D( 0    ,1  )
};

static const Point2D bigProfileNoTop[] = {
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,2   ),
  Point2D( 0   ,2   )
};

static const Point2D bigProfileWithTop[] = {
  Point2D( 0   ,0   ),
  Point2D( 0.6 ,0   ),
  Point2D( 0.6 ,1.6 ),
  Point2D( 0.75,1.6 ),
  Point2D( 0.75,2   ),
  Point2D( 0   ,2   )
};
/*
LPDIRECT3DRMMATERIAL CQuartoDlg::createMaterial(COLORREF emissive, COLORREF specular) {
  LPDIRECT3DRMMATERIAL material = m_d3.createMaterial(5);
  setMaterialColor(material,emissive,specular);
  return material;
}

void CQuartoDlg::setMaterialColor(LPDIRECT3DRMMATERIAL material, COLORREF emissive, COLORREF specular) {
  float r = (float)GetRValue(emissive) / 255;
  float g = (float)GetGValue(emissive) / 255;
  float b = (float)GetBValue(emissive) / 255;
  material->SetEmissive(r,g,b);
  r = (float)GetRValue(specular) / 255;
  g = (float)GetGValue(specular) / 255;
  b = (float)GetBValue(specular) / 255;
  material->SetSpecular(r,g,b);
}
*/

static Profile createProfile(const Point2DArray &points) {
  Profile result;
  ProfilePolygon polygon;
  ProfileCurve curve(TT_PRIM_LINE);
  polygon.m_start = points[0];
  polygon.m_closed = false;
  for(size_t i = 1; i < points.size(); i++) {
    curve.addPoint(points[i]);
  }
  polygon.addCurve(curve);
  polygon.reverseOrder();
  result.addPolygon(polygon);
  
  return result;
}

static Profile createProfile(const Point2D *data, int n) {
  Point2DArray points(n);
  for(int i = 0; i < n; i++) {
    points.add(data[i]);
  }
  return createProfile(points);
}

LPD3DXMESH BrickObject::createMesh(LPDIRECT3DDEVICE device, BYTE attr) { // static
  Profile profile;
  if(ISBIG(attr)) {
    if(ISWITHTOP(attr)) {
      profile = createProfile(bigProfileWithTop,ARRAYSIZE(bigProfileWithTop));
    } else {
      profile = createProfile(bigProfileNoTop,ARRAYSIZE(bigProfileNoTop));
    }
  } else {
    if(ISWITHTOP(attr)) {
      profile = createProfile(smallProfileWithTop,ARRAYSIZE(smallProfileWithTop));
    } else {
      profile = createProfile(smallProfileNoTop,ARRAYSIZE(smallProfileNoTop));
    }
  }

  ProfileRotationParameters param;
  param.m_alignx     = 0;
  param.m_aligny     = 1;
  param.m_rotateAxis = 1;
  param.m_rad        = 2*M_PI;
  param.m_edgeCount  = ISSQUARE(attr) ? 4 : 20;
  param.m_smoothness = ISSQUARE(attr) ? 0 : ROTATESMOOTH;
  param.m_useColor   = true;
  param.m_color      = ISBLACK( attr) ? D3D_BLUE : D3D_RED;

  return rotateProfile(device, profile, param, true);
}

class BrickMarker : public D3LineArray {
private:
  D3PosDirUpScale &m_pdus;
public:
  BrickMarker(D3Scene &scene, const Cube3D &box, D3PosDirUpScale &pdus)
    : D3LineArray(scene, box.m_lbn, box.m_rtf)
    , m_pdus(pdus)
  {}
  D3PosDirUpScale getPDUS() const {
    return m_pdus;
  }
};


BrickObject::BrickObject(D3Scene &scene, BYTE attr)
: SceneObjectWithMesh(scene, createMesh(scene.getDevice(), attr)) {
  m_marked = false;
  m_brickMarker = new BrickMarker(scene, getBoundingBox(getMesh()), m_pdus);
}

BrickObject::~BrickObject() {
  delete m_brickMarker;
}

void BrickObject::draw() {
  getDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
  prepareDraw();
  getMesh()->DrawSubset(0);
  if(m_marked) {
    m_brickMarker->draw();
  }
}
