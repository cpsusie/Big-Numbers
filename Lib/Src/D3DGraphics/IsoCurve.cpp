#include "pch.h"
#include <XMLUtil.h>
#include <MFCUtil/2DXML.h>
#include <D3DGraphics/IsoCurve.h>

IsoCurveParameters::IsoCurveParameters() {
  m_cellSize         = 0.25;
  m_boundingBox      = Rectangle2D(-5,-5,10,10);
  m_machineCode      = true;
  m_includeTime      = false;
  m_tInterval        = DoubleInterval(0,10);
  m_frameCount       = 20;
}

void IsoCurveParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.createRoot(_T("IsoCurve"));
  doc.setValue( root  , _T("expr"        ), m_expr        );
  doc.setValue( root  , _T("cellsize"    ), m_cellSize    );
  setValue(doc, root  , _T("boundingbox" ), m_boundingBox );
  doc.setValue( root  , _T("machinecode" ), m_machineCode );
  doc.setValue( root  , _T("includetime" ), m_includeTime );
  if (m_includeTime) {
    setValue(doc, root, _T("timeinterval"), m_tInterval   );
    doc.setValue( root, _T("framecount"  ), m_frameCount  );
  }
}

void IsoCurveParameters::getDataFromDoc(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("IsoCurve"));
  doc.getValueLF(root, _T("expr"          ), m_expr        );
  doc.getValue(  root, _T("cellsize"      ), m_cellSize    );
  getValue(doc,  root, _T("boundingbox"   ), m_boundingBox );
  doc.getValue(  root, _T("machinecode"   ), m_machineCode );
  doc.getValue(  root, _T("includetime"   ), m_includeTime );
  if (m_includeTime) {
    getValue(doc, root, _T("timeinterval"), m_tInterval   );
    doc.getValue( root, _T("framecount"  ), m_frameCount  );
  }
}
