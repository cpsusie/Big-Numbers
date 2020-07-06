#include "pch.h"
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneContainer.h>

void D3SceneContainer::incrLevel() {
  m_lock.wait();
  m_renderLevel++;
  debugLog(_T("thr:%5d, incr renderLevel:%u\n"),GetCurrentThreadId(), m_renderLevel);
  m_lock.notify();
}

void D3SceneContainer::decrLevel() {
  m_lock.wait();
  try {
    m_renderLevel--;
    debugLog(_T("thr:%5d, decr renderLevel:%u\n"),GetCurrentThreadId(), m_renderLevel);
    if(m_renderLevel == 0) {
      renderAccumulatedAndReset();
    }
    m_lock.notify();
  } catch(...) {
    m_lock.notify();
    throw;
  }
}

void D3SceneContainer::render(BYTE renderFlags, CameraSet cameraSet) {
  m_lock.wait();
  try {
    if(renderFlags & SC_RENDERNOW) {
      doRender(renderFlags, cameraSet);
    } else {
      m_accRenderFlags |= renderFlags;
      m_accCameraSet   |= cameraSet;
      if(m_renderLevel == 0) {
        renderAccumulatedAndReset();
      }
    }
    m_lock.notify();
  } catch(...) {
    m_lock.notify();
    throw;
  }
}

void D3SceneContainer::renderAccumulatedAndReset() {
  if(m_accRenderFlags || !m_accCameraSet.isEmpty()) {
    doRender(m_accRenderFlags, m_accCameraSet);
    m_accRenderFlags = 0;
    m_accCameraSet.clear();
  }
}

void D3SceneContainer::doRender(BYTE renderFlags, CameraSet cameraSet) {
  logRenderCall(renderFlags, cameraSet);
  if(renderFlags & SC_RENDER3D) {
    getScene().render(cameraSet);
  }
}

static String flagsToStr(BYTE flags) {
  const TCHAR *delim = NULL;
  String result;
#define ADDFLAG(f) if(flags & (SC_##f)) { if(delim) result += delim; else delim = _T(" "); result += _T(#f); }
  ADDFLAG(RENDER3D      )
  ADDFLAG(RENDERINFO    )
  ADDFLAG(RENDERNOW     )
  ADDFLAG(RENDERUSERMASK)
  return result;
#undef ADDFLAG
}

void D3SceneContainer::logRenderCall(BYTE flags, CameraSet cameraSet) {
  debugLog(_T("thr:%5d, callrender level:%u, flags:%-25s,camset:%s\n")
          ,GetCurrentThreadId()
          ,m_renderLevel, flagsToStr(flags).cstr(), cameraSet.toString().cstr()
          );
}
