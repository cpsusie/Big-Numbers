#include "pch.h"
#include <Math/MathXML.h>
#include <MFCUtil/MFCXml.h>
#include <D3DGraphics/IsoSurfaceParameters.h>
#include <D3DGraphics/D3XML.h>

static void getValueOld3(XMLDoc &doc, XMLNodePtr n, IsoSurfaceParameters                   &v) {
  getValue(  doc, n, _T("cellsize"         ), v.m_cellSize        );
  getValue(  doc, n, _T("lambda"           ), v.m_lambda, 0.25    );
  getValue(  doc, n, _T("boundingbox"      ), v.m_boundingBox     );
  getValue(  doc, n, _T("tetrahedral"      ), v.m_tetrahedral     );
  getValue(  doc, n, _T("tetraoptimize4"   ), v.m_tetraOptimize4  );
  getValue(  doc, n, _T("originoutside"    ), v.m_originOutside   );
  getValue(  doc, n, _T("doublesided"      ), v.m_doubleSided     );
  getValue(  doc, n, _T("animation"        ), v.m_animation       );
}

static void getValueOld2(XMLDoc &doc, XMLNodePtr n, IsoSurfaceParameters                   &v) {
  try {
    getValue( doc, n, _T("cellsize"          ), v.m_cellSize                );
    getValue( doc, n, _T("lambda"            ), v.m_lambda, 0.25            );
    getValue( doc, n, _T("boundingbox"       ), v.m_boundingBox             );
    getValue( doc, n, _T("tetrahedral"       ), v.m_tetrahedral             );
    getValue( doc, n, _T("tetraoptimize4"    ), v.m_tetraOptimize4          );
    getValue( doc, n, _T("originoutside"     ), v.m_originOutside           );
    getValue( doc, n, _T("doublesided"       ), v.m_doubleSided             );
    getValue( doc, n, _T("includetime"       ), v.m_animation.m_includeTime );
    if(v.m_animation.m_includeTime) {
      getValue(doc, n, _T("timeinterval"      ), v.m_animation.m_timeInterval);
      getValue(doc, n, _T("framecount"        ), v.m_animation.m_frameCount  );
    }
  } catch(Exception e) {
    getValueOld3(doc, n, v);
  }
}

void getValueOld1(XMLDoc &doc, XMLNodePtr n, IsoSurfaceParameters                   &v) {
  try {
    getValue(  doc,  n, _T("cellsize"          ), v.m_cellSize                       );
    getValue(  doc,  n, _T("lambda"            ), v.m_lambda,  0.25                  );
    getValue(  doc,  n, _T("boundingbox"       ), v.m_boundingBox                    );
    getValue(  doc,  n, _T("tetrahedral"       ), v.m_tetrahedral                    );
    getValue(  doc,  n, _T("tetraoptimize4"    ), v.m_tetraOptimize4                 );
    getValue(  doc,  n, _T("originoutside"     ), v.m_originOutside                  );
    getValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
    v.m_showStatistics = false;
  } catch(Exception e) {
    getValueOld2(doc, n, v);
  }
}

void getValue(XMLDoc &doc, XMLNodePtr n, IsoSurfaceParameters                      &v) {
  try {
    getValue(doc,  n, _T("cellsize"          ), v.m_cellSize                       );
    getValue(doc,  n, _T("lambda"            ), v.m_lambda, 0.25                   );
    getValue(doc,  n, _T("boundingbox"       ), v.m_boundingBox                    );
    getValue(doc,  n, _T("tetrahedral"       ), v.m_tetrahedral                    );
    getValue(doc,  n, _T("tetraoptimize4"    ), v.m_tetraOptimize4                 );
    getValue(doc,  n, _T("showStatistics"    ), v.m_showStatistics                 );
    getValue(doc,  n, _T("originoutside"     ), v.m_originOutside                  );
    getValue(doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v      );
  } catch(Exception e) {
    getValueOld1(doc, n, v);
  }
}


void setValue(XMLDoc &doc, XMLNodePtr n, const IsoSurfaceParameters                &v) {
  setValue(  doc,  n, _T("cellsize"          ), v.m_cellSize                       );
  setValue(  doc,  n, _T("lambda"            ), v.m_lambda                         );
  setValue(  doc,  n, _T("boundingbox"       ), v.m_boundingBox                    );
  setValue(  doc,  n, _T("tetrahedral"       ), v.m_tetrahedral                    );
  setValue(  doc,  n, _T("tetraoptimize4"    ), v.m_tetraOptimize4                 );
  setValue(  doc,  n, _T("showStatistics"    ), v.m_showStatistics                 );
  setValue(  doc,  n, _T("originoutside"     ), v.m_originOutside                  );
  setValue(  doc,  n, _T("common"            ), (D3SurfaceCommonParameters&)v);
}

