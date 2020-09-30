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

void setValue(XMLDoc &doc, XMLNodePtr n, const D3MATERIAL &v) {
  setValue(doc, n, (const D3DMATERIAL&)v);
  setValue(doc, n, _T("specularhighlights"), v.m_specularHighlights);
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3MATERIAL &v) {
  getValue(doc, n, (D3DMATERIAL&)v);
  getValue(doc, n, _T("specularhighlights"), v.m_specularHighlights);
}

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Material &v) {
  setValue(doc, n, _T("id"), v.getId());
  if(v.isDefined()) {
    setValue(doc, n, (const D3MATERIAL &)v);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3Material &v) {
  int id;
  getValue(doc, n, _T("id"), id);
  if(id < 0) {
    v.setUndefined();
  } else {
    D3MATERIAL m;
    getValue(doc, n, m);
    v = D3Material(id);
    v = m;
  }
}
