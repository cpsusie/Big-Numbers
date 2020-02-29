#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3SceneEditor.h>

void D3SceneEditor::OnCameraSplit(bool vertical) {
  D3Camera &cam = *getSelectedCAM();
  const WindowPair wp = m_sceneContainer->split3DWindow(cam.getHwnd(), vertical);
  if(!wp.isEmpty()) {
    cam.setHwnd(wp.getWindow(0));
    getScene().addCamera(wp.getWindow(1), &cam);
    const D3CameraArray &allCams = getScene().getCameraArray();
    CameraSet camSet;
    const int index1 = allCams.findCameraIndex(wp.getWindow(0));
    const int index2 = allCams.findCameraIndex(wp.getWindow(1));
    camSet.add(index1);
    camSet.add(index2);
    render(SE_RENDERALL, camSet);
  }
}

void D3SceneEditor::OnCameraRemove() {
  D3Camera *cam = getSelectedCAM();
  if(cam && m_sceneContainer->delete3DWindow(cam->getHwnd())) {
    selectCAM(-1);
    getScene().removeCamera(*cam);
    renderActive(SE_RENDERALL);
  }
}
