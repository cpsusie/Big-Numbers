#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3PickedInfo.h>
#include <D3DGraphics/D3SceneObjectVisual.h>

D3Camera *D3Scene::getPickedCamera(const CPoint &p) const {
  const int index = m_cameraArray.findCameraIndex(p);
  return (index < 0) ? nullptr : m_cameraArray[index];
}

D3SceneObjectVisual *D3Scene::getPickedVisual(const CPoint &p, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  const D3Camera *camera = getPickedCamera(p);
  if(camera == nullptr) {
    return nullptr;
  }
  return camera->getPickedVisual(camera->screenToWin(p), mask, hitPoint, ray, dist, info);
}

D3SceneObjectVisual *D3Scene::getPickedVisual(const D3Camera &camera, const D3Ray &ray, long mask, D3DXVECTOR3 *hitPoint, float *dist, D3PickedInfo *info) const {
  float                minDistance   = -1;
  D3SceneObjectVisual *closestObject = nullptr;
  D3PickedInfo         closestInfo, tmpInfo, *pinfo = info ? &tmpInfo : nullptr;

  getDevice().setCurrentCamera(&camera);
  for(Iterator<D3SceneObjectVisual*> it = getVisualIterator(mask); it.hasNext();) {
    D3SceneObjectVisual *obj = it.next();
    if(!obj->isVisible()) {
      continue;
    }
    float distance;
    if(obj->intersectsWithRay(ray, distance, pinfo)) {
      if((closestObject == nullptr) || (distance < minDistance)) {
        closestObject = obj;
        minDistance   = distance;
        if(pinfo) {
          closestInfo   = tmpInfo;
        }
      }
    }
  }
  if(closestObject) {
    if(hitPoint) {
      *hitPoint = ray.getHitPoint(minDistance);
    }
    if(dist) {
      *dist = minDistance;
    }
    if(info) {
      *info = closestInfo;
    }
  }
  getDevice().setCurrentCamera(nullptr);
  return closestObject;
}
