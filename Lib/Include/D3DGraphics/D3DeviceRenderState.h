#pragma once

#include <MFCUtil/D3DeviceFactory.h>

class D3DeviceRenderState {
public:
  D3DFILLMODE    m_fillMode;
  D3DSHADEMODE   m_shadeMode;
  D3DCOLOR       m_ambientColor;
  DWORD          m_fvf;
  D3DCULL        m_cullMode;
  D3DBLEND       m_srcBlend;
  D3DBLEND       m_dstBlend;
  D3DZBUFFERTYPE m_zEnable;
  bool           m_normalizeNormals : 1;
  bool           m_alphaBlendEnable : 1;
  bool           m_lighting : 1;
  bool           m_specularHighLightEnable : 1;

  D3DeviceRenderState() {
    setDefault();
  }
  D3DeviceRenderState &getValuesFromDevice(LPDIRECT3DDEVICE device);
  D3DeviceRenderState &setValuesToDevice(  LPDIRECT3DDEVICE device);
  D3DeviceRenderState &setDefault();
};
