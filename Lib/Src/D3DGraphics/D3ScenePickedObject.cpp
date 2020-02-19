#include "pch.h"
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObject.h>

D3Camera *D3Scene::getPickedCamera(const CPoint &p) const {
  const int index = m_cameraArray.findCameraIndex(p);
  return (index < 0) ? NULL : m_cameraArray[index];
}

D3SceneObject *D3Scene::getPickedObject(const CPoint &p, long mask, D3DXVECTOR3 *hitPoint, D3Ray *ray, float *dist, D3PickedInfo *info) const {
  const D3Camera *camera = getPickedCamera(p);
  if(camera == NULL) {
    return NULL;
  }
  return camera->getPickedObject(camera->screenToWin(p), mask, hitPoint, ray, dist, info);
}

D3SceneObject *D3Scene::getPickedObject(const D3Ray &ray, long mask, D3DXVECTOR3 *hitPoint, float *dist, D3PickedInfo *info) const {
  float          minDistance = -1;
  D3SceneObject *closestObject = NULL;
  for(Iterator<D3SceneObject*> it = getObjectIterator(mask); it.hasNext();) {
    D3SceneObject *obj = it.next();
    if(!obj->isVisible()) {
      continue;
    }
    float distance;
    if(obj->intersectsWithRay(ray, distance, info)) {
      if((closestObject == NULL) || (distance < minDistance)) {
        closestObject = obj;
        minDistance = distance;
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
  }
  return closestObject;
}
