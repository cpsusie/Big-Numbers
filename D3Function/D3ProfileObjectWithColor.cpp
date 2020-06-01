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

void D3ProfileObjectWithColor::modifyContextMenu(HMENU menu) {
  removeMenuItem(menu, ID_OBJECT_REMOVE);
}

void D3ProfileObjectWithColor::draw() {
  __super::draw();
  if(m_dlg->getNormalsMode() != NORMALS_INVISIBLE) {
    m_normalsObj->draw();
  }
}
