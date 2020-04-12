#pragma once

#include <PropertyContainer.h>
#include <MFCUtil/ColorSpace.h>
#include "D3DeviceRenderState.h"
#include "D3Material.h"

class D3Light;
class D3Camera;

class D3Device : public D3DeviceRenderState {
private:
  HWND             m_hwnd;
  LPDIRECT3DDEVICE m_device;
  D3DCAPS          m_deviceCaps;
  D3Material       m_material;
  // only set in scene.render
  const D3Camera  *m_currentCamera;
  template<typename T> void setDevRenderState(D3DRENDERSTATETYPE id, T value) {
    V(m_device->SetRenderState(id, (DWORD)value));
  }
  D3Device(const D3Device &src);            // not implemented
  D3Device &operator=(const D3Device &src); // not implemented
public:
  D3Device(HWND hwnd);
  ~D3Device();

  inline operator LPDIRECT3DDEVICE() {
    return m_device;
  }
  inline HWND getHwnd() const {
    return m_hwnd;
  }
  inline const D3Camera *getCurrentCamera() const {
    return m_currentCamera;
  }
  D3Device &setCurrentCamera(const D3Camera *camera) {
    m_currentCamera = camera;
    return *this;
  }

  // call device.Clear(camera.backgroundColor),setup view- and projMatrix corresponding to camera, device->BeginScene()
  // and setCurrentCamera(&camera);
  void beginRender(const D3Camera &camera);
  // call device->EndScene()M Present(NULL,NULL,currentCamera.getHwnd(), and setCurrentCamera(NULL);
  void endRender();
  const D3DCAPS &getDeviceCaps() const {
    return m_deviceCaps;
  }

  inline UINT getTextureCoordCount() const {
    return m_deviceCaps.FVFCaps & D3DFVFCAPS_TEXCOORDCOUNTMASK;
  }
  inline UINT getMaxLightCount() const {
    return m_deviceCaps.MaxActiveLights;
  }

  // Set device fill-mode (D3DRS_FILLMODE), and update m_renderState.m_fillMode, if different from current
  inline D3Device &setFillMode(D3DFILLMODE fillMode) {
    if(fillMode != getFillMode()) {
      setDevRenderState(D3DRS_FILLMODE, fillMode);
      m_fillMode = fillMode;
    }
    return *this;
  }
  inline D3DFILLMODE getFillMode() const {
    return m_fillMode;
  }
  // Set device' shade-mode (D3DRS_SHADEMODE), and update m_renderState.m_shademode, if different from current
  inline D3Device &setShadeMode(D3DSHADEMODE shadeMode) {
    if(shadeMode != getShadeMode()) {
      setDevRenderState(D3DRS_SHADEMODE, shadeMode);
      m_shadeMode = shadeMode;
    }
    return *this;
  }
  inline D3DSHADEMODE getShadeMode() const {
    return m_shadeMode;
  }
  // Set device' ambient light color (D3DRS_AMBIENT), and update m_renderState.m_ambientColor, if different from current
  inline D3Device &setAmbientColor(D3DCOLOR ambientColor) {
    if(ambientColor != getAmbientColor()) {
      setDevRenderState(D3DRS_AMBIENT, ambientColor);
      m_ambientColor = ambientColor;
    }
    return *this;
  }
  inline D3DCOLOR getAmbientColor() const {
    return m_ambientColor;
  }
  // call device->SetFVF and update m_renderState.m_fvf if different from current
  inline D3Device &setFVF(DWORD fvf) {
    if(fvf != getFVF()) {
      V(m_device->SetFVF(fvf));
      m_fvf = fvf;
    }
    return *this;
  }
  inline DWORD getFVF() const {
    return m_fvf;
  }
  // set device cullmode (D3DRS_CULLMODE) and and update m_renderState.m_cullmode if different from current
  inline D3Device &setCullMode(D3DCULL cullMode) {
    if(cullMode != getCullMode()) {
      setDevRenderState(D3DRS_CULLMODE, cullMode);
      m_cullMode = cullMode;
    }
    return *this;
  }
  inline D3DCULL getCullMode() const {
    return m_cullMode;
  }
  // set device srcblend (D3DRS_SRCBLEND) and and update m_renderState.m_srcBlend if different from current
  inline D3Device &setSrcBlend(D3DBLEND blend) {
    if(blend != getSrcBlend()) {
      setDevRenderState(D3DRS_SRCBLEND, blend);
      m_srcBlend = blend;
    }
    return *this;
  }
  inline D3DBLEND getSrcBlend() const {
    return m_srcBlend;
  }
  // set device dstblend (D3DRS_DESTBLEND) and and update m_renderState.m_dstBlend if different from current
  inline D3Device &setDstBlend(D3DBLEND blend) {
    if(blend != getDstBlend()) {
      setDevRenderState(D3DRS_DESTBLEND, blend);
      m_dstBlend = blend;
    }
    return *this;
  }
  inline D3DBLEND getDstBlend() const {
    return m_dstBlend;
  }

  // set device zenable (D3DRS_ZENABLE) and and update m_renderState.m_zEnable if different from current
  inline D3Device &setZEnable(D3DZBUFFERTYPE bufferType) {
    if(bufferType != getZEnable()) {
      setDevRenderState(D3DRS_ZENABLE, bufferType);
      m_zEnable = bufferType;
    }
    return *this;
  }
  inline D3DZBUFFERTYPE getZEnable() const {
    return m_zEnable;
  }

  // set device normalizeNormals (D3DRS_NORMALIZENORMALS) and and update m_renderState.m_normalizeNormals if different from current
  inline D3Device &setNormalizeNormalsEnable(bool enabled) {
    if(enabled != isNormalizeNormalsEnable()) {
      setDevRenderState(D3DRS_NORMALIZENORMALS, enabled ? TRUE : FALSE);
      m_normalizeNormals = enabled;
    }
    return *this;
  }
  inline bool isNormalizeNormalsEnable() const {
    return m_normalizeNormals;
  }

  // set device enableAlphaBlend (D3DRS_ALPHABLENDENABLE) and and update m_renderState.m_alphaBlendEnable if different from current
  inline D3Device &setAlphaBlendEnable(bool enabled) {
    if(enabled != isAlphaBlendEnable()) {
      setDevRenderState(D3DRS_ALPHABLENDENABLE, enabled ? TRUE : FALSE);
      m_alphaBlendEnable = enabled;
    }
    return *this;
  }
  inline bool isAlphaBlendEnable() const {
    return m_alphaBlendEnable;
  }

  // set device lightning (D3DRS_LIGHTING) and and update m_renderState.m_lighting if different from current
  inline D3Device &setLightingEnable(bool enabled) {
    if(enabled != isLightingEnable()) {
      setDevRenderState(D3DRS_LIGHTING, enabled ? TRUE : FALSE);
      m_lighting = enabled;
    }
    return *this;
  }
  inline bool isLightingEnable() const {
    return m_lighting;
  }

  // set device specularHighLightEnable (D3DRS_SPECULARENABLE),
  // and update m_renderState.m_specularHighLightEnable if different from current
  inline D3Device &setSpecularEnable(bool enabled) {
    if(enabled != getSpecularEnable()) {
      setDevRenderState(D3DRS_SPECULARENABLE, enabled ? TRUE : FALSE);
      m_specularHighLightEnable = enabled;
    }
    return *this;
  }
  inline bool getSpecularEnable() const {
    return m_specularHighLightEnable;
  }
  // call m_device->SetMaterial(&mat),
  D3Device &setMaterial(const D3Material &matarial);
  inline const D3Material &getMaterial() const {
    return m_material;
  }
  inline D3Device &setLight(UINT index, const D3DLIGHT &light) {
    V(m_device->SetLight(index, &light));
    return *this;
  }

  inline D3Device &enableLight(UINT index, bool enabled) {
    V(m_device->LightEnable(index, enabled ? TRUE : FALSE));
    return *this;
  }
  D3Device &setLight(const D3Light &light);
  D3DLIGHT  getLight(UINT index) const;
  inline D3Device &setSamplerState(DWORD sampler, D3DSAMPLERSTATETYPE type, DWORD value) {
    V(m_device->SetSamplerState(sampler, D3DSAMP_MINFILTER, D3DTEXF_LINEAR));
    return *this;
  }
  inline D3Device &setTexture(DWORD stage, LPDIRECT3DTEXTURE texture) {
    V(m_device->SetTexture(stage, texture));
    return *this;
  }

  // set's the specified transformation on m_device. Doesn't notify propertyChangeListeners
  inline D3Device &setTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m) {
    V(m_device->SetTransform(id, &m));
    return *this;
  }

  inline D3DXMATRIX getTransformation(D3DTRANSFORMSTATETYPE id) const {
    D3DXMATRIX m;
    V(m_device->GetTransform(id, &m));
    return m;
  }

  // call setTransformation(D3DTS_PROJECTION, m). No notification
  inline D3Device &setProjMatrix(const D3DXMATRIX  &m) { return setTransformation(D3DTS_PROJECTION, m); }
  // call setTransformation(D3DTS_VIEW, m). No notification
  inline D3Device &setViewMatrix(const D3DXMATRIX  &m) { return setTransformation(D3DTS_VIEW      , m); }
  // call setTransformation(D3DTS_WORLD, m). No notification
  inline D3Device &setWorldMatrix(const D3DXMATRIX &m) { return setTransformation(D3DTS_WORLD     , m); }

  inline D3DXMATRIX      getProjMatrix()  const { return getTransformation(D3DTS_PROJECTION); }
  inline D3DXMATRIX      getViewMatrix()  const { return getTransformation(D3DTS_VIEW      ); }
  inline D3DXMATRIX      getWorldMatrix() const { return getTransformation(D3DTS_WORLD     ); }

  inline D3Device &setStreamSource(LPDIRECT3DVERTEXBUFFER buffer, int vertexSize, DWORD fvf) {
    V(m_device->SetStreamSource(0, buffer, 0, vertexSize));
    return setFVF(fvf);
  }
  inline D3Device &setIndices(LPDIRECT3DINDEXBUFFER indexBuffer) {
    V(m_device->SetIndices(indexBuffer));
    return *this;
  }
  inline D3Device &drawPrimitive(D3DPRIMITIVETYPE pt, int startVertex, int primitiveCount) {
    V(m_device->DrawPrimitive(pt, startVertex, primitiveCount));
    return *this;
  }
  inline D3Device &drawIndexedPrimitive(D3DPRIMITIVETYPE pt, int baseVertexIndex, UINT minVertexIndex, UINT numVertices, UINT startIndex, UINT primCount) {
    V(m_device->DrawIndexedPrimitive(pt, baseVertexIndex, minVertexIndex, numVertices, startIndex, primCount));
    return *this;
  }

  template<typename VertexType> LPDIRECT3DVERTEXBUFFER allocateVertexBuffer(UINT count, UINT *bufferSize = NULL) {
    const UINT vertexSize = sizeof(VertexType);
    UINT tmp, &totalSize = bufferSize ? *bufferSize : tmp;
    totalSize = vertexSize * count;
    LPDIRECT3DVERTEXBUFFER result;
    V(m_device->CreateVertexBuffer(totalSize, 0, VertexType::FVF_Flags, D3DPOOL_DEFAULT, &result, NULL)); TRACE_CREATE(result);
    return result;
  }
};
