#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3Scene.h>

UINT D3Scene::addMaterial(const D3MATERIAL &material) {
  const UINT oldCount = getMaterialCount();
  const UINT id       = getFirstFreeMaterialId();
  D3Material  m(id);
  m = material;
  m_materialMap.put(id, m);
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
  return id;
}

void D3Scene::setAllMaterials(const MaterialMap &materialMap) {
  const UINT oldCount = getMaterialCount();
  m_materialMap = materialMap;
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

UINT D3Scene::addMaterialWithColor(D3DCOLOR color, bool specularHighlights) {
  return addMaterial(D3Material::createMaterialWithColor(color, specularHighlights));
}

UINT D3Scene::getFirstFreeMaterialId() const {
  for(UINT id = 0;; id++) {
    if(m_materialMap.get(id) == NULL) {
      return id;
    }
  }
}

void D3Scene::removeMaterial(UINT materialId) {
  if((materialId == 0) || (!isMaterialDefined(materialId))) { // cannot remove material 0
    return;
  }
  const UINT oldCount = getMaterialCount();
  m_materialMap.remove(materialId);
  const UINT newCount = getMaterialCount();
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

void D3Scene::removeAllMaterials() {
  const UINT oldCount = getMaterialCount();
  m_materialMap.clear();
  const UINT newCount = getMaterialCount();
  notifyPropertyChanged(SP_MATERIALCOUNT, &oldCount, &newCount);
}

void D3Scene::setMaterial(const D3Material &material) {
  if(!material.isDefined()) {
    addMaterial(material);
  } else {
    const UINT id = material.getId();
    if(!isMaterialDefined(id)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("id=%u, material undefined"), id);
    }
    D3Material *m = m_materialMap.get(id);
    setProperty(SP_MATERIALPARAMETERS, *m, material);
  }
}

void D3Scene::setLightControlMaterial(const D3Material &lcMaterial) {
  assert(lcMaterial.isDefined());
  D3Material *m = m_materialMap.get(lcMaterial.getId());
  *m = lcMaterial;
}

String D3Scene::getMaterialString(UINT materialId) const {
  return getMaterial(materialId).toString();
}

static int materialIdCmp(const D3Material &m1, const D3Material &m2) {
  return m1.getId() - m2.getId();
}

String D3Scene::getMaterialString() const {
  Array<D3Material> matArray(getMaterialCount());
  for(ConstIterator<Entry<CompactUIntKeyType, D3Material> > it = m_materialMap.getIterator(); it.hasNext();) {
    const Entry<CompactUIntKeyType, D3Material> &e = it.next();
    matArray.add(e.getValue());
  }
  return matArray.sort(materialIdCmp).toString(_T("\n"));
}
