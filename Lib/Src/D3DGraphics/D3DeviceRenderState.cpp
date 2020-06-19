#include "pch.h"
#include <D3DGraphics/D3DeviceRenderState.h>

template<typename T> T getRenderState(LPDIRECT3DDEVICE device, D3DRENDERSTATETYPE id) {
  DWORD value;
  V(device->GetRenderState(id, &value));
  return (T)value;
}

static inline bool getBoolProperty(LPDIRECT3DDEVICE device, D3DRENDERSTATETYPE id) {
  return getRenderState<BOOL>(device, id) ? true : false;
}

template<typename T> void setRenderState(LPDIRECT3DDEVICE device, D3DRENDERSTATETYPE id, T value) {
  V(device->SetRenderState(id, (DWORD)value));
}

void setBoolProperty(LPDIRECT3DDEVICE device, D3DRENDERSTATETYPE id, bool value) {
  setRenderState(device, id, value ? TRUE : FALSE);
}

D3DeviceRenderState &D3DeviceRenderState::getValuesFromDevice(LPDIRECT3DDEVICE device) {
  m_fillMode                 = getRenderState<D3DFILLMODE>(   device, D3DRS_FILLMODE );
  m_shadeMode                = getRenderState<D3DSHADEMODE>(  device, D3DRS_SHADEMODE);
  m_ambientColor             = getRenderState<D3DCOLOR>(      device, D3DRS_AMBIENT  );
  V(device->GetFVF(&m_fvf));
  m_cullMode                 = getRenderState<D3DCULL>(       device, D3DRS_CULLMODE );
  m_srcBlend                 = getRenderState<D3DBLEND>(      device, D3DRS_SRCBLEND );
  m_dstBlend                 = getRenderState<D3DBLEND>(      device, D3DRS_DESTBLEND);
  m_zEnable                  = getRenderState<D3DZBUFFERTYPE>(device, D3DRS_ZENABLE  );
  m_normalizeNormals         = getBoolProperty(device, D3DRS_NORMALIZENORMALS);
  m_alphaBlendEnable         = getBoolProperty(device, D3DRS_ALPHABLENDENABLE);
  m_lighting                 = getBoolProperty(device, D3DRS_LIGHTING        );
  m_specularHighlights       = getBoolProperty(device, D3DRS_SPECULARENABLE  );
  return *this;
}

D3DeviceRenderState &D3DeviceRenderState::setValuesToDevice(LPDIRECT3DDEVICE device) {
  setRenderState( device, D3DRS_FILLMODE         , m_fillMode               );
  setRenderState( device, D3DRS_SHADEMODE        , m_shadeMode              );
  setRenderState( device, D3DRS_AMBIENT          , m_ambientColor           );
  V(device->SetFVF(m_fvf));
  setRenderState( device, D3DRS_CULLMODE         , m_cullMode               );
  setRenderState( device, D3DRS_SRCBLEND         , m_srcBlend               );
  setRenderState( device, D3DRS_DESTBLEND        , m_dstBlend               );
  setRenderState( device, D3DRS_ZENABLE          , m_zEnable                );
  setBoolProperty(device, D3DRS_NORMALIZENORMALS , m_normalizeNormals       );
  setBoolProperty(device, D3DRS_ALPHABLENDENABLE , m_alphaBlendEnable       );
  setBoolProperty(device, D3DRS_LIGHTING         , m_lighting               );
  setBoolProperty(device, D3DRS_SPECULARENABLE   , m_specularHighlights     );
  return *this;
}

D3DeviceRenderState &D3DeviceRenderState::setDefault() {
  m_fillMode           = D3DFILL_SOLID;
  m_shadeMode          = D3DSHADE_GOURAUD;
  m_ambientColor       = D3DCOLOR_XRGB(50, 50, 50);
  m_cullMode           = D3DCULL_CCW;
  m_srcBlend           = D3DBLEND_ONE;
  m_dstBlend           = D3DBLEND_ZERO;
  m_zEnable            = D3DZB_TRUE;
  m_normalizeNormals   = true;
  m_alphaBlendEnable   = false;
  m_lighting           = true;
  m_specularHighlights = true;
  return *this;
}
