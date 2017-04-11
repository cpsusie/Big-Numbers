#include "pch.h"
#include <ByteFile.h>
#include <MFCUtil/ColorDlg.h>
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3CoordinateSystem.h>

D3SceneEditor::D3SceneEditor() {
  m_sceneContainer     = NULL;
  m_currentEditor      = NULL;
  m_currentSceneObject = NULL;
  m_coordinateSystem   = NULL;
  m_lightDlgThread     = NULL;
  m_materialDlgThread  = NULL;
  m_paramFileName      = _T("Untitled");
  m_stateFlags.add(SE_MOUSEVISIBLE);
}

D3SceneEditor::~D3SceneEditor() {
  close();
}

void D3SceneEditor::init(D3SceneContainer *sceneContainer) {
  m_sceneContainer    = sceneContainer;
  m_lightDlgThread    = CPropertyDlgThread::startThread(new CLightDlg(   this));
  m_materialDlgThread = CPropertyDlgThread::startThread(new CMaterialDlg(this));

  m_currentControl = CONTROL_IDLE;
  getScene().addPropertyChangeListener(this);
}

void D3SceneEditor::close() {
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

void D3SceneEditor::setEnabled(bool enabled) {
  if (enabled) {
    m_stateFlags.add(SE_ENABLED);
    enablePropertyChanges();
    enableRender();
    render(RENDER_INFO);
  } else {
    OnLightControlHide();
    if(m_lightDlgThread->isDialogVisible()) {
      m_lightDlgThread->setDialogVisible(false);
    }
    if(m_materialDlgThread->isDialogVisible()) {
      m_materialDlgThread->setDialogVisible(false);
    }
    disablePropertyChanges();
    m_stateFlags.remove(SE_ENABLED);
    render(RENDER_INFO);
    disableRender();
  }
  setMouseVisible(true);
}

#define MK_CTRLSHIFT (MK_CONTROL | MK_SHIFT)

static USHORT getCtrlShiftState() {
  USHORT flags = 0;
  if(shiftKeyPressed()) flags |= MK_SHIFT;
  if(ctrlKeyPressed())  flags |= MK_CONTROL;
  return flags;
}

void D3SceneEditor::setMouseVisible(bool visible) {
  if(visible == isMouseVisible()) {
    return;
  }
  if(visible) {
    ShowCursor(TRUE);
    m_stateFlags.add(SE_MOUSEVISIBLE);
  } else {
    ShowCursor(FALSE);
    m_stateFlags.remove(SE_MOUSEVISIBLE);
  }
}

void D3SceneEditor::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(!isPropertyChangesEnabled()) return;
  if(source == &getScene()) {
    switch(id) {
    case SP_FILLMODE                :
    case SP_SHADEMODE               :
    case SP_CAMERAPDUS              :
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
        LIGHT tmp = getScene().getLightParam(lp.m_index); // to keep pos and direction as they are
        getScene().setLightParam(CLightDlg::copyModifiableValues(tmp, lp));
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

void D3SceneEditor::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastMouse = point;
  m_pickedRay = getScene().getPickRay(m_lastMouse);
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;

  default:
    { D3SceneObject *pickedObj = getScene().getPickedObject(point, PICK_ALL, &m_pickedPoint, &m_pickedInfo);
      if(pickedObj == NULL) {
        if(m_currentControl != CONTROL_CAMERA_WALK) {
          setCurrentObject(NULL);
          setCurrentControl(CONTROL_IDLE);
        } else {
          setMouseVisible(false);
          getMessageWindow()->SetCapture();
        }
        m_pickedInfo.clear();
      } else {
        setCurrentObject(pickedObj);
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

void D3SceneEditor::OnLButtonUp(UINT nFlags, CPoint point) {
  switch(m_currentControl) {
  case CONTROL_SPOTLIGHTPOINT:
    setCurrentControl(CONTROL_IDLE);
    render(RENDER_INFO);
    break;
  case CONTROL_OBJECT_POS:
  case CONTROL_CAMERA_WALK:
    setMouseVisible(true);
    ReleaseCapture();
    render(RENDER_INFO);
    break;
  }
}

void D3SceneEditor::OnContextMenu(CWnd *pwnd, CPoint point) {
  m_lastMouse = screenPTo3DP(point);
  m_pickedRay = getScene().getPickRay(m_lastMouse);
  D3SceneObject *pickedObject = getScene().getPickedObject(m_lastMouse, PICK_ALL, &m_pickedPoint, &m_pickedInfo);
  if(pickedObject == NULL) {
    OnContextMenuBackground(point);
    m_pickedInfo.clear();
  } else {
    setCurrentObject(pickedObject);
    OnContextMenuSceneObject(point);
  }
}

void D3SceneEditor::OnMouseMove(UINT nFlags, CPoint point) {
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

void D3SceneEditor::OnLButtonDblClk(UINT nFlags, CPoint point) {
}

BOOL D3SceneEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
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

BOOL D3SceneEditor::PreTranslateMessage(MSG *pMsg) {
  if(!isEnabled()) return false;
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
    case ID_CONTROL_OBJECT_POS            : OnControlObjectPos()             ; return true;
    case ID_CONTROL_OBJECT_SCALE          : OnControlObjectScale()           ; return true;
    case ID_CONTROL_OBJECT_KEEPFOCUS      : OnControlObjectKeepFocus()       ; return true;
    case ID_CONTROL_OBJECT_SCALE_KEEPFOCUS: OnControlObjectScaleKeepFocus()  ; return true;
    case ID_CONTROL_OBJECT_MOVEROTATE     : OnControlObjectMoveRotate()      ; return true;
    case ID_OBJECT_RESETSCALE             : OnObjectResetScale()             ; return true;
    case ID_OBJECT_ADJUSTMATERIAL         : OnObjectEditMaterial()           ; return true;
    case ID_OBJECT_REMOVE                 : OnObjectRemove()                 ; return true;
    case ID_OBJECT_STARTANIMATION         : OnObjectStartAnimation()         ; return true;
    case ID_OBJECT_STARTBCKANIMATION      : OnObjectStartBckAnimation()      ; return true;
    case ID_OBJECT_STARTALTANIMATION      : OnObjectStartAltAnimation()      ; return true;
    case ID_OBJECT_REVERSEANIMATION       : OnObjectReverseAnimation()       ; return true;
    case ID_OBJECT_STOPANIMATION          : OnObjectStopAnimation()          ; return true;
    case ID_OBJECT_CONTROL_SPEED          : OnObjectControlSpeed()           ; return true;
    case ID_CONTROL_CAMERA_WALK           : OnControlCameraWalk()            ; return true;
    case ID_CONTROL_CAMERA_KEEPFOCUS      : OnControlCameraKeepFocus()       ; return true;
    case ID_EDIT_AMBIENTLIGHT             : OnEditAmbientLight()             ; return true;
    case ID_EDIT_BACKGROUNDCOLOR          : OnEditBackgroundColor()          ; return true;
    case ID_FILLMODE_POINT                : OnFillmodePoint()                ; return true;
    case ID_FILLMODE_WIREFRAME            : OnFillmodeWireframe()            ; return true;
    case ID_FILLMODE_SOLID                : OnFillmodeSolid()                ; return true;
    case ID_SHADING_FLAT                  : OnShadingFlat()                  ; return true;
    case ID_SHADING_GOURAUD               : OnShadingGouraud()               ; return true;
    case ID_SHADING_PHONG                 : OnShadingPhong()                 ; return true;
    case ID_ENABLE_SPECULARHIGHLIGHT      : getScene().enableSpecular(true)  ; return true;
    case ID_DISABLE_SPECULARHIGHLIGHT     : getScene().enableSpecular(false) ; return true;
    case ID_SHOWCOORDINATESYSTEM          : setCoordinateSystemVisible(true) ; return true;
    case ID_HIDECOORDINATESYSTEM          : setCoordinateSystemVisible(false); return true;
    case ID_ADDLIGHT_DIRECTIONAL          : OnAddLightDirectional()          ; return true;
    case ID_ADDLIGHT_POINT                : OnAddLightPoint()                ; return true;
    case ID_ADDLIGHT_SPOT                 : OnAddLightSpot()                 ; return true;
    case ID_LIGHT_REMOVE                  : OnLightRemove()                  ; return true;
    case ID_LIGHT_ENSABLE                 : setLightEnabled(true)            ; return true;
    case ID_LIGHT_DISABLE                 : setLightEnabled(false)           ; return true;
    case ID_LIGHT_ADJUSTCOLORS            : OnLightAdjustColors()            ; return true;
    case ID_LIGHT_ADJUSTANGLES            : OnLightAdjustAngles()            ; return true;
    case ID_SHOW_LIGHTCONTROLS            : setLightControlsVisible(true)    ; return true;
    case ID_HIDE_LIGHTCONTROLS            : setLightControlsVisible(false)   ; return true;
    case ID_LIGHTCONTROL_HIDE             : OnLightControlHide()             ; return true;
    case ID_LIGHTCONTROL_SPOTAT           : OnLightControlSpotAt()           ; return true;
    case ID_SAVESCENEPARAMETERS           : OnSaveSceneParameters()          ; return true;
    case ID_LOADSCENEPARAMETERS           : OnLoadSceneParameters()          ; return true;
    default:
      if ((ID_SELECT_LIGHT0 <= pMsg->wParam) && (pMsg->wParam <= ID_SELECT_LIGHT20)) {
        const int index = (int)pMsg->wParam - ID_SELECT_LIGHT0;
        setCurrentObject(getScene().setLightControlVisible(index, true));
        return true;
      }
    }
  }
  return false;
}

// ---------------------------------- Controlling object -----------------------------------------

void D3SceneEditor::OnMouseMoveObjectPos(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjectXZ(pt);
    break;
  case MK_CONTROL  :
    rotateCurrentObjectFrwBckw(double(pt.x - m_lastMouse.x) / 100.0
                              ,double(pt.y - m_lastMouse.y) / 100.0);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentObjectLeftRight(double(pt.x - m_lastMouse.x) / 100.0);
    break;
  }
}

void D3SceneEditor::moveCurrentObjectXY(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentObjectPos() - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.m_orig + newPickedRay.m_dir * dist;
  setCurrentObjectPos(newPickedPoint + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void D3SceneEditor::moveCurrentObjectXZ(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentObjectPos() - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  const D3Ray       ray1           = getScene().getPickRay(CPoint(pt.x,pt.y+1));
  const double      dRaydPixel     = length((newPickedRay.m_orig + dist * newPickedRay.m_dir) - (ray1.m_orig + dist * ray1.m_dir));
  const CSize       dMouse         = pt - m_lastMouse;
  const D3PosDirUpScale &camPDUS   = getScene().getCameraPDUS();
  D3DXVECTOR3       newPickedPoint = m_pickedPoint 
                                   + (float)(-dMouse.cy * dRaydPixel) * camPDUS.getDir()
                                   + (float)( dMouse.cx * dRaydPixel) * camPDUS.getRight();

  setCurrentObjectPos(newPickedPoint + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

D3DXVECTOR3 D3SceneEditor::getCurrentObjectPos() {
  D3SceneObject *obj = getCurrentObject();
  assert(obj != NULL);
  switch(obj->getType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_LIGHTCONTROL  :
  case SOTYPE_ANIMATEDOBJECT:
    return obj->getPos();
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
    return D3DXVECTOR3(0,0,0);
  }
}

void D3SceneEditor::setCurrentObjectPos(const D3DXVECTOR3 &pos) {
  D3SceneObject *obj = getCurrentObject();
  if(obj == NULL) return;
  switch(obj->getType()) {
  case SOTYPE_LIGHTCONTROL:
    getScene().setLightPosition(getCurrentLightControl()->getLightIndex(), pos);
    // continue case
  case SOTYPE_VISUALOBJECT:
    obj->setPos(pos);
    render(RENDER_ALL);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
  }
}

void D3SceneEditor::setCurrentObjectOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  obj->getPDUS().setOrientation(dir, up);
  render(RENDER_ALL);
}

void D3SceneEditor::setCurrentObjectScale(const D3DXVECTOR3 &scale) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  obj->getPDUS().setScale(scale);
  render(RENDER_ALL);
}

void D3SceneEditor::OnMouseWheelObjectPos(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = getCurrentObjectPos();
      const D3DXVECTOR3 dir    = objPos - getScene().getCameraPos();
      const float       dist   = length(dir);
      if(dist > 0) {
        setCurrentObjectPos(objPos + unitVector(dir) * dist / 30.0 * (float)sign(zDelta));
      }
    }
    break;
  case MK_CONTROL  :
    rotateCurrentObjectFrwBckw(0, (double)sign(zDelta) / -50.0);
    break;
  case MK_SHIFT    :
    rotateCurrentObjectFrwBckw((double)sign(zDelta) / -50, 0);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentObjectLeftRight((double)sign(zDelta) / 50.0);
    break;
  }
}

void D3SceneEditor::rotateCurrentObjectFrwBckw(double angle1, double angle2) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();
  const D3DXVECTOR3 cameraUp    = getScene().getCameraUp();
  const D3DXVECTOR3 cameraRight = getScene().getCameraRight();
  const D3DXVECTOR3 newDir      = rotate(rotate(pdus.getDir(),cameraUp,angle1),cameraRight,angle2);
  const D3DXVECTOR3 newUp       = rotate(rotate(pdus.getUp() ,cameraUp,angle1),cameraRight,angle2);
  setCurrentObjectOrientation(newDir, newUp);
}

void D3SceneEditor::rotateCurrentObjectLeftRight(double angle) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();
  const D3DXVECTOR3 cameraDir = getScene().getCameraDir();
  setCurrentObjectOrientation(rotate(pdus.getDir(),cameraDir, angle)
                             ,rotate(pdus.getUp(), cameraDir, angle));
}

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

void D3SceneEditor::OnMouseWheelObjectScale(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustCurrentObjectScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustCurrentObjectScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustCurrentObjectScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustCurrentObjectScale(VADJUST_Z  , factor); break;
  }
}

void D3SceneEditor::OnMouseWheelObjectPosKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 camPos  = getScene().getCameraPos();
      const float       dist    = length(m_focusPoint - camPos);
      D3DXVECTOR3       dp      = pdus.getPos() - m_focusPoint;
      m_focusPoint = camPos + (float)(dist * (1.0 - (float)sign(zDelta)/30)) * getScene().getCameraDir();
      setCurrentObjectPos(m_focusPoint + dp);
    }
    break;
  case MK_CONTROL  :
  case MK_SHIFT    :
  case MK_CTRLSHIFT:
    { disableRender();
      const D3DXVECTOR3 objFocus = invers(pdus.getRotationMatrix()) * (m_focusPoint - pdus.getPos());
      OnMouseWheelObjectPos(nFlags, zDelta, pt);
      enableRender();
      setCurrentObjectPos(m_focusPoint - pdus.getRotationMatrix() * objFocus);
    }
    break;
  }
}

void D3SceneEditor::OnMouseWheelObjectScaleKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();

  disableRender();
  const D3DXVECTOR3 objFocus = invers(pdus.getScaleMatrix()) * (m_focusPoint - pdus.getPos());
  OnMouseWheelObjectScale(nFlags, zDelta, pt);
  enableRender();
  setCurrentObjectPos(m_focusPoint - pdus.getScaleMatrix() * objFocus);
}

void D3SceneEditor::OnMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
  }
}

void D3SceneEditor::adjustCurrentObjectScale(int component, double factor) {
  D3SceneObject *obj = getCurrentVisualObject();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();
  D3DXVECTOR3 scale = pdus.getScale();
  if(component & VADJUST_X) {
    scale.x *= (float)factor;
  }
  if(component & VADJUST_Y) {
    scale.y *= (float)factor;
  }
  if(component & VADJUST_Z) {
    scale.z *= (float)factor;
  }
  setCurrentObjectScale(scale);
}

// ------------------------------------- controlling camera -----------------------------------------

void D3SceneEditor::OnMouseMoveCameraWalk(UINT nFlags, CPoint pt) {
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

void D3SceneEditor::OnMouseWheelCameraWalk( UINT nFlags, short zDelta, CPoint pt) {
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

void D3SceneEditor::OnMouseWheelCameraKeepFocus(UINT nFlags, short zDelta, CPoint pt) {
  const D3DXVECTOR3     &center  = m_focusPoint;
        D3PosDirUpScale  camPDUS = getScene().getCameraPDUS();
  const float            radius  = length(center - camPDUS.getPos());
  
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setCameraPos(camPDUS.getPos() + camPDUS.getDir() * radius / 30.0f * (float)sign(zDelta));
    break;
  case MK_CONTROL  :
    {       D3DXVECTOR3 newPos = camPDUS.getPos() + unitVector(camPDUS.getUp()) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      const D3DXVECTOR3 newUp  = crossProduct(camPDUS.getRight(), newDir);
      getScene().setCameraPDUS(camPDUS.setPos(center - newDir * radius)
                                      .setOrientation(newDir,newUp));
    }
    break;
  case MK_SHIFT    :
    { const D3DXVECTOR3 newPos = camPDUS.getPos() + unitVector(camPDUS.getRight()) * radius / 20.0f * (float)sign(zDelta);
      const D3DXVECTOR3 newDir = unitVector(center - newPos);
      getScene().setCameraPDUS(camPDUS.setPos(center - newDir * radius)
                                      .setOrientation(newDir, camPDUS.getUp()));
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void D3SceneEditor::walkWithCamera(double dist, double angle) {
  D3PosDirUpScale    pdus = getScene().getCameraPDUS();
  const D3DXVECTOR3 &dir  = pdus.getDir();
  const D3DXVECTOR3 &up   = pdus.getUp();
  getScene().setCameraPDUS(
    pdus.setPos(pdus.getPos() + (float)dist * dir)
        .setOrientation(rotate(dir, up, angle), up));
}

void D3SceneEditor::sidewalkWithCamera(double upDist, double rightDist) {
  D3PosDirUpScale pdus = getScene().getCameraPDUS();
  getScene().setCameraPDUS(
     pdus.setPos(pdus.getPos() 
               + (float)upDist    * pdus.getUp()
               + (float)rightDist * pdus.getRight()));

}

void D3SceneEditor::rotateCameraUpDown(double angle) {
  D3PosDirUpScale   pdus  = getScene().getCameraPDUS();
  const D3DXVECTOR3 right = pdus.getRight();
  getScene().setCameraPDUS(
    pdus.setOrientation(
       rotate(pdus.getDir(), right, angle)
      ,rotate(pdus.getUp() , right, angle)));
}

void D3SceneEditor::rotateCameraLeftRight(double angle) {
  D3PosDirUpScale    pdus = getScene().getCameraPDUS();
  const D3DXVECTOR3 &dir  = pdus.getDir();
  getScene().setCameraPDUS(
    pdus.setOrientation(
       dir
      ,rotate(pdus.getUp(), dir, angle)));
}

// ------------------------------------- controlling lights -----------------------------------------

void D3SceneEditor::OnMouseMoveLight(UINT nFlags, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  if(lc) {
    switch(lc->getLightType()) {
    case D3DLIGHT_POINT:
      OnMouseMoveLightPoint(nFlags, pt);
      break;
    case D3DLIGHT_SPOT :
      OnMouseMoveLightSpot(nFlags, pt);
      break;
    }
  }
}

void D3SceneEditor::OnMouseMoveLightPoint(UINT nFlags, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjectXZ(pt);
    break;
  case MK_CONTROL  :
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void D3SceneEditor::OnMouseMoveLightSpot(UINT nFlags, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  const LIGHT       param = lc->getLightParam();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjectXZ(pt);
    break;
  case MK_CONTROL  :
    { const double angle1 = (double)(pt.x - m_lastMouse.x) / 100.0;
      const double angle2 = (double)(pt.y - m_lastMouse.y) / 100.0;
      const D3DXVECTOR3 newDir = rotate(rotate(dir,getScene().getCameraUp(),angle1),getScene().getCameraRight(),angle2);
      getScene().setLightDirection(lc->getLightIndex(), newDir);
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void D3SceneEditor::OnMouseWheelLight(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  switch(lc->getLightType()) {
  case D3DLIGHT_DIRECTIONAL: OnMouseWheelLightDirectional(nFlags, zDelta, pt); break;
  case D3DLIGHT_POINT      : OnMouseWheelLightPoint(      nFlags, zDelta, pt); break;
  case D3DLIGHT_SPOT       : OnMouseWheelLightSpot(       nFlags, zDelta, pt); break;
  }
}

void D3SceneEditor::OnMouseWheelLightDirectional(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_DIRECTIONAL));
  D3LightControlDirectional &ctrl = *(D3LightControlDirectional*)lc;
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

void D3SceneEditor::OnMouseWheelLightPoint(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  D3LightControlPoint &ctrl = *(D3LightControlPoint*)lc;
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

void D3SceneEditor::OnMouseWheelLightSpot(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  D3LightControlSpot &ctrl = *(D3LightControlSpot*)lc;
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

void D3SceneEditor::OnMouseWheelLightSpotAngle(UINT nFlags, short zDelta, CPoint pt) {
  const D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL || (lc->getLightType() != D3DLIGHT_SPOT)) {
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

void D3SceneEditor::OnAddLightDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void D3SceneEditor::OnAddLightPoint()       { addLight(D3DLIGHT_POINT);       }
void D3SceneEditor::OnAddLightSpot()        { addLight(D3DLIGHT_SPOT);        }

void D3SceneEditor::addLight(D3DLIGHTTYPE type) {
  LIGHT lp = getScene().getDefaultLightParam(type);
  switch(type) {
  case D3DLIGHT_DIRECTIONAL:
    break;
  case D3DLIGHT_POINT      :
    lp.Position = m_pickedRay.m_orig + m_pickedRay.m_dir * 2;
    break;
  case D3DLIGHT_SPOT       :
    lp.Position  = m_pickedRay.m_orig + m_pickedRay.m_dir * 3;
    lp.Direction = unitVector((getScene().getCameraPos() + 5 * getScene().getCameraDir()) - lp.Position);
    break;
  }
  getScene().setLightParam(lp);
  getScene().setLightControlVisible(lp.m_index, true);
}

void D3SceneEditor::setCurrentObject(D3SceneObject *obj) {
  m_currentSceneObject = obj;
  const D3LightControl *lc = getCurrentLightControl();
  if(lc) {
    m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  } else if(obj) {
    if(obj->hasMaterial()) {
      m_materialDlgThread->setCurrentDialogProperty(&obj->getMaterial());
    }
  }
  render(RENDER_INFO);
}

D3SceneObject *D3SceneEditor::getCurrentVisualObject() {
  if(m_currentSceneObject == NULL) {
    return NULL;
  }
  switch(m_currentSceneObject->getType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return m_currentSceneObject;
  default:
    return NULL;
  }
}

D3AnimatedSurface *D3SceneEditor::getCurrentAnimatedobject() const {
  if(m_currentSceneObject == NULL || m_currentSceneObject->getType() != SOTYPE_ANIMATEDOBJECT) {
    return NULL;
  }
  return (D3AnimatedSurface*)m_currentSceneObject;
}

D3LightControl *D3SceneEditor::getCurrentLightControl() {
  if((m_currentSceneObject == NULL) || (m_currentSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_currentSceneObject;
}

const D3LightControl *D3SceneEditor::getCurrentLightControl() const {
  if((m_currentSceneObject == NULL) || (m_currentSceneObject->getType() != SOTYPE_LIGHTCONTROL)) {
    return NULL;
  }
  return (D3LightControl*)m_currentSceneObject;
}

void D3SceneEditor::setLightControlsVisible(bool visible) {
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

void D3SceneEditor::setSpotToPointAt(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
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

void D3SceneEditor::OnContextMenuSceneObject(CPoint point) {
  switch(m_currentSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    OnContextMenuVisualObject(point);
    break;
  case SOTYPE_LIGHTCONTROL:
    OnContextMenuLightControl(point);
    break;
  }
}

CMenu &D3SceneEditor::loadMenu(CMenu &menu, int id) {
  const int ret = menu.LoadMenu(id);
  if(!ret) {
    throwException(_T("Loadmenu(%d) failed"), id);
  }
  return menu;
}

void D3SceneEditor::showContextMenu(CMenu &menu, CPoint point) {
  TrackPopupMenu(*menu.GetSubMenu(0), TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, 0, *getMessageWindow(),NULL);
}

static String lightMenuText(const LIGHT &light) {
  const TCHAR enabledChar = light.m_enabled?_T('+'):_T('-');

  switch(light.Type) {
  case D3DLIGHT_DIRECTIONAL: return format(_T("%3d%c Directional. Dir:%s")
                                          ,light.m_index,enabledChar
                                          ,toString(light.Direction).cstr());
  case D3DLIGHT_POINT      : return format(_T("%3d%c Point. Pos:%s")
                                          ,light.m_index,enabledChar
                                          ,toString(light.Position).cstr()
                                          );
  case D3DLIGHT_SPOT       : return format(_T("%3d%c Spot. Pos:%s, Dir:%s")
                                          ,light.m_index,enabledChar
                                          ,toString(light.Position).cstr()
                                          ,toString(light.Direction).cstr()
                                          );
  }
  return EMPTYSTRING;
}

void D3SceneEditor::OnContextMenuBackground(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_BACKGROUND);
  const int visibleLightCount = (int)getScene().getLightsVisible().size();
  if(visibleLightCount == 0) {
    removeMenuItem(menu,ID_HIDE_LIGHTCONTROLS);
  }
  if(visibleLightCount == getScene().getLightCount()) {
    removeMenuItem(menu, ID_SHOW_LIGHTCONTROLS);
  }
  const CompactArray<LIGHT> lights = getScene().getAllLights();
  BitSet definedLights(getScene().getMaxLightCount());
  for(size_t i = 0; i < lights.size(); i++) {
    const LIGHT &l = lights[i];
    definedLights.add(l.m_index);
    setMenuItemText(menu, ID_SELECT_LIGHT0 + l.m_index, lightMenuText(l));
   }
   definedLights.invert().remove(21,definedLights.getCapacity()-1);
   for (Iterator<size_t> it = definedLights.getIterator(); it.hasNext();) {
     removeMenuItem(menu, (int)it.next() + ID_SELECT_LIGHT0);
   }
   switch(getScene().getFillMode()) {
   case D3DFILL_POINT    : removeMenuItem(menu, ID_FILLMODE_POINT      ); break;
   case D3DFILL_WIREFRAME: removeMenuItem(menu, ID_FILLMODE_WIREFRAME  ); break;
   case D3DFILL_SOLID    : removeMenuItem(menu, ID_FILLMODE_SOLID      ); break;
   }
   switch(getScene().getShadeMode()) {
   case D3DSHADE_FLAT    : removeMenuItem(menu, ID_SHADING_FLAT        ); break;
   case D3DSHADE_GOURAUD : removeMenuItem(menu, ID_SHADING_GOURAUD     ); break;
   case D3DSHADE_PHONG   : removeMenuItem(menu, ID_SHADING_PHONG       ); break;
   }
   removeMenuItem(menu, isCoordinateSystemVisible()
                       ?ID_SHOWCOORDINATESYSTEM
                       :ID_HIDECOORDINATESYSTEM);
   removeMenuItem(menu, getScene().isSpecularEnabled()
                       ?ID_ENABLE_SPECULARHIGHLIGHT
                       :ID_DISABLE_SPECULARHIGHLIGHT);

   m_sceneContainer->modifyContextMenu(*menu.GetSubMenu(0));
   showContextMenu(menu, point);
}

void D3SceneEditor::OnContextMenuVisualObject(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_VISUALOBJECT);

  int checkedItem = -1;
  switch(m_currentControl) {
  case CONTROL_OBJECT_POS      : checkedItem = ID_CONTROL_OBJECT_POS      ; break;
  case CONTROL_OBJECT_SCALE    : checkedItem = ID_CONTROL_OBJECT_SCALE    ; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }
  switch(m_currentSceneObject->getType()) {
  case SOTYPE_VISUALOBJECT  :
    removeMenuItem(menu, ID_OBJECT_STARTANIMATION   );
    removeMenuItem(menu, ID_OBJECT_STARTBCKANIMATION);
    removeMenuItem(menu, ID_OBJECT_STARTALTANIMATION);
    removeMenuItem(menu, ID_OBJECT_REVERSEANIMATION );
    removeMenuItem(menu, ID_OBJECT_STOPANIMATION    );
    removeMenuItem(menu, ID_OBJECT_CONTROL_SPEED    );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3AnimatedSurface *obj = getCurrentAnimatedobject();
      if(obj->isRunning()) {
        removeMenuItem(menu, ID_OBJECT_STARTANIMATION   );
        removeMenuItem(menu, ID_OBJECT_STARTBCKANIMATION);
        removeMenuItem(menu, ID_OBJECT_STARTALTANIMATION);
        switch(obj->getAnimationType()) {
        case ANIMATE_ALTERNATING:
          removeMenuItem(menu, ID_OBJECT_REVERSEANIMATION );
          break;
        }
      } else {
        removeMenuItem(menu, ID_OBJECT_REVERSEANIMATION );
        removeMenuItem(menu, ID_OBJECT_STOPANIMATION    );
        removeMenuItem(menu, ID_OBJECT_CONTROL_SPEED    );
      }
    }
    break;
  }
  if(m_currentSceneObject) {
    m_currentSceneObject->modifyContextMenu(*menu.GetSubMenu(0));
  }
  showContextMenu(menu, point);
}

void D3SceneEditor::OnContextMenuLightControl(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  const LIGHT light = lc->getLightParam();
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_LIGHTCONTROL);
  if(light.isEnabled()) { // light is on
    removeMenuItem(menu, ID_LIGHT_ENSABLE);
  } else {                                             // light is off
    removeMenuItem(menu, ID_LIGHT_DISABLE      );
    removeMenuItem(menu, ID_LIGHT_ADJUSTCOLORS );
    removeMenuItem(menu, ID_LIGHTCONTROL_SPOTAT);
  }
  if(light.Type != D3DLIGHT_SPOT) {
    removeMenuItem(menu, ID_LIGHTCONTROL_SPOTAT);
    removeMenuItem(menu, ID_LIGHT_ADJUSTANGLES );
  }
  showContextMenu(menu, point);
}

void D3SceneEditor::OnControlCameraWalk()      {  setCurrentControl(CONTROL_CAMERA_WALK        ); }
void D3SceneEditor::OnControlObjectPos()       {  setCurrentControl(CONTROL_OBJECT_POS         ); }
void D3SceneEditor::OnControlObjectScale()     {  setCurrentControl(CONTROL_OBJECT_SCALE       ); }

void D3SceneEditor::OnControlObjectMoveRotate() {
//  setSelectedObject(m_calculatedObject);
  setCurrentControl(CONTROL_OBJECT_POS);
}

void D3SceneEditor::OnControlObjectKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_POS_KEEPFOCUS);
  }
}

void D3SceneEditor::OnControlObjectScaleKeepFocus() {
  if(moveLastMouseToFocusPoint()) {
    setCurrentControl(CONTROL_OBJECT_SCALE_KEEPFOCUS);
  }
}

bool D3SceneEditor::moveLastMouseToFocusPoint() {
  D3DXVECTOR3 hitPoint;
  D3SceneObject *obj = getScene().getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &hitPoint);
  if((obj == NULL) || (obj != m_currentSceneObject)) {
    return false;
  }

  const D3DXVECTOR3 dp       = obj->getPos() - hitPoint;
  const D3DXVECTOR3 camPos   = getScene().getCameraPos();
  const float       distance = length(camPos - hitPoint);
  m_focusPoint = camPos + distance * getScene().getCameraDir();

  obj->setPos(m_focusPoint + dp);
  return true;
}

void D3SceneEditor::OnControlCameraKeepFocus() {
  D3SceneObject *obj = getScene().getPickedObject(m_lastMouse, ~PICK_LIGHTCONTROL, &m_focusPoint, &m_pickedInfo);
  if(obj == NULL) return;
  getScene().setCameraLookAt(m_focusPoint);
  setCurrentControl(CONTROL_CAMERA_KEEPFOCUS);
}

void D3SceneEditor::OnObjectControlSpeed() {
  setCurrentControl(CONTROL_ANIMATION_SPEED);
}

void D3SceneEditor::setCurrentControl(CurrentObjectControl control) {
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
    setCurrentObject(NULL);
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

void D3SceneEditor::OnObjectEditMaterial() {
  if(m_currentSceneObject 
  && m_currentSceneObject->getType() == SOTYPE_VISUALOBJECT
  && m_currentSceneObject->hasMaterial()) {
    m_materialDlgThread->setCurrentDialogProperty(&m_currentSceneObject->getMaterial());
    m_materialDlgThread->setDialogVisible(true);
    m_lightDlgThread->setDialogVisible(false);
    setCurrentControl(CONTROL_MATERIAL);
  }
}

void D3SceneEditor::OnObjectResetScale() {
  getScene().getObjPDUS().setScaleAll(1);
}

void D3SceneEditor::OnObjectStartAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_FORWARD);
}

void D3SceneEditor::OnObjectStartBckAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_BACKWARD);
}

void D3SceneEditor::OnObjectStartAltAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
  if(obj == NULL) return;
  obj->startAnimation(ANIMATE_ALTERNATING);
}

void D3SceneEditor::OnObjectReverseAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
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

void D3SceneEditor::OnObjectStopAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedobject();
  if(obj == NULL) return;
  obj->stopAnimation();
}

void D3SceneEditor::OnObjectRemove() {
  if(m_currentSceneObject == NULL) return;
  getScene().removeSceneObject(m_currentSceneObject);
  delete m_currentSceneObject;
  setCurrentObject(NULL);
  render(RENDER_ALL);
}

void D3SceneEditor::setLightEnabled(bool enabled) {
  getScene().setLightEnabled(getCurrentLightControl()->getLightIndex(), enabled);
  render(RENDER_ALL);
}

void D3SceneEditor::OnLightAdjustColors() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  m_lightDlgThread->setCurrentDialogProperty(&lc->getLightParam());
  m_lightDlgThread->setDialogVisible(true);
  m_materialDlgThread->setDialogVisible(false);
  setCurrentControl(CONTROL_LIGHTCOLOR);
}

void D3SceneEditor::OnLightAdjustAngles() {
  setCurrentControl(CONTROL_SPOTANGLES);
}

void D3SceneEditor::OnLightControlSpotAt() {
  setCurrentControl(CONTROL_SPOTLIGHTPOINT);
}

void D3SceneEditor::OnLightControlHide() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  getScene().setLightControlVisible(lc->getLightIndex(), false);
  render(RENDER_ALL);
}

void D3SceneEditor::OnLightRemove() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  const int lightIndex = lc->getLightIndex();
  getScene().removeLight(lightIndex);
  setCurrentObject(NULL);
  render(RENDER_ALL);
}

void D3SceneEditor::OnEditAmbientLight() {
  const D3DCOLOR oldColor = getScene().getGlobalAmbientColor();
  CColorDlg dlg("Ambient color", SP_AMBIENTLIGHT, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_AMBIENTLIGHTCOLOR);
  if(dlg.DoModal() != IDOK) {
    getScene().setGlobalAmbientColor(oldColor);
    getScene().render();
  }
  m_currentEditor = NULL;
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::OnEditBackgroundColor() {
  const D3DCOLOR oldColor = getScene().getBackgroundColor();
  CColorDlg dlg("Background color", SP_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_BACKGROUNDCOLOR);

  if(dlg.DoModal() != IDOK) {
    getScene().setBackgroundColor(oldColor);
  }
  m_currentEditor = NULL;
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::OnFillmodePoint()     { getScene().setFillMode(D3DFILL_POINT     ); }
void D3SceneEditor::OnFillmodeWireframe() { getScene().setFillMode(D3DFILL_WIREFRAME ); }
void D3SceneEditor::OnFillmodeSolid()     { getScene().setFillMode(D3DFILL_SOLID     ); }

void D3SceneEditor::OnShadingFlat()       { getScene().setShadeMode(D3DSHADE_FLAT    ); }
void D3SceneEditor::OnShadingGouraud()    { getScene().setShadeMode(D3DSHADE_GOURAUD ); }
void D3SceneEditor::OnShadingPhong()      { getScene().setShadeMode(D3DSHADE_PHONG   ); }

void D3SceneEditor::setCoordinateSystemVisible(bool visible) {
  if(visible) {
    if(m_coordinateSystem == NULL) {
      m_coordinateSystem  = new D3CoordinateSystem(getScene());
      getScene().addSceneObject(m_coordinateSystem);
    } else {
      m_coordinateSystem->setVisible(true);
    }
  } else {
    if(isCoordinateSystemVisible()) {
      m_coordinateSystem->setVisible(false);
    }
  }
  render(RENDER_3D);
}

static const TCHAR *extensions = _T("Scene-files (*.scn)\0*.scn\0\0");

void D3SceneEditor::OnSaveSceneParameters() {
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

void D3SceneEditor::OnLoadSceneParameters() {
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
  caseStr(MATERIAL               )
  caseStr(LIGHTCOLOR             )
  caseStr(BACKGROUNDCOLOR        )
  caseStr(AMBIENTLIGHTCOLOR      )
  default: return _T("?");
  }
}

String D3SceneEditor::stateFlagsToString() const {
  String result = _T("(");
  const TCHAR *delim = NULL;
#define ADDDELIM { if(delim) result += delim; else delim=_T(" "); }
  if(isEnabled())                { ADDDELIM; result += _T("enabled"); }
  if(isPropertyChangesEnabled()) { ADDDELIM; result += _T("prop");    }
  if(isMouseVisible())           { ADDDELIM; result += _T("mouse");   }
  result += _T(")");
  return result;
}

String D3SceneEditor::toString() const {
  if(!isEnabled()) return EMPTYSTRING;

  String result = format(_T("Current Motion:%s Selected:%s %s")
                        ,controlString(m_currentControl)
                        ,m_currentSceneObject?m_currentSceneObject->getName().cstr():_T("--")
                        ,stateFlagsToString().cstr()
                        );
  if(m_pickedRay.isSet()) {
    result += format(_T("\nPicked ray:%s"), m_pickedRay.toString().cstr());
    if (m_pickedInfo.isSet()) {
      result += format(_T("\nPicked point:%s, info:%s")
                      ,::toString(m_pickedPoint).cstr()
                      ,m_pickedInfo.toString().cstr());
    }
  }
  switch (m_currentControl) {
  case CONTROL_IDLE                  :
    return result;
  case CONTROL_CAMERA_WALK           :
    return result + format(_T("\n%s\nView angel:%.1lf, zn:%.3lf")
                           ,getScene().getCameraString().cstr()
                           ,degrees(getScene().getViewAngel())
                           ,getScene().getNearViewPlane());

  case CONTROL_CAMERA_KEEPFOCUS      :
    result += format(_T("\n%s\nView angel:%.1lf, zn:%.3lf")
                    ,getScene().getCameraString().cstr()
                    ,degrees(getScene().getViewAngel())
                    ,getScene().getNearViewPlane());
    if(hasFocusPoint()) {
      result += format(_T("\nFocuspoint:%s"), ::toString(m_focusPoint).cstr());
    }
    return result;

  case CONTROL_OBJECT_POS            :
  case CONTROL_OBJECT_SCALE          :
    if(m_currentSceneObject != NULL) {
      result += format(_T("\nObject:\n%s")
                      ,m_currentSceneObject->getPDUS().toString().cstr());
    }
    return result;
  case CONTROL_OBJECT_POS_KEEPFOCUS  :
  case CONTROL_OBJECT_SCALE_KEEPFOCUS:
    if(m_currentSceneObject != NULL) {
      result += format(_T("\nObject:\n%s")
                      ,m_currentSceneObject->getPDUS().toString().cstr());
    }
    if(hasFocusPoint()) {
      result += format(_T("\nFocuspoint:%s"), ::toString(m_focusPoint).cstr());
    }
    return result ;

  case CONTROL_LIGHTCOLOR            :
  case CONTROL_LIGHT                 :
  case CONTROL_SPOTLIGHTPOINT        :
  case CONTROL_SPOTANGLES            :
    { const D3LightControl *lc = getCurrentLightControl();
      if(lc) {
        result += format(_T("\nLight:%s"), ::toString(lc->getLightParam()).cstr());
      }
      return result;
    }
  case CONTROL_ANIMATION_SPEED       :
    { D3AnimatedSurface *obj = getCurrentAnimatedobject();
      if(obj) {
        result += format(_T("\nFrames/sec:%.2lf"), obj->getFramePerSec());
      }
      return result;
    }
  case CONTROL_MATERIAL              :
    if(m_currentSceneObject && m_currentSceneObject->hasMaterial()) {
      result += format(_T("\nMaterial:%s"), ::toString(m_currentSceneObject->getMaterial()).cstr());
    }
    return result;
    
  case CONTROL_BACKGROUNDCOLOR       :
    return result + format(_T("\nBackground color:%s")
                          ,::toString(getScene().getBackgroundColor(),false).cstr());
  case CONTROL_AMBIENTLIGHTCOLOR     :
    return result + format(_T("\nAmbient color:%s")
                          ,::toString(getScene().getGlobalAmbientColor(),false).cstr());
  }
  return result;
}
