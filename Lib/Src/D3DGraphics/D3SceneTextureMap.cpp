#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Texture.h>

UINT D3Scene::addTexture(LPDIRECT3DTEXTURE texture) {
  const UINT oldCount = getTextureCount();
  const UINT id       = getFirstFreeTextureId();
  D3Texture t(id);
  t = texture;
  m_textureMap.put(id, t);
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_TEXTURECOUNT, &oldCount, &newCount);
  return id;
}

void D3Scene::setAllTextures(const TextureMap &textureMap) {
  const UINT oldCount = getTextureCount();
  m_textureMap = textureMap;
  const UINT newCount = oldCount+1;
  notifyPropertyChanged(SP_TEXTURECOUNT, &oldCount, &newCount);
}

UINT D3Scene::getFirstFreeTextureId() const {
  for(UINT id = 0;; id++) {
    if(m_textureMap.get(id) == nullptr) {
      return id;
    }
  }
}

void D3Scene::removeTexture(UINT textureId) {
  if(!isTextureDefined(textureId)) {
    return;
  }
  const UINT oldCount = getTextureCount();
  m_textureMap.remove(textureId);
  const UINT newCount = getTextureCount();
  notifyPropertyChanged(SP_TEXTURECOUNT, &oldCount, &newCount);
}

void D3Scene::removeAllTextures() {
  const UINT oldCount = getTextureCount();
  m_textureMap.clear();
  const UINT newCount = getTextureCount();
  notifyPropertyChanged(SP_TEXTURECOUNT, &oldCount, &newCount);

}

void D3Scene::setTexture(const D3Texture &t) {
  if(!t.isDefined()) {
    addTexture(t.getImage());
  } else {
    const UINT id = t.getId();
    if(!isTextureDefined(id)) {
      throwInvalidArgumentException(__TFUNCTION__, _T("id=%u, Texture undefined"), id);
    }
    D3Texture *d3t = m_textureMap.get(id);
    setProperty(SP_TEXTUREIMAGE, *d3t, t);
  }
}
