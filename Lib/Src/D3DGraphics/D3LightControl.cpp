#include "pch.h"
#include <CompactHashMap.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/Profile.h>
#include <D3DGraphics/MeshCreators.h>
#include <D3DGraphics/D3LightControl.h>

bool D3LightControl::s_renderEffectEnabled = false;

D3LightControl::D3LightControl(D3Scene &scene, int lightIndex)
  : D3SceneObjectWithMesh(scene), m_lightIndex(lightIndex)
{
  m_size       =  1;
  m_materialId = -1;
  m_effect     = NULL;
}

D3LightControl::~D3LightControl() {
  SAFERELEASE(m_effect);
  if(hasMaterial()) {
    getScene().removeMaterial(m_materialId);
  }
}

bool D3LightControl::isVisible() const {
  return getDevice().getCurrentCamera()->isLightControlVisible(m_lightIndex);
}

static LPD3DXMESH &optimizeMesh(LPD3DXMESH &mesh) {
  DWORD *rgdwAdjacency = new DWORD[mesh->GetNumFaces() * 3]; TRACE_NEW(rgdwAdjacency);
  if(rgdwAdjacency == NULL ) {
    throwException(_T("Out of memory"));
  }

  try {
    V(mesh->GenerateAdjacency(1e-6f, rgdwAdjacency));
    V(mesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL));
    SAFEDELETEARRAY(rgdwAdjacency);
  } catch(...) {
    SAFEDELETEARRAY(rgdwAdjacency);
    throw;
  }
  return mesh;
}

static const Point2D directionalMeshProfilePoints[] = {
  Point2D(0   , 0)
 ,Point2D(-0.47, 0.25)
 ,Point2D(-0.47, 0.1)
 ,Point2D(-1   , 0.1)
 ,Point2D(-1   ,   0)
};

static LPD3DXMESH createDirectionalMesh(AbstractMeshFactory &factory) {
  Profile prof;
  prof.addLineStrip(directionalMeshProfilePoints, ARRAYSIZE(directionalMeshProfilePoints));
  ProfileRotationParameters param;
  param.m_alignx     = 0;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = ROTATESMOOTH;
  param.m_rotateAxis = 0;
  return rotateProfile(factory, prof, param, false);
}

static const Point2D spotMeshProfilePoints[] = {
  Point2D(0   , 0)
 ,Point2D(0   , 0.17)
 ,Point2D(-0.25, 0.13)
 ,Point2D(-0.3 , 0.1)
 ,Point2D(-1.0 , 0.1)
 ,Point2D(-1.0 , 0)
};

static LPD3DXMESH createSpotMesh(AbstractMeshFactory &factory) {
  Profile prof;
  prof.addLineStrip(spotMeshProfilePoints, ARRAYSIZE(spotMeshProfilePoints));

  ProfileRotationParameters param;
  param.m_alignx     = 0;
  param.m_aligny     = 1;
  param.m_rad        = radians(360);
  param.m_edgeCount  = 20;
  param.m_smoothness = ROTATESMOOTH | NORMALSMOOTH;
  param.m_rotateAxis = 0;
  return rotateProfile(factory, prof, param, false);
}

static LPD3DXMESH createLCMesh(AbstractMeshFactory &factory, D3DLIGHTTYPE lightType) {
  LPD3DXMESH mesh;
  switch(lightType) {
  case D3DLIGHT_POINT      : mesh = createSphereMesh(     factory, 1 ); break;
  case D3DLIGHT_SPOT       : mesh = createSpotMesh(       factory    ); break;
  case D3DLIGHT_DIRECTIONAL: mesh = createDirectionalMesh(factory    ); break;
  }
  return optimizeMesh(mesh);
}

static CompactIntHashMap<LPD3DXMESH> m_meshCache;
static LPD3DXMESH getMesh(AbstractMeshFactory &factory, D3DLIGHTTYPE lightType) {
  LPD3DXMESH *mp = m_meshCache.get(lightType);
  if(mp != NULL) {
    return *mp;
  }
  m_meshCache.put(lightType, createLCMesh(factory, lightType));
  return *m_meshCache.get(lightType);
}

D3Light D3LightControl::getLight() const { // public
  const D3Light result = getScene().getLight(m_lightIndex);
  if((result.getIndex() != m_lightIndex) || (result.Type != getLightType())) {
    showError(_T("%s:Light %d is undefined")
             ,__TFUNCTION__
             ,m_lightIndex);
  }
  return result;
}

LPD3DXMESH D3LightControl::getMesh() const {
  return ::getMesh(getScene(), getLightType());
}

void D3LightControl::createMaterial() { // protected
  m_materialId = getScene().addMaterial(createMaterialFromLight(getLight()));
}

D3DCOLORVALUE D3LightControl::getMaterialColor(const D3Light &l) { // static
  return l.isEnabled() ? l.Diffuse : getDisabledMaterialColor();
}

D3DMATERIAL D3LightControl::createMaterialFromLight(const D3Light &l) { // static
  D3DMATERIAL mat;
  ZeroMemory(&mat, sizeof(D3DMATERIAL));
  mat.Power = 0.7f;
  const D3DCOLORVALUE color = getMaterialColor(l);
  mat.Diffuse  = color;
  mat.Specular = color;
#define EMSIVEFACTOR 0.4f
  mat.Emissive = D3DXCOLOR(color.r*EMSIVEFACTOR, color.g*EMSIVEFACTOR, color.b*EMSIVEFACTOR, 1);
  return mat;
}

D3DCOLORVALUE D3LightControl::getMaterialColor() const {
  return getMaterialColor(getLight());
}

void D3LightControl::updateMaterial() const {
  D3Material mat = getScene().getMaterial(getMaterialId());
  mat = createMaterialFromLight(getLight());
  getScene().setLightControlMaterial(mat);
}

bool D3LightControl::isDifferentMaterial(const D3Light &l1, const D3Light &l2) { // static, public
  if(s_renderEffectEnabled) return false;
  return createMaterialFromLight(l1) != createMaterialFromLight(l2);
}

void D3LightControl::draw() {
  D3Device &device = getDevice();
  device.setFillMode(D3DFILL_SOLID).setShadeMode(D3DSHADE_GOURAUD);
  if(!s_renderEffectEnabled) {
    updateMaterial();
    device.setWorldMatrix(getWorld()).setMaterial(getMaterial());
    drawSubset(0);
  } else {
    prepareEffect();
    UINT passCount;
    V(m_effect->Begin( &passCount, 0));
    for(UINT pass = 0; pass < passCount; pass++) {
      V(m_effect->BeginPass(pass));
      drawSubset(0);
      V(m_effect->EndPass());
    }
    V(m_effect->End());
  }
}

void D3LightControl::prepareEffect() {
  D3Device &device = getDevice();
  if(m_effect == NULL) {
    createEffect();
  }

  const D3DXMATRIX       mView  = device.getViewMatrix();
  const D3DXMATRIX       mProj  = device.getProjMatrix();
  const D3DXVECTOR3      camPos = device.getCurrentCamera()->getPos();
  const D3DXMATRIX       mWorld = getWorld();
  const D3DCOLORVALUE    color  = getMaterialColor();

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

  StringArray compilerErrors;
  m_effect = compileEffect(getDirectDevice(), effectSourceText, compilerErrors);
  if(m_effect == NULL) {
    showWarning(compilerErrors.toString(_T("\n")));
    return;
  }

  // Save technique handles for use when rendering
  m_renderWith1LightNoTextureHandle = m_effect->GetTechniqueByName( "RenderWith1LightNoTexture"    );
  m_materialDiffuseColorHandle      = m_effect->GetParameterByName( NULL, "g_MaterialDiffuseColor" );
  m_lightDirHandle                  = m_effect->GetParameterByName( NULL, "g_LightDir"             );
  m_worldHandle                     = m_effect->GetParameterByName( NULL, "g_mWorld"               );
  m_worldViewProjectionHandle       = m_effect->GetParameterByName( NULL, "g_mWorldViewProjection" );
}
