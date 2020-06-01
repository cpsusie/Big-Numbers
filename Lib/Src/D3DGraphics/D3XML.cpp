#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/MFCXML.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3World.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3CameraArray.h>
#include <D3DGraphics/D3Scene.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXVECTOR3 &v) {
  setValue(doc, n, _T("x"), v.x);
  setValue(doc, n, _T("y"), v.y);
  setValue(doc, n, _T("z"), v.z);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DXVECTOR3 &v) {
  getValue(doc, n, _T("x"), v.x);
  getValue(doc, n, _T("y"), v.y);
  getValue(doc, n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DVECTOR &v) {
  setValue(doc, n, _T("x"), v.x);
  setValue(doc, n, _T("y"), v.y);
  setValue(doc, n, _T("z"), v.z);
}
void getValue(XMLDoc &doc, XMLNodePtr n, D3DVECTOR &v) {
  getValue(doc, n, _T("x"), v.x);
  getValue(doc, n, _T("y"), v.y);
  getValue(doc, n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DXQUATERNION &q) {
  setValue(doc, n, _T("x"), q.x);
  setValue(doc, n, _T("y"), q.y);
  setValue(doc, n, _T("z"), q.z);
  setValue(doc, n, _T("w"), q.w);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DXQUATERNION &q) {
  getValue(doc, n, _T("x"), q.x);
  getValue(doc, n, _T("y"), q.y);
  getValue(doc, n, _T("z"), q.z);
  getValue(doc, n, _T("w"), q.w);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DMATERIAL &v) {
  setValue(doc, n, _T("diffuse"  ), v.Diffuse  );
  setValue(doc, n, _T("ambient"  ), v.Ambient  );
  setValue(doc, n, _T("specular" ), v.Specular );
  setValue(doc, n, _T("emissive" ), v.Emissive );
  setValue(doc, n, _T("power"    ), v.Power    );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DMATERIAL &v) {
  memset(&v, 0, sizeof(D3DMATERIAL));
  getValue(doc, n, _T("diffuse"  ), v.Diffuse  );
  getValue(doc, n, _T("ambient"  ), v.Ambient  );
  getValue(doc, n, _T("specular" ), v.Specular );
  getValue(doc, n, _T("emissive" ), v.Emissive );
  getValue(doc, n, _T("power"    ), v.Power    );
}

void setValue(XMLDoc &doc, XMLNodePtr n, const MaterialMap &map) {
  setValue<MaterialMap, CompactUIntKeyType, D3Material>(doc, n, map);
}

void getValue(XMLDoc &doc, XMLNodePtr n, MaterialMap &map) {
  getValue<MaterialMap, CompactUIntKeyType, D3Material>(doc, n, map);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DLIGHT &v) {
  setValue(doc, n, _T("type"     ), toString(v.Type));
  setValue(doc, n, _T("diffuse"  ), v.Diffuse       );
  setValue(doc, n, _T("ambient"  ), v.Ambient       );
  setValue(doc, n, _T("specular" ), v.Specular      );

  switch(v.Type) {
  case D3DLIGHT_DIRECTIONAL:
    setValue(doc, n, _T("direction"   ), v.Direction   );
    break;
  case D3DLIGHT_POINT      :
    setValue(doc, n, _T("position"    ), v.Position    );
    setValue(doc, n, _T("range"       ), v.Range       );
    setValue(doc, n, _T("attenuation0"), v.Attenuation0);
    setValue(doc, n, _T("attenuation1"), v.Attenuation1);
    setValue(doc, n, _T("attenuation2"), v.Attenuation2);
    break;
  case D3DLIGHT_SPOT       :
    setValue(doc, n, _T("position"    ), v.Position    );
    setValue(doc, n, _T("direction"   ), v.Direction   );
    setValue(doc, n, _T("range"       ), v.Range       );
    setValue(doc, n, _T("attenuation0"), v.Attenuation0);
    setValue(doc, n, _T("attenuation1"), v.Attenuation1);
    setValue(doc, n, _T("attenuation2"), v.Attenuation2);
    setValue(doc, n, _T("falloff"     ), v.Falloff     );
    setValue(doc, n, _T("theta"       ), v.Theta       );
    setValue(doc, n, _T("phi"         ), v.Phi         );
    break;
  }
}

static D3DLIGHTTYPE strToLightType(const String &str) {
  static const D3DLIGHTTYPE typeTable[] = { D3DLIGHT_POINT, D3DLIGHT_SPOT,D3DLIGHT_DIRECTIONAL };
  for(int i = 0; i < ARRAYSIZE(typeTable); i++) {
    const D3DLIGHTTYPE type = typeTable[i];
    if(str.equalsIgnoreCase(toString(type))) {
      return type;
    }
  }
  throwInvalidArgumentException(__TFUNCTION__, _T("s=%s"), str.cstr());
  return D3DLIGHT_DIRECTIONAL;
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3DLIGHT &v) {
  String typeStr;
  getValue(doc, n, _T("type"     ), typeStr         );
  memset(&v, 0, sizeof(D3DLIGHT));

  v.Type = strToLightType(typeStr);

  getValue(doc, n, _T("diffuse"  ), v.Diffuse       );
  getValue(doc, n, _T("ambient"  ), v.Ambient       );
  getValue(doc, n, _T("specular" ), v.Specular      );

  switch(v.Type) {
  case D3DLIGHT_DIRECTIONAL:
    getValue(doc, n, _T("direction"   ), v.Direction   );
    break;
  case D3DLIGHT_POINT      :
    getValue(doc, n, _T("position"    ), v.Position    );
    getValue(doc, n, _T("range"       ), v.Range       );
    getValue(doc, n, _T("attenuation0"), v.Attenuation0);
    getValue(doc, n, _T("attenuation1"), v.Attenuation1);
    getValue(doc, n, _T("attenuation2"), v.Attenuation2);
    break;
  case D3DLIGHT_SPOT       :
    getValue(doc, n, _T("position"    ), v.Position    );
    getValue(doc, n, _T("direction"   ), v.Direction   );
    getValue(doc, n, _T("range"       ), v.Range       );
    getValue(doc, n, _T("attenuation0"), v.Attenuation0);
    getValue(doc, n, _T("attenuation1"), v.Attenuation1);
    getValue(doc, n, _T("attenuation2"), v.Attenuation2);
    getValue(doc, n, _T("falloff"     ), v.Falloff     );
    getValue(doc, n, _T("theta"       ), v.Theta       );
    getValue(doc, n, _T("phi"         ), v.Phi         );
    break;
  }
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Light &v) {
  setValue(doc, n, _T("index"  ), v.getIndex());
  setValue(doc, n, _T("enabled"), v.isEnabled());
  setValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3Light &v) {
  int  index;
  bool enabled;
  getValue(doc, n, _T("index"  ), index);
  getValue(doc, n, _T("enabled"), enabled);
  v = D3Light(index);
  v.setEnabled(enabled);
  getValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const LightArray &a) {
  setValue<LightArray, D3Light>(doc, n, a);
}

void getValue(XMLDoc &doc, XMLNodePtr n, LightArray &a) {
  getValue<LightArray, D3Light>(doc, n, a);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3World &w) {
  setValue(doc, n, _T("pos"        ), w.getPos()        );
  setValue(doc, n, _T("orientation"), w.getOrientation());
  setValue(doc, n, _T("scale"      ), w.getScale()      );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3World &w) {
  D3DXVECTOR3    pos, scale;
  D3DXQUATERNION q;
  getValue(doc, n, _T("pos"        ), pos  );
  getValue(doc, n, _T("orientation"), q    );
  getValue(doc, n, _T("scale"      ), scale);
  w.setPos(pos).setOrientation(q).setScale(scale);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Camera &c) {
  setValue(doc, n, _T("righthanded"    ), c.getRightHanded()         );
  setValue(doc, n, _T("nearviewplane"  ), c.getNearViewPlane()       );
  setValue(doc, n, _T("farviewplane"  ) , c.getFarViewPlane()        );
  setValue(doc, n, _T("viewangle"      ), c.getViewAngle()           );
  setValue(doc, n, _T("winsize"        ), c.getWinSize()             );
  setValue(doc, n, _T("world"          ), c.getD3World()             );
  setValue(doc, n, _T("backgroundcolor"), c.getBackgroundColor()     );
  setValue(doc, n, _T("visiblelights"  ), c.getLightControlsVisible());
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3Camera &c) {
  bool       rightHanded;
  float      nearViewPlane, farViewPlane, viewAngle;
  CSize      winSize;
  D3World    world;
  D3PCOLOR   backgroundColor;
  BitSet     lightControlsVisible(10);

  getValue(doc, n, _T("righthanded"    ), rightHanded         );
  getValue(doc, n, _T("nearviewplane"  ), nearViewPlane       );
  getValue(doc, n, _T("farviewplane"   ), farViewPlane        );
  getValue(doc, n, _T("viewangle"      ), viewAngle           );
  getValue(doc, n, _T("winsize"        ), winSize             );
  getValue(doc, n, _T("world"          ), world               );
  getValue(doc, n, _T("backgroundcolor"), backgroundColor     );
  getValue(doc, n, _T("visiblelights"  ), lightControlsVisible);

  c.setRightHanded(  rightHanded  );
  c.setNearViewPlane(nearViewPlane);
  c.setFarViewPlane( farViewPlane );
  c.setViewAngle(    viewAngle    );
  setWindowSize(c.getHwnd(), winSize);
  c.setD3World(world);
  c.setBackgroundColor(backgroundColor);
  c.setLightControlsVisible(lightControlsVisible);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3CameraArray &a) {
  const UINT count = (UINT)a.size();
  setValue(doc, n, _T("count"), count);
  for(UINT i = 0; i < count; i++) {
    const String id = format(_T("cam%u"), i);
    setValue(doc, n, id.cstr(), *a[i]);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3CameraArray &a) {
  UINT count;
  getValue(doc, n, _T("count"), count);
  if(count != a.size()) {
    throwInvalidArgumentException(__TFUNCTION__, _T("a.size()=%zu, xmldoc.count=%u"), a.size(), count);
  }
  if(count == 0) return;
  
  for(XMLNodePtr child = doc.getChild(n, _T("cam0")); child; child = child->nextSibling) {
    const String idStr = (wchar_t*)child->nodeName;
    UINT  id;
    _stscanf(idStr.cstr(), _T("cam%u"), &id);
    D3Camera *cam = a[id];
    getValue(doc, n, idStr.cstr(), *cam);
  }
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Scene &s) {
  setValue(doc, n, _T("ambientcolor"), s.getAmbientColor());
  setValue(doc, n, _T("materials"   ), s.getAllMaterials());
  setValue(doc, n, _T("lights"      ), s.getAllLights()   );
  setValue(doc, n, _T("cameras"     ), s.getCameraArray() );
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3Scene &s) {
  D3PCOLOR    ambientColor;
  MaterialMap materialMap;
  LightArray  lightArray;
  getValue(doc, n, _T("ambientcolor"), ambientColor);
  getValue(doc, n, _T("materials"   ), materialMap );
  getValue(doc, n, _T("lights"      ), lightArray  );
  s.setAmbientColor(ambientColor);
  s.setAllMaterials(materialMap );
  s.setAllLights(lightArray     );
  getValue(doc, n, _T("cameras"     ), (D3CameraArray&)s.getCameraArray() );
}
