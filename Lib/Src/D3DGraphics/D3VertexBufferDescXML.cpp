#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3Xml.h>

void setValue(XMLDoc &doc, XMLNodePtr n, const struct _D3DVERTEXBUFFER_DESC &d) {
  setValue(doc, n, _T("type"  ), resourceTypeToString(d.Type  ));
  setValue(doc, n, _T("format"), formatToString(      d.Format));
  setValue(doc, n, _T("pool"  ), poolToString(        d.Pool  ));
  setValue(doc, n, _T("usage" ), usageToString(       d.Usage ));
  setValue(doc, n, _T("fvf"   ), FVFToString(         d.FVF   ));
  setValue(doc, n, _T("size"  ),                      d.Size  );
}

void getValue(XMLDoc &doc, XMLNodePtr n, struct _D3DVERTEXBUFFER_DESC &d) {
  String typeStr, formatStr, poolStr, usageStr, fvfStr;
  getValue(doc, n, _T("type"  ), typeStr  );
  getValue(doc, n, _T("format"), formatStr);
  getValue(doc, n, _T("pool"  ), poolStr  );
  getValue(doc, n, _T("usage" ), usageStr );
  getValue(doc, n, _T("fvf"   ), fvfStr   );
  getValue(doc, n, _T("size"  ), d.Size   );

  d.Type   = resourceTypeFromString(typeStr  );
  d.Format = formatFromString(      formatStr);
  d.Pool   = poolFromString(        poolStr  );
  d.Usage  = usageFromString(       usageStr );
  d.FVF    = FVFFromString(         fvfStr   );
}
