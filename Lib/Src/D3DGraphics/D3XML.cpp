#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/D3ToString.h>

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
  getValue(doc, n, _T("diffuse"  ), v.Diffuse  );
  getValue(doc, n, _T("ambient"  ), v.Ambient  );
  getValue(doc, n, _T("specular" ), v.Specular );
  getValue(doc, n, _T("emissive" ), v.Emissive );
  doc.getValue( n, _T("power"    ), v.Power    );
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const MaterialMap &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  for(Iterator<Entry<CompactUIntKeyType, MATERIAL> > it = v.getEntryIterator(); it.hasNext();) {
    const Entry<CompactUIntKeyType, MATERIAL> &e = it.next();
    String id = format(_T("id%d"), e.getKey());
    setValue(doc, n, id.cstr(), e.getValue());
  }
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, MaterialMap &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  v.clear();
  for(XMLNodePtr child = n->firstChild; child; child = child->nextSibling) {
    const String idStr = (wchar_t*)child->nodeName;
    const UINT   id    = _wtoi(idStr.cstr()+2);
    D3DMATERIAL  d3m;
    getValue(doc, n, idStr.cstr(), d3m);
    MATERIAL     mat(id);
    mat = d3m;
    v.put(id, mat);
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

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const LIGHT &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  doc.setValue(n, _T("enabled"), v.isEnabled());
  setValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, LIGHT &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  bool enabled;
  doc.getValue(n, _T("enabled"), enabled);
  v.setEnabled(enabled);
  getValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const LightArray &v) {
  XMLNodePtr n = doc.createNode(parent, tag);
  for(size_t i = 0; i < v.size(); i++) {
    String tagName = format(_T("index%zu"), i);
    setValue(doc, n, tagName.cstr(), v[i]);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, LightArray &v) {
  XMLNodePtr n = doc.getChild(parent, tag);
  v.clear(-1);
  for(XMLNodePtr child = n->firstChild; child; child = child->nextSibling) {
    const String idStr = (wchar_t*)child->nodeName;
    LIGHT light((UINT)v.size());
    getValue(doc, n, idStr.cstr(), light);
    v.add(light);
  }
}
