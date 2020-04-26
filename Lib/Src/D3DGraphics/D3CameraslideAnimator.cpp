#include "pch.h"
#include <Math/SigmoidIterator.h>
#include <LinearIterator.h>
#include <D3DGraphics/D3Camera.h>

#define FL_UPDATEPOS           0x01
#define FL_UPDATESCALE         0x02
#define FL_UPDATEORIENTATION   0x04
#define FL_UPDATEWORLD         (FL_UPDATEPOS | FL_UPDATESCALE | FL_UPDATEORIENTATION)
#define FL_UPDATEVIEWANGLE     0x08
#define FL_UPDATENEARVIEWPLANE 0x10
#define FL_UPDATEBACKGROUND    0x20

D3CameraSlideAnimator::D3CameraSlideAnimator(D3Camera &cam, const D3World &newWorld, float newViewAngle, float newNearViewPlane)
: m_cam(cam)
, m_oldWorld(cam.getD3World())              , m_newWorld(    newWorld)
, m_oldVewAngle(cam.getViewAngle())         , m_newViewAngle(newViewAngle)
, m_oldNearViewPlane(cam.getNearViewPlane()), m_newNearViewPlane(newNearViewPlane)
, m_oldBackground(cam.getBackgroundColor())
, m_iterator(NULL)
{
  if(m_newWorld.getPos()         != m_oldWorld.getPos()  )       setFlag(FL_UPDATEPOS        );
  if(m_newWorld.getScale()       != m_oldWorld.getScale())       setFlag(FL_UPDATESCALE      );
  if(m_newWorld.getOrientation() != m_oldWorld.getOrientation()) setFlag(FL_UPDATEORIENTATION);
  if(m_newViewAngle != 0)      setFlag(FL_UPDATEVIEWANGLE    );
  if(m_newNearViewPlane != 0)  setFlag(FL_UPDATENEARVIEWPLANE);
}

template<typename T> T interpolate(const T &v1, const T &v2, float t) {
  return t*v2 + (1.0f - t)*v1;
}

D3CameraSlideAnimator::~D3CameraSlideAnimator() {
  SAFEDELETE(m_iterator);
}

D3CameraSlideAnimator &D3CameraSlideAnimator::changeToBackground(const D3DCOLOR newBackgroundColor) {
  m_newBackground = newBackgroundColor;
  setFlag(FL_UPDATEBACKGROUND);
  return *this;
}

D3CameraSlideAnimator &D3CameraSlideAnimator::animate(UINT msec, UINT steps, CameraSlideType type) {
  switch(type) {
  case CAMSLIDE_LINEAR : m_iterator = new LinearIterator( 0, 1, steps); TRACE_NEW(m_iterator); break;
  case CAMSLIDE_SIGMOID: m_iterator = new SigmoidIterator(0, 1, steps); TRACE_NEW(m_iterator); break;
  }
  const double sleepTime = (double)msec / (double)steps;
  try {
    m_cam.beginAnimate();
    while(m_iterator->hasNext()) {
      const float t = (float)m_iterator->next();
      if((t == 0) || (t == 1)) {
        continue;
      }
      if(sleepTime > 50) {
        Sleep((UINT)sleepTime);
      }
      if(isSet(FL_UPDATEWORLD)) {
        D3World w(m_oldWorld);
        if(isSet(FL_UPDATEPOS)) w.setPos(interpolate(w.getPos(), m_newWorld.getPos(), t));
        if(isSet(FL_UPDATESCALE)) w.setScale(interpolate(w.getScale(), m_newWorld.getScale(), t));
        if(isSet(FL_UPDATEORIENTATION)) w.setOrientation(interpolate(w.getOrientation(), m_newWorld.getOrientation(), t));
        m_cam.setD3World(w);
      }
      if(isSet(FL_UPDATEVIEWANGLE)) {
        m_cam.setViewAngle(interpolate(m_oldVewAngle, m_newViewAngle, t));
      }
      if(isSet(FL_UPDATENEARVIEWPLANE)) {
        m_cam.setNearViewPlane(interpolate(m_oldNearViewPlane, m_newNearViewPlane, t));
      }
      if(isSet(FL_UPDATEBACKGROUND)) {
        const RGBColor bldColor = RGBColor::blendColor(m_oldBackground, m_newBackground, t);
        m_cam.setBackgroundColor(bldColor);
      }
      m_cam.doRender(); // bypass sceneContainers render system and render camera directly
      m_cam.endAnimate();
    }
  } catch(...) {
    m_cam.endAnimate();
    throw;
  }
  return *this;
}
