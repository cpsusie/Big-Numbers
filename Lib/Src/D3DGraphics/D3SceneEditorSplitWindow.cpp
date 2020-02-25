#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneEditor.h>

void D3SceneEditor::OnSplitWindow(bool vertical) {
  const WindowPair wp = m_sceneContainer->createNew3DWindow(getCurrentCamera()->getHwnd(), vertical);
  if(!wp.isEmpty()) {
    getCurrentCamera()->setHwnd(wp.getWindow(0));
    getScene().addCamera(wp.getWindow(1), getCurrentCamera());
  }
}
