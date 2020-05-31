#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/MFCXML.h>
#include <MFCUtil/IsoCurveParameters.h>

IsoCurveParameters::IsoCurveParameters() {
  m_cellSize    = 0.25;
  m_boundingBox = Rectangle2D(-5, -5, 10, 10);
  m_machineCode = true;
  m_animation.reset();
}

void IsoCurveParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr    root = doc.createRoot(_T("IsoCurve"));
  setValue(doc, root, _T("expr"       ), m_expr       );
  setValue(doc, root, _T("cellsize"   ), m_cellSize   );
  setValue(doc, root, _T("boundingbox"), m_boundingBox);
  setValue(doc, root, _T("machinecode"), m_machineCode);
  setValue(doc, root, _T("animation"  ), m_animation  );
}

void IsoCurveParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr       root = doc.getRoot();
    XMLDoc::checkTag(root, _T("IsoCurve"));
    getValueLF(doc,  root, _T("expr"       ), m_expr       );
    getValue(  doc,  root, _T("cellsize"   ), m_cellSize   );
    getValue(  doc,  root, _T("boundingbox"), m_boundingBox);
    getValue(  doc,  root, _T("machinecode"), m_machineCode);
    getValue(  doc,  root, _T("animation"  ), m_animation  );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void IsoCurveParameters::getDataFromDocOld(XMLDoc &doc) {
  XMLNodePtr        root = doc.getRoot();
  XMLDoc::checkTag( root, _T("IsoCurve"));
  getValueLF(  doc, root, _T("expr"         ), m_expr        );
  getValue(    doc, root, _T("cellsize"     ), m_cellSize    );
  getValue(    doc, root, _T("boundingbox"  ), m_boundingBox );
  getValue(    doc, root, _T("machinecode"  ), m_machineCode );
  getValue(    doc, root, _T("includetime"  ), m_animation.m_includeTime );
  if(m_animation.m_includeTime) {
    getValue(  doc, root, _T("timeinterval"), m_animation.m_timeInterval);
    getValue(  doc, root, _T("framecount"  ), m_animation.m_frameCount  );
  }
}
