#include "pch.h"
#include <D3DGraphics/D3World.h>
#include <D3DGraphics/D3XML.h>

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
