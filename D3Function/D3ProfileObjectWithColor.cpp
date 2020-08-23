#include "stdafx.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3SceneObjectVisualNormals.h>
#include <D3DGraphics/resource.h>
#include "ProfileDlg.h"
#include "D3ProfileObjectWithColor.h"

D3ProfileObjectWithColor::D3ProfileObjectWithColor(CProfileDlg *dlg)
: m_dlg(dlg)
, D3SceneObjectWithMesh(dlg->getScene()
                       ,::rotateProfile(dlg->getScene().getDevice()
                                       ,dlg->getProfile()
                                       ,dlg->getRotateParameters()
                                       ,dlg->isDoubleSided()))
{
  const ProfileRotationParameters param = dlg->getRotateParameters();
  if(param.isSet(PRROT_USECOLOR)) {
    m_materialId = m_dlg->getScene().addMaterialWithColor(param.m_color);
  } else {
    m_materialId = m_dlg->getScene().addMaterial(D3Material().setDefault());
  }
  m_normalsObj = new D3SceneObjectVisualNormals(this); TRACE_NEW(m_normalsObj);
}

D3ProfileObjectWithColor::~D3ProfileObjectWithColor() {
  SAFEDELETE(m_normalsObj);
  getScene().removeMaterial(m_materialId);
}

void ProfileObjectProperties::resetProperties() {
  memset(this, 0, sizeof(ProfileObjectProperties));
}

D3SceneEditor &D3ProfileObjectWithColor::getD3SceneEditor() const {
  return m_dlg->getD3SceneEditor();
}

void D3ProfileObjectWithColor::getAllProperties(ProfileObjectProperties &prop) const {
  prop.m_world     = getWorld();
  prop.m_fillMode  = getFillMode();
  prop.m_shadeMode = getShadeMode();
  prop.m_material  = getMaterial();

  const D3SceneObjectPoint cor = getD3SceneEditor().getCenterOfRotation();
  if(cor.m_obj == this) {
    prop.m_centerOfRotation = cor;
  } else {
    prop.m_centerOfRotation.reset();
  }
  D3Scene &s = getScene();
}

void D3ProfileObjectWithColor::setAllProperties(const ProfileObjectProperties &prop) {
  getWorld() = prop.m_world;
  setFillMode( prop.m_fillMode);
  setShadeMode(prop.m_shadeMode);
  D3Scene &s = getScene();
  D3Material m(getMaterialId());
  m = prop.m_material;
  s.setMaterial(m);
  D3SceneObjectPoint cor = prop.m_centerOfRotation;
  if(!cor.isEmpty()) {
    cor.m_obj = this;
  }
  getD3SceneEditor().getCenterOfRotation() = cor;
}

void D3ProfileObjectWithColor::modifyContextMenu(HMENU menu) {
  removeMenuItem(menu, ID_OBJECT_REMOVE);
}

void D3ProfileObjectWithColor::draw() {
  __super::draw();
  if(m_dlg->getNormalsMode() != NORMALS_INVISIBLE) {
    m_normalsObj->draw();
  }
}
