#include "stdafx.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Cube.h>
#include <D3DGraphics/D3SceneObjectWireFrameBox.h>
#include <D3DGraphics/Profile3D.h>
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

static Profile2D createProfile(const Point2DArray &points) {
  Profile2D        result;
  ProfilePolygon2D polygon;
  ProfileCurve2D   curve(TT_PRIM_LINE);
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

static Profile2D createProfile(const Point2D *data, int n) {
  Point2DArray points(n);
  for(int i = 0; i < n; i++) {
    points.add(data[i]);
  }
  return createProfile(points);
}

LPD3DXMESH BrickObject::createMesh(AbstractMeshFactory &amf, BYTE attr) { // static
  Profile2D profile;
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
  param.m_converter = Point2DTo3DConverter('z', 'y');
  param.m_rad                = (float)(2*M_PI);
  param.m_edgeCount          = ISSQUARE(attr) ? 4 : 20;
  if(!ISSQUARE(attr)) param.setFlag(PRROT_ROTATESMOOTH);
//  param.setFlag(PRROT_INVERTNORMALS);
  return rotateProfile(amf, profile, param, false);
}

class BrickMarker : public D3SceneObjectWireFrameBox {
public:
  BrickMarker(BrickObject *parent)
    : D3SceneObjectWireFrameBox(parent
                               ,::getBoundingBox(parent->getMesh())
                               ,format(_T("%s(Marker)"),parent->getName().cstr()))
  {
  }
  int getMaterialId() const override {
    return GameBoardObject::getMarkerMaterialId();
  }
  D3DXMATRIX &getWorld() override {
    return getParent()->getWorld();
  }
};

BrickObject::BrickObject(GameBoardObject *board, BYTE attr)
: D3SceneObjectWithMesh(board
                       ,createMesh(board->getDevice(), attr)
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
  getDevice().setSpecularHighlights(true).setTexture(0, nullptr);
  __super::draw();
  if(m_marked) {
    m_brickMarker->draw();
  }
}

#if defined(_DEBUG)
String BrickObject::toString() const {
  return format(_T("%s\nWorld:\n%s")
               ,getName().cstr()
               ,indentString(((D3World)(*this)).toString(),2).cstr());
}
#endif
