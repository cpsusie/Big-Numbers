#pragma once

#include <CompactArray.h>
#include <Thread.h>
#include <BitSet.h>
#include <Date.h>
#include <PropertyContainer.h>
#include <NumberInterval.h>
#include "Function2DSurface.h"
#include "ParametricSurface.h"
#include "IsoSurface.h"

class D3SceneObject;
class D3LightControl;

typedef enum {
  SP_FILLMODE
 ,SP_SHADEMODE
 ,SP_CAMERAPOS
 ,SP_CAMERAORIENTATION
 ,SP_OBJECTPOS
 ,SP_OBJECTORIENTATION
 ,SP_OBJECTSCALE
 ,SP_PROJECTIONTRANSFORMATION
 ,SP_LIGHTPARAMETERS
 ,SP_LIGHTCOUNT
 ,SP_AMBIENTLIGHT
 ,SP_SPECULARENABLED
 ,SP_MATERIALPARAMETERS
 ,SP_RENDERTIME
 ,SP_OBJECTCOUNT
 ,SP_BACKGROUNDCOLOR
 ,SP_ANIMATIONFRAMEINDEX
} D3SceneProperty;

#define MATERIAL       D3DMATERIAL9

#define RGB_GETRED(d3c)      ((BYTE)((d3c)>>16))
#define RGB_GETGREEN(d3c)    ((BYTE)(((WORD)(d3c)) >> 8))
#define RGB_GETBLUE( d3c)    ((BYTE)(d3c))

#define D3DCOLOR2COLORREF(c) RGB(RGB_GETRED(c), RGB_GETGREEN(c), RGB_GETBLUE(c))
#define COLORREF2D3DCOLOR(c) D3DCOLOR_XRGB(GetRValue(c), GetGValue(c), GetBValue(c))

D3DCOLORVALUE colorToColorValue(D3DCOLOR c);

#define D3D_BLACK  D3DCOLOR_XRGB(  0,  0,  0)
#define D3D_WHITE  D3DCOLOR_XRGB(255,255,255)
#define D3D_RED    D3DCOLOR_XRGB(255,  0,  0)
#define D3D_GREEN  D3DCOLOR_XRGB(0  ,255,  0)
#define D3D_BLUE   D3DCOLOR_XRGB(0  ,0  ,255)
#define D3D_YELLOW D3DCOLOR_XRGB(255,255,  0)

#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }

inline bool operator==(const MATERIAL &m1, const MATERIAL &m2) {
  return memcmp(&m1, &m2, sizeof(MATERIAL)) == 0;
}

inline bool operator!=(const MATERIAL &m1, const MATERIAL &m2) {
  return !(m1 == m2);
}

class LIGHT : public D3DLIGHT9 { // sent to listener for id=SP_LIGHTPARAMETERS
public:
  int   m_lightIndex;
  bool  m_enabled;

  // only valid for spot lights
  void setInnerAngle(float rad); // maintain invariant:0 <= Theta <= Phi <= pi
  void setOuterAngle(float rad);
  inline float getInnerAngle() const {
    return Theta;
  }
  inline float getOuterAngle() const {
    return Phi;
  }
};

inline bool operator==(const LIGHT &l1, const LIGHT &l2) {
  return memcmp(&l1, &l2, sizeof(LIGHT)) == 0;
}

inline bool operator!=(const LIGHT &l1, const LIGHT &l2) {
  return !(l1 == l2);
}

class D3PCOLOR {
public:
  const D3DCOLOR m_color;
  inline D3PCOLOR(D3DCOLOR c) : m_color(c) {}
  inline operator D3DCOLOR() const { return m_color; }
};

typedef enum {
  SOTYPE_VISUALOBJECT
 ,SOTYPE_LIGHTCONTROL
 ,SOTYPE_ANIMATEDOBJECT
} SceneObjectType;

#define PICK_MASK(type) (1 << (type))

#define PICK_VISUALOBJECT   PICK_MASK(SOTYPE_VISUALOBJECT)
#define PICK_LIGHTCONTROL   PICK_MASK(SOTYPE_LIGHTCONTROL)
#define PICK_ANIMATEDOBJECT PICK_MASK(SOTYPE_ANIMATEDOBJECT)
#define PICK_ALL (-1)

class D3PickedInfo {
public:
  int m_faceIndex;
  int m_i1,m_i2,m_i3;           // Indices into vertexArray
};

String toString(const LIGHT          &light     );
String toString(const MATERIAL       &material  );
String toString(      D3DFORMAT       f         );
int    formatToSize(  D3DFORMAT       f         );
String toString(      D3DPOOL         pool      );
String toString(      D3DRESOURCETYPE type      );
String usageToString( DWORD           usage     );
String FVFToString(   DWORD           fvf       );
int    FVFToSize(     DWORD           fvf       );
String toString(const D3DDISPLAYMODE &mode      );
String vertexToString(const char *v, DWORD FVF  , int dec=3);

String toString(      D3PCOLOR              c   );
String toString(const D3DCOLORVALUE        &c   );
String toString(const D3DVERTEXBUFFER_DESC &desc);
String toString(const D3DINDEXBUFFER_DESC  &desc);

class D3Scene : public PropertyContainer {
private:
  static LIGHT getDefaultDirectionalLight();
  static LIGHT getDefaultPointLight();
  static LIGHT getDefaultSpotLight();

  static MATERIAL getDefaultMaterial();
  HWND                              m_hwnd;
  DIRECT3DDEVICE                    m_device;
  D3DFILLMODE                       m_fillMode;
  D3DSHADEMODE                      m_shadeMode;
  D3DCOLOR                          m_backgroundColor;
  int                               m_maxLightCount;
  static int                        m_textureCoordCount;
  BitSet                           *m_lightsEnabled;
  BitSet                           *m_lightsDefined;
  MATERIAL                          m_material;
  CompactArray<D3SceneObject*>      m_objectArray;
  int                               m_oldObjectCount;
  D3PosDirUpScale                   m_cameraPDUS, m_objectPDUS;
  float                             m_viewAngel;
  float                             m_nearViewPlane;
  Timestamp                         m_renderTime;
  bool                              m_initDone;
  
  friend class D3SceneObject;
  void notifyPropertyChanged(int id, const void *oldValue, const void *newValue);
  void notifyIfObjectArrayChanged();
  void initCameraTrans(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &lookAt, const D3DXVECTOR3 &up);
  void initObjTrans(const D3DXVECTOR3 &pos   = D3DXVECTOR3(0,0, 0)
                   ,const D3DXVECTOR3 &dir   = D3DXVECTOR3(0,0,-1)
                   ,const D3DXVECTOR3 &up    = D3DXVECTOR3(0,1, 0)
                   ,const D3DXVECTOR3 &scale = D3DXVECTOR3(1,1, 1));
  void updateViewMatrix();
  void updateProjMatrix();
  void setProjMatrix( const D3DXMATRIX &m);
  inline void setViewMatrix(const D3DXMATRIX &m) {
    setTransformation(D3DTS_VIEW, m);
  }
  void setTransformation(D3DTRANSFORMSTATETYPE id, const D3DXMATRIX &m);
  D3DXMATRIX getTransformation(D3DTRANSFORMSTATETYPE id) const;
  int getFirstFreeLightIndex() const; // return -1 if none exist
  D3LightControl *findLightControlByLightIndex(int lightIndex);
  D3LightControl *addLightControl(    unsigned int lightIndex);
  void destroyLightControl(           unsigned int lightIndex);
public:
  D3Scene();
  ~D3Scene();
  void init(HWND hwnd);
  void initTrans();
  void render();
  void OnSize();
  HWND getHwnd() {
    return m_hwnd;
  }
  DIRECT3DDEVICE getDevice() const {
    return m_device;
  }
  void addSceneObject(   D3SceneObject *obj);
  void removeSceneObject(D3SceneObject *obj);
  void removeAllSceneObjects();

  void setFillMode(D3DFILLMODE fillMode);
  inline D3DFILLMODE getFillMode() const {
    return m_fillMode;
  }
  void setShadeMode(D3DSHADEMODE shadeMode);
  inline D3DSHADEMODE getShadeMode() const {
    return m_shadeMode;
  }

  const D3PosDirUpScale &getObjPDUS() const {
    return m_objectPDUS;
  }
  const D3DXVECTOR3 &getObjPos() const {
    return m_objectPDUS.getPos();
  }
  const D3DXVECTOR3 &getObjDir() const {
    return m_objectPDUS.getDir();
  }
  const D3DXVECTOR3 &getObjUp() const {
    return m_objectPDUS.getUp();
  }
  D3DXVECTOR3 getObjRight() const {
    return m_objectPDUS.getRight();
  }
  const D3DXVECTOR3 &getObjScale() const {
    return m_objectPDUS.getScale();
  }
  void setObjPos(        const D3DXVECTOR3 &pos);
  void setObjOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  void setObjScale(const D3DXVECTOR3 &scale);
  String getObjString() const {
    return format(_T("Obj:(%s)"), m_objectPDUS.toString().cstr());
  }

  const D3PosDirUpScale &getCameraPDUS() const {
    return m_cameraPDUS;
  }
  const D3DXVECTOR3 &getCameraPos() const {
    return m_cameraPDUS.getPos();
  }
  const D3DXVECTOR3 &getCameraDir() const {
    return m_cameraPDUS.getDir();
  }
  const D3DXVECTOR3 &getCameraUp() const {
    return m_cameraPDUS.getUp();
  }
  D3DXVECTOR3 getCameraRight() const {
    return m_cameraPDUS.getRight();
  }
  void setCameraPos(const D3DXVECTOR3 &pos);
  void setCameraOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up);
  void setCameraLookAt(const D3DXVECTOR3 &point);
  String getCameraString() const {
    return format(_T("Cam:(%s)"), m_cameraPDUS.toString().cstr());
  }
  void setViewAngel(float angel);
  float getViewAngel() const {
    return m_viewAngel;
  }
  void setNearViewPlane(float zn);
  float getNearViewPlane() const {
    return m_nearViewPlane;
  }
  inline D3DXMATRIX getProjMatrix() const {
    return getTransformation(D3DTS_PROJECTION);
  }

  void setAnimationFrameIndex(int &oldValue, int newValue);
  D3DCOLOR getGlobalAmbientColor() const;
  void setGlobalAmbientColor(D3DCOLOR color);
  void enableSpecular(bool enabled);
  bool isSpecularEnabled() const;

  void setLightDirection(          unsigned int index, const D3DXVECTOR3 &dir);
  void setLightPosition(           unsigned int index, const D3DXVECTOR3 &pos);
  String getLightString(           unsigned int index) const;
  String getLightString() const;

  LIGHT getDefaultLightParam(D3DLIGHTTYPE type = D3DLIGHT_DIRECTIONAL);
  void setLightParam(              const LIGHT &param);
  LIGHT getLightParam(             unsigned int index) const;
  inline D3DLIGHTTYPE getLightType(unsigned int index) const {
    return getLightParam(index).Type;
  }
  void removeLight(                unsigned int index);
  void setLightEnabled(            unsigned int index, bool enabled);
  inline bool isLightEnabled(      unsigned int index) const {
    return m_lightsEnabled->contains(index);
  }
  inline bool isLightDefined(      unsigned int index) const {
    return m_lightsDefined->contains(index);
  }
  inline bool isLightVisible(      unsigned int index) const {
    return getLightsVisible().contains(index);
  }
  inline const BitSet &getLightsDefined() const {
    return *m_lightsDefined;
  }
  inline const BitSet &getLightsEnabled() const {
    return *m_lightsEnabled;
  }
  BitSet getLightsVisible() const;
  void setLightControlVisible(     unsigned int index, bool visible);

  inline int getMaxLightCount() const {
    return m_maxLightCount;
  }
  inline int getLightCount() const {
    return (int)m_lightsDefined->size();
  }
  inline int getLightEnabledCount() const {
    return (int)m_lightsEnabled->size();
  }

  const MATERIAL &getMaterial() const {
    return m_material;
  }
  void setMaterial(const MATERIAL &material);
  String getMaterialString() const;

  void setBackgroundColor(D3DCOLOR color);
  D3DCOLOR getBackgroundColor() const {
    return m_backgroundColor;
  }
  static inline int getTextureCoordCount() {
    return m_textureCoordCount;
  }
  D3Ray getPickRay(const CPoint &point);
  D3SceneObject *getPickedObject(const CPoint &point, long mask = PICK_ALL, D3DXVECTOR3 *hitPoint = NULL, D3PickedInfo *info = NULL);

  void saveState(const String &fileName) const;
  void loadState(const String &fileName);
  void save(ByteOutputStream &s) const;
  void load(ByteInputStream  &s);
};

class MeshArray : public CompactArray<LPD3DXMESH> {
public:
  MeshArray() {
  }
  explicit MeshArray(unsigned int capacity) : CompactArray<LPD3DXMESH>(capacity) {
  }
  MeshArray::MeshArray(const MeshArray &src) : CompactArray<LPD3DXMESH>(src.getCapacity()) {
    addAll(src);
  }
  MeshArray &operator=(const MeshArray &src);
  ~MeshArray() {
    clear();
  }
  void add(const LPD3DXMESH &m) {
    add((UINT)size(), m);
  }
  void add(UINT index, const LPD3DXMESH &m, UINT count = 1);
  bool addAll(const MeshArray &src);
  void remove(UINT index, UINT count = 1);
  void removeLast() {
    remove((UINT)size()-1);
  }

  void clear(int capacity=0);
};

LPD3DXMESH     createMeshFrom2DFunction(   DIRECT3DDEVICE device, Function2D &f, const DoubleInterval &xInterval, const DoubleInterval &yInterval, UINT nx, UINT ny, bool doubleSided);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const Function2DSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const Function2DSurfaceParameters &param);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const ParametricSurfaceParameters &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const ParametricSurfaceParameters &param);
LPD3DXMESH     createMesh(                 DIRECT3DDEVICE device, const IsoSurfaceParameters        &param);
MeshArray      createMeshArray(CWnd *wnd,  DIRECT3DDEVICE device, const IsoSurfaceParameters        &param);
LPD3DXMESH     createSphereMesh(           DIRECT3DDEVICE device, double                            radius);
D3SceneObject *createIsoSurfaceDebugObject(D3Scene &scene,        const IsoSurfaceParameters        &param);
LPD3DXMESH     createMeshFromVertexFile(   DIRECT3DDEVICE device, const String &fileName, bool doubleSided);
//LPD3DXMESH     createMeshMarchingCube(     DIRECT3DDEVICE device, const IsoSurfaceParameters        &param);

LPDIRECT3DTEXTURE9 loadTextureFromFile(     LPDIRECT3DDEVICE9 device, const String &fileName);
LPDIRECT3DTEXTURE9 loadTextureFromResource( LPDIRECT3DDEVICE9 device, int resId, const String &typeName);
LPDIRECT3DTEXTURE9 loadTextureFromByteArray(LPDIRECT3DDEVICE9 device, ByteArray &ba);
LPDIRECT3DTEXTURE9 getTextureFromBitmap(    LPDIRECT3DDEVICE9 device, HBITMAP bm);

void dumpMesh(LPD3DXMESH mesh, const String &fileName="");
void dumpVertexBuffer(LPDIRECT3DVERTEXBUFFER9 vertexBuffer, FILE *f);
void dumpIndexBuffer( LPDIRECT3DINDEXBUFFER9  indexBuffer , FILE *f);

#define USE_SCENEMATERIAL  0x0001
#define USE_SCENEFILLMODE  0x0002
#define USE_SCENESHADEMODE 0x0004

#define USE_SCENEPARAMS  (USE_SCENEMATERIAL | USE_SCENEFILLMODE | USE_SCENESHADEMODE)

class D3SceneObject {
protected:
  D3Scene &m_scene;
  String   m_name;
  bool     m_visible;
  void    *m_userData;
  DIRECT3DDEVICE getDevice() const {
    return m_scene.m_device;
  }
public:
  D3SceneObject(D3Scene &scene, const String &name="Untitled") : m_scene(scene) {
    m_visible  = true;
    m_name     = name;
    m_userData = NULL;
  }
  virtual ~D3SceneObject() {
  };

  D3Scene &getScene() {
    return m_scene;
  }
  const D3Scene &getScene() const {
    return m_scene;
  }
  void *getUserData() {
    return m_userData;
  }
  void setUserData(void *p) {
    m_userData = p;
  }
  void prepareDraw(UINT flags = USE_SCENEPARAMS);
  void setVisible(bool visible) {
    m_visible = visible;
  }
  virtual LPD3DXMESH getMesh() {
    return NULL;
  }
  inline bool isVisible() const {
    return m_visible;
  }
  virtual D3PosDirUpScale getPDUS() const {
    return m_scene.getObjPDUS();
  }
  virtual D3DXMATRIX getWorldMatrix() const {
    return getPDUS().getWorldMatrix();
  }
  D3DXVECTOR3 getPos() const {
    return getPDUS().getPos();
  }
  D3DXVECTOR3 getDir() const {
    return getPDUS().getDir();
  }
  D3DXVECTOR3 getUp() const {
    return getPDUS().getUp();
  }
  D3DXVECTOR3 getRight() const {
    return getPDUS().getRight();
  }
  const D3DXVECTOR3 &getScale() const {
    return getPDUS().getScale();
  }

  virtual void draw() = 0;
  bool intersectsWithRay(const D3Ray &ray, float &dist, D3PickedInfo *info = NULL) const;

  String getName() const {
    return m_name;
  }
  void setName(const String &name) {
    m_name = name;
  }
  virtual SceneObjectType getType() const {
    return SOTYPE_VISUALOBJECT;
  }
};

class SceneObjectWithVertexBuffer : public D3SceneObject {
protected:
  int                     m_primitiveCount;
  DWORD                   m_fvf;
  int                     m_vertexSize;
  LPDIRECT3DVERTEXBUFFER9 m_vertexBuffer;
  void *allocateVertexBuffer(int vertexSize, UINT count, DWORD fvf);
  void unlockVertexBuffer();
  void prepareDraw(UINT flags = USE_SCENEPARAMS);
public:
  SceneObjectWithVertexBuffer(D3Scene &scene);
  ~SceneObjectWithVertexBuffer();

  inline LPDIRECT3DVERTEXBUFFER9 &getVertexBuffer() {
    return m_vertexBuffer;
  }
};

class SceneObjectWithIndexBuffer : public SceneObjectWithVertexBuffer {
protected:
  LPDIRECT3DINDEXBUFFER9 m_indexBuffer;
  void *allocateIndexBuffer(bool int32, int count);
  void unlockIndexBuffer();
  void prepareDraw(UINT flags = USE_SCENEPARAMS);
public:
  SceneObjectWithIndexBuffer(D3Scene &scene);
  ~SceneObjectWithIndexBuffer();
};

class SceneObjectWithMesh : public D3SceneObject {
protected:
  LPD3DXMESH m_mesh;
  void createMesh(DWORD faceCount, DWORD vertexCount, DWORD fvf);
  void *lockVertexBuffer();
  void *lockIndexBuffer();
  void unlockVertexBuffer();
  void unlockIndexBuffer();
  void releaseMesh();
public:
  SceneObjectWithMesh(D3Scene &scene, LPD3DXMESH mesh = NULL); // if mesh != NULL, it will be released when Object is destroyed
  ~SceneObjectWithMesh();
  LPD3DXMESH getMesh() {
    return m_mesh;
  }
  void draw();
};

class D3LineArray : public SceneObjectWithVertexBuffer {
private:
  void initBuffer(const Line *lines, int n);
public:
  D3LineArray(D3Scene &scene, const Line *lines, int n);
  D3LineArray(D3Scene &scene, const Vertex p1, const Vertex p2); // make a wireframe box
  void draw();
};

class D3Curve : public SceneObjectWithVertexBuffer {
public:
  D3Curve(D3Scene &scene, const VertexArray &points);
  void draw();
};

class D3CurveArray : public SceneObjectWithVertexBuffer {
private:
  CompactIntArray m_curveSize;
public:
  D3CurveArray(D3Scene &scene, const CurveArray &curves);
  void draw();
};

typedef enum {
  ANIMATE_FORWARD
 ,ANIMATE_BACKWARD
 ,ANIMATE_ALTERNATING
} AnimationType;

class D3AnimatedSurface : public D3SceneObject {
private:
  friend class MeshAnimationThread;
  MeshArray            m_meshArray;
  int                  m_nextMeshIndex, m_lastRenderedIndex;
  MeshAnimationThread *m_animator;
public:
  D3AnimatedSurface(D3Scene &scene, const MeshArray &meshArray);
  ~D3AnimatedSurface();
  void startAnimation(AnimationType type = ANIMATE_FORWARD);
  void stopAnimation();
  bool isRunning() const;
  AnimationType getAnimationType() const;
  void scaleSpeed(double factor); // sleepTime /= factor
  LPD3DXMESH getMesh();
  void draw();
  SceneObjectType getType() const {
    return SOTYPE_ANIMATEDOBJECT;
  }
};

class D3LineArrow : public SceneObjectWithVertexBuffer {
private:
  D3DCOLOR m_color;
  MATERIAL getMaterial() const;
public:
  D3LineArrow(D3Scene &scene, const Vertex &from, const Vertex &to, D3DCOLOR color = 0);
  void setColor(D3DCOLOR color);
  void draw();
};

class D3PickRayArrow : public D3LineArrow {
public:
  D3PickRayArrow(D3Scene &scene, const D3Ray &ray) : D3LineArrow(scene, ray.m_orig, ray.m_orig + 2 * ray.m_dir) {
  }
  D3DXMATRIX getWorldMatrix() const {
    D3DXMATRIX m;
    return *D3DXMatrixIdentity(&m);
  }
  D3PosDirUpScale getPDUS() const {
    D3PosDirUpScale pdus;
    pdus.setWorldMatrix(getWorldMatrix());
    return pdus;
  }
};

CurveArray createSphereObject(double r);
