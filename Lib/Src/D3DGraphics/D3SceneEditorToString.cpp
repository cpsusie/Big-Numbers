#include "pch.h"
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3Scene.h>
#include <D3DGraphics/D3SceneObjectVisual.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3ToString.h>

#define SCENE  (*getScene())

#if defined(caseStr)
#undef caseStr
#endif
#define caseStr(s) case CONTROL_##s: return _T(#s);

String toString(D3EditorControl control) {
  switch(control) {
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
  ADDFLAG(RENDER       );
  ADDFLAG(LIGHTCONTROLS);
  ADDFLAG(MOUSEVISIBLE );
  result += _T(")");
  return result;
}

String D3SceneEditor::getSelectedString() const {
  if(!hasObj()) {
    return _T("/");
  } else {
    return getCurrentObj()->getInfoString();
  }
}

class TextSection {
public:
  String m_head;
  String m_text;
  TextSection(const String &head) : m_head(head) {
  }
  TextSection &printf(const TCHAR *format, ...);
  TextSection &addText(const String &s);
};

TextSection &TextSection::printf(const TCHAR *format, ...) {
  va_list argptr;
  va_start(argptr, format);
  addText(vformat(format, argptr));
  va_end(argptr);
  return *this;
}

TextSection &TextSection::addText(const String &s) {
  m_text += s;
  return *this;
}

class FormattedText : public Array<TextSection> {
private:
  UINT m_maxHeadLength;
public:
  inline FormattedText() : m_maxHeadLength(0) {
  }
  TextSection &addSection(const String &head);
  String toString() const;
};

TextSection &FormattedText::addSection(const String &head) {
  add(TextSection(head));
  if(head.length() > m_maxHeadLength) {
    m_maxHeadLength = (UINT)head.length();
  }
  return last();
}

String FormattedText::toString() const {
  String result;
  const size_t n = size();
  for(size_t i = 0; i < n; i++) {
    const TextSection &section = (*this)[i];
    if(i) {
      result += _T("\n");
    }
    result += format(_T("%-*s: %s")
                    ,m_maxHeadLength, section.m_head.cstr()
                    ,indentString(section.m_text,m_maxHeadLength+2).trim().cstr());
  }
  return result;
}

String D3SceneEditor::toString() const {
  if(!isEnabled()) return EMPTYSTRING;
  FormattedText text;
  text.addSection("Current Control").printf(_T("%s State:%s, %s")
                                           ,::toString(getCurrentControl()).cstr()
                                           ,stateFlagsToString().cstr()
                                           ,handednessToString(SCENE.getRightHanded()).cstr()
                                           );
  text.addSection("Current Camera").printf(_T("%d, %s"),m_selectedCameraIndex
                                          ,hasCAM() ? getSelectedCAM()->toString().cstr() : _T("/")
                                          );
  text.addSection("Current Object").addText(getSelectedString());
  if(!m_pickedRay.isEmpty()) {
    text.addSection("Picked Ray" ).addText(m_pickedRay.toString());
    if(!m_pickedInfo.m_info.isEmpty() &&
       !getScene()->isVisual(m_pickedInfo.m_info.getVisual())) {
      ((D3SceneEditor*)this)->m_pickedInfo.clear(); // TODO this pointer is sime invalid!!
    }
    text.addSection("Picked Info").addText(m_pickedInfo.toString());
  }
  if(!m_centerOfRotation.isEmpty()) {
    text.addSection("Center of rotation").addText(m_centerOfRotation.toString());
  }
  switch(m_propertyDialogMap.getVisiblePropertyId()) {
  case SP_LIGHTPARAMETERS   :
    { D3Light tmp;
      text.addSection("Dlg-Light").addText(m_propertyDialogMap.getProperty(SP_LIGHTPARAMETERS,tmp).toString());
    }
    break;
  case SP_MATERIALPARAMETERS:
    { D3Material tmp;
      text.addSection("Dlg-Material").addText(m_propertyDialogMap.getProperty(SP_MATERIALPARAMETERS,tmp).toString());
    }
    break;
  }

  switch(getCurrentControl()) {
  case CONTROL_CAMERA_WALK           :
    if(hasCAM()) {
      text.addSection("Camera World").addText(getSelectedCAM()->getD3World().toString());
    }
    break;

  case CONTROL_LIGHTCOLOR            :
  case CONTROL_LIGHT                 :
  case CONTROL_SPOTLIGHTPOINT        :
  case CONTROL_SPOTLIGHTANGLES       :
    { const D3LightControl *lc = getCurrentLightControl();
      if(lc) {
        const D3Light light = lc->getLight();
        text.addSection(light.getName()).addText(light.toString());
        text.addSection("LightControl" ).addText(((D3World)*lc).toString());
      }
    }
    break;
  case CONTROL_ANIMATION_SPEED       :
    { D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
      if(obj) {
        text.addSection("Frames/sec").printf(_T("%.2f"), obj->getFramePerSec());
      }
    }
    break;
  case CONTROL_MATERIAL              :
    if(m_currentObj && m_currentObj->hasMaterial()) {
      text.addSection("Material").addText(m_currentObj->getMaterial().toString());
    }
    break;

  case CONTROL_BACKGROUNDCOLOR       :
    if(hasCAM()) {
      text.addSection("Bckgnd.Color").addText(::toString(getSelectedCAM()->getBackgroundColor(),false));
    }
    break;
  case CONTROL_AMBIENTLIGHTCOLOR     :
    text.addSection("Amb.Color").addText(::toString(SCENE.getAmbientColor(),false));
    break;
  }
  if(m_propertyDialogMap.isDialogVisible()) {
    text.addSection("Map.Dlg").printf(_T("Id:%d, Type:%s")
                                     ,m_propertyDialogMap.getVisiblePropertyId()
                                     ,m_propertyDialogMap.getVisibleTypeName().cstr());
  }
  return text.toString();
}
