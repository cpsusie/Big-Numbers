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
#include "D3Light.h"
#include "D3Material.h"
#include "D3Texture.h"

class D3Device;
class D3LightControl;
class D3Ray;
class D3PickedInfo;

typedef enum {
  SP_AMBIENTCOLOR              // D3DCOLOR
 ,SP_RIGHTHANDED               // bool
 ,SP_LIGHTPARAMETERS           // D3Light
 ,SP_LIGHTCOUNT                // UINT
 ,SP_MATERIALPARAMETERS        // D3Material
 ,SP_MATERIALCOUNT             // UINT
 ,SP_TEXTUREIMAGE              // D3Texture
 ,SP_TEXTURECOUNT              // UINT
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

class D3Scene : public PropertyContainer {
private:
  friend class D3SceneObjectIterator;
  friend class D3Camera;

  D3Device                 *m_device;
  bool                      m_rightHanded;
  D3DCOLOR                  m_ambientColor;
  BitSet                    m_lightsEnabled, m_lightsDefined;
  D3Material                m_undefinedMaterial;
  MaterialMap               m_materialMap;
  D3Texture                 m_undefinedTexture;
  TextureMap                m_textureMap;
  D3VisualArray             m_visualArray;
  D3CameraArray             m_cameraArray;

  void notifyVisualCountChanged(UINT oldCount);

  // throw exception if no more lights available
  UINT getFirstFreeLightIndex() const;
  UINT getFirstFreeMaterialId() const;
  UINT getFirstFreeTextureId() const;
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
    if(m_device == NULL) {
      throwException(_T("Device not initialized"));
    }
    return *m_device;
  }
  LPDIRECT3DDEVICE getDirectDevice() const;
  // notify listeners with notification id = SP_CAMERACOUNT
  // if src != NULL, the new camera will be a clone of this (except hwnd)
  // else default-values will be used
  D3Camera       &addCamera(HWND wnd, D3Camera *src=NULL);
  // notify listeners with notification id = SP_CAMERACOUNT
  void            removeCamera(D3Camera &camera);
  inline const D3CameraArray &getCameraArray() const {
    return m_cameraArray;
  }
  inline UINT getCameraCount() const {
    return (UINT)m_cameraArray.size();
  }
  inline CameraSet getActiveCameraSet() const {
    return m_cameraArray.getActiveCameraSet();
  }
  void close();
  void render(const D3Camera &camera);
  void render(CameraSet cameraSet);
  // notify listeners with notification id=SP_VISUALCOUNT
  void addVisual(D3SceneObjectVisual *obj);
  // Remove obj from scene. if obj is an animated object, the animationthread will be stopped.
  // Does NOT delete the object
  // notify listeners with notification id=SP_VISUALCOUNT
  void removeVisual(D3SceneObjectVisual *obj);
  void removeAllVisuals();
  D3VisualIterator getVisualIterator(long mask = OBJMASK_ALL) const;
  // Return true, if obj is in m_visualArray, else false
  bool isVisual(const D3SceneObjectVisual *obj) const;
  D3SceneObjectVisual *getLastVisual() const {
    return m_visualArray.last();
  }
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

  // notify listeners with notification id=SP_RIGHTHANDED
  void setRightHanded(bool rightHanded);
  inline bool getRightHanded() const {
    return m_rightHanded;
  }
  void setAnimationFrameIndex(int &oldValue, int newValue);

// --------------------------- D3Light ----------------------------

  // notify listeners with notification id=SP_LIGHTPARAMETERS
  void     setLight(      const D3Light &param, bool force = false);
  D3Light  getLight(      UINT lightIndex) const;
  // Return Array of defined lights
  LightArray          getAllLights() const;
  void                setAllLights(const LightArray &a);
  inline D3DLIGHTTYPE getLightType(UINT lightIndex) const {
    return getLight(lightIndex).Type;
  }
  // notify listeners with notification id=SP_LIGHTCOUNT
  UINT addLight(                   const D3DLIGHT &light);
  // notify listeners with notification id=SP_LIGHTCOUNT
  void removeLight(                UINT lightIndex);
  // notify listeners with notification id=SP_LIGHTPARAMETERS
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

  UINT getMaxLightCount() const;
  inline UINT getLightCount() const {
    return (UINT)m_lightsDefined.size();
  }
  inline UINT getLightEnabledCount() const {
    return (UINT)m_lightsEnabled.size();
  }
  void   setLightDirection(        UINT lightIndex, const D3DXVECTOR3 &dir);
  void   setLightPosition(         UINT lightIndex, const D3DXVECTOR3 &pos);
  String getLightString(           UINT lightIndex) const;
  String getLightString() const;

// --------------------------- D3Material ----------------------------

  inline const D3Material &getMaterial(UINT materialId) const {
    const D3Material *m = m_materialMap.get(materialId);
    return m ? *m : m_undefinedMaterial;
  }
  const MaterialMap &getAllMaterials() const {
    return m_materialMap;
  }
  // notify listeners with either SP_MATERIALPARAMETERS or SP_MATERIALCOUNT in case of new material
  void setMaterial(const D3Material  &material);
  // notify listeners by notification id=SP_MATERIALCOUNT
  UINT addMaterial(const D3MATERIAL &material);
  // notify listeners by notification id=SP_MATERIALCOUNT
  void setAllMaterials(const MaterialMap &materialMap);
  // add a material with the specified diffuse and emissive color, and return it's id
  // uses D3Material::createMaterialWithColor
  UINT addMaterialWithColor(D3DCOLOR color, bool specularHighlights=true);
  // notify listeners by notification id=SP_MATERIALCOUNT
  void removeAllMaterials();
  // notify listeners by notification id=SP_MATERIALCOUNT
  void removeMaterial(UINT materialId);
  // assume lcMaterial is defined. Dont notify listeners about change in material
  void setLightControlMaterial(const D3Material &lcMaterial);

  inline bool isMaterialDefined(UINT materialId) const {
    return m_materialMap.get(materialId) != NULL;
  }
  inline UINT getMaterialCount() const {
    return (UINT)m_materialMap.size();
  }
  String getMaterialString(UINT materialId) const;
  String getMaterialString() const;

  UINT getTextureCoordCount() const;

  UINT addTexture(LPDIRECT3DTEXTURE texture);
  // notify listeners with either SP_TEXTUREIMAGE or SP_TEXTURECOUNT in case of new texture
  void setTexture(const D3Texture &texture);
  void setAllTextures(const TextureMap &textureMap);
  void removeTexture(UINT textureId);
  void removeAllTextures();
  const D3Texture &getTexture(UINT textureId) const {
    const D3Texture *t = m_textureMap.get(textureId);
    return (t != NULL) ? *t : m_undefinedTexture;
  }
  inline bool isTextureDefined(UINT textureId) const {
    return m_textureMap.get(textureId) != NULL;
  }
  inline UINT getTextureCount() const {
    return (UINT)m_textureMap.size();
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
  D3SceneObjectVisual *getPickedVisual(const D3Camera &camera, const D3Ray  &ray, long mask = OBJMASK_ALL, D3DXVECTOR3 *hitPoint = NULL, float *dist = NULL, D3PickedInfo *info = NULL) const;

  void save(const String &fileName) const;
  void load(const String &fileName);
  void save(         XMLDoc &doc) const;
  void load(         XMLDoc &doc);
};
