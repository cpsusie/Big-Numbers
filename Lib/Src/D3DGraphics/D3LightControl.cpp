#include "pch.h"
#include <D3DGraphics/D3LightControl.h>

DECLARE_THISFILE;

D3LightControl::D3LightControl(D3Scene &scene, int lightIndex) : SceneObjectWithMesh(scene), m_lightIndex(lightIndex) {
  m_size = 1;
#ifdef USE_RENDEREFFECT
  m_effect = NULL;
#endif
}

D3LightControl::~D3LightControl() {
#ifdef USE_RENDEREFFECT
  SAFE_RELEASE(m_effect);
#endif
}

LPD3DXMESH &D3LightControl::optimizeMesh(LPD3DXMESH &mesh) { // static
  DWORD *rgdwAdjacency = new DWORD[mesh->GetNumFaces() * 3];
  if(rgdwAdjacency == NULL ) {
    throwException(_T("Out of memory"));
  }

  try {
    V(mesh->GenerateAdjacency(1e-6f, rgdwAdjacency));
    V(mesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL));
    delete[] rgdwAdjacency;
  } catch(...) {
    delete[] rgdwAdjacency;
    throw;
  }
  return mesh;
}

LIGHT D3LightControl::getLightParam() const {
  const LIGHT result = m_scene.getLightParam(m_lightIndex);
  assert(result.Type == getLightType());
  return result;
}

D3DMATERIAL D3LightControl::getMaterial() const {
  D3DMATERIAL mat;
  ZeroMemory(&mat, sizeof(D3DMATERIAL));
  D3DCOLORVALUE color = getColor();
  mat.Diffuse  = color;
  mat.Specular = color;
  mat.Power = 0.7f;
#define EMSIVEFACTOR 0.4f
  mat.Emissive = D3DXCOLOR(color.r*EMSIVEFACTOR, color.g*EMSIVEFACTOR, color.b*EMSIVEFACTOR,1);
  return mat;
}

D3DCOLORVALUE D3LightControl::getColor() const {
  const LIGHT light = getLightParam();
  return light.m_enabled ? light.Diffuse : getDisabledColor();
}

D3PosDirUpScale D3LightControl::getPDUS() const {
  D3PosDirUpScale pdus;
  pdus.setWorldMatrix(getWorldMatrix());
  return pdus;
}

#ifndef USE_RENDEREFFECT

void D3LightControl::draw() {
  prepareDraw(USE_SCENEFILLMODE | USE_SCENESHADEMODE);
  V(getDevice()->SetMaterial(&getMaterial()));
  V(m_mesh->DrawSubset(0));
}

#else

void D3LightControl::draw() {
  prepareEffect();
  UINT passCount;
  V(m_effect->Begin( &passCount, 0));
  for(UINT pass = 0; pass < passCount; pass++) {
    V(m_effect->BeginPass(pass));
    V(m_mesh->DrawSubset(0));
    V(m_effect->EndPass());
  }
  V(m_effect->End());
}

void D3LightControl::prepareEffect() {
  D3Scene &scene = getScene();
  if(m_effect == NULL) {
    createEffect();
  }

  const D3PosDirUpScale &pdus   = scene.getCameraPDUS();
  const D3DXMATRIX       mView  = pdus.getViewMatrix();
  const D3DXMATRIX       mProj  = scene.getProjMatrix();
  const D3DXVECTOR3      camPos = pdus.getPos();
  const D3DXMATRIX       mWorld = getWorldMatrix();
  const D3DCOLORVALUE    color  = getColor();

  V(m_effect->SetTechnique( m_renderWith1LightNoTextureHandle));
  V(m_effect->SetVector(    m_materialDiffuseColorHandle, (D3DXVECTOR4*)&color));

  D3DXVECTOR3 vEyePt = unitVector(camPos);

  V(m_effect->SetValue(m_lightDirHandle, &vEyePt, sizeof(D3DXVECTOR3)));

  // Rotate arrow model to point towards origin
  D3DXMATRIXA16 worldViewProj = mWorld * mView * mProj;

  V(m_effect->SetMatrix(m_worldViewProjectionHandle, &worldViewProj));
  V(m_effect->SetMatrix(m_worldHandle, &mWorld));
}

void D3LightControl::createEffect() {
  const char *effectSourceText = 
      "float4   g_MaterialDiffuseColor;    // Material's diffuse color                             \r\n"
      "float3   g_LightDir;                // Light's direction in world space                     \r\n"
      "float4x4 g_mWorld;                  // World matrix for object                              \r\n"
      "float4x4 g_mWorldViewProjection;    // World * View * Projection matrix                     \r\n"
      "                                                                                            \r\n"
      "struct VS_OUTPUT                                                                            \r\n"
      "{                                                                                           \r\n"
      "    float4 Position   : POSITION;   // vertex position                                      \r\n"
      "    float4 Diffuse    : COLOR0;     // vertex diffuse color                                 \r\n"
      "};                                                                                          \r\n"
      "                                                                                            \r\n"
      "VS_OUTPUT RenderWith1LightNoTextureVS( float4 vPos    : POSITION,                           \r\n"
      "                                       float3 vNormal : NORMAL )                            \r\n"
      "{                                                                                           \r\n"
      "    VS_OUTPUT Output;                                                                       \r\n"
      "                                                                                            \r\n"
      "    // Transform the position from object space to homogeneous projection space             \r\n"
      "    Output.Position = mul(vPos, g_mWorldViewProjection);                                    \r\n"
      "                                                                                            \r\n"
      "    // Transform the normal from object space to world space                                \r\n"
      "    float3 vNormalWorldSpace;                                                               \r\n"
      "    vNormalWorldSpace = normalize(mul(vNormal, (float3x3)g_mWorld)); // normal (world space)\r\n"
      "                                                                                            \r\n"
      "    // Compute simple directional lighting equation                                         \r\n"
      "    Output.Diffuse.rgb = g_MaterialDiffuseColor * max(0,dot(vNormalWorldSpace, g_LightDir));\r\n"
      "    Output.Diffuse.a = 1.0f;                                                                \r\n"
      "                                                                                            \r\n"
      "    return Output;                                                                          \r\n"
      "}                                                                                           \r\n"
      "                                                                                            \r\n"
      "float4 RenderWith1LightNoTexturePS(float4 Diffuse : COLOR0) : COLOR0                        \r\n"
      "{                                                                                           \r\n"
      "    return Diffuse;                                                                         \r\n"
      "}                                                                                           \r\n"
      "                                                                                            \r\n"
      "technique RenderWith1LightNoTexture                                                         \r\n"
      "{                                                                                           \r\n"
      "    pass P0                                                                                 \r\n"
      "    {                                                                                       \r\n"
      "        VertexShader = compile vs_2_0 RenderWith1LightNoTextureVS();                        \r\n"
      "        PixelShader  = compile ps_2_0 RenderWith1LightNoTexturePS();                        \r\n"
      "    }                                                                                       \r\n"
      "}                                                                                           \r\n"
      ;

  const int textlen = (int)strlen(effectSourceText);
  DWORD Flags = D3DXFX_NOT_CLONEABLE;
#ifdef D3DXFX_LARGEADDRESS_HANDLE
  Flags |= D3DXFX_LARGEADDRESSAWARE;
#endif
  LPD3DXBUFFER compilerErrors = NULL;
  try {
    V(D3DXCreateEffect(getDevice(), effectSourceText, textlen, NULL, NULL, Flags, NULL, &m_effect, &compilerErrors));
  } catch (Exception e) {
    const String errorMsg = (char*)compilerErrors->GetBufferPointer();
    AfxMessageBox(errorMsg.cstr(), MB_ICONWARNING);
    return;
  }

  // Save technique handles for use when rendering
  m_renderWith1LightNoTextureHandle = m_effect->GetTechniqueByName( "RenderWith1LightNoTexture"    );
  m_materialDiffuseColorHandle      = m_effect->GetParameterByName( NULL, "g_MaterialDiffuseColor" );
  m_lightDirHandle                  = m_effect->GetParameterByName( NULL, "g_LightDir"             );
  m_worldHandle                     = m_effect->GetParameterByName( NULL, "g_mWorld"               );
  m_worldViewProjectionHandle       = m_effect->GetParameterByName( NULL, "g_mWorldViewProjection" );
}

#endif
