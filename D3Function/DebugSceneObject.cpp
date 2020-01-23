#include "stdafx.h"

#ifdef DEBUG_POLYGONIZER

#include "Debugger.h"
#include "DebugIsoSurface.h"
// --------------------------------------------- DebugSceneobject -------------------------------------

DebugSceneobject::~DebugSceneobject() {
  deleteMeshObject();
  deleteOctaObject();
  deleteTetraObject();
  deleteFacesObject();
  deleteVertexObject();
  deleteVisibleVertexArrayObject();
}

void DebugSceneobject::setMeshObject(D3SceneObject *obj) {
  deleteMeshObject();
  m_meshObject = obj;
}

void DebugSceneobject::setOctaObject(D3SceneObject *obj) {
  deleteOctaObject();
  m_octaObject = obj;
}

void DebugSceneobject::setTetraObject(D3SceneObject *obj) {
  deleteTetraObject();
  m_tetraObject = obj;
}

void DebugSceneobject::setFacesObject(D3SceneObject *obj) {
  deleteFacesObject();
  m_facesObject = obj;
}

void DebugSceneobject::setVertexObject(D3SceneObject *obj) {
  deleteVertexObject();
  m_vertexObject = obj;
}

void DebugSceneobject::setVisibleVertexArrayObject(D3SceneObject *obj) {
  deleteVisibleVertexArrayObject();
  m_visibleVertexArrayObject = obj;
}

void DebugSceneobject::draw() {
  if((m_visibleParts & MESH_VISIBLE   ) && m_meshObject              ) m_meshObject->draw();
  if((m_visibleParts & OCTA_VISIBLE   ) && m_octaObject              ) m_octaObject->draw();
  if((m_visibleParts & TETRA_VISIBLE  ) && m_tetraObject             ) m_tetraObject->draw();
  if((m_visibleParts & FACES_VISIBLE  ) && m_facesObject             ) m_facesObject->draw();
  if((m_visibleParts & VERTEX_VISIBLE ) && m_vertexObject            ) m_vertexObject->draw();
  if((m_visibleParts & VERTEX_VISIBLE)  && m_visibleVertexArrayObject) m_visibleVertexArrayObject->draw();
}

void DebugSceneobject::deleteMeshObject() {
  if(m_meshObject) {
    m_fillMode = m_meshObject->getFillMode();
    m_shadeMode = m_meshObject->getShadeMode();
    SAFEDELETE(m_meshObject);
  }
}

void DebugSceneobject::deleteOctaObject()               { SAFEDELETE(m_octaObject              ); }
void DebugSceneobject::deleteTetraObject()              { SAFEDELETE(m_tetraObject             ); }
void DebugSceneobject::deleteFacesObject()              { SAFEDELETE(m_facesObject             ); }
void DebugSceneobject::deleteVertexObject()             { SAFEDELETE(m_vertexObject            ); }
void DebugSceneobject::deleteVisibleVertexArrayObject() { SAFEDELETE(m_visibleVertexArrayObject); }

#endif // DEBUG_POLYGONIZER
