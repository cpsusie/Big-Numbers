#include "pch.h"
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/D3XML.h>

void ExprFunctionR2R1SurfaceParameters::putDataToDoc(XMLDoc &doc) {
  XMLNodePtr       root = doc.createRoot(_T("Function2DSurface"));
  setValue(doc,    root, _T("expr"       ), m_expr            );
  setValue(doc,    root, _T("xinterval"  ), m_xInterval       );
  setValue(doc,    root, _T("yinterval"  ), m_yInterval       );
  setValue(doc,    root, _T("pointcount" ), m_pointCount      );
  setValue(doc,    root, _T("machinecode"), m_machineCode     );
  setValue(doc,    root, _T("doublesided"), m_doubleSided     );
  setValue(doc,    root, _T("common"     ), *(D3SurfaceCommonParameters*)this);
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDoc(XMLDoc &doc) {
  try {
    XMLNodePtr       root = doc.getRoot();
    XMLDoc::checkTag(root, _T("Function2DSurface"));
    getValueLF(doc,  root, _T("expr"       ), m_expr            );
    getValue(doc,    root, _T("xinterval"  ), m_xInterval       );
    getValue(doc,    root, _T("yinterval"  ), m_yInterval       );
    getValue(doc,    root, _T("pointcount" ), m_pointCount      );
    getValue(doc,    root, _T("machinecode"), m_machineCode     );
    getValue(doc,    root, _T("common"     ), *(D3SurfaceCommonParameters*)this);
  } catch(...) {
    getDataFromDocOld1(doc);
  }
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDocOld1(XMLDoc &doc) {
  try {
    XMLNodePtr       root = doc.getRoot();
    XMLDoc::checkTag(root, _T("Function2DSurface"));
    getValueLF(doc,  root, _T("expr"       ), m_expr       );
    getValue(doc,    root, _T("xinterval"  ), m_xInterval  );
    getValue(doc,    root, _T("yinterval"  ), m_yInterval  );
    getValue(doc,    root, _T("pointcount" ), m_pointCount );
    getValue(doc,    root, _T("machinecode"), m_machineCode);
  } catch(...) {
    getDataFromDocOld2(doc);
  }
}

void ExprFunctionR2R1SurfaceParameters::getDataFromDocOld2(XMLDoc &doc) {
  XMLNodePtr       root = doc.getRoot();
  XMLDoc::checkTag(root, _T("Function2DSurface"));
  getValueLF(doc,  root, _T("expr"       ), m_expr       );
  getValue(doc,    root, _T("xinterval"  ), m_xInterval  );
  getValue(doc,    root, _T("yinterval"  ), m_yInterval  );
  getValue(doc,    root, _T("pointcount" ), m_pointCount );
  getValue(doc,    root, _T("machinecode"), m_machineCode);
}
