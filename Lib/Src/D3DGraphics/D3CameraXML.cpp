#include "pch.h"
#include <MFCUtil/MFCXML.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3CameraArray.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3Camera &c) {
  setValue(doc, n, _T("righthanded"    ), c.getRightHanded()         );
  setValue(doc, n, _T("nearviewplane"  ), c.getNearViewPlane()       );
  setValue(doc, n, _T("farviewplane"   ), c.getFarViewPlane()        );
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
