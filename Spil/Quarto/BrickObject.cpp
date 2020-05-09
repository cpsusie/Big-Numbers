#include "stdafx.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Cube.h>
#include <D3DGraphics/D3SceneObjectWireFrameBox.h>
#include <D3DGraphics/Profile.h>
#include "GraphicObjects.h"

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
//  polygon.reverseOrder();
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

LPD3DXMESH BrickObject::createMesh(AbstractMeshFactory &amf, BYTE attr) { // static
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
  param.m_aligny     = 2;
  param.m_rotateAxis = 2;
  param.m_rad        = 2*M_PI;
  param.m_edgeCount  = ISSQUARE(attr) ? 4 : 20;
  param.m_smoothness = ISSQUARE(attr) ? 0 : ROTATESMOOTH;

  return rotateProfile(amf, profile, param, true);
}

class BrickMarker : public D3SceneObjectWireFrameBox {
public:
  BrickMarker(BrickObject *parent) 
    : D3SceneObjectWireFrameBox(parent
                               ,::getBoundingBox(parent->getMesh())
                               ,format(_T("%s(Marker)"),parent->getName().cstr()))
  {
  }
  int getMaterialId() const {
    return GameBoardObject::getMarkerMaterialId();
  }
  D3DXMATRIX &getWorld() {
    return getParent()->getWorld();
  }
};

BrickObject::BrickObject(GameBoardObject *board, BYTE attr)
: D3SceneObjectWithMesh(board
                       ,createMesh(board->getScene(), attr)
                       ,format(_T("Brick(%d):%s"),attr,Brick::toString(attr).cstr())
                       )
, m_attr(attr)
, m_marked(false)
, m_visible(true)
{
  m_brickMarker = new BrickMarker(this); TRACE_NEW(m_brickMarker);
}


BrickObject::~BrickObject() {
  SAFEDELETE(m_brickMarker);
}

GameBoardObject &BrickObject::getBoard() const {
  return *(GameBoardObject*)getParent();
}

int BrickObject::getMaterialId() const {
  return getBoard().getBrickMaterialId(ISBLACK(m_attr));
}

D3DXMATRIX &BrickObject::getWorld() {
  D3World w(getParent()->getWorld());
  return m_world = w.setPos(w.getPos() + rotate(m_pos, w.getOrientation()));
}

void BrickObject::draw() {
  getDevice().setSpecularEnable(true);
  __super::draw();
  if(m_marked) {
    m_brickMarker->draw();
  }
}

#ifdef _DEBUG
String BrickObject::toString() const {
  return format(_T("%s\nWorld:\n%s")
               ,getName().cstr()
               ,indentString(((D3World)(*this)).toString(),2).cstr());
}
#endif
