#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneEditor.h>

void D3SceneEditor::OnSplit3DWindow(bool vertical) {
  const WindowPair wp = m_sceneContainer->split3DWindow(getCurrentCamera()->getHwnd(), vertical);
  if(!wp.isEmpty()) {
    getCurrentCamera()->setHwnd(wp.getWindow(0));
    getScene().addCamera(wp.getWindow(1), getCurrentCamera());
    const D3CameraArray &cams = getScene().getCameraArray();
    CameraSet camSet;
    const int index1 = cams.findCameraIndex(wp.getWindow(0));
    const int index2 = cams.findCameraIndex(wp.getWindow(1));
    camSet.add(index1);
    camSet.add(index2);
    render(SE_RENDERALL, camSet);
  }
}

void D3SceneEditor::OnDelete3DWindow() {
  D3Camera *cam = getCurrentCamera();
  if(cam == NULL) {
    return;
  }
  if(m_sceneContainer->delete3DWindow(cam->getHwnd())) {
    selectCamera(-1);
    getScene().removeCamera(*cam);
    renderVisible(SE_RENDERALL);
  }
}
