#pragma once

#include <XMLDoc.h>
#include "D3Math.h"
#include "Cube3D.h"

void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Point3D     &p   );
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       Point3D     &p   );
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXVECTOR3 &v   );
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       D3DXVECTOR3 &v   );
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const D3DXCube3   &cube);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       D3DXCube3   &cube);
void setValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag, const Cube3D      &cube);
void getValue(XMLDoc &doc, XMLNodePtr parent, const TCHAR *tag,       Cube3D      &cube);
