#include "stdafx.h"

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
}

DebugSceneobject::~DebugSceneobject() {
  deleteMeshObject();
  deleteOctaObject();
  deleteTetraObject();
  deleteFacesObject();
  deleteVertexObject();
}

void DebugSceneobject::initOctaTetraVertex(D3SceneObjectVisual *octaObject
                                          ,D3SceneObjectVisual *tetraObject
                                          ,D3SceneObjectVisual *vertexObject)
{
  m_octaObject   = octaObject;
  m_tetraObject  = tetraObject;
  m_vertexObject = vertexObject;
}

void DebugSceneobject::setMeshObject(D3SceneObjectVisual *obj) {
  D3DXMATRIX oldWorld;
  const bool oldExist = m_meshObject != NULL;
  if(oldExist) oldWorld = m_meshObject->getWorld();
  deleteMeshObject();
  m_meshObject = obj;
  if(oldExist && m_meshObject) {
    m_meshObject->getWorld() = oldWorld;
  }
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

#endif // DEBUG_POLYGONIZER
