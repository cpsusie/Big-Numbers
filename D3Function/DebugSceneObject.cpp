#include "stdafx.h"

#ifdef ISODEBUGGER
#include <D3DGraphics/D3Camera.h>
#include "Debugger.h"
#include "DebugIsoSurface.h"
// --------------------------------------------- DebugSceneobject -------------------------------------

DebugSceneobject::DebugSceneobject(D3Scene &scene, Debugger &debugger)
  : D3SceneObjectVisual( scene, _T("Debug Polygonizer"))
  , m_debugger(debugger)
  , m_meshObject(  NULL)
  , m_octaObject(  NULL)
  , m_tetraObject( NULL)
  , m_facesObject( NULL)
  , m_vertexObject(NULL)
  , m_visibleParts(0   )
  , m_fillMode(    D3DFILL_WIREFRAME)
  , m_shadeMode(   D3DSHADE_FLAT    )
  , m_debugLightIndex(-1)
{
  resetCameraFocus(true);
  createDebugLight();
}

DebugSceneobject::~DebugSceneobject() {
  destroyDebugLight();
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
  if(isSet(MESH_VISIBLE) && m_meshObject  ) {
    m_meshObject->draw();
    if(isSet(OCTA_VISIBLE) && m_octaObject) {
      m_octaObject->draw();
      if(isSet(TETRA_VISIBLE ) && m_tetraObject ) m_tetraObject->draw();
      if(isSet(FACES_VISIBLE ) && m_facesObject ) m_facesObject->draw();
      if(isSet(VERTEX_VISIBLE) && m_vertexObject) m_vertexObject->draw();
    }
  }
}

void DebugSceneobject::deleteMeshObject() {
  if(m_meshObject) {
    m_fillMode  = m_meshObject->getFillMode();
    m_shadeMode = m_meshObject->getShadeMode();
    SAFEDELETE(m_meshObject);
  }
}

void DebugSceneobject::deleteOctaObject()   { SAFEDELETE(m_octaObject   ); }
void DebugSceneobject::deleteTetraObject()  { SAFEDELETE(m_tetraObject  ); }
void DebugSceneobject::deleteFacesObject()  { SAFEDELETE(m_facesObject  ); }
void DebugSceneobject::deleteVertexObject() { SAFEDELETE(m_vertexObject ); }

D3Camera *DebugSceneobject::dbgCAM() const {
  return getScene().getCameraArray()[0];
}

void DebugSceneobject::debugRotateFocusCam(const D3DXVECTOR3 &axis, float rad) {
  D3Camera *cam = dbgCAM();
  D3World   w   = cam->getD3World();
  cam->setD3World(w.setOrientation(w.getOrientation() * createRotation(axis, rad), getCubeCenter()));
}

void DebugSceneobject::createDebugLight() {
  m_debugLightIndex = getScene().addLight(D3Light::createDefaultLight());
}

void DebugSceneobject::destroyDebugLight() {
  if(hasDebugLight()) {
    getScene().removeLight(m_debugLightIndex);
    m_debugLightIndex = -1;
  }
}
bool DebugSceneobject::hasDebugLight() const {
  return (m_debugLightIndex >= 0) && getScene().isLightDefined(m_debugLightIndex);
}

void DebugSceneobject::adjustDebugLightDir() {
  if(!hasDebugLight()) return;
  const D3World    &cw       = dbgCAM()->getD3World();
  const D3DXVECTOR3 dir      = getCubeCenter() - cw.getPos();
  const D3DXVECTOR3 up       = cw.getUp();
  const D3DXVECTOR3 lightDir = rotate(dir, up, D3DX_PI / 4);
  getScene().setLightDirection(m_debugLightIndex, lightDir);
}

bool DebugSceneobject::hasCubeCenter() const {
  return isSet(OCTA_VISIBLE);
}

D3DXVECTOR3 DebugSceneobject::getCubeCenter() const {
  return hasCubeCenter() ? m_octaObject->getCenter() : D3DXORIGIN;
}

void DebugSceneobject::updateCamDistance() {
  if(hasCubeCenter()) {
    m_currentCamDistance = length(dbgCAM()->getPos() - getCubeCenter());
  }
}

D3DXVECTOR3 DebugSceneobject::getCubeCamVector() const {
  const DebugIsoSurface &surf = m_debugger.getDebugSurface();
  if(surf.hasFaceNormal()) {
    return surf.getCurrentFaceNormal();
  } else {
    const D3World  &w = dbgCAM()->getD3World();
    return -w.getDir();
  }
}

void DebugSceneobject::handleDebuggerPaused() {
  if(hasCubeCenter()) {
    const D3DXVECTOR3 cc  = getCubeCenter();
    D3Camera         *cam = dbgCAM();
    D3World           w   = cam->getD3World();
    cam->setD3World(w.setPos(cc + m_currentCamDistance * getCubeCamVector())).setLookAt(cc);
    adjustDebugLightDir();
  }
}

void DebugSceneobject::resetCameraFocus(bool resetViewAngleAndDistance) {
  D3Camera *cam = dbgCAM();
  if(resetViewAngleAndDistance) {
    m_currentCamDistance = 0.25;
    cam->setViewAngle(0.2864f);
  }
  if(hasCubeCenter()) {
    handleDebuggerPaused();
  } else {
    D3World w = cam->getD3World();
    cam->setD3World(w.resetOrientation());
  }
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

#endif // ISODEBUGGER
