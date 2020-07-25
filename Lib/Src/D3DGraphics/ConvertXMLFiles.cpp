#include "pch.h"
#include <Scandir.h>
#include <FileNameSplitter.h>
#include <FileContent.h>
#include <D3DGraphics/MeshCreators.h>
#include <D3DGraphics/ExprFunctionR2R1SurfaceParameters.h>
#include <D3DGraphics/ExprParametricR1R3SurfaceParameters.h>
#include <D3DGraphics/ExprParametricR2R3SurfaceParameters.h>
#include <D3DGraphics/ExprIsoSurfaceParameters.h>

static void convertXMLFile(const String &fileName) {
  String errorMsg;

  try {
    ExprIsoSurfaceParameters data;
    data.load(fileName);
    data.save(fileName);
    return;
  } catch(Exception e) {
    errorMsg = e.what();
    errorMsg += "\n";
  }
  try {
    ExprParametricR2R3SurfaceParameters data;
    data.load(fileName);
    errorMsg = EMPTYSTRING;
    data.save(fileName);
    return;
  } catch(Exception e) {
    errorMsg += e.what();
    errorMsg += "\n";
  }
  try {
    ExprFunctionR2R1SurfaceParameters data;
    data.load(fileName);
    errorMsg = EMPTYSTRING;
    data.save(fileName);
    return;
  } catch(Exception e) {
    errorMsg += e.what();
    errorMsg += "\n";
  }
  try {
    ExprParametricR1R3SurfaceParameters data;
    data.load(fileName);
    errorMsg = EMPTYSTRING;
    data.save(fileName);
    return;
  } catch(Exception e) {
    errorMsg += e.what();
    errorMsg += "\n";
  }
  throwException(errorMsg);
}

static String convertAllFiles(const String &dir, const String &pattern) {
  const DirList list = scandir(FileNameSplitter::getChildName(dir,pattern), SELECTFILE);
  String msg;
  UINT okCount = 0;
  for(size_t i = 0; i < list.size(); i++) {
    const String fileName = FileNameSplitter::getChildName(dir,list[i].name);
    try {
      const FileContent oldContent(fileName);
      convertXMLFile(fileName);
      const FileContent newContent(fileName);
      if(newContent != oldContent) {
        msg += format(_T("%s converted ok\n"), fileName.cstr());
      } else {
        okCount++;
      }
    } catch(Exception e) {
      msg += format(_T("Error converting %s\n%s\n"), fileName.cstr(), indentString(e.what(),4).cstr());
    }
  }
  msg += format(_T("%u %s-files of %zu ok\n"), okCount, pattern.cstr(), list.size());
  return msg;
}

String D3convertAllXMLFiles(const String &dir) {
  String msg;
  msg =  convertAllFiles(dir, _T("*.par"));
  msg += convertAllFiles(dir, _T("*.imp"));
  msg += convertAllFiles(dir, _T("*.exp"));
  return msg;
}
