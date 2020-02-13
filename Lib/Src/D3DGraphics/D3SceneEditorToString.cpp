#include "pch.h"
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/D3SceneEditor.h>

String toString(CurrentObjectControl control) {
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

  String result = format(_T("Current Object :%s\nCurrent Control:%s State:%s")
                        ,getSelectedString().cstr()
                        ,::toString(getCurrentControl()).cstr()
                        ,stateFlagsToString().cstr()
                        );
  if(!m_pickedRay.isEmpty()) {
    result += format(_T("\nPicked ray:%s"), m_pickedRay.toString().cstr());
    if(!m_pickedInfo.isEmpty()) {
      result += format(_T("\nPicked point:%s, info:%s")
                      ,::toString(m_pickedPoint).cstr()
                      ,m_pickedInfo.toString().cstr());
    }
  }
  if(getCenterOfRotation() != D3DXORIGIN) {
    result += format(_T("\nCenter of rotation:MP:%s WP:%s")
                    ,::toString(getCenterOfRotation()).cstr()
                    ,::toString(getCurrentVisual()->getWorldMatrix() * getCenterOfRotation()).cstr()
                    );
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
    return result + _T("\n") + handednessToString(getScene().getRightHanded());
  case CONTROL_CAMERA_WALK           :
    return result + format(_T("\n%s"), getScene().getCamString().cstr());

  case CONTROL_OBJECT_POS            :
  case CONTROL_OBJECT_SCALE          :
    if(m_currentObj != NULL) {
      result += format(_T("\nObject:\n%s")
                      ,m_currentObj->getPDUS().toString().cstr());
    }
    return result;

  case CONTROL_LIGHTCOLOR            :
  case CONTROL_LIGHT                 :
  case CONTROL_SPOTLIGHTPOINT        :
  case CONTROL_SPOTLIGHTANGLES       :
    { const D3LightControl *lc = getCurrentLightControl();
      if(lc) {
        result += format(_T("\n%s"), lc->getLight().toString().cstr());
      }
      return result;
    }
  case CONTROL_ANIMATION_SPEED       :
    { D3AnimatedSurface *obj = getCurrentAnimatedObj();
      if(obj) {
        result += format(_T("\nFrames/sec:%.2lf"), obj->getFramePerSec());
      }
      return result;
    }
  case CONTROL_MATERIAL              :
    if(m_currentObj && m_currentObj->hasMaterial()) {
      result += format(_T("\nMaterial:%s"), m_currentObj->getMaterial().toString().cstr());
    }
    return result;

  case CONTROL_BACKGROUNDCOLOR       :
    return result + format(_T("\nBackground color:%s")
                          ,::toString(getScene().getBackgroundColor(),false).cstr());
  case CONTROL_AMBIENTLIGHTCOLOR     :
    return result + format(_T("\nAmbient color:%s")
                          ,::toString(getScene().getAmbientColor(),false).cstr());
  }
  return result;
}
