#include "stdafx.h"
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/ColorDlg.h>
#include <ByteFile.h>
#include "SceneEditor.h"

SceneEditor::SceneEditor() {
  m_sceneContainer      = NULL;
  m_currentEditor       = NULL;
  m_selectedSceneObject = NULL;
  m_lightDlgThread      = NULL;
  m_materialDlgThread   = NULL;
  m_mouseVisible        = true;
  m_editorEnabled       = false;
  m_paramFileName       = _T("Untitled");
}

SceneEditor::~SceneEditor() {
  close();
}

void SceneEditor::init(D3SceneContainer *sceneContainer) {
  m_sceneContainer    = sceneContainer;
  m_lightDlgThread    = CPropertyDlgThread::startThread(new CLightDlg(   this));
  m_materialDlgThread = CPropertyDlgThread::startThread(new CMaterialDlg(this));

  m_currentControl = CONTROL_IDLE;

  getScene().setLightDirection(0, rotate(getScene().getCameraDir(), getScene().getCameraRight(), 0.2f));
  getScene().addPropertyChangeListener(this);
}

void SceneEditor::close() {
  getScene().removePropertyChangeListener(this);
  if (m_lightDlgThread) {
    m_lightDlgThread->kill();
    m_lightDlgThread = NULL;
  }
  if (m_materialDlgThread) {
    m_materialDlgThread->kill();
    m_materialDlgThread = NULL;
  }
}

void SceneEditor::setEnabled(bool enabled) {
  m_editorEnabled = enabled;
}

#define MK_CTRLSHIFT (MK_CONTROL | MK_SHIFT)

static USHORT getCtrlShiftState() {
  USHORT flags = 0;
  if(shiftKeyPressed()) flags |= MK_SHIFT;
  if(ctrlKeyPressed())  flags |= MK_CONTROL;
  return flags;
}

void SceneEditor::setMouseVisible(bool visible) {
  if(visible == m_mouseVisible) {
    return;
  }
  ShowCursor(visible?TRUE:FALSE);
  m_mouseVisible = visible;
}

void SceneEditor::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(!m_editorEnabled) return;
  if(source == &getScene()) {
    switch(id) {
    case SP_FILLMODE                :
    case SP_SHADEMODE               :
    case SP_CAMERAPOS               :
    case SP_CAMERAORIENTATION       :
    case SP_OBJECTPOS               :
    case SP_OBJECTORIENTATION       :
    case SP_OBJECTSCALE             :
    case SP_PROJECTIONTRANSFORMATION:
    case SP_LIGHTPARAMETERS         :
    case SP_AMBIENTLIGHT            :
    case SP_SPECULARENABLED         :
    case SP_MATERIALPARAMETERS      :
    case SP_OBJECTCOUNT             :
    case SP_BACKGROUNDCOLOR         :
      render(RENDER_ALL);
      break;
    case SP_ANIMATIONFRAMEINDEX     :
      render(RENDER_3D);
      break;
    }
  } else if(source == m_lightDlgThread->getPropertyContainer()) {
    switch(id) {
    case SP_LIGHTPARAMETERS:
      { const LIGHT &lp = *(LIGHT*)newValue;
        getScene().setLightParam(lp);
      }
      break;
    }
  } else if(source == m_materialDlgThread->getPropertyContainer()) {
    switch(id) {
    case SP_MATERIALPARAMETERS:
      getScene().setMaterial(*(MATERIAL*)newValue);
      break;
    }
  } else if(source == m_currentEditor) {
    switch(id) {
    case SP_BACKGROUNDCOLOR:
      getScene().setBackgroundColor(*(D3DCOLOR*)newValue);
      break;
    case SP_AMBIENTLIGHT           :
      getScene().setGlobalAmbientColor(*(D3DCOLOR*)newValue);
      break;
    }
  }
}

void SceneEditor::setSelectedObject(D3SceneObject *obj) {
  m_selectedSceneObject = obj;
  const D3LightControl *lc = getSelectedLightControl();
  if(lc) {
    m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  } else if(obj) {
    if(obj->hasMaterial()) {
      m_materialDlgThread->setCurrentDialogProperty(&obj->getMaterial());
    }
  }
  render(RENDER_INFO);
}

void SceneEditor::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastMouse = point;
  m_pickedRay = getScene().getPickRay(m_lastMouse);
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;

  default:
    { D3DXVECTOR3 hitPoint;
      D3SceneObject *pickedObj = getScene().getPickedObject(point, PICK_ALL, &hitPoint);
      if(pickedObj == NULL) {
        if(m_currentControl != CONTROL_CAMERA_WALK) {
          setSelectedObject(NULL);
          setCurrentControl(CONTROL_IDLE);
        } else {
          setMouseVisible(false);
          getMessageWindow()->SetCapture();
        }
      } else {
        m_pickedPoint = hitPoint;
        setSelectedObject(pickedObj);
        switch(pickedObj->getType()) {
        case SOTYPE_VISUALOBJECT  :
        case SOTYPE_ANIMATEDOBJECT:
          setCurrentControl(CONTROL_OBJECT_POS);
          break;
        case SOTYPE_LIGHTCONTROL  :
          setCurrentControl(CONTROL_LIGHT);
          break;
        }
      }
    }
    break;
  }
}

void SceneEditor::OnLButtonUp(UINT nFlags, CPoint point) {
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setCurrentControl(CONTROL_IDLE);
    break;
  case CONTROL_OBJECT_POS:
  case CONTROL_CAMERA_WALK:
    setMouseVisible(true);
    ReleaseCapture();
    break;
  }
}

void SceneEditor::OnContextMenu(CWnd *pwnd, CPoint point) {
  m_lastMouse = screenPTo3DP(point);
  m_pickedRay = getScene().getPickRay(m_lastMouse);
  D3DXVECTOR3 hitPoint;
  D3SceneObject *pickedObject = getScene().getPickedObject(m_lastMouse, PICK_ALL, &hitPoint);
  if(pickedObject == NULL) {
    OnContextMenuBackground(point);
  } else {
    m_pickedPoint = hitPoint;
    setSelectedObject(pickedObject);
    OnContextMenuSceneObject(point);
  }
}

void SceneEditor::OnMouseMove(UINT nFlags, CPoint point) {
  if(!(nFlags & MK_LBUTTON)) {
    return;
  }
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK   :
    OnMouseMoveCameraWalk(nFlags, point);
    break;
  case CONTROL_OBJECT_POS    :
    OnMouseMoveObjectPos(nFlags, point);
    break;
  case CONTROL_LIGHT          :
    OnMouseMoveLight(nFlags, point);
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;
  }
  m_lastMouse = point;
}

void SceneEditor::OnLButtonDblClk(UINT nFlags, CPoint point) {
}

BOOL SceneEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK           :
    OnMouseWheelCameraWalk(          nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_POS            :
    OnMouseWheelObjectPos(           nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_SCALE          :
    OnMouseWheelObjectScale(         nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_POS_KEEPFOCUS  :
    OnMouseWheelObjectPosKeepFocus(  nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_SCALE_KEEPFOCUS:
    OnMouseWheelObjectScaleKeepFocus(nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_CAMERA_KEEPFOCUS      :
    OnMouseWheelCameraKeepFocus(     nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_ANIMATION_SPEED       :
    OnMouseWheelAnimationSpeed(      nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_LIGHT                 :
    OnMouseWheelLight(               nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_SPOTANGLES            :
    OnMouseWheelLightSpotAngle(      nFlags, zDelta, pt);
    return TRUE;
  }
  return FALSE;
}

BOOL SceneEditor::PreTranslateMessage(MSG *pMsg) {
  if(!m_editorEnabled) return false;
  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_ESCAPE:
      setEnabled(false);
      render(RENDER_ALL);
      return true;
    }
    break;
  case WM_LBUTTONDOWN:
    OnLButtonDown((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_LBUTTONUP  :
    OnLButtonUp((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_MOUSEMOVE  :
    OnMouseMove((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_MOUSEWHEEL :
    OnMouseWheel((UINT)(pMsg->wParam&0xffff), (short)(pMsg->wParam >> 16), screenPTo3DP(pMsg->pt));
    return true;
  case WM_RBUTTONUP:
    OnContextMenu(getMessageWindow(), pMsg->pt);
    return true;
  case WM_COMMAND:
    switch (pMsg->wParam) {
    case ID_CONTROL_OBJECT_POS            : OnControlObjectPos()           ; return true;
    case ID_CONTROL_OBJECT_SCALE          : OnControlObjectScale()         ; return true;
    case ID_CONTROL_OBJECT_KEEPFOCUS      : OnControlObjectKeepFocus()     ; return true;
    case ID_CONTROL_OBJECT_SCALE_KEEPFOCUS: OnControlObjectScaleKeepFocus(); return true;
    case ID_CONTROL_OBJECT_MOVEROTATE     : OnControlObjectMoveRotate()    ; return true;
    case ID_OBJECT_RESETSCALE             : OnObjectResetScale()           ; return true;
    case ID_OBJECT_ADJUSTMATERIAL         : OnObjectEditMaterial()         ; return true;
    case ID_OBJECT_REMOVE                 : OnObjectRemove()               ; return true;
    case ID_OBJECT_STARTANIMATION         : OnObjectStartAnimation()       ; return true;
    case ID_OBJECT_STARTBCKANIMATION      : OnObjectStartBckAnimation()    ; return true;
    case ID_OBJECT_STARTALTANIMATION      : OnObjectStartAltAnimation()    ; return true;
    case ID_OBJECT_REVERSEANIMATION       : OnObjectReverseAnimation()     ; return true;
    case ID_OBJECT_STOPANIMATION          : OnObjectStopAnimation()        ; return true;
    case ID_OBJECT_CONTROL_SPEED          : OnObjectControlSpeed()         ; return true;
    case ID_CONTROL_CAMERA_WALK           : OnControlCameraWalk()          ; return true;
    case ID_CONTROL_CAMERA_KEEPFOCUS      : OnControlCameraKeepFocus()     ; return true;
    case ID_EDIT_AMBIENTLIGHT             : OnEditAmbientLight()           ; return true;
    case ID_EDIT_BACKGROUNDCOLOR          : OnEditBackgroundColor()        ; return true;
    case ID_ADDLIGHT_DIRECTIONAL          : OnAddLightDirectional()        ; return true;
    case ID_ADDLIGHT_POINT                : OnAddLightPoint()              ; return true;
    case ID_ADDLIGHT_SPOT                 : OnAddLightSpot()               ; return true;
    case ID_LIGHT_REMOVE                  : OnLightRemove()                ; return true;
    case ID_LIGHT_ENSABLE                 : setLightEnabled(true)          ; return true;
    case ID_LIGHT_DISABLE                 : setLightEnabled(false)         ; return true;
    case ID_LIGHT_ADJUSTCOLORS            : OnLightAdjustColors()          ; return true;
    case ID_LIGHT_ADJUSTANGLES            : OnLightAdjustAngles()          ; return true;
    case ID_SHOW_LIGHTCONTROLS            : setLightControlsVisible(true)  ; return true;
    case ID_HIDE_LIGHTCONTROLS            : setLightControlsVisible(false) ; return true;
    case ID_LIGHTCONTROL_HIDE             : OnLightControlHide()           ; return true;
    case ID_LIGHTCONTROL_SPOTAT           : OnLightControlSpotAt()         ; return true;
    case ID_SAVESCENEPARAMETERS           : OnSaveSceneParameters()        ; return true;
    case ID_LOADSCENEPARAMETERS           : OnLoadSceneParameters()        ; return true;
    }
  }
  return false;
}

// ---------------------------------- Controlling object -----------------------------------------

void SceneEditor::OnMouseMoveObjectPos(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_VISUALOBJECT);
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_VISUALOBJECT);
    break;
  case MK_CONTROL  :
    rotateObjectFrwBckw(double(pt.x - m_lastMouse.x) / 100.0
                       ,double(pt.y - m_lastMouse.y) / 100.0);
    break;
  case MK_CTRLSHIFT:
    rotateObjectLeftRight(double(pt.x - m_lastMouse.x) / 100.0);
    break;
  }
}

void SceneEditor::moveSceneObjectXY(CPoint pt, SceneObjectType type, int lightIndex) {
  const D3DXVECTOR3 dp             = getSceneObjectPos(type,lightIndex) - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.m_orig + newPickedRay.m_dir * dist;
  setSceneObjectPos(newPickedPoint + dp, type, lightIndex);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void SceneEditor::moveSceneObjectXZ(CPoint pt, SceneObjectType type, int lightIndex) {
  const D3DXVECTOR3 dp             = getSceneObjectPos(type,lightIndex) - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  const D3Ray       ray1           = getScene().getPickRay(CPoint(pt.x,pt.y+1));
  const double      dRaydPixel     = length((newPickedRay.m_orig + dist * newPickedRay.m_dir) - (ray1.m_orig + dist * ray1.m_dir));
  const CSize       dMouse         = pt - m_lastMouse;
  D3DXVECTOR3       newPickedPoint = m_pickedPoint 
                                   + (float)(-dMouse.cy * dRaydPixel) * getScene().getCameraDir()
                                   + (float)( dMouse.cx * dRaydPixel) * getScene().getCameraRight();

  setSceneObjectPos(newPickedPoint + dp, type, lightIndex);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

D3DXVECTOR3 SceneEditor::getSceneObjectPos(SceneObjectType type, int lightIndex) {
  switch(type) {
  case SOTYPE_VISUALOBJECT:
    return getScene().getObjPos();
    break;
  case SOTYPE_LIGHTCONTROL:
    return getScene().getLightParam(lightIndex).Position;
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
    return D3DXVECTOR3(0,0,0);
  }
}

void SceneEditor::setSceneObjectPos(const D3DXVECTOR3 &pos, SceneObjectType type, int lightIndex) {
  switch(type) {
  case SOTYPE_VISUALOBJECT:
    getScene().setObjPos(pos);
    break;
  case SOTYPE_LIGHTCONTROL:
    getScene().setLightPosition(lightIndex, pos);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), type);
  }
}

void SceneEditor::OnMouseWheelObjectPos(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = getScene().getObjPos();
      const D3DXVECTOR3 camPos = getScene().getCameraPos();
      const D3DXVECTOR3 dir    = objPos - camPos;
      const float       dist   = length(dir);
      if(dist > 0) {
        getScene().setObjPos(objPos + unitVector(dir) * dist / 30.0 * (float)sign(zDelta));
      }
    }
    break;
  case MK_CONTROL  :
    rotateObjectFrwBckw(0, (double)sign(zDelta) / -50.0);
    break;
  case MK_SHIFT    :
    rotateObjectFrwBckw((double)sign(zDelta) / -50, 0);
    break;
  case MK_CTRLSHIFT:
    rotateObjectLeftRight((double)sign(zDelta) / 50.0);
    break;
  }
}

void SceneEditor::rotateObjectFrwBckw(double angle1, double angle2) {
  const D3DXVECTOR3 cameraUp    = getScene().getCameraUp();
  const D3DXVECTOR3 cameraRight = getScene().getCameraRight();
  const D3DXVECTOR3 objDir      = rotate(rotate(getScene().getObjDir(),cameraUp,angle1),cameraRight,angle2);
  const D3DXVECTOR3 objUp       = rotate(rotate(getScene().getObjUp() ,cameraUp,angle1),cameraRight,angle2);
  getScene().setObjOrientation(objDir, objUp);
}

void SceneEditor::rotateObjectLeftRight(double angle) {
  const D3DXVECTOR3 cameraDir = getScene().getCameraDir();
  getScene().setObjOrientation(rotate(getScene().getObjDir(),cameraDir, angle), rotate(getScene().getObjUp(), cameraDir, angle));
}

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

void SceneEditor::OnMouseWheelObjectScale(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustScale(VADJUST_Z  , factor); break;
  }
}

void SceneEditor::OnMouseWheelObjectPosKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3PosDirUpScale pdus = getScene().getObjPDUS();

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 camPos  = getScene().getCameraPos();
      const float       dist    = length(m_focusPoint - camPos);
      D3DXVECTOR3       dp      = pdus.getPos() - m_focusPoint;
      m_focusPoint = camPos + (float)(dist * (1.0 - (float)sign(zDelta)/30)) * getScene().getCameraDir();
      getScene().setObjPos(m_focusPoint + dp);
    }
    break;
  case MK_CONTROL  :
  case MK_SHIFT    :
  case MK_CTRLSHIFT:
    { m_editorEnabled = false;
      const D3DXVECTOR3 objFocus = invers(pdus.getRotationMatrix()) * (m_focusPoint - pdus.getPos());
      OnMouseWheelObjectPos(nFlags, zDelta, pt);
      m_editorEnabled = true;
      getScene().setObjPos(m_focusPoint - getScene().getObjPDUS().getRotationMatrix() * objFocus);
    }
    break;
  }
}

void SceneEditor::OnMouseWheelObjectScaleKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3PosDirUpScale pdus = getScene().getObjPDUS();
  m_editorEnabled = false;

  const D3DXVECTOR3 objFocus = invers(pdus.getScaleMatrix()) * (m_focusPoint - pdus.getPos());
  OnMouseWheelObjectScale(nFlags, zDelta, pt);
  m_editorEnabled = true;
  getScene().setObjPos(m_focusPoint - getScene().getObjPDUS().getScaleMatrix() * objFocus);
}

void SceneEditor::OnMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
  }
}

void SceneEditor::adjustScale(int component, double factor) {
  D3DXVECTOR3 scale = getScene().getObjScale();
  if(component & VADJUST_X) {
    scale.x *= (float)factor;
  }
  if(component & VADJUST_Y) {
    scale.y *= (float)factor;
  }
  if(component & VADJUST_Z) {
    scale.z *= (float)factor;
  }
  getScene().setObjScale(scale);
}

// ------------------------------------- controlling camera -----------------------------------------

void SceneEditor::OnMouseMoveCameraWalk(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    walkWithCamera(double(pt.y - m_lastMouse.y) / -20, double(pt.x - m_lastMouse.x) / -400.0);
    break;
  case MK_CONTROL  :
    rotateCameraUpDown(double(pt.y - m_lastMouse.y) / 400.0);
    break;
  case MK_SHIFT    :
    sidewalkWithCamera(double(pt.y - m_lastMouse.y) / -40, double(pt.x - m_lastMouse.x) / 40);
    break;
  case MK_CTRLSHIFT:
    rotateCameraLeftRight(double(pt.x - m_lastMouse.x) / 200.0);
    break;
  }
}

void SceneEditor::OnMouseWheelCameraWalk( UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    walkWithCamera((double)sign(zDelta) * 0.25, 0);
    break;
  case MK_CONTROL  :
    getScene().setNearViewPlane(getScene().getNearViewPlane() * (1 + (float)sign(zDelta)*0.05f));
    break;
  case MK_SHIFT    :
    walkWithCamera((double)sign(zDelta) * 1.5, 0);
    break;
  case MK_CTRLSHIFT:
    { const float a = getScene().getViewAngel();
      const float d = ((a > D3DX_PI/2) ? (D3DX_PI - a) : a) / (D3DX_PI/2);
      getScene().setViewAngel(a + d * sign(zDelta) * 0.04f);
    }
    break;
  }
}

void SceneEditor::OnMouseWheelCameraKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 &center = m_focusPoint;
  const D3DXVECTOR3 camPos  = getScene().getCameraPos();
  const float       radius  = length(center - camPos);
  
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setCameraPos(camPos + getScene().getCameraDir() * radius / 30.0f * (float)sign(zDelta));
    break;
  case MK_CONTROL  :
    { const D3DXVECTOR3 up     = getScene().getCameraUp();
            D3DXVECTOR3 newPos = camPos + unitVector(up) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      const D3DXVECTOR3 newUp  = crossProduct(getScene().getCameraRight(), newDir);
      m_editorEnabled = false;
      getScene().setCameraPos(center - newDir * radius);
      m_editorEnabled = true;
      getScene().setCameraOrientation(newDir, newUp);
    }
    break;
  case MK_SHIFT    :
    { const D3DXVECTOR3 right  = getScene().getCameraRight();
            D3DXVECTOR3 newPos = camPos + unitVector(right) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      m_editorEnabled = false;
      getScene().setCameraPos(center - newDir * radius);
      m_editorEnabled = true;
      getScene().setCameraOrientation(newDir, getScene().getCameraUp());
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void SceneEditor::walkWithCamera(double dist, double angle) {
  const D3DXVECTOR3 dir = getScene().getCameraDir();
  const D3DXVECTOR3 up  = getScene().getCameraUp();
  getScene().setCameraPos(getScene().getCameraPos() + (float)dist * dir);
  getScene().setCameraOrientation(rotate(dir, up, angle), up);
}

void SceneEditor::sidewalkWithCamera(double upDist, double rightDist) {
  getScene().setCameraPos(getScene().getCameraPos() + (float)upDist * getScene().getCameraUp() + (float)rightDist * getScene().getCameraRight());
}

void SceneEditor::rotateCameraUpDown(double angle) {
  const D3DXVECTOR3 right = getScene().getCameraRight();;
  getScene().setCameraOrientation(rotate(getScene().getCameraDir(), right, angle), rotate(getScene().getCameraUp() , right, angle));
}

void SceneEditor::rotateCameraLeftRight(double angle) {
  const D3DXVECTOR3 dir = getScene().getCameraDir();
  getScene().setCameraOrientation(dir, rotate(getScene().getCameraUp(), dir, angle));
}

// ------------------------------------- controlling lights -----------------------------------------

void SceneEditor::OnMouseMoveLight(UINT nFlags, CPoint pt) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc) {
    switch(lc->getLightType()) {
    case D3DLIGHT_POINT:
      OnMouseMoveLightPoint(*(D3LightControlPoint*)lc, nFlags, pt);
      break;
    case D3DLIGHT_SPOT :
      OnMouseMoveLightSpot(*(D3LightControlSpot*)lc, nFlags, pt);
      break;
    }
  }
}

void SceneEditor::OnMouseMoveLightPoint(D3LightControlPoint &ctrl, UINT nFlags, CPoint pt) {
  const D3DXVECTOR3 pos = ctrl.getLightParam().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_CONTROL  :
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void SceneEditor::OnMouseMoveLightSpot(D3LightControlSpot &ctrl, UINT nFlags, CPoint pt) {
  const LIGHT       param = ctrl.getLightParam();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveSceneObjectXY(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_SHIFT    :
    moveSceneObjectXZ(pt, SOTYPE_LIGHTCONTROL, ctrl.getLightIndex());
    break;
  case MK_CONTROL  :
    { const double angle1 = (double)(pt.x - m_lastMouse.x) / 100.0;
      const double angle2 = (double)(pt.y - m_lastMouse.y) / 100.0;
      const D3DXVECTOR3 newDir = rotate(rotate(dir,getScene().getCameraUp(),angle1),getScene().getCameraRight(),angle2);
      getScene().setLightDirection(ctrl.getLightIndex(), newDir);
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void SceneEditor::OnMouseWheelLight(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  switch(lc->getLightType()) {
  case D3DLIGHT_DIRECTIONAL: OnMouseWheelLightDirectional(*(D3LightControlDirectional*)lc, nFlags, zDelta, pt); break;
  case D3DLIGHT_POINT      : OnMouseWheelLightPoint(      *(D3LightControlPoint*)lc      , nFlags, zDelta, pt); break;
  case D3DLIGHT_SPOT       : OnMouseWheelLightSpot(       *(D3LightControlSpot*)lc       , nFlags, zDelta, pt); break;
  }
}

void SceneEditor::OnMouseWheelLightDirectional(D3LightControlDirectional &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 dir = ctrl.getLightParam().Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    ctrl.setSphereRadius(ctrl.getSphereRadius() * (1.0f-(float)sign(zDelta)*0.04f));
    render(RENDER_ALL);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCameraRight(), -(double)sign(zDelta)*0.06));
    break;
  case MK_SHIFT    : 
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCameraUp(),    -(double)sign(zDelta)*0.06));
    break;
  case MK_CTRLSHIFT: 
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCameraDir(),    (double)sign(zDelta)*0.06));
    break;
  }
}

void SceneEditor::OnMouseWheelLightPoint(D3LightControlPoint &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3 pos = ctrl.getLightParam().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCameraDir()   * (float)sign(zDelta)*0.04f);
    break;
  case MK_CONTROL  :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCameraUp()    * (float)sign(zDelta)*0.04f);
    break;
  case MK_SHIFT    : 
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCameraRight() * (float)sign(zDelta)*0.04f);
    break;
  case MK_CTRLSHIFT: 
    break;
  }
}

void SceneEditor::OnMouseWheelLightSpot(D3LightControlSpot &ctrl, UINT nFlags, short zDelta, CPoint pt) {
  const LIGHT       param = ctrl.getLightParam();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : 
    getScene().setLightPosition( ctrl.getLightIndex(), pos + getScene().getCameraUp() * (float)sign(zDelta)*0.04f);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCameraRight(), -(float)sign(zDelta)*0.06));
    break;
  case MK_SHIFT    : 
    getScene().setLightPosition( ctrl.getLightIndex(), pos + getScene().getCameraRight()   * (float)sign(zDelta)*0.04f);
    break;
  case MK_CTRLSHIFT: 
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCameraUp(), (float)sign(zDelta)*0.06));
    break;
  }
}

void SceneEditor::OnMouseWheelLightSpotAngle(UINT nFlags, short zDelta, CPoint pt) {
  const D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL || lc->getLightType() != D3DLIGHT_SPOT) {
    return;
  }
  LIGHT light = lc->getLightParam();
  switch(nFlags) {
  case 0           :
    light.setInnerAngle(light.getInnerAngle() * (1.0f - (float)sign(zDelta) / 10.0f));
    break;
  case MK_SHIFT    :
    light.setOuterAngle(light.getOuterAngle() * (1.0f - (float)sign(zDelta) / 10.0f));
    break;
  default:
    return;
  }
  getScene().setLightParam(light);
}
// ------------------------------------------------------------------------------------------------------------

void SceneEditor::OnAddLightDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void SceneEditor::OnAddLightPoint()       { addLight(D3DLIGHT_POINT);       }
void SceneEditor::OnAddLightSpot()        { addLight(D3DLIGHT_SPOT);        }

void SceneEditor::addLight(D3DLIGHTTYPE type) {
  LIGHT lp = getScene().getDefaultLightParam(type);
  switch(type) {
  case D3DLIGHT_DIRECTIONAL:
    break;
  case D3DLIGHT_POINT      :
    lp.Position = m_pickedRay.m_orig + m_pickedRay.m_dir * 2;
    break;
  case D3DLIGHT_SPOT       :
    lp.Position  = m_pickedRay.m_orig + m_pickedRay.m_dir * 3;
    lp.Direction = (getScene().getCameraPos() + 5 * getScene().getCameraDir()) - lp.Position;
    break;
  }
  getScene().setLightParam(lp);
  getScene().setLightControlVisible(lp.m_index, true);
}

D3SceneObject *SceneEditor::getSelectedVisualObject() {
  if(m_selectedSceneObject == NULL) {
    return NULL;
  }
  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return m_selectedSceneObject;
  default:
    return NULL;
  }
}

D3AnimatedSurface *SceneEditor::getSelectedAnimatedobject() {
  if(m_selectedSceneObject == NULL || m_selectedSceneObject->getType() != SOTYPE_ANIMATEDOBJECT) {
    return NULL;
  }
  return (D3AnimatedSurface*)m_selectedSceneObject;
}

D3LightControl *SceneEditor::getSelectedLightControl() {
  if((m_selectedSceneObject == NULL) || (m_selectedSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_selectedSceneObject;
}

const D3LightControl *SceneEditor::getSelectedLightControl() const {
  if((m_selectedSceneObject == NULL) || (m_selectedSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_selectedSceneObject;
}

void SceneEditor::setLightControlsVisible(bool visible) {
  BitSet visibleLightSet = getScene().getLightsVisible();
  if(visible) {
    BitSet missing = getScene().getLightsDefined() - visibleLightSet;
    if(missing.isEmpty()) {
      return;
    }
    for(Iterator<size_t> it = missing.getIterator(); it.hasNext();) {
      getScene().setLightControlVisible((UINT)it.next(), true);
    }
  } else {
    if(visibleLightSet.isEmpty()) {
      return;
    }
    for(Iterator<size_t> it = visibleLightSet.getIterator(); it.hasNext();) {
      getScene().setLightControlVisible((UINT)it.next(), false);
    }
  }
  render(RENDER_ALL);
}

void SceneEditor::setSpotToPointAt(CPoint point) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL || lc->getLightType() != D3DLIGHT_SPOT) {
    return;
  }
  D3DXVECTOR3 pointInSpace;
  D3SceneObject *obj = getScene().getPickedObject(point, ~PICK_LIGHTCONTROL, &pointInSpace);
  if(obj == NULL) {
    return;
  }
  LIGHT param = lc->getLightParam();
  param.Direction = unitVector(pointInSpace - param.Position);
  getScene().setLightParam(param);
}

void SceneEditor::OnContextMenuSceneObject(CPoint point) {
  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    OnContextMenuVisualObject(point);
    break;
  case SOTYPE_LIGHTCONTROL:
    OnContextMenuLightControl(point);
    break;
  }
}

static void removeMenuCommand(CMenu &menu, int command) {
  menu.RemoveMenu(command, MF_BYCOMMAND);
}

CMenu &SceneEditor::loadMenu(CMenu &menu, int id) {
  const int ret = menu.LoadMenu(id);
  if(!ret) {
    throwException(_T("Loadmenu(%d) failed"), id);
  }
  return menu;
}

void SceneEditor::showContextMenu(CMenu &menu, CPoint point) {
  TrackPopupMenu(*menu.GetSubMenu(0), TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, 0, *getMessageWindow(),NULL);
}

void SceneEditor::OnContextMenuBackground(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_BACKGROUND);
  int checkedItem = -1;
  switch(m_currentControl) {
  case CONTROL_CAMERA_WALK     : checkedItem = ID_CONTROL_CAMERA_WALK     ; break;
  case CONTROL_CAMERA_KEEPFOCUS: checkedItem = ID_CONTROL_CAMERA_KEEPFOCUS; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }
  const int visibleLightCount = (int)getScene().getLightsVisible().size();
  if(visibleLightCount == 0) {
    removeMenuCommand(menu,ID_HIDE_LIGHTCONTROLS);
  }
  if(visibleLightCount == getScene().getLightCount()) {
    removeMenuCommand(menu, ID_SHOW_LIGHTCONTROLS);
  }
  showContextMenu(menu, point);
}

void SceneEditor::OnContextMenuVisualObject(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_VISUALOBJECT );

  int checkedItem = -1;
  switch(m_currentControl) {
  case CONTROL_OBJECT_POS      : checkedItem = ID_CONTROL_OBJECT_POS      ; break;
  case CONTROL_OBJECT_SCALE    : checkedItem = ID_CONTROL_OBJECT_SCALE    ; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }
  switch(m_selectedSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
    removeMenuCommand(menu, ID_OBJECT_STARTANIMATION   );
    removeMenuCommand(menu, ID_OBJECT_STARTBCKANIMATION);
    removeMenuCommand(menu, ID_OBJECT_STARTALTANIMATION);
    removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
    removeMenuCommand(menu, ID_OBJECT_STOPANIMATION    );
    removeMenuCommand(menu, ID_OBJECT_CONTROL_SPEED    );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3AnimatedSurface *obj = getSelectedAnimatedobject();
      if(obj->isRunning()) {
        removeMenuCommand(menu, ID_OBJECT_STARTANIMATION   );
        removeMenuCommand(menu, ID_OBJECT_STARTBCKANIMATION);
        removeMenuCommand(menu, ID_OBJECT_STARTALTANIMATION);
        switch(obj->getAnimationType()) {
        case ANIMATE_ALTERNATING:
          removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
          break;
        }
      } else {
        removeMenuCommand(menu, ID_OBJECT_REVERSEANIMATION );
        removeMenuCommand(menu, ID_OBJECT_STOPANIMATION    );
        removeMenuCommand(menu, ID_OBJECT_CONTROL_SPEED    );
      }
    }
    break;
  }
  showContextMenu(menu, point);
}

void SceneEditor::OnContextMenuLightControl(CPoint point) {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_LIGHTCONTROL);
  if(getScene().isLightEnabled(lc->getLightIndex())) { // light is on
    removeMenuCommand(menu, ID_LIGHT_ENSABLE);
  } else {                                          // light is off
    removeMenuCommand(menu, ID_LIGHT_DISABLE      );
    removeMenuCommand(menu, ID_LIGHT_ADJUSTCOLORS );
    removeMenuCommand(menu, ID_LIGHTCONTROL_SPOTAT);
  }
  if(lc->getLightType() != D3DLIGHT_SPOT) {
    removeMenuCommand(menu, ID_LIGHTCONTROL_SPOTAT);
    removeMenuCommand(menu, ID_LIGHT_ADJUSTANGLES );
  }
  showContextMenu(menu, point);
}

void SceneEditor::OnControlCameraWalk()      {  setCurrentControl(CONTROL_CAMERA_WALK        ); }

void SceneEditor::OnControlObjectPos()       {  setCurrentControl(CONTROL_OBJECT_POS         ); }
void SceneEditor::OnControlObjectScale()     {  setCurrentControl(CONTROL_OBJECT_SCALE       ); }

void SceneEditor::OnControlObjectMoveRotate() {
//  setSelectedObject(m_calculatedObject);
  setCurrentControl(CONTROL_OBJECT_POS);
}

void SceneEditor::OnControlObjectKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_POS_KEEPFOCUS);
  }
}

void SceneEditor::OnControlObjectScaleKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_SCALE_KEEPFOCUS);
  }
}

bool SceneEditor::moveLastMouseToFocusPoint() {
  D3DXVECTOR3 hitPoint;
  D3SceneObject *obj = getScene().getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &hitPoint);
  if((obj == NULL) || (obj != m_selectedSceneObject)) {
    return false;
  }

  const D3DXVECTOR3 dp       = obj->getPos() - hitPoint;
  const D3DXVECTOR3 camPos   = getScene().getCameraPos();
  const float       distance = length(camPos - hitPoint);
  m_focusPoint = camPos + distance * getScene().getCameraDir();

  getScene().setObjPos(m_focusPoint + dp);
  return true;
}

void SceneEditor::OnControlCameraKeepFocus() {
  D3SceneObject *obj = getScene().getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &m_focusPoint);
  if(obj == NULL) {
    return;
  }
  getScene().setCameraLookAt(m_focusPoint);
  setCurrentControl(CONTROL_CAMERA_KEEPFOCUS);
}

void SceneEditor::OnObjectControlSpeed() {
  setCurrentControl(CONTROL_ANIMATION_SPEED);
}

void SceneEditor::setCurrentControl(CurrentObjectControl control) {
  m_currentControl = control;
  switch(m_currentControl) {
  case CONTROL_OBJECT_POS:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_HAND));
    render(RENDER_INFO);
    break;
  case CONTROL_CAMERA_KEEPFOCUS:
//    showInfo(EMPTYSTRING);
    // nb continue case
  case CONTROL_CAMERA_WALK     :
    setSelectedObject(NULL);
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_NORMAL));
    render(RENDER_INFO);
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_CROSS));
    render(RENDER_INFO);
    break;
  default:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_NORMAL));
    render(RENDER_INFO);
    break;
  }
}

void SceneEditor::OnObjectEditMaterial() {
  if(m_selectedSceneObject 
  && m_selectedSceneObject->getType() == SOTYPE_VISUALOBJECT
  && m_selectedSceneObject->hasMaterial()) {
    m_materialDlgThread->setCurrentDialogProperty(&m_selectedSceneObject->getMaterial());
    m_materialDlgThread->setDialogVisible(true);
    m_lightDlgThread->setDialogVisible(false);
  }
}

void SceneEditor::OnObjectResetScale() {
  getScene().setObjScale(D3DXVECTOR3(1,1,1));
}

void SceneEditor::OnObjectStartAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_FORWARD);
}

void SceneEditor::OnObjectStartBckAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_BACKWARD);
}

void SceneEditor::OnObjectStartAltAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_ALTERNATING);
}

void SceneEditor::OnObjectReverseAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if((obj == NULL) || !obj->isRunning()) return;
  switch(obj->getAnimationType()) {
  case ANIMATE_FORWARD    :
    obj->stopAnimation();
    obj->startAnimation(ANIMATE_BACKWARD);
    break;
  case ANIMATE_BACKWARD   :
    obj->stopAnimation();
    obj->startAnimation(ANIMATE_FORWARD);
    break;
  case ANIMATE_ALTERNATING:
    break;
  }
}

void SceneEditor::OnObjectStopAnimation() {
  D3AnimatedSurface *obj = getSelectedAnimatedobject();
  if(obj == NULL) return;
  obj->stopAnimation();
}

void SceneEditor::OnObjectRemove() {
  getScene().removeSceneObject(m_selectedSceneObject);
  delete m_selectedSceneObject;
  setSelectedObject(NULL);
  render(RENDER_ALL);
}

void SceneEditor::setLightEnabled(bool enabled) {
  getScene().setLightEnabled(getSelectedLightControl()->getLightIndex(), enabled);
  render(RENDER_ALL);
}

void SceneEditor::OnLightAdjustColors() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  m_lightDlgThread->setDialogVisible(true);
  m_materialDlgThread->setDialogVisible(false);
}

void SceneEditor::OnLightAdjustAngles() {
  setCurrentControl(CONTROL_SPOTANGLES);
}

void SceneEditor::OnLightControlSpotAt() {
  setCurrentControl(CONTROL_SPOTLIGHTPOINT);
}

void SceneEditor::OnLightControlHide() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  getScene().setLightControlVisible(lc->getLightIndex(), false);
  render(RENDER_ALL);
}

void SceneEditor::OnLightRemove() {
  D3LightControl *lc = getSelectedLightControl();
  if(lc == NULL) {
    return;
  }
  const int lightIndex = lc->getLightIndex();
  getScene().removeLight(lightIndex);
  setSelectedObject(NULL);
  render(RENDER_ALL);
}

void SceneEditor::OnEditBackgroundColor() {
  const D3DCOLOR oldColor = getScene().getBackgroundColor();
  CColorDlg dlg("Background color", SP_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  if(dlg.DoModal() != IDOK) {
    getScene().setBackgroundColor(oldColor);
    getScene().render();
  }
  m_currentEditor = NULL;
}

void SceneEditor::OnEditAmbientLight() {
  const D3DCOLOR oldColor = getScene().getGlobalAmbientColor();
  CColorDlg dlg("Ambient color", SP_AMBIENTLIGHT, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  if(dlg.DoModal() != IDOK) {
    getScene().setGlobalAmbientColor(oldColor);
    getScene().render();
  }
  m_currentEditor = NULL;
}

static const TCHAR *extensions = _T("Scene-files (*.scn)\0*.scn\0\0");

void SceneEditor::OnSaveSceneParameters() {
  CFileDialog dlg(FALSE, _T("*.scn"), m_paramFileName.cstr());
  dlg.m_ofn.lpstrFilter = extensions;
  dlg.m_ofn.lpstrTitle  = _T("Save scene parameters");
  if(dlg.DoModal() != IDOK) {
    return;
  }

  if(_tcsclen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    getScene().save(ByteOutputFile(dlg.m_ofn.lpstrFile));
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch (Exception e) {
    Message(_T("%s"), e.what());
  }
}

void SceneEditor::OnLoadSceneParameters() {
  CFileDialog dlg(TRUE, _T("*.scn"), m_paramFileName.cstr());
  dlg.m_ofn.lpstrFilter = extensions;
  dlg.m_ofn.lpstrTitle = _T("Load scene parameters");
  if(dlg.DoModal() != IDOK) {
    return;
  }

  if(_tcsclen(dlg.m_ofn.lpstrFile) == 0) {
    return;
  }
  try {
    getScene().load(ByteInputFile(dlg.m_ofn.lpstrFile));
    render(RENDER_ALL);
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch(Exception e) {
    Message(_T("%s"), e.what());
  }
}

String SceneEditor::getSelectedString() const {
  if(m_selectedSceneObject == NULL) {
    return "--";
  } else {
    return m_selectedSceneObject->getName();
  }
}

const TCHAR *controlString(CurrentObjectControl control) {
  switch(control) {
#define caseStr(s) case CONTROL_##s: return _T(#s);
  caseStr(IDLE                   )
  caseStr(CAMERA_WALK            )
  caseStr(OBJECT_POS             )
  caseStr(OBJECT_SCALE           )
  caseStr(OBJECT_POS_KEEPFOCUS   )
  caseStr(OBJECT_SCALE_KEEPFOCUS )
  caseStr(CAMERA_KEEPFOCUS       )
  caseStr(LIGHT                  )
  caseStr(SPOTLIGHTPOINT         )
  caseStr(SPOTANGLES             )
  caseStr(ANIMATION_SPEED        )
  default: return _T("?");
  }
}

String SceneEditor::toString() const {
  const String camStr      = getScene().getCameraString();
  const String objStr      = getScene().getObjString();
  const String lightStr    = getScene().getLightString();
  const String materialStr = getScene().getMaterialString();
  const float  viewAngel   = getScene().getViewAngel();
  double       zn          = getScene().getNearViewPlane();
  const String focusStr    = hasFocusPoint() 
                           ? format(_T("Focuspoint:%s\n"), ::toString(m_focusPoint).cstr())
                           : EMPTYSTRING;
  const String rayStr      = m_pickedRay.isSet() ? format(_T("Picked ray:%s\n"), m_pickedRay.toString().cstr()) : EMPTYSTRING;

  
  return format(_T("Current Motion:%s Selected:%s\n%s View angel:%.1lf, zn:%.3lf\n%s\n%s%s%s\n%s")
               ,controlString(m_currentControl), getSelectedString().cstr()
               ,camStr.cstr(), degrees(viewAngel), zn
               ,objStr.cstr()
               ,focusStr.cstr()
               ,rayStr.cstr()
               ,lightStr.cstr()
               ,materialStr.cstr());
}
