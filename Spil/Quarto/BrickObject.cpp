#include "stdafx.h"
#include <D3DGraphics/Profile.h>
#include "GraphicObjects.h"

DECLARE_THISFILE;

static const Point2D smallProfileNoTop[] = {
  Point2D( 0    ,0   )
 ,Point2D( 0.6  ,0   )
 ,Point2D( 0.6  ,1   )
 ,Point2D( 0    ,1   )
};

static const Point2D smallProfileWithTop[] = {
  Point2D( 0    ,0   )
 ,Point2D( 0.6  ,0   )
 ,Point2D( 0.6  ,0.6 )
 ,Point2D( 0.75 ,0.6 )
 ,Point2D( 0.75 ,1   )
 ,Point2D( 0    ,1   )
};

static const Point2D bigProfileNoTop[] = {
  Point2D( 0    ,0   )
 ,Point2D( 0.6  ,0   )
 ,Point2D( 0.6  ,2   )
 ,Point2D( 0    ,2   )
};

static const Point2D bigProfileWithTop[] = {
  Point2D( 0    ,0   )
 ,Point2D( 0.6  ,0   )
 ,Point2D( 0.6  ,1.6 )
 ,Point2D( 0.75 ,1.6 )
 ,Point2D( 0.75 ,2   )
 ,Point2D( 0    ,2   )
};

static Profile createProfile(const Point2DArray &points) {
  Profile        result;
  ProfilePolygon polygon;
  ProfileCurve   curve(TT_PRIM_LINE);
  polygon.m_start  = points[0];
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

  return rotateProfile(device, profile, param, true);
}

class BrickMarker : public D3LineArray {
private:
  D3PosDirUpScale &m_pdus;
public:
  BrickMarker(D3Scene &scene, const D3DXCube3 &box, D3PosDirUpScale &pdus)
    : D3LineArray(scene, box.m_lbn, box.m_rtf)
    , m_pdus(pdus)
  {}
  D3PosDirUpScale getPDUS() const {
    return m_pdus;
  }
};

D3DMATERIAL *BrickObject::s_material[2] = {
  NULL
 ,NULL
};

BrickObject::BrickObject(D3Scene &scene, BYTE attr)
: SceneObjectWithMesh(scene, createMesh(scene.getDevice(), attr))
, m_attr(attr)
, m_marked(false)
{
  m_brickMarker = new BrickMarker(scene, getBoundingBox(getMesh()), m_pdus);
  setName(format(_T("Brick(%d):%s")
                ,attr
                ,Brick::toString(attr).cstr()
                )
         );
}

BrickObject::~BrickObject() {
  delete m_brickMarker;
}

const D3DMATERIAL *BrickObject::getMaterial() const {
  const int index = ISBLACK(m_attr) ? 0 : 1;
  if(s_material[index] == NULL) {
    D3DMATERIAL *mat = new D3DMATERIAL(getScene().getMaterial());
    s_material[index] = mat;
    mat->Diffuse  = colorToColorValue(ISBLACK(m_attr) ? D3D_BLUE : D3D_RED);
    mat->Specular = colorToColorValue(D3D_WHITE);
    mat->Ambient  = colorToColorValue(D3D_BLACK);
    mat->Emissive = mat->Ambient;
  }
  return s_material[index];
}

void BrickObject::draw() {
  getDevice()->SetRenderState(D3DRS_LIGHTING, TRUE);
  prepareDraw(USE_SCENEFILLMODE|USE_SCENESHADEMODE);
  getDevice()->SetMaterial(getMaterial());
  getMesh()->DrawSubset(0);
  if(m_marked) {
    m_brickMarker->draw();
  }
}

#ifdef _DEBUG
String BrickObject::toString() const {
  return format(_T("%s\nPDUS:\n%s")
               ,getName().cstr()
               ,indentString(getPDUS().toString(),2).cstr());
}
#endif