#include "pch.h"
#include <MFCUtil/MFCXML.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3DLIGHT &v) {
  setValue(doc, n, _T("type"     ), lightTypeToString(v.Type));
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

void getValue(XMLDoc &doc, XMLNodePtr n, D3DLIGHT &v) {
  memset(&v, 0, sizeof(D3DLIGHT));

  String typeStr;
  getValue(doc, n, _T("type"     ), typeStr         );
  v.Type = lightTypeFromString(typeStr);

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
  setValue(doc, n, _T("index"     ), v.getIndex());
  setValue(doc, n, _T("enabled"   ), v.isEnabled());
  setValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3Light &v) {
  int  index;
  bool enabled;
  getValue(doc, n, _T("index"     ), index  );
  getValue(doc, n, _T("enabled"   ), enabled);
  v = D3Light(index);
  v.setEnabled(enabled);
  getValue(doc, n, _T("parameters"), (D3DLIGHT&)v);
}
