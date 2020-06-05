#include "pch.h"
#include <MFCUtil/MFCXML.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Scene &s) {
  setValue(doc, n, _T("ambientcolor"), s.getAmbientColor());
  setValue(doc, n, _T("materials"   ), s.getAllMaterials());
  setValue(doc, n, _T("lights"      ), s.getAllLights()   );
  setValue(doc, n, _T("cameras"     ), s.getCameraArray() );

  XMLNodePtr vn = doc.createNode(n, _T("visualObjects"));
  D3VisualIterator it = s.getVisualIterator(OBJMASK_VISUALOBJECT /*| OBJMASK_ANIMATEDOBJECT*/); // TODO
  setValue(doc, vn, it);
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
