#include "pch.h"
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>

void ExprIsoSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("IsoSurface"));
  doc.setValue(    root, _T("expr"              ), m_expr            );
  doc.setValue(    root, _T("cellsize"          ), m_cellSize        );
  doc.setValue(    root, _T("lambda"            ), m_lambda          );
  setValue( doc,   root, _T("boundingbox"       ), m_boundingBox     );
  doc.setValue(    root, _T("tetrahedral"       ), m_tetrahedral     );
  doc.setValue(    root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  doc.setValue(    root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  doc.setValue(    root, _T("originoutside"     ), m_originOutside   );
  doc.setValue(    root, _T("machinecode"       ), m_machineCode     );
  doc.setValue(    root, _T("doublesided"       ), m_doubleSided     );
  setValue(doc,    root, _T("animation"         ), m_animation       );
}

void ExprIsoSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr     root = doc.getRoot();
    checkTag(      root, _T("IsoSurface"));
    doc.getValueLF(root, _T("expr"              ), m_expr            );
    doc.getValue(  root, _T("cellsize"          ), m_cellSize        );
    m_lambda = doc.getValueOrDefault(root, _T("lambda"), 0.25);
    getValue(doc,  root, _T("boundingbox"       ), m_boundingBox     );
    doc.getValue(  root, _T("tetrahedral"       ), m_tetrahedral     );
    doc.getValue(  root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
    doc.getValue(  root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
    doc.getValue(  root, _T("originoutside"     ), m_originOutside   );
    doc.getValue(  root, _T("machinecode"       ), m_machineCode     );
    doc.getValue(  root, _T("doublesided"       ), m_doubleSided     );
    getValue(doc,  root, _T("animation"         ), m_animation       );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void ExprIsoSurfaceParameters::getDataFromDocOld(XMLDoc &doc) {
  XMLNodePtr root = doc.getRoot();
  checkTag(root, _T("IsoSurface"));
  doc.getValueLF( root, _T("expr"              ), m_expr            );
  doc.getValue(   root, _T("cellsize"          ), m_cellSize        );
  m_lambda = doc.getValueOrDefault(root, _T("lambda"), 0.25);
  getValue( doc,  root, _T("boundingbox"       ), m_boundingBox     );
  doc.getValue(   root, _T("tetrahedral"       ), m_tetrahedral     );
  doc.getValue(   root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  doc.getValue(   root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  doc.getValue(   root, _T("originoutside"     ), m_originOutside   );
  doc.getValue(   root, _T("machinecode"       ), m_machineCode     );
  doc.getValue(   root, _T("doublesided"       ), m_doubleSided     );
  doc.getValue(   root, _T("includetime"       ), m_animation.m_includeTime );
  if(m_animation.m_includeTime) {
    getValue(doc, root, _T("timeinterval"      ), m_animation.m_timeInterval);
    doc.getValue( root, _T("framecount"        ), m_animation.m_frameCount  );
  }
}
