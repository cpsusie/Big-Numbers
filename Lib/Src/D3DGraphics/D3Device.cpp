#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3Light.h>

D3Device::D3Device(HWND hwnd) {
  m_device          = DirectXDeviceFactory::createDevice(hwnd);
  getValuesFromDevice(m_device).setDefault().setValuesToDevice(m_device);
  V(m_device->GetDeviceCaps(&m_deviceCaps));
  m_currentCamera = NULL;
}

D3Device::~D3Device() {
  SAFERELEASE(m_device);
}

void D3Device::beginRender(const D3Camera &camera) {
  m_renderLock.wait();
  try {
    setCurrentCamera(&camera);
    V(m_device->Clear(0
                      ,NULL
                      ,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
                      ,camera.getBackgroundColor()
                      ,1.0f
                      ,0
                      ));
    setViewMatrix(camera.getViewMatrix()).setProjMatrix(camera.getProjMatrix());
    V(m_device->BeginScene());
  } catch(...) {
    setCurrentCamera(NULL).m_renderLock.notify();
    throw;
  }
}

// call device->EndScene()M Present(NULL,NULL,currentCamera.getHwnd(), and setCurrentCamera(NULL);
void D3Device::endRender() {
  try {
    V(m_device->EndScene());
    V(m_device->Present(NULL, NULL, m_currentCamera->getHwnd(), NULL));
    setCurrentCamera(NULL).m_renderLock.notify();
  } catch(...) {
    setCurrentCamera(NULL).m_renderLock.notify();
    throw;
  }
}

D3Device &D3Device::setMaterial(const D3Material &material) {
  if(material != m_material) {
    V(m_device->SetMaterial(&material));
    if(material.getOpacity() < 1.0f) {
       setCullMode(D3DCULL_CCW)
      .setZEnable(D3DZB_FALSE)
      .setAlphaBlendEnable(true)
      .setSrcBlend(D3DBLEND_SRCALPHA)
      .setDstBlend(D3DBLEND_INVSRCALPHA);
    } else {
       setCullMode(D3DCULL_CCW)
      .setZEnable(D3DZB_TRUE)
      .setAlphaBlendEnable(false);
    }
    m_material = material;
  }
  return *this;
}

D3Device &D3Device::setLight(const D3Light &light) {
  if(light.isDefined()) {
    const UINT index = light.getIndex();
    setLight(index, light).enableLight(index,light.isEnabled());
  } else {
    enableLight(light.getIndex(), false);
  }
  return *this;
}

D3DLIGHT D3Device::getLight(UINT lightIndex) const {
  D3DLIGHT lp;
  V(m_device->GetLight(lightIndex, &lp));
  return lp;
}
