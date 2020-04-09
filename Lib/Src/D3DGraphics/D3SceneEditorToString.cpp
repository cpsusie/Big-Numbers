#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3ToString.h>

String toString(D3EditorControl control) {
  switch(control) {
#define caseStr(s) case CONTROL_##s: return _T(#s);
  caseStr(IDLE                   )
  caseStr(CAMERA_WALK            )
  caseStr(CAMERA_PROJECTION      )
  caseStr(OBJECT_POS             )
  caseStr(OBJECT_SCALE           )
  caseStr(LIGHT                  )
  caseStr(SPOTLIGHTPOINT         )
  caseStr(SPOTLIGHTANGLES        )
  caseStr(ANIMATION_SPEED        )
  caseStr(MATERIAL               )
  caseStr(LIGHTCOLOR             )
  caseStr(BACKGROUNDCOLOR        )
  caseStr(AMBIENTLIGHTCOLOR      )
  default: return _T("?");
  }
}

String D3SceneEditor::stateFlagsToString() const {
  const TCHAR *delim = NULL;
  String result = _T("(");
#define ADDFLAG(f) { if(isSet(SE_##f)) { if(delim) result += delim; else delim=_T(","); result += _T(#f); } }
  ADDFLAG(INITDONE     );
  ADDFLAG(ENABLED      );
  ADDFLAG(PROPCHANGES  );
  ADDFLAG(RENDER3D     );
  ADDFLAG(RENDERINFO   );
  ADDFLAG(LIGHTCONTROLS);
  ADDFLAG(MOUSEVISIBLE );
  result += _T(")");
  return result;
}

static String getMeshString(LPD3DXMESH mesh) {
  return ::toString(mesh, FORMAT_BUFFERDESC | FORMAT_VERTEXBUFFER)
       + "\n"
       + ::toString(mesh, FORMAT_BUFFERDESC | FORMAT_INDEXBUFFER);
}

#define HEADLEN 18

String D3SceneEditor::getSelectedString() const {
  if(!hasObj()) {
    return _T("/");
  } else {
    const D3SceneObjectVisual &visual = *getCurrentObj();
    String result = visual.getName();
    if(visual.hasFillMode()) {
      result += " ";
      result += ::toString(visual.getFillMode());
    }
    if(visual.hasShadeMode()) {
      result += " ";
      result += ::toString(visual.getShadeMode());
    }
    if(visual.hasMesh()) {
      if(result.length()) result += "\n";
      result += indentString(getMeshString(visual.getMesh()),HEADLEN+2);
    }
    return result.trimRight();
  }
}

#define PRINTHEAD(label) result += format(_T("%-*s: "), HEADLEN, label)
#define NEWLINE(label)   result += _T("\n"); PRINTHEAD(label)
String D3SceneEditor::toString() const {
  if(!isEnabled()) return EMPTYSTRING;
  String result;
  PRINTHEAD(_T("Current Control"))
         + format(_T("%s State:%s")
                 ,::toString(getCurrentControl()).cstr()
                 ,stateFlagsToString().cstr()
                 );
  NEWLINE(_T("Current Object")) + getSelectedString();
  NEWLINE(_T("Current Camera"))
       + format(_T("%d, %s")
               ,m_selectedCameraIndex
               ,hasCAM() ? getSelectedCAM()->toString().cstr() : _T("/")
               );
  if(!m_pickedRay.isEmpty()) {
    NEWLINE(_T("Picked Ray" )) + m_pickedRay.toString();
    NEWLINE(_T("Picked Info")) + m_pickedInfo.toString();
  }
  if(!m_centerOfRotation.isEmpty()) {
    NEWLINE(_T("Center of rotation")) + m_centerOfRotation.toString();
  }
  switch(m_propertyDialogMap.getVisiblePropertyId()) {
  case SP_LIGHTPARAMETERS   :
    { D3Light tmp;
      NEWLINE(_T("Dlg-Light")) + m_propertyDialogMap.getProperty(SP_LIGHTPARAMETERS,tmp).toString();
    }
    break;
  case SP_MATERIALPARAMETERS:
    { D3Material tmp;
      NEWLINE(_T("Dlg-Material")) + m_propertyDialogMap.getProperty(SP_MATERIALPARAMETERS,tmp).toString();
    }
    break;
  }

  switch(getCurrentControl()) {
  case CONTROL_IDLE                  :
    result += _T("\n") + handednessToString(getScene().getRightHanded());
    break;
  case CONTROL_CAMERA_WALK           :
    if(hasCAM()) {
      NEWLINE(_T("Camera World")) + getSelectedCAM()->getD3World().toString();
    }
    break;
  case CONTROL_OBJECT_POS            :
  case CONTROL_OBJECT_SCALE          :
    if(hasObj()) {
      NEWLINE(_T("Object")) + D3World(getCurrentObj()->getWorld()).toString();
    }
    break;

  case CONTROL_LIGHTCOLOR            :
  case CONTROL_LIGHT                 :
  case CONTROL_SPOTLIGHTPOINT        :
  case CONTROL_SPOTLIGHTANGLES       :
    { const D3LightControl *lc = getCurrentLightControl();
      if(lc) {
        const D3Light light = lc->getLight();
        NEWLINE(light.getName().cstr()) + light.toString();
        NEWLINE(_T("LightControl")) + ((D3World)*lc).toString();
      }
    }
    break;
  case CONTROL_ANIMATION_SPEED       :
    { D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
      if(obj) {
        NEWLINE(_T("Frames/sec")) + format(_T("%.2f"), obj->getFramePerSec());
      }
    }
    break;
  case CONTROL_MATERIAL              :
    if(m_currentObj && m_currentObj->hasMaterial()) {
      NEWLINE(_T("Material"))     + m_currentObj->getMaterial().toString();
    }
    break;

  case CONTROL_BACKGROUNDCOLOR       :
    if(hasCAM()) {
      NEWLINE(_T("Bckgnd.Color")) + ::toString(getSelectedCAM()->getBackgroundColor(),false);
    }
    break;
  case CONTROL_AMBIENTLIGHTCOLOR     :
    NEWLINE(_T("Amb.Color"))      + ::toString(getScene().getAmbientColor(),false);
    break;
  }
  if(m_propertyDialogMap.isDialogVisible()) {
    NEWLINE(_T("Map.Dlg")) + format(_T("Id:%d, Type:%s")
                                   ,m_propertyDialogMap.getVisiblePropertyId()
                                   ,m_propertyDialogMap.getVisibleTypeName().cstr());
  }
  return result;
}
