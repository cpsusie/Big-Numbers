#include "stdafx.h"
#include <D3DGraphics/D3Camera.h>

#ifdef DEBUG_POLYGONIZER
#include "Debugger.h"
#include "DebugIsoSurface.h"
// --------------------------------------------- DebugSceneobject -------------------------------------

DebugSceneobject::DebugSceneobject(D3Scene &scene)
  : D3SceneObjectVisual( scene, _T("Debug Polygonizer"))
  , m_meshObject(  NULL)
  , m_octaObject(  NULL)
  , m_tetraObject( NULL)
  , m_facesObject( NULL)
  , m_vertexObject(NULL)
  , m_visibleParts(0)
  , m_fillMode(    D3DFILL_WIREFRAME)
  , m_shadeMode(   D3DSHADE_FLAT)
{
  resetCameraFocus(true);
}

DebugSceneobject::~DebugSceneobject() {
  deleteMeshObject();
  deleteOctaObject();
  deleteTetraObject();
  deleteFacesObject();
  deleteVertexObject();
}

void DebugSceneobject::initOctaTetraVertex(OctaObject          *octaObject
                                          ,D3SceneObjectVisual *tetraObject
                                          ,D3SceneObjectVisual *vertexObject)
{
  m_octaObject   = octaObject;
  m_tetraObject  = tetraObject;
  m_vertexObject = vertexObject;
}

void DebugSceneobject::setMeshObject(DebugMeshObject *obj) {
  deleteMeshObject();
  m_meshObject = obj;
}

void DebugSceneobject::setFacesObject(D3SceneObjectVisual *obj) {
  deleteFacesObject();
  m_facesObject = obj;
}

void DebugSceneobject::draw() {
  if((m_visibleParts & MESH_VISIBLE   ) && m_meshObject   ) {
    m_meshObject->draw();
    if((m_visibleParts & OCTA_VISIBLE) && m_octaObject) {
      m_octaObject->draw();
      if((m_visibleParts & TETRA_VISIBLE  ) && m_tetraObject  ) m_tetraObject->draw();
      if((m_visibleParts & FACES_VISIBLE  ) && m_facesObject  ) m_facesObject->draw();
      if((m_visibleParts & VERTEX_VISIBLE ) && m_vertexObject ) m_vertexObject->draw();
    }
  }
}

void DebugSceneobject::deleteMeshObject() {
  if(m_meshObject) {
    m_fillMode = m_meshObject->getFillMode();
    m_shadeMode = m_meshObject->getShadeMode();
    SAFEDELETE(m_meshObject);
  }
}

void DebugSceneobject::deleteOctaObject()   { SAFEDELETE(m_octaObject   ); }
void DebugSceneobject::deleteTetraObject()  { SAFEDELETE(m_tetraObject  ); }
void DebugSceneobject::deleteFacesObject()  { SAFEDELETE(m_facesObject  ); }
void DebugSceneobject::deleteVertexObject() { SAFEDELETE(m_vertexObject ); }

D3Camera *DebugSceneobject::dbgCAM() {
  return getScene().getCameraArray()[0];
}

void DebugSceneobject::debugRotateFocusCam(const D3DXVECTOR3 &axis, float rad) {
  D3World w = dbgCAM()->getD3World();
  dbgCAM()->setD3World(w.setOrientation(w.getOrientation() * createRotation(axis, rad), getCubeCenter()));
}

bool DebugSceneobject::hasCubeCenter() const {
  return (m_visibleParts & OCTA_VISIBLE) != 0;
}

D3DXVECTOR3 DebugSceneobject::getCubeCenter() const {
  if(hasCubeCenter()) {
    D3DXVECTOR3 c = m_octaObject->getCenter();
    return c;
//    D3DXMATRIX  m = m_octaObject->getWorld();
//    return inverse(m) * c;
  } else {
    return D3DXORIGIN;
  }
}

void DebugSceneobject::resetCameraFocus(bool resetViewAngleAndDistance) {
  if(resetViewAngleAndDistance) {
    m_currentCamDistance = 0.25;
    dbgCAM()->setViewAngle(0.2864f);
  }
  D3World w = dbgCAM()->getD3World();
  w.resetOrientation();
  if(hasCubeCenter()) {
    w.setPos(getCubeCenter() - m_currentCamDistance * w.getDir());
  }
  dbgCAM()->setD3World(w);
}

#define DBG_CAMADJANGLE (D3DX_PI / 8)

bool DebugSceneobject::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  switch(nChar) {
  case VK_UP   : debugRotateFocusCam(dbgCAM()->getRight(), -DBG_CAMADJANGLE); return true;
  case VK_DOWN : debugRotateFocusCam(dbgCAM()->getRight(),  DBG_CAMADJANGLE); return true;
  case VK_LEFT : debugRotateFocusCam(dbgCAM()->getUp()   , -DBG_CAMADJANGLE); return true;
  case VK_RIGHT: debugRotateFocusCam(dbgCAM()->getUp()   ,  DBG_CAMADJANGLE); return true;
  case VK_HOME :
    if(ctrlKeyPressed()) {
      resetCameraFocus(true);
    } else {
      resetCameraFocus(false);
    }
    return true;
  }
  return false;
}

#endif // DEBUG_POLYGONIZER
