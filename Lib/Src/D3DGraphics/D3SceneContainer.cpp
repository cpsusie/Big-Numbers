#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneContainer.h>

void D3SceneContainer::decrLevel() {
  if(--m_renderLevel == 0) {
    if(m_accumulatedRenderFlags || !m_accumulatedCameraSet.isEmpty()) {
      render(m_accumulatedRenderFlags, m_accumulatedCameraSet);
    }
  }
}

void D3SceneContainer::render(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDERNOW) {
    doRender(renderFlags, cameraSet);
    return;
  }
  if(m_renderLevel > 0) {
    m_accumulatedRenderFlags |= renderFlags;
    m_accumulatedCameraSet   |= cameraSet;
  } else {
    doRender(renderFlags, cameraSet);
    m_accumulatedRenderFlags = 0;
    m_accumulatedCameraSet.clear();
  }
}

void D3SceneContainer::doRender(BYTE renderFlags, CameraSet cameraSet) {
  if(renderFlags & SC_RENDER3D) {
    getScene().render(cameraSet);
  }
}
