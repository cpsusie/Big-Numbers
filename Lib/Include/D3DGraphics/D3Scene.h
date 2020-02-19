#pragma once

#include <CompactArray.h>
#include <BitSet.h>
#include <XMLDoc.h>
#include <NumberInterval.h>
#include <PropertyContainer.h>
#include <MFCUtil/ColorSpace.h>
#include "D3Math.h"
#include "D3CameraArray.h"
#include "D3SceneObjectVisual.h"
#include "D3AbstractTextureFactory.h"
#include "D3AbstractMeshFactory.h"
#include "Light.h"
#include "Material.h"

class D3Device;
class D3LightControl;
class D3Ray;
class D3PickedInfo;

typedef enum {
  SP_AMBIENTCOLOR              // D3DCOLOR
 ,SP_RIGHTHANDED               // bool
 ,SP_LIGHTPARAMETERS           // LIGHT
 ,SP_LIGHTCOUNT                // UINT
 ,SP_MATERIALPARAMETERS        // MATERIAL
 ,SP_MATERIALCOUNT             // UINT
 ,SP_CAMERACOUNT               // UINT
 ,SP_VISUALCOUNT               // UINT
 ,SP_ANIMATIONFRAMEINDEX       // int
} D3SceneProperty;

#define OBJTYPE_MASK(type) (1 << (type))

#define OBJMASK_VISUALOBJECT   OBJTYPE_MASK(SOTYPE_VISUALOBJECT  )
#define OBJMASK_LIGHTCONTROL   OBJTYPE_MASK(SOTYPE_LIGHTCONTROL  )
#define OBJMASK_ANIMATEDOBJECT OBJTYPE_MASK(SOTYPE_ANIMATEDOBJECT)
#define OBJMASK_ALL            (-1)

typedef Iterator<D3SceneObjectVisual*> D3VisualIterator;

// call SAFERELEASE(effect); on the returned value when finished use
LPD3DXEFFECT compileEffect(LPDIRECT3DDEVICE device, const String &srcText, StringArray &errorMsg);

class D3Scene : public PropertyContainer
              , public AbstractMeshFactory
              , public AbstractTextureFactory
{
private:
  friend class D3SceneObjectIterator;
  friend class D3Camera;

  static int       s_textureCoordCount;
  D3Device        *m_device;
  bool             m_rightHanded;
  int              m_maxLightCount;
  D3DCOLOR         m_ambientColor;
  BitSet           m_lightsEnabled, m_lightsDefined;
  MATERIAL         m_undefinedMaterial;
  MaterialMap      m_materialMap;
  D3VisualArray    m_visualArray;
  D3CameraArray    m_cameraArray;

  void notifyVisualCountChanged(UINT oldCount);

  // Return -1 if none exist
  int  getFirstFreeLightIndex() const;
  UINT getFirstFreeMaterialId() const;
  D3LightControl *findLightControlByLightIndex(int lightIndex);
  D3LightControl *addLightControl(    UINT lightIndex);
  // Remove the lightControl associated with the given lightIndex from the scene, if its allocated,
  // AND delete the LightControl. the light will remain in its current state
  void destroyLightControl(           UINT lightIndex);
  // Remove and destroy all allocated lightcontrols
  // The lights will remain in their current states
  void destroyAllLightControls();
  void removeAllLights();
  void removeAllCameras();
  void removeVisual(size_t index);

public:
  D3Scene(bool rightHanded = true);
  ~D3Scene();
  void initDevice(HWND hwnd);
  inline D3Device &getDevice() const {
    if (m_device == NULL) {
      throwException(_T("Device not initialized"));
    }
    return *m_device;
  }
  LPDIRECT3DDEVICE getDirectDevice() const;
  D3Camera       &addCamera(HWND wnd);
  void            removeCamera(D3Camera &camera);
  inline const D3CameraArray &getCameraArray() const {
    return m_cameraArray;
  }
  inline UINT getCameraCount() const {
    return (UINT)m_cameraArray.size();
  }
  void close();
  void render(const D3Camera &camera);
  void render(CameraSet cameraSet);
  void addVisual(D3SceneObjectVisual *obj);
  // Remove obj from scene. if obj is an animated object, the animationthread will be stopped.
  // Does NOT delete the object
  void removeVisual(D3SceneObjectVisual *obj);
  void removeAllVisuals();
  D3VisualIterator getVisualIterator(long mask = OBJMASK_ALL) const;
  bool isVisual(const D3SceneObjectVisual *obj) const;
  void stopAllAnimations();
  inline int getVisualCount() const {
    return (int)m_visualArray.size();
  }

  // set ambient light (D3DRS_AMBIENT) , and notifies properyChangeListerners. (property-id=SP_AMBIENTCOLOR)
  D3Scene &setAmbientColor(D3DCOLOR color) {
    if(color != getAmbientColor()) {
      setProperty(SP_AMBIENTCOLOR, m_ambientColor,color);
    }
    return *this;
  }
  inline D3PCOLOR getAmbientColor() const {
    return m_ambientColor;
  }

  void setRightHanded(bool rightHanded);
  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  void setAnimationFrameIndex(int &oldValue, int newValue);

// --------------------------- LIGHT ----------------------------

  void   setLight(            const LIGHT &param);
  LIGHT  getLight(      UINT lightIndex) const;
  // Return Array of defined lights
  LightArray          getAllLights() const;
  inline D3DLIGHTTYPE getLightType(UINT lightIndex) const {
    return getLight(lightIndex).Type;
  }
  UINT addLight(                   const D3DLIGHT &light);
  void removeLight(                UINT lightIndex);
  void setLightEnabled(            UINT lightIndex, bool enabled);
  inline bool isLightEnabled(      UINT lightIndex) const {
    return m_lightsEnabled.contains(lightIndex);
  }
  inline bool isLightDefined(      UINT lightIndex) const {
    return m_lightsDefined.contains(lightIndex);
  }
  inline const BitSet &getLightsDefined() const {
    return m_lightsDefined;
  }
  inline const BitSet &getLightsEnabled() const {
    return m_lightsEnabled;
  }
  inline bool isLightControlVisible(UINT lightIndex) const {
    return getLightControlsVisible().contains(lightIndex);
  }
  BitSet getLightControlsVisible() const;
  D3LightControl *setLightControlVisible(UINT lightIndex, bool visible);

  inline int getMaxLightCount() const {
    return m_maxLightCount;
  }
  inline int getLightCount() const {
    return (int)m_lightsDefined.size();
  }
  inline int getLightEnabledCount() const {
    return (int)m_lightsEnabled.size();
  }
  void   setLightDirection(        UINT lightIndex, const D3DXVECTOR3 &dir);
  void   setLightPosition(         UINT lightIndex, const D3DXVECTOR3 &pos);
  String getLightString(           UINT lightIndex) const;
  String getLightString() const;
  void   setSpecularEnable(bool enabled);
  bool   getSpecularEnable() const;

// --------------------------- Texture ------------------------------
// call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromFile(const String &fileName) {
    return AbstractTextureFactory::loadTextureFromFile(getDirectDevice(), fileName);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromResource(int resId, const String &typeName) {
    return AbstractTextureFactory::loadTextureFromResource(getDirectDevice(), resId, typeName);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromByteArray(ByteArray &ba) {
    return AbstractTextureFactory::loadTextureFromByteArray(getDirectDevice(), ba);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE getTextureFromBitmap(HBITMAP bm) {
    return AbstractTextureFactory::getTextureFromBitmap(getDirectDevice(), bm);
  }
  // call SAFERELEASE(texture); on the returned value when finished use
  inline LPDIRECT3DTEXTURE loadTextureFromBitmapResource(int id) {
    return AbstractTextureFactory::loadTextureFromBitmapResource(getDirectDevice(), id);
  }

// --------------------------- MATERIAL ----------------------------

  inline const MATERIAL &getMaterial(UINT materialId) const {
    const MATERIAL *m = m_materialMap.get(materialId);
    return m ? *m : m_undefinedMaterial;
  }
  const MaterialMap &getAllMaterials() const {
    return m_materialMap;
  }
  void setMaterial(const MATERIAL    &material);
  UINT addMaterial(const D3DMATERIAL &material);
  void removeMaterial(UINT materialId);
  // assume lcMaterial is defined. Dont notify listeners about change in material
  void setLightControlMaterial(const MATERIAL &lcMaterial);

  inline bool isMaterialDefined(UINT materialId) const {
    return m_materialMap.get(materialId) != NULL;
  }
  inline UINT getMaterialCount() const {
    return (UINT)m_materialMap.size();
  }
  String getMaterialString(UINT materialId) const;
  String getMaterialString() const;

  static inline int getTextureCoordCount() {
    return s_textureCoordCount;
  }
  // p in screen-coordinates
  // Return the camera which uses device with HWND at the given point, or NULL if none
  D3Camera            *getPickedCamera(const CPoint &p) const;
  // p in screen-coordinates
  // if hitPoint != NULL, it will receive point (in worldspace) of rays intersection with nearest object
  // if ray      != NULL, it will receive value returned by getPicRay(point)
  // if dist     != NULL, is will receieve distnce to hitpoint, if any
  // if info     != NULL, it will receieve info returned by SceneObject::intersectWithRay, for the closest, visible object
  D3SceneObjectVisual *getPickedVisual(const CPoint &p  , long mask = OBJMASK_ALL, D3DXVECTOR3 *hitPoint = NULL, D3Ray *ray  = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;
  D3SceneObjectVisual *getPickedVisual(const D3Ray  &ray, long mask = OBJMASK_ALL, D3DXVECTOR3 *hitPoint = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;

  LPDIRECT3DINDEXBUFFER   allocateIndexBuffer( bool int32, UINT count, UINT *bufferSize = NULL);
  LPD3DXMESH              allocateMesh(        DWORD fvf , UINT faceCount, UINT vertexCount, DWORD options);
  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(         XMLDoc &doc) const;
  void load(         XMLDoc &doc);
  void saveLights(   XMLDoc &doc, XMLNodePtr parent) const;
  void loadLights(   XMLDoc &doc, XMLNodePtr parent);
  void saveMaterials(XMLDoc &doc, XMLNodePtr parent) const;
  void loadMaterials(XMLDoc &doc, XMLNodePtr parent);
};

LPD3DXMESH        createMeshFromVertexFile(     AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
LPD3DXMESH        createMeshFromObjFile(        AbstractMeshFactory &amf, const String &fileName, bool doubleSided);
//LPD3DXMESH     createMeshMarchingCube(     LPDIRECT3DDEVICE device, const IsoSurfaceParameters        &param);
