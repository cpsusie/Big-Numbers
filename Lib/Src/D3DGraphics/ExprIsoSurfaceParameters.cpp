#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/D3XML.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>

void ExprIsoSurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("IsoSurface"));
  setValue(doc,    root, _T("expr"              ), m_expr            );
  setValue(doc,    root, _T("cellsize"          ), m_cellSize        );
  setValue(doc,    root, _T("lambda"            ), m_lambda          );
  setValue(doc,    root, _T("boundingbox"       ), m_boundingBox     );
  setValue(doc,    root, _T("tetrahedral"       ), m_tetrahedral     );
  setValue(doc,    root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  setValue(doc,    root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  setValue(doc,    root, _T("originoutside"     ), m_originOutside   );
  setValue(doc,    root, _T("machinecode"       ), m_machineCode     );
  setValue(doc,    root, _T("doublesided"       ), m_doubleSided     );
  setValue(doc,    root, _T("animation"         ), m_animation       );
}

void ExprIsoSurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr       root = doc.getRoot();
    XMLDoc::checkTag(root, _T("IsoSurface"));
    getValueLF(doc,  root, _T("expr"              ), m_expr            );
    getValue(doc,    root, _T("cellsize"          ), m_cellSize        );
    getValue(doc,    root, _T("lambda"            ), m_lambda, 0.25    );
    getValue(doc,    root, _T("boundingbox"       ), m_boundingBox     );
    getValue(doc,    root, _T("tetrahedral"       ), m_tetrahedral     );
    getValue(doc,    root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
    getValue(doc,    root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
    getValue(doc,    root, _T("originoutside"     ), m_originOutside   );
    getValue(doc,    root, _T("machinecode"       ), m_machineCode     );
    getValue(doc,    root, _T("doublesided"       ), m_doubleSided     );
    getValue(doc,    root, _T("animation"         ), m_animation       );
  } catch(...) {
    getDataFromDocOld(doc);
  }
}

void ExprIsoSurfaceParameters::getDataFromDocOld(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("IsoSurface"));
  getValueLF(doc,  root, _T("expr"              ), m_expr            );
  getValue(doc,    root, _T("cellsize"          ), m_cellSize        );
  getValue(doc,    root, _T("lambda"            ), m_lambda,  0.25   );
  getValue(doc,    root, _T("boundingbox"       ), m_boundingBox     );
  getValue(doc,    root, _T("tetrahedral"       ), m_tetrahedral     );
  getValue(doc,    root, _T("tetraoptimize4"    ), m_tetraOptimize4  );
  getValue(doc,    root, _T("adaptivecellsize"  ), m_adaptiveCellSize);
  getValue(doc,    root, _T("originoutside"     ), m_originOutside   );
  getValue(doc,    root, _T("machinecode"       ), m_machineCode     );
  getValue(doc,    root, _T("doublesided"       ), m_doubleSided     );
  getValue(doc,    root, _T("includetime"       ), m_animation.m_includeTime );
  if(m_animation.m_includeTime) {
    getValue(doc,  root, _T("timeinterval"      ), m_animation.m_timeInterval);
    getValue(doc,  root, _T("framecount"        ), m_animation.m_frameCount  );
  }
}
