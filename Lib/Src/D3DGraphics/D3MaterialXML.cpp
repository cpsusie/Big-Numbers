#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>

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
