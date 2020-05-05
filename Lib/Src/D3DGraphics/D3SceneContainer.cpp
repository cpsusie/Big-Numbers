#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneContainer.h>

void D3SceneContainer::decrLevel() {
  if(--m_renderLevel == 0) {
    if(m_accRenderFlags || !m_accCameraSet.isEmpty()) {
      render(m_accRenderFlags, m_accCameraSet);
    }
  }
}

void D3SceneContainer::render(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDERNOW) {
    doRender(renderFlags, cameraSet);
    return;
  }
  if(m_renderLevel > 0) {
    m_accRenderFlags |= renderFlags;
    m_accCameraSet   |= cameraSet;
  } else {
    doRender(renderFlags, cameraSet);
    m_accRenderFlags = 0;
    m_accCameraSet.clear();
  }
}

void D3SceneContainer::doRender(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDER3D) {
    getScene().render(cameraSet);
  }
}
