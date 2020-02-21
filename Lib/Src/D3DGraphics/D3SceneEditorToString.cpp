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

String D3SceneEditor::getSelectedString() const {
  if(m_currentObj == NULL) {
    return _T("--");
  } else {
    String result = m_currentObj->getName();
    if(m_currentObj->hasFillMode()) {
      result += format(_T(" %s"), ::toString(m_currentObj->getFillMode()).cstr());
    }
    if(m_currentObj->hasShadeMode()) {
      result += format(_T(" %s"), ::toString(m_currentObj->getShadeMode()).cstr());
    }
    return result;
  }
}

String D3SceneEditor::toString() const {
  if(!isEnabled()) return EMPTYSTRING;
  String result;
  result += format(    _T("Current Control:%s State:%s\nCurrent Object :%s")
                        ,::toString(getCurrentControl()).cstr()
                        ,stateFlagsToString().cstr()
                        ,getSelectedString().cstr()
                        );
  result += format(    _T("\nCurrent Camera :%d, %s")
                  ,m_currentCameraIndex
                  ,m_currentCamera ? m_currentCamera->toString().cstr() : _T("--")
                  );
  if(!m_pickedRay.isEmpty()) {
    result += format(  _T("\nPicked Ray     :%s"), m_pickedRay.toString().cstr());
    if(!m_pickedInfo.isEmpty()) {
      result += format(_T("\nPicked Point   :%s, Info:%s")
                      ,::toString(m_pickedPoint).cstr()
                      ,m_pickedInfo.toString().cstr());
    }
  }
  if(!m_centerOfRotation.isEmpty()) {
    result += format(_T("\nCenter of rotation:%s"), m_centerOfRotation.toString().cstr());
  }
  switch(m_propertyDialogMap.getVisibleDialogId()) {
  case SP_LIGHTPARAMETERS   :
    { LIGHT tmp;
      result += format(_T("\nDlg-light:%s"), m_propertyDialogMap.getProperty(SP_LIGHTPARAMETERS,tmp).toString().cstr());
    }
    break;
  case SP_MATERIALPARAMETERS:
    { MATERIAL tmp;
      result += format(_T("\nDlg-material:%s"), m_propertyDialogMap.getProperty(SP_MATERIALPARAMETERS,tmp).toString().cstr());
    }
    break;
  }

  switch(getCurrentControl()) {
  case CONTROL_IDLE                  :
    result += _T("\n") + handednessToString(getScene().getRightHanded());
    break;
  case CONTROL_CAMERA_WALK           :
    if(m_currentCamera) {
      result += format(_T("\nCamera World:%s"), m_currentCamera->getWorld().toString().cstr());
    }
    break;
  case CONTROL_OBJECT_POS            :
  case CONTROL_OBJECT_SCALE          :
    if(m_currentObj != NULL) {
      result += format(_T("\nObject:\n%s"), D3World(m_currentObj->getWorld()).toString().cstr());
    }
    break;

  case CONTROL_LIGHTCOLOR            :
  case CONTROL_LIGHT                 :
  case CONTROL_SPOTLIGHTPOINT        :
  case CONTROL_SPOTLIGHTANGLES       :
    { const D3LightControl *lc = getCurrentLightControl();
      if(lc) {
        result += format(_T("\n%s"), lc->getLight().toString().cstr());
        result += format(_T("\nLightControl:%s"), D3World(m_currentObj->getWorld()).toString().cstr());
      }
    }
    break;
  case CONTROL_ANIMATION_SPEED       :
    { D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
      if(obj) {
        result += format(_T("\nFrames/sec:%.2lf"), obj->getFramePerSec());
      }
    }
    break;
  case CONTROL_MATERIAL              :
    if(m_currentObj && m_currentObj->hasMaterial()) {
      result += format(_T("\nMaterial:%s"), m_currentObj->getMaterial().toString().cstr());
    }
    break;

  case CONTROL_BACKGROUNDCOLOR       :
    if(m_currentCamera) {
      result += format(_T("\nBackground color:%s"),::toString(m_currentCamera->getBackgroundColor(),false).cstr());
    }
    break;
  case CONTROL_AMBIENTLIGHTCOLOR     :
    result += format(_T("\nAmbient color:%s"),::toString(getScene().getAmbientColor(),false).cstr());
    break;
  }
  return result;
}
