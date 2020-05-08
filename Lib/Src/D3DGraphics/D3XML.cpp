#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/2DXML.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3World.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3CameraArray.h>
#include <D3DGraphics/D3Scene.h>

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXVECTOR3 &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), v.x);
  doc.setValue(n, _T("y"), v.y);
  doc.setValue(n, _T("z"), v.z);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXVECTOR3 &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), v.x);
  doc.getValue(n, _T("y"), v.y);
  doc.getValue(n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DVECTOR &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), v.x);
  doc.setValue(n, _T("y"), v.y);
  doc.setValue(n, _T("z"), v.z);
}
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DVECTOR &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), v.x);
  doc.getValue(n, _T("y"), v.y);
  doc.getValue(n, _T("z"), v.z);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXQUATERNION &q) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("x"), q.x);
  doc.setValue(n, _T("y"), q.y);
  doc.setValue(n, _T("z"), q.z);
  doc.setValue(n, _T("w"), q.w);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DXQUATERNION &q) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("x"), q.x);
  doc.getValue(n, _T("y"), q.y);
  doc.getValue(n, _T("z"), q.z);
  doc.getValue(n, _T("w"), q.w);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DCOLORVALUE &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("r"      ), v.r        );
  doc.setValue(n, _T("g"      ), v.g        );
  doc.setValue(n, _T("b"      ), v.b        );
  doc.setValue(n, _T("a"      ), v.a        );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DCOLORVALUE &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  doc.getValue(n, _T("r"      ), v.r        );
  doc.getValue(n, _T("g"      ), v.g        );
  doc.getValue(n, _T("b"      ), v.b        );
  doc.getValue(n, _T("a"      ), v.a        );
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DMATERIAL &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("diffuse"  ), v.Diffuse  );
  setValue(doc, n, _T("ambient"  ), v.Ambient  );
  setValue(doc, n, _T("specular" ), v.Specular );
  setValue(doc, n, _T("emissive" ), v.Emissive );
  doc.setValue( n, _T("power"    ), v.Power    );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DMATERIAL &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  memset(&v, 0, sizeof(D3DMATERIAL));
  getValue(doc, n, _T("diffuse"  ), v.Diffuse  );
  getValue(doc, n, _T("ambient"  ), v.Ambient  );
  getValue(doc, n, _T("specular" ), v.Specular );
  getValue(doc, n, _T("emissive" ), v.Emissive );
  doc.getValue( n, _T("power"    ), v.Power    );
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const MaterialMap &map) {
  XMLNodePtr n = doc.createNode(parent, tag);
  for(Iterator<Entry<CompactUIntKeyType, D3Material> > it = map.getEntryIterator(); it.hasNext();) {
    const Entry<CompactUIntKeyType, D3Material> &e = it.next();
    String id = format(_T("id%d"), e.getKey());
    setValue(doc, n, id.cstr(), e.getValue());
  }
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, MaterialMap &map) {
  XMLNodePtr n = doc.getChild(parent, tag);
  map.clear();
  for(XMLNodePtr child = n->firstChild; child; child = child->nextSibling) {
    const String idStr = (wchar_t*)child->nodeName;
    const UINT   id    = _wtoi(idStr.cstr()+2);
    D3DMATERIAL  d3m;
    getValue(doc, n, idStr.cstr(), d3m);
    D3Material   mat(id);
    mat = d3m;
    map.put(id, mat);
  }
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DLIGHT &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue( n, _T("type"     ), toString(v.Type));
  setValue(doc, n, _T("diffuse"  ), v.Diffuse       );
  setValue(doc, n, _T("ambient"  ), v.Ambient       );
  setValue(doc, n, _T("specular" ), v.Specular      );

  switch(v.Type) {
  case D3DLIGHT_DIRECTIONAL:
    setValue(doc, n, _T("direction"   ), v.Direction   );
    break;
  case D3DLIGHT_POINT      :
    setValue(doc, n, _T("position"    ), v.Position    );
    doc.setValue( n, _T("range"       ), v.Range       );
    doc.setValue( n, _T("attenuation0"), v.Attenuation0);
    doc.setValue( n, _T("attenuation1"), v.Attenuation1);
    doc.setValue( n, _T("attenuation2"), v.Attenuation2);
    break;
  case D3DLIGHT_SPOT       :
    setValue(doc, n, _T("position"    ), v.Position    );
    setValue(doc, n, _T("direction"   ), v.Direction   );
    doc.setValue( n, _T("range"       ), v.Range       );
    doc.setValue( n, _T("attenuation0"), v.Attenuation0);
    doc.setValue( n, _T("attenuation1"), v.Attenuation1);
    doc.setValue( n, _T("attenuation2"), v.Attenuation2);
    doc.setValue( n, _T("falloff"     ), v.Falloff     );
    doc.setValue( n, _T("theta"       ), v.Theta       );
    doc.setValue( n, _T("phi"         ), v.Phi         );
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

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3DLIGHT &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  String typeStr;
  doc.getValue( n, _T("type"     ), typeStr         );
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
    doc.getValue( n, _T("range"       ), v.Range       );
    doc.getValue( n, _T("attenuation0"), v.Attenuation0);
    doc.getValue( n, _T("attenuation1"), v.Attenuation1);
    doc.getValue( n, _T("attenuation2"), v.Attenuation2);
    break;
  case D3DLIGHT_SPOT       :
    getValue(doc, n, _T("position"    ), v.Position    );
    getValue(doc, n, _T("direction"   ), v.Direction   );
    doc.getValue( n, _T("range"       ), v.Range       );
    doc.getValue( n, _T("attenuation0"), v.Attenuation0);
    doc.getValue( n, _T("attenuation1"), v.Attenuation1);
    doc.getValue( n, _T("attenuation2"), v.Attenuation2);
    doc.getValue( n, _T("falloff"     ), v.Falloff     );
    doc.getValue( n, _T("theta"       ), v.Theta       );
    doc.getValue( n, _T("phi"         ), v.Phi         );
    break;
  }
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3Light &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("enabled"), v.isEnabled());
  setValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3Light &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  bool enabled;
  doc.getValue(n, _T("enabled"), enabled);
  v.setEnabled(enabled);
  getValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const LightArray &a) {
  XMLNodePtr n = doc.createNode(parent, tag);
  for(size_t i = 0; i < a.size(); i++) {
    const D3Light &light = a[i];
    String tagName = format(_T("index%d"), light.getIndex());
    setValue(doc, n, tagName.cstr(), light);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, LightArray &a) {
  XMLNodePtr n = doc.getChild(parent, tag);
  a.clear(-1);
  for(XMLNodePtr child = n->firstChild; child; child = child->nextSibling) {
    const String idStr = (wchar_t*)child->nodeName;
    int index;
    _stscanf(idStr.cstr(), _T("index%d"), &index);
    D3Light light(index);
    getValue(doc, n, idStr.cstr(), light);
    a.add(light);
  }
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3World &w) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("pos"        ), w.getPos()        );
  setValue(doc, n, _T("orientation"), w.getOrientation());
  setValue(doc, n, _T("scale"      ), w.getScale()      );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3World &w) {
  XMLNodePtr     n = doc.getChild(parent, tag);
  D3DXVECTOR3    pos, scale;
  D3DXQUATERNION q;
  getValue(doc, n, _T("pos"        ), pos  );
  getValue(doc, n, _T("orientation"), q    );
  getValue(doc, n, _T("scale"      ), scale);
  w.setPos(pos).setOrientation(q).setScale(scale);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3Camera &c) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue( n, _T("righthanded"    ), c.getRightHanded()         );
  doc.setValue( n, _T("nearviewplane"  ), c.getNearViewPlane()       );
  doc.setValue( n, _T("viewangle"      ), c.getViewAngle()           );
  setValue(doc, n, _T("winsize"        ), c.getWinSize()             );
  setValue(doc, n, _T("world"          ), c.getD3World()             );
  setValue(doc, n, _T("backgroundcolor"), c.getBackgroundColor()     );
  setValue(doc, n, _T("visiblelights"  ), c.getLightControlsVisible());
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3Camera &c) {
  bool       rightHanded;
  float      nearViewPlane, viewAngle;
  CSize      winSize;
  D3World    world;
  D3PCOLOR   backgroundColor;
  BitSet     lightControlsVisible(10);
  XMLNodePtr n = doc.getChild(parent, tag);

  doc.getValue( n, _T("righthanded"    ), rightHanded         );
  doc.getValue( n, _T("nearviewplane"  ), nearViewPlane       );
  doc.getValue( n, _T("viewangle"      ), viewAngle           );
  getValue(doc, n, _T("winsize"        ), winSize             );
  getValue(doc, n, _T("world"          ), world               );
  getValue(doc, n, _T("backgroundcolor"), backgroundColor     );
  getValue(doc, n, _T("visiblelights"  ), lightControlsVisible);

  c.setRightHanded(  rightHanded  );
  c.setNearViewPlane(nearViewPlane);
  c.setViewAngle(    viewAngle    );
  setWindowSize(c.getHwnd(), winSize);
  c.setD3World(world);
  c.setBackgroundColor(backgroundColor);
  c.setLightControlsVisible(lightControlsVisible);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3CameraArray &a) {
  XMLNodePtr n = doc.createNode(parent, tag);
  const UINT count = (UINT)a.size();
  doc.setValue(n, _T("count"), count);
  for(UINT i = 0; i < count; i++) {
    const String id = format(_T("cam%u"), i);
    setValue(doc, n, id.cstr(), *a[i]);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3CameraArray &a) {
  XMLNodePtr n = doc.getChild(parent, tag);
  UINT count;
  doc.getValue(n, _T("count"), count);
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

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3Scene &s) {
  XMLNodePtr n = doc.createNode(parent, tag);
  setValue(doc, n, _T("ambientcolor"), s.getAmbientColor());
  setValue(doc, n, _T("materials"   ), s.getAllMaterials());
  setValue(doc, n, _T("lights"      ), s.getAllLights()   );
  setValue(doc, n, _T("cameras"     ), s.getCameraArray() );
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, D3Scene &s) {
  XMLNodePtr  n = doc.getChild(parent, tag);
  D3PCOLOR    ambientColor;
  MaterialMap materialMap;
  LightArray  lightArray;
  getValue(doc, n, _T("ambientcolor"), ambientColor);
  s.setAmbientColor(ambientColor);
  getValue(doc, n, _T("materials"   ), materialMap );
  s.setAllMaterials(materialMap);
  getValue(doc, n, _T("lights"      ), lightArray  );
  s.setAllLights(lightArray);
  getValue(doc, n, _T("cameras"     ), (D3CameraArray&)s.getCameraArray() );
  
}
