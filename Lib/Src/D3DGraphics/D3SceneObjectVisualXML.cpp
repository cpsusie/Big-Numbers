#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3XML.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const D3SceneObjectVisual &v) {
  setValue(doc, n, _T("name"           ), v.getName()          );
  setValue(doc, n, _T("world"          ), D3World(v.getWorld()));
  setValue(doc, n, _T("visible"        ), v.isVisible()        );
  setValue(doc, n, _T("hasfillmode"    ), v.hasFillMode()      );
  setValue(doc, n, _T("hasshademode"   ), v.hasShadeMode()     );
  setValue(doc, n, _T("hastextureid"   ), v.hasTextureId()     );
  if(v.hasFillMode()) {
    setValue(doc, n, _T("fillmode"       ), fillModeToString(v.getFillMode()));
  }
  if(v.hasShadeMode()) {
    setValue(doc, n, _T("hasshademode"   ), shadeModeToString(v.getShadeMode()));
  }
  if(v.hasMaterial()) {
    setValue(doc, n, _T("materialid"), v.getMaterialId());
  }
  if(v.hasTextureId()) {
    setValue(doc, n, _T("textureid"), v.getTextureId());
  }
  setValue(doc, n, _T("hasmesh"        ), v.hasMesh()          );
  if(v.hasMesh()) {
    setValue(doc, n, _T("mesh"), v.getMesh());
  } else {
    setValue(doc, n, _T("hasvertexbuffer"), v.hasVertexBuffer()  );
    if(v.hasVertexBuffer()) {
      setValue(doc, n, _T("vertexbuffer"), v.getVertexBuffer());
    }
    setValue(doc, n, _T("hasindexbuffer"), v.hasIndexBuffer()  );
    if(v.hasIndexBuffer()) {
      setValue(doc, n, _T("indexbuffer"), v.getIndexBuffer());
    }
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, D3SceneObjectVisual &s) {
  // TODO
}
