#include "pch.h"
#include <ByteFile.h>
#include <MFCUtil/ColorDlg.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>
#include <D3DGraphics/D3SelectedCube.h>

D3SceneEditor::D3SceneEditor()
: m_sceneContainer(  NULL)
, m_currentEditor(   NULL)
, m_currentObj(      NULL)
, m_currentControl(  CONTROL_IDLE)
, m_coordinateSystem(NULL)
, m_selectedCube(    NULL)
, m_paramFileName(_T("Untitled"))
, m_stateFlags(      SE_MOUSEVISIBLE)
{
  CHECKINVARIANT();
}

D3SceneEditor::~D3SceneEditor() {
  close();
}

void D3SceneEditor::init(D3SceneContainer *sceneContainer) {
  m_sceneContainer    = sceneContainer;
  CLightDlg *dlg1 = new CLightDlg(   this); TRACE_NEW(dlg1);
  m_propertyDialogMap.addDialog(dlg1);
  CMaterialDlg *dlg2 = new CMaterialDlg(this); TRACE_NEW(dlg2);
  m_propertyDialogMap.addDialog(dlg2);

  m_currentControl = CONTROL_IDLE;
  getScene().addPropertyChangeListener(this);
  setFlags(SE_INITDONE);
  CHECKINVARIANT();
}

void D3SceneEditor::close() {
  CHECKINVARIANT();
  getScene().removePropertyChangeListener(this);
  m_propertyDialogMap.clear();
  getScene().removeSceneObject(m_coordinateSystem);
  SAFEDELETE(m_coordinateSystem);
  getScene().removeSceneObject(m_selectedCube);
  SAFEDELETE(m_selectedCube);
  clrFlags(SE_INITDONE);
}

D3SceneEditor &D3SceneEditor::setEnabled(bool enabled, BYTE flags) {
  if(isInitDone()) {
    flags &= ~SE_INITDONE;
    if(enabled) {
      setFlags(flags);
      render(SE_RENDERINFO);
    } else {
      clrFlags(flags);
      if(!isSet(SE_LIGHTCONTROLS)) {
        OnLightControlHide();
        m_propertyDialogMap.hideDialog();
      }
      render(SE_RENDERINFO);
    }
    setMouseVisible(true);
  }
  return *this;
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
    setFlags(SE_MOUSEVISIBLE);
  } else {
    ShowCursor(FALSE);
    clrFlags(SE_MOUSEVISIBLE);
  }
}

void D3SceneEditor::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(!isSet(SE_PROPCHANGES)) {
    return;
  }
  if(source == &getScene()) {
    switch(id) {
    case SP_CAMERAPDUS              :
    case SP_PROJECTIONTRANSFORMATION:
    case SP_MATERIALPARAMETERS      :
    case SP_LIGHTPARAMETERS         :
    case SP_OBJECTCOUNT             :
    case SP_RIGHTHANDED             :
      render(SE_RENDERALL);
      break;
    case SP_ANIMATIONFRAMEINDEX     :
      render(SE_RENDER3D);
      break;
    }
  } else if(m_propertyDialogMap.hasPropertyContainer(source)) {
    switch(id) {
    case SP_LIGHTPARAMETERS:
      { const LIGHT &newLight = *(LIGHT*)newValue;
        LIGHT tmp = getScene().getLight(newLight.getIndex()); // to keep pos and direction as they are
        if(tmp.getIndex() != newLight.getIndex()) {
          showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__,newLight.getIndex());
        } else {
          getScene().setLight(CLightDlg::copyModifiableValues(tmp, newLight));
        }
      }
      break;
    case SP_MATERIALPARAMETERS:
      getScene().setMaterial(*(MATERIAL*)newValue);
      break;
    }
  } else if(source == m_currentEditor) {
    switch(id) {
    case SP_BACKGROUNDCOLOR   :
      getScene().setBackgroundColor(*(D3DCOLOR*)newValue);
      render(SE_RENDERALL);
      break;
    case SP_AMBIENTCOLOR      :
      getScene().setAmbientColor(*(D3DCOLOR*)newValue);
      render(SE_RENDERALL);
      break;
    }
  }
}

void D3SceneEditor::OnLButtonDown(UINT nFlags, CPoint point) {
  CHECKINVARIANT();
  m_lastMouse = point;
  switch(getCurrentControl()) {
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;

  default:
    { D3SceneObject *pickedObj = getScene().getPickedObject(point, OBJMASK_ALL, &m_pickedPoint, &m_pickedRay, NULL, &m_pickedInfo);
      if(pickedObj == NULL) {
        m_pickedInfo.clear();
        if(getCurrentControl() == CONTROL_CAMERA_WALK) {
          setMouseVisible(false);
          getMessageWindow()->SetCapture();
        } else {
          resetCurrentControl();
        }
      } else {
        resetCurrentControl();
        setCurrentObj(pickedObj);
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
  CHECKINVARIANT();
}

void D3SceneEditor::OnLButtonUp(UINT nFlags, CPoint point) {
  CHECKINVARIANT();
  switch(getCurrentControl()) {
  case CONTROL_SPOTLIGHTPOINT:
    setCurrentControl(CONTROL_IDLE);
    break;
  case CONTROL_OBJECT_POS:
  case CONTROL_CAMERA_WALK:
    setMouseVisible(true);
    ReleaseCapture();
    render(SE_RENDERINFO);
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnContextMenu(CWnd *pwnd, CPoint point) {
  m_lastMouse = screenPTo3DP(point);
  D3SceneObject *pickedObj = getScene().getPickedObject(m_lastMouse, OBJMASK_ALL, &m_pickedPoint, &m_pickedRay,NULL,&m_pickedInfo);
  if(pickedObj == NULL) {
    OnContextMenuBackground(point);
    m_pickedInfo.clear();
  } else {
    resetCurrentControl();
    setCurrentObj(pickedObj);
    OnContextMenuObj(point);
  }
}

void D3SceneEditor::OnMouseMove(UINT nFlags, CPoint point) {
  CHECKINVARIANT();
  if(!(nFlags & MK_LBUTTON)) {
    return;
  }
  switch(getCurrentControl()) {
  case CONTROL_CAMERA_WALK   :
    OnMouseMoveCameraWalk(nFlags, point);
    break;
  case CONTROL_OBJECT_POS    :
    OnMouseMoveObjPos(nFlags, point);
    break;
  case CONTROL_LIGHT         :
    OnMouseMoveLight(nFlags, point);
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;
  }
  m_lastMouse = point;
  CHECKINVARIANT();
}

void D3SceneEditor::OnLButtonDblClk(UINT nFlags, CPoint point) {
}

BOOL D3SceneEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  switch(getCurrentControl()) {
  case CONTROL_CAMERA_WALK           :
    OnMouseWheelCameraWalk(          nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_POS            :
    OnMouseWheelObjPos(              nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_OBJECT_SCALE          :
    OnMouseWheelObjScale(            nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_ANIMATION_SPEED       :
    OnMouseWheelAnimationSpeed(      nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_LIGHT                 :
    OnMouseWheelLight(               nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_SPOTLIGHTANGLES            :
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
      setEnabled(false).setEnabled(true,SE_RENDERINFO).render(SE_RENDERALL);
      return true;
    }
    break;
  case WM_LBUTTONDOWN:
    if(!ptIn3DWindow(pMsg->pt)) return false;
    OnLButtonDown((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_LBUTTONUP  :
    if(!ptIn3DWindow(pMsg->pt)) return false;
    OnLButtonUp((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_MOUSEMOVE  :
    if(!ptIn3DWindow(pMsg->pt)) return false;
    OnMouseMove((UINT)pMsg->wParam, screenPTo3DP(pMsg->pt));
    return true;
  case WM_MOUSEWHEEL :
    if(!ptIn3DWindow(pMsg->pt)) return false;
    OnMouseWheel((UINT)(pMsg->wParam&0xffff), (short)(pMsg->wParam >> 16), screenPTo3DP(pMsg->pt));
    return true;
  case WM_RBUTTONUP:
    if(!ptIn3DWindow(pMsg->pt)) return false;
    OnContextMenu(getMessageWindow(), pMsg->pt);
    return true;
  case WM_COMMAND:
    switch (pMsg->wParam) {
    case ID_CONTROL_OBJECT_POS            : OnControlObjPos()                   ; return true;
    case ID_CONTROL_OBJECT_SCALE          : OnControlObjScale()                 ; return true;
    case ID_CONTROL_OBJECT_MOVEROTATE     : OnControlObjMoveRotate()            ; return true;
    case ID_OBJECT_RESETPOSITION          : OnObjectResetPosition()             ; return true;
    case ID_OBJECT_RESETSCALE             : OnObjectResetScale()                ; return true;
    case ID_OBJECT_RESETORIENTATION       : OnObjectResetOrientation()          ; return true;
    case ID_OBJECT_ADJUSTMATERIAL         : OnObjectEditMaterial()              ; return true;
    case ID_OBJECT_REMOVE                 : OnObjectRemove()                    ; return true;
    case ID_OBJECT_SETCENTEROFROTATION    : OnObjectSetCenterOfRotation()       ; return true;
    case ID_OBJECT_STARTANIMATION         : OnObjectStartAnimation()            ; return true;
    case ID_OBJECT_STARTBCKANIMATION      : OnObjectStartBckAnimation()         ; return true;
    case ID_OBJECT_STARTALTANIMATION      : OnObjectStartAltAnimation()         ; return true;
    case ID_OBJECT_REVERSEANIMATION       : OnObjectReverseAnimation()          ; return true;
    case ID_OBJECT_STOPANIMATION          : OnObjectStopAnimation()             ; return true;
    case ID_OBJECT_CONTROL_SPEED          : OnObjectControlSpeed()              ; return true;
    case ID_OBJECT_CREATECUBE             : OnObjectCreateCube()                ; return true;
    case ID_CONTROL_CAMERA_WALK           : OnControlCameraWalk()               ; return true;
    case ID_RESETCAMERA                   : OnResetCamera()                     ; return true;
    case ID_RIGHTHANDED                   : SetRightHanded(true)                ; return true;
    case ID_LEFTHANDED                    : SetRightHanded(false)               ; return true;
    case ID_EDIT_AMBIENTLIGHT             : OnEditAmbientLight()                ; return true;
    case ID_EDIT_BACKGROUNDCOLOR          : OnEditBackgroundColor()             ; return true;
    case ID_FILLMODE_POINT                : OnFillmodePoint()                   ; return true;
    case ID_FILLMODE_WIREFRAME            : OnFillmodeWireframe()               ; return true;
    case ID_FILLMODE_SOLID                : OnFillmodeSolid()                   ; return true;
    case ID_SHADING_FLAT                  : OnShadingFlat()                     ; return true;
    case ID_SHADING_GOURAUD               : OnShadingGouraud()                  ; return true;
    case ID_SHADING_PHONG                 : OnShadingPhong()                    ; return true;
    case ID_ENABLE_SPECULARHIGHLIGHT      : setSpecularEnable(true)             ; return true;
    case ID_DISABLE_SPECULARHIGHLIGHT     : setSpecularEnable(false)            ; return true;
    case ID_SHOWCOORDINATESYSTEM          : setCoordinateSystemVisible(true)    ; return true;
    case ID_HIDECOORDINATESYSTEM          : setCoordinateSystemVisible(false)   ; return true;
    case ID_ADDLIGHT_DIRECTIONAL          : OnAddLightDirectional()             ; return true;
    case ID_ADDLIGHT_POINT                : OnAddLightPoint()                   ; return true;
    case ID_ADDLIGHT_SPOT                 : OnAddLightSpot()                    ; return true;
    case ID_LIGHT_REMOVE                  : OnLightRemove()                     ; return true;
    case ID_LIGHT_ENSABLE                 : setLightEnabled(true)               ; return true;
    case ID_LIGHT_DISABLE                 : setLightEnabled(false)              ; return true;
    case ID_LIGHT_ADJUSTCOLORS            : OnLightAdjustColors()               ; return true;
    case ID_LIGHT_ADJUSTSPOTANGLES        : OnLightAdjustSpotAngles()           ; return true;
    case ID_SHOW_LIGHTCONTROLS            : setLightControlsVisible(true)       ; return true;
    case ID_HIDE_LIGHTCONTROLS            : setLightControlsVisible(false)      ; return true;
    case ID_LIGHTCONTROL_HIDE             : OnLightControlHide()                ; return true;
    case ID_LIGHTCONTROL_SPOTAT           : OnLightControlSpotAt()              ; return true;
    case ID_LIGHTCONTROL_ENABLEEFFECT     : setLightControlRenderEffect(true)   ; return true;
    case ID_LIGHTCONTROL_DISABLEEFFECT    : setLightControlRenderEffect(false)  ; return true;
//    case ID_SAVESCENEPARAMETERS           : OnSaveSceneParameters()             ; return true;
//    case ID_LOADSCENEPARAMETERS           : OnLoadSceneParameters()             ; return true;
    default:
      if((ID_SELECT_LIGHT0 <= pMsg->wParam) && (pMsg->wParam <= ID_SELECT_LIGHT20)) {
        const int index = (int)pMsg->wParam - ID_SELECT_LIGHT0;
        setCurrentObj(getScene().setLightControlVisible(index, true));
        return true;
      }
    }
  }
  return false;
}

// ---------------------------------- Controlling object -----------------------------------------

void D3SceneEditor::OnMouseMoveObjPos(UINT nFlags, CPoint pt) {
  CHECKINVARIANT();
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjXZ(pt);
    break;
  case MK_CONTROL  :
    rotateCurrentVisualFrwBckw(float(pt.x - m_lastMouse.x) / 100.0f
                              ,float(pt.y - m_lastMouse.y) / 100.0f);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentVisualLeftRight(float(pt.x - m_lastMouse.x) / 100.0f);
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::moveCurrentObjXY(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentObjPos() - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickedRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.getHitPoint(dist);
  setCurrentObjPos(newPickedPoint  + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void D3SceneEditor::moveCurrentObjXZ(CPoint pt) {
  const D3DXVECTOR3      dp             = getCurrentObjPos() - m_pickedPoint;
  const float            dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray            newPickedRay   = getScene().getPickedRay(pt);
  const D3Ray            ray1           = getScene().getPickedRay(CPoint(pt.x,pt.y+1));
  const float            dRaydPixel     = length((newPickedRay.m_orig + dist * newPickedRay.m_dir) - (ray1.m_orig + dist * ray1.m_dir));
  const CSize            dMouse         = pt - m_lastMouse;
  const D3PosDirUpScale &camPDUS        = getScene().getCamPDUS();
  D3DXVECTOR3            newPickedPoint = m_pickedPoint
                                        - (dRaydPixel * dMouse.cy) * camPDUS.getDir()
                                        + (dRaydPixel * dMouse.cx) * camPDUS.getRight();

  setCurrentObjPos(newPickedPoint + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

D3DXVECTOR3 D3SceneEditor::getCurrentObjPos() {
  CHECKINVARIANT();
  switch(getCurrentObjType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_LIGHTCONTROL  :
  case SOTYPE_ANIMATEDOBJECT:
    return getCurrentObj()->getPos();
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), getCurrentObjType());
    return D3DXVECTOR3(0,0,0);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentObjPos(const D3DXVECTOR3 &pos) {
  CHECKINVARIANT();
  D3SceneObject *obj = getCurrentObj();
  if(obj == NULL) return;
  switch(obj->getType()) {
  case SOTYPE_LIGHTCONTROL  :
    getScene().setLightPosition(getCurrentLightControl()->getLightIndex(), pos);
    // continue case
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    obj->setPos(pos);
    render(SE_RENDERALL);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualPDUS(const D3PosDirUpScale &pdus) {
  CHECKINVARIANT();
  D3SceneObject *obj = getCurrentVisual();
  if(obj) {
    obj->getPDUS() = pdus;
    render(SE_RENDERALL);
  }
  CHECKINVARIANT();
}

const D3PosDirUpScale *D3SceneEditor::getCurrentVisualPDUS() const {
  CHECKINVARIANT();
  D3SceneObject *obj = getCurrentVisual();
  return (obj == NULL) ? NULL : &obj->getPDUS();
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus == NULL) return;
  const D3DXVECTOR3 centerOfRot = getCenterOfRotation();
  if(centerOfRot == D3DXORIGIN) {
    setCurrentVisualPDUS(D3PosDirUpScale(*pdus).setOrientation(dir, up));
  } else {
    D3PosDirUpScale tmp = *pdus;
    const D3DXVECTOR3 crOldPos = tmp.getWorldMatrix() * centerOfRot;
    const D3DXVECTOR3 crNewPos = tmp.setOrientation(dir, up).getWorldMatrix() * centerOfRot;
    setCurrentVisualPDUS(tmp.setPos(tmp.getPos() + crOldPos - crNewPos));
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualScale(const D3DXVECTOR3 &scale) {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus) setCurrentVisualPDUS(D3PosDirUpScale(*pdus).setScale(scale));
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectResetPosition() {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus) setCurrentVisualPDUS(D3PosDirUpScale(*pdus).resetPos());
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectResetScale() {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus) setCurrentVisualPDUS(D3PosDirUpScale(*pdus).resetScale());
  CHECKINVARIANT();
}
void D3SceneEditor::OnObjectResetOrientation() {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus) setCurrentVisualPDUS(D3PosDirUpScale(*pdus).resetOrientation());
  CHECKINVARIANT();
}

#define signDelta ((float)sign(zDelta))

void D3SceneEditor::OnMouseWheelObjPos(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = getCurrentObjPos();
      const D3DXVECTOR3 dir    = objPos - getScene().getCamPos();
      const float       dist   = length(dir);
      if(dist > 0) {
        setCurrentObjPos(objPos + unitVector(dir) * dist / 30.0f * signDelta);
      }
    }
    break;
  case MK_CONTROL  :
    rotateCurrentVisualFrwBckw(0, signDelta / -50.0f   );
    break;
  case MK_SHIFT    :
    rotateCurrentVisualFrwBckw(   signDelta / -50.0f, 0);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentVisualLeftRight( signDelta / 50.0f    );
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::rotateCurrentVisualFrwBckw(float angle1, float angle2) {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus == NULL) return;
  const D3DXVECTOR3 cameraUp    = getScene().getCamUp();
  const D3DXVECTOR3 cameraRight = getScene().getCamRight();
  const D3DXVECTOR3 newDir      = rotate(rotate(pdus->getDir(),cameraUp,angle1),cameraRight,angle2);
  const D3DXVECTOR3 newUp       = rotate(rotate(pdus->getUp() ,cameraUp,angle1),cameraRight,angle2);
  setCurrentVisualOrientation(newDir, newUp);
  CHECKINVARIANT();
}

void D3SceneEditor::rotateCurrentVisualLeftRight(float angle) {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus == NULL) return;
  const D3DXVECTOR3 cameraDir = getScene().getCamDir();
  const D3DXVECTOR3 newDir    = rotate(pdus->getDir(), cameraDir, angle);
  const D3DXVECTOR3 newUp     = rotate(pdus->getUp() , cameraDir, angle);
  setCurrentVisualOrientation(newDir, newUp);
  CHECKINVARIANT();
}

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

void D3SceneEditor::OnMouseWheelObjScale(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  const float factor = (1.0f+ 0.04f*signDelta);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustCurrentVisualScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustCurrentVisualScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustCurrentVisualScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustCurrentVisualScale(VADJUST_Z  , factor); break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::adjustCurrentVisualScale(int component, float factor) {
  CHECKINVARIANT();
  const D3PosDirUpScale *pdus = getCurrentVisualPDUS();
  if(pdus == NULL) return;
  D3DXVECTOR3 scale = pdus->getScale();
  if(component & VADJUST_X) scale.x *= factor;
  if(component & VADJUST_Y) scale.y *= factor;
  if(component & VADJUST_Z) scale.z *= factor;
  setCurrentVisualScale(scale);
  CHECKINVARIANT();
}

// ------------------------------------- controlling camera -----------------------------------------

void D3SceneEditor::OnMouseMoveCameraWalk(UINT nFlags, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    walkWithCamera(float(pt.y - m_lastMouse.y) / -20.0f, float(pt.x - m_lastMouse.x) / -400.0f);
    break;
  case MK_CONTROL  :
    rotateCameraUpDown(float(pt.y - m_lastMouse.y) / 400.0f);
    break;
  case MK_SHIFT    :
    sidewalkWithCamera(float(pt.y - m_lastMouse.y) / -40.0f, float(pt.x - m_lastMouse.x) / 40.0f);
    break;
  case MK_CTRLSHIFT:
    rotateCameraLeftRight(float(pt.x - m_lastMouse.x) / 200.0f);
    break;
  }
}

void D3SceneEditor::OnMouseWheelCameraWalk(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3Ray ray = getScene().getPickedRay(pt);
      moveCamera(ray.m_dir, 0.25f*signDelta);
    }
    break;
  case MK_CONTROL  :
    getScene().setNearViewPlane(getScene().getNearViewPlane() * (1 + 0.05f*signDelta));
    break;
  case MK_SHIFT    :
    walkWithCamera(1.5f * signDelta, 0);
    break;
  case MK_CTRLSHIFT:
    { const float a = getScene().getViewAngel();
      const float d = ((a > D3DX_PI/2) ? (D3DX_PI - a) : a) / (D3DX_PI/2);
      getScene().setViewAngel(a + d * 0.04f * signDelta);
    }
    break;
  }
}

void D3SceneEditor::walkWithCamera(float dist, float angle) {
  D3PosDirUpScale    pdus = getScene().getCamPDUS();
  const D3DXVECTOR3 &dir  = pdus.getDir();
  const D3DXVECTOR3 &up   = pdus.getUp();
  getScene().setCamPDUS(
    pdus.setPos(pdus.getPos() + dist * dir)
        .setOrientation(rotate(dir, up, angle), up));
}

void D3SceneEditor::sidewalkWithCamera(float upDist, float rightDist) {
  D3PosDirUpScale pdus = getScene().getCamPDUS();
  getScene().setCamPDUS(
     pdus.setPos(pdus.getPos()
               + upDist    * pdus.getUp()
               + rightDist * pdus.getRight()));

}

void D3SceneEditor::moveCamera(const D3DXVECTOR3 &dir, float dist) {
  D3PosDirUpScale pdus = getScene().getCamPDUS();
  getScene().setCamPDUS(pdus.setPos(pdus.getPos() + unitVector(dir) * dist));
}

void D3SceneEditor::rotateCameraUpDown(float angle) {
  D3PosDirUpScale   pdus  = getScene().getCamPDUS();
  const D3DXVECTOR3 right = pdus.getRight();
  getScene().setCamPDUS(
    pdus.setOrientation(
       rotate(pdus.getDir(), right, angle)
      ,rotate(pdus.getUp() , right, angle)));
}

void D3SceneEditor::rotateCameraLeftRight(float angle) {
  D3PosDirUpScale    pdus = getScene().getCamPDUS();
  const D3DXVECTOR3 &dir  = pdus.getDir();
  getScene().setCamPDUS(
    pdus.setOrientation(
       dir
      ,rotate(pdus.getUp(), dir, angle)));
}

// ------------------------------------- controlling lights -----------------------------------------

void D3SceneEditor::OnMouseMoveLight(UINT nFlags, CPoint pt) {
  CHECKINVARIANT();
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
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseMoveLightPoint(UINT nFlags, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjXZ(pt);
    break;
  case MK_CONTROL  :
    break;
  case MK_CTRLSHIFT:
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseMoveLightSpot(UINT nFlags, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  const LIGHT       param = lc->getLight();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentObjXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentObjXZ(pt);
    break;
  case MK_CONTROL  :
    { const float angle1 = (float)(pt.x - m_lastMouse.x) / 100.0f;
      const float angle2 = (float)(pt.y - m_lastMouse.y) / 100.0f;
      const D3DXVECTOR3 newDir = rotate(rotate(dir,getScene().getCamUp(),angle1),getScene().getCamRight(),angle2);
      getScene().setLightDirection(lc->getLightIndex(), newDir);
    }
    break;
  case MK_CTRLSHIFT:
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLight(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  switch(lc->getLightType()) {
  case D3DLIGHT_DIRECTIONAL: OnMouseWheelLightDirectional(nFlags, zDelta, pt); break;
  case D3DLIGHT_POINT      : OnMouseWheelLightPoint(      nFlags, zDelta, pt); break;
  case D3DLIGHT_SPOT       : OnMouseWheelLightSpot(       nFlags, zDelta, pt); break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightDirectional(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_DIRECTIONAL));
  D3LightControlDirectional &ctrl = *(D3LightControlDirectional*)lc;
  const D3DXVECTOR3 dir = ctrl.getLight().Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    ctrl.setSphereRadius(ctrl.getSphereRadius() * (1.0f-0.04f*signDelta));
    render(SE_RENDERALL);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCamRight(), -0.06f*signDelta));
    break;
  case MK_SHIFT    :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCamUp(),    -0.06f*signDelta));
    break;
  case MK_CTRLSHIFT:
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCamDir(),    0.06f*signDelta));
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightPoint(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  D3LightControlPoint &ctrl = *(D3LightControlPoint*)lc;
  const D3DXVECTOR3 pos = ctrl.getLight().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCamDir()   * 0.04f*signDelta);
    break;
  case MK_CONTROL  :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCamUp()    * 0.04f*signDelta);
    break;
  case MK_SHIFT    :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + getScene().getCamRight() * 0.04f*signDelta);
    break;
  case MK_CTRLSHIFT:
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightSpot(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  D3LightControlSpot &ctrl = *(D3LightControlSpot*)lc;
  const LIGHT       param = ctrl.getLight();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setLightPosition( ctrl.getLightIndex(), pos + getScene().getCamUp() * 0.04f*signDelta);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCamRight(), -0.06f*signDelta));
    break;
  case MK_SHIFT    :
    getScene().setLightPosition( ctrl.getLightIndex(), pos + getScene().getCamRight()   * 0.04f*signDelta);
    break;
  case MK_CTRLSHIFT:
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, getScene().getCamUp(), 0.06f*signDelta));
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightSpotAngle(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  const D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL || (lc->getLightType() != D3DLIGHT_SPOT)) {
    resetCurrentControl();
    CHECKINVARIANT();
    return;
  }
  LIGHT light = lc->getLight();
  switch(nFlags) {
  case 0           :
    light.setInnerAngle(light.getInnerAngle() * (1.0f - signDelta / 10.0f));
    break;
  case MK_SHIFT    :
    light.setOuterAngle(light.getOuterAngle() * (1.0f - signDelta / 10.0f));
    break;
  default:
    CHECKINVARIANT();
    return;
  }
  getScene().setLight(light);
  CHECKINVARIANT();
}
// ------------------------------------------------------------------------------------------------------------

void D3SceneEditor::OnAddLightDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void D3SceneEditor::OnAddLightPoint()       { addLight(D3DLIGHT_POINT);       }
void D3SceneEditor::OnAddLightSpot()        { addLight(D3DLIGHT_SPOT);        }

void D3SceneEditor::addLight(D3DLIGHTTYPE type) {
  CHECKINVARIANT();
  D3DLIGHT lp = LIGHT::createDefaultLight(type);
  switch(type) {
  case D3DLIGHT_DIRECTIONAL:
    break;
  case D3DLIGHT_POINT      :
    m_pickedPoint = lp.Position = m_pickedRay.getHitPoint(2);
    break;
  case D3DLIGHT_SPOT       :
    m_pickedPoint = lp.Position  = m_pickedRay.getHitPoint(3);
    lp.Direction  = unitVector((getScene().getCamPos() + 5 * getScene().getCamDir()) - lp.Position);
    break;
  }
  getScene().setLightControlVisible(getScene().addLight(lp), true);
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentObj(D3SceneObject *obj) {
  CHECKINVARIANT();
  if(obj == NULL) {
    switch(getCurrentControlObjType()) {
    case SOTYPE_VISUALOBJECT:
      if(getCurrentVisual()) {
        setCurrentControl(CONTROL_IDLE);
      }
      break;
    case SOTYPE_LIGHTCONTROL:
      if(getCurrentLightControl()) {
        setCurrentControl(CONTROL_IDLE);
      }
      break;
    }
  } else if(obj->getType() != getCurrentControlObjType()) {
    setCurrentControl(CONTROL_IDLE);
  }
  m_currentObj = obj;
  const D3LightControl *lc = getCurrentLightControl();
  if(m_propertyDialogMap.isDialogVisible()) {
    if(lc) {
      m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, &lc->getLight());
    } else if(obj && obj->hasMaterial()) {
      m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, &obj->getMaterial());
    }
  }
  render(SE_RENDERINFO);
  CHECKINVARIANT();
}

D3SceneObject *D3SceneEditor::getCurrentVisual() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return getCurrentObj();
  }
  return NULL;
}

D3AnimatedSurface *D3SceneEditor::getCurrentAnimatedObj() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
    return (D3AnimatedSurface*)getCurrentObj();
  }
  return NULL;
}

D3LightControl *D3SceneEditor::getCurrentLightControl() {
  switch(getCurrentObjType()) {
  case SOTYPE_LIGHTCONTROL:
    return (D3LightControl*)getCurrentObj();
  }
  return NULL;
}

const D3LightControl *D3SceneEditor::getCurrentLightControl() const {
  switch(getCurrentObjType()) {
  case SOTYPE_LIGHTCONTROL:
    return (D3LightControl*)getCurrentObj();
  }
  return NULL;
}

void D3SceneEditor::setLightControlsVisible(bool visible) {
  CHECKINVARIANT();
  BitSet visibleLightSet = getScene().getLightControlsVisible();
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
  render(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::setLightControlRenderEffect(bool enabled) {
  CHECKINVARIANT();
  if (enabled != D3LightControl::isRenderEffectEnabled()) {
    D3LightControl::enableRenderEffect(enabled);
    render(SE_RENDER3D);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setSpecularEnable(bool enabled) {
  getScene().setSpecularEnable(enabled);
  render(SE_RENDER3D);
}

void D3SceneEditor::setSpotToPointAt(CPoint point) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
    resetCurrentControl();
    CHECKINVARIANT();
    return;
  }
  D3SceneObject *obj = getScene().getPickedObject(point, ~OBJMASK_LIGHTCONTROL, &m_pickedPoint, &m_pickedRay, NULL, &m_pickedInfo);
  if(obj == NULL) {
    m_pickedInfo.clear();
    CHECKINVARIANT();
    return;
  }
  LIGHT param = lc->getLight();
  param.Direction = unitVector(m_pickedPoint - param.Position);
  getScene().setLight(param);
  CHECKINVARIANT();
}

void D3SceneEditor::OnContextMenuObj(CPoint point) {
  switch(getCurrentObjType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    OnContextMenuVisualObj(point);
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
  const TCHAR enabledChar = light.isEnabled() ?_T('+'):_T('-');
  const int   index       = light.getIndex();
  switch(light.Type) {
  case D3DLIGHT_DIRECTIONAL: return format(_T("%3d%c Directional. Dir:%s")
                                          ,index,enabledChar,toString(light.Direction).cstr());
  case D3DLIGHT_POINT      : return format(_T("%3d%c Point.       Pos:%s")
                                          ,index,enabledChar,toString(light.Position).cstr()
                                          );
  case D3DLIGHT_SPOT       : return format(_T("%3d%c Spot.        Pos:%s, Dir:%s")
                                          ,index,enabledChar
                                          ,toString(light.Position).cstr()
                                          ,toString(light.Direction).cstr()
                                          );
  }
  return EMPTYSTRING;
}

void D3SceneEditor::OnContextMenuBackground(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_BACKGROUND);
  const int visibleLightCount = (int)getScene().getLightControlsVisible().size();
  if(visibleLightCount == 0) {
    removeMenuItem(menu,ID_HIDE_LIGHTCONTROLS);
  }
  if(visibleLightCount == getScene().getLightCount()) {
    removeMenuItem(menu, ID_SHOW_LIGHTCONTROLS);
  }
  const LightArray lights = getScene().getAllLights();
  BitSet definedLights(getScene().getMaxLightCount());
  for(size_t i = 0; i < lights.size(); i++) {
    const LIGHT &l = lights[i];
    definedLights.add(l.getIndex());
    setMenuItemText(menu, ID_SELECT_LIGHT0 + l.getIndex(), lightMenuText(l));
   }
   definedLights.invert().remove(21,definedLights.getCapacity()-1);
   for(Iterator<size_t> it = definedLights.getIterator(); it.hasNext();) {
     removeMenuItem(menu, (int)it.next() + ID_SELECT_LIGHT0);
   }
   if(getCurrentVisual() == NULL) {
     removeMenuItem(menu, ID_CONTROL_OBJECT_MOVEROTATE);
   }
   removeMenuItem(menu, isCoordinateSystemVisible()
                       ?ID_SHOWCOORDINATESYSTEM
                       :ID_HIDECOORDINATESYSTEM);
   removeMenuItem(menu, getScene().isSpecularEnabled()
                       ?ID_ENABLE_SPECULARHIGHLIGHT
                       :ID_DISABLE_SPECULARHIGHLIGHT);

   removeMenuItem(menu, getScene().getRightHanded()
                       ?ID_RIGHTHANDED
                       :ID_LEFTHANDED);

   m_sceneContainer->modifyContextMenu(*menu.GetSubMenu(0));
   showContextMenu(menu, point);
}

void D3SceneEditor::OnContextMenuVisualObj(CPoint point) {
  CMenu menu;
  loadMenu(menu, IDR_CONTEXT_MENU_VISUALOBJECT);

  int checkedItem = -1;
  switch(getCurrentControl()) {
  case CONTROL_OBJECT_POS      : checkedItem = ID_CONTROL_OBJECT_POS      ; break;
  case CONTROL_OBJECT_SCALE    : checkedItem = ID_CONTROL_OBJECT_SCALE    ; break;
  }
  if(checkedItem != -1) {
    checkMenuItem(menu, checkedItem, true);
  }
  switch(getCurrentObjType()) {
  case SOTYPE_VISUALOBJECT  :
    removeSubMenuContainingId(menu, ID_OBJECT_STARTANIMATION   );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3AnimatedSurface *obj = getCurrentAnimatedObj();
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
  if(!m_currentObj->hasFillMode()) {
    removeSubMenuContainingId(menu, ID_FILLMODE_WIREFRAME);
  } else {
    switch(m_currentObj->getFillMode()) {
    case D3DFILL_SOLID     : removeMenuItem(menu, ID_FILLMODE_SOLID    ); break;
    case D3DFILL_WIREFRAME : removeMenuItem(menu, ID_FILLMODE_WIREFRAME); break;
    case D3DFILL_POINT     : removeMenuItem(menu, ID_FILLMODE_POINT    ); break;
    }
  }
  if(!m_currentObj->hasShadeMode()) {
    removeSubMenuContainingId(menu, ID_SHADING_FLAT);
  } else {
    switch(m_currentObj->getShadeMode()) {
    case D3DSHADE_FLAT     : removeMenuItem(menu, ID_SHADING_FLAT    ); break;
    case D3DSHADE_GOURAUD  : removeMenuItem(menu, ID_SHADING_GOURAUD ); break;
    case D3DSHADE_PHONG    : removeMenuItem(menu, ID_SHADING_PHONG   ); break;
    }
  }
  m_currentObj->modifyContextMenu(*menu.GetSubMenu(0));
  showContextMenu(menu, point);
}

void D3SceneEditor::OnContextMenuLightControl(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  const LIGHT light = lc->getLight();
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
    removeMenuItem(menu, ID_LIGHTCONTROL_SPOTAT    );
    removeMenuItem(menu, ID_LIGHT_ADJUSTSPOTANGLES );
  }
  removeMenuItem(menu
                ,D3LightControl::isRenderEffectEnabled()
                ?ID_LIGHTCONTROL_ENABLEEFFECT
                :ID_LIGHTCONTROL_DISABLEEFFECT);
  showContextMenu(menu, point);
}

void D3SceneEditor::OnControlCameraWalk()      { setCurrentControl(CONTROL_CAMERA_WALK   ); }
void D3SceneEditor::OnControlObjPos()          { setCurrentControl(CONTROL_OBJECT_POS    ); }
void D3SceneEditor::OnControlObjScale()        { setCurrentControl(CONTROL_OBJECT_SCALE  ); }

void D3SceneEditor::OnControlObjMoveRotate() {
//  setSelectedObject(m_calculatedObject);
  setCurrentControl(CONTROL_OBJECT_POS);
}

void D3SceneEditor::OnObjectControlSpeed() {
  setCurrentControl(CONTROL_ANIMATION_SPEED);
}

void D3SceneEditor::OnObjectCreateCube() {
  setCurrentControl(CONTROL_ANIMATION_SPEED);
}

void D3SceneEditor::resetCurrentControl() {
  setCurrentObj(NULL);
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::setCurrentControl(CurrentObjectControl control) {
  CHECKINVARIANT();
  switch(control) {
  case CONTROL_OBJECT_POS:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_HAND));
    break;
  case CONTROL_CAMERA_WALK     :
    setCurrentObj(NULL);
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_CROSS));
    break;
  default:
    setWindowCursor(*get3DWindow(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  }
  m_currentControl = control;
  render(SE_RENDERINFO);
  CHECKINVARIANT();
}

SceneObjectType D3SceneEditor::getCurrentControlObjType() const {
  switch(getCurrentControl()) {
  case CONTROL_IDLE             :
  case CONTROL_CAMERA_WALK      : return SOTYPE_NULL;
  case CONTROL_OBJECT_POS       :
  case CONTROL_OBJECT_SCALE     :
  case CONTROL_ANIMATION_SPEED  :
  case CONTROL_MATERIAL         : return SOTYPE_VISUALOBJECT;
  case CONTROL_LIGHT            :
  case CONTROL_SPOTLIGHTPOINT   :
  case CONTROL_SPOTLIGHTANGLES  :
  case CONTROL_LIGHTCOLOR       : return SOTYPE_LIGHTCONTROL;
  case CONTROL_BACKGROUNDCOLOR  :
  case CONTROL_AMBIENTLIGHTCOLOR: return SOTYPE_NULL;
  default                       :
    showWarning(_T("%s:Unknown controlType:%d"), __TFUNCTION__, getCurrentControl());
    return SOTYPE_NULL;
  }
}

void D3SceneEditor::OnObjectEditMaterial() {
  CHECKINVARIANT();
  if(m_currentObj && m_currentObj->hasMaterial()) {
    m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, &m_currentObj->getMaterial());
    setCurrentControl(CONTROL_MATERIAL);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnResetCamera() {
  getScene().resetCamTrans();
}

void D3SceneEditor::SetRightHanded(bool rightHanded) {
  getScene().setRightHanded(rightHanded);
}

typedef enum {
  ANIMATION_STOPPED
 ,ANIMATION_FORWARD
 ,ANIMATION_BACKWARDS
 ,ANIMATION_ALTERNATING
} AnimationState;

static void setCurrentAnimationState(D3AnimatedSurface *obj, AnimationState newState) {
  if(obj == NULL) return;
  switch(newState) {
  case ANIMATION_STOPPED    :
    if(obj->isRunning()) obj->stopAnimation();
    break;
  case ANIMATION_FORWARD    :
    if(obj->isRunning()) obj->stopAnimation();
    obj->startAnimation(ANIMATE_FORWARD    );
    break;
  case ANIMATION_BACKWARDS  :
    if(obj->isRunning()) obj->stopAnimation();
    obj->startAnimation(ANIMATE_BACKWARD   );
    break;
  case ANIMATION_ALTERNATING:
    if(obj->isRunning()) obj->stopAnimation();
    obj->startAnimation(ANIMATE_ALTERNATING);
    break;
  }
}

void D3SceneEditor::OnObjectStartAnimation() {
  CHECKINVARIANT();
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_FORWARD);
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectStartBckAnimation() {
  CHECKINVARIANT();
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_BACKWARDS);
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectStartAltAnimation() {
  CHECKINVARIANT();
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_ALTERNATING);
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectReverseAnimation() {
  CHECKINVARIANT();
  D3AnimatedSurface *obj = getCurrentAnimatedObj();
  if((obj == NULL) || !obj->isRunning()) return;
  switch(obj->getAnimationType()) {
  case ANIMATE_FORWARD    : setCurrentAnimationState(obj, ANIMATION_BACKWARDS); break;
  case ANIMATE_BACKWARD   : setCurrentAnimationState(obj, ANIMATION_FORWARD  ); break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectStopAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_STOPPED);
}

void D3SceneEditor::OnMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  const float factor = (1.0f + 0.04f*signDelta);
  D3AnimatedSurface *obj = getCurrentAnimatedObj();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
    render(SE_RENDERINFO);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectRemove() {
  CHECKINVARIANT();
  if(m_currentObj == NULL) return;
  D3SceneObject *obj = m_currentObj;
  getScene().removeSceneObject(m_currentObj);
  setCurrentObj(NULL);
  SAFEDELETE(obj);
  render(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectSetCenterOfRotation() {
  setCenterOfRotation();
}

void D3SceneEditor::resetCenterOfRotation() {
  m_centerOfRotation.reset();
  render(SE_RENDERINFO);
}

void D3SceneEditor::setCenterOfRotation() {
  m_centerOfRotation.set(m_currentObj, m_pickedInfo.getMeshPoint());
  render(SE_RENDERINFO);
}

void D3SceneEditor::setLightEnabled(bool enabled) {
  CHECKINVARIANT();
  getScene().setLightEnabled(getCurrentLightControl()->getLightIndex(), enabled);
  render(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnLightAdjustColors() {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, &lc->getLight());
  setCurrentControl(CONTROL_LIGHTCOLOR);
  CHECKINVARIANT();
}

void D3SceneEditor::OnLightAdjustSpotAngles() {
  setCurrentControl(CONTROL_SPOTLIGHTANGLES);
}

void D3SceneEditor::OnLightControlSpotAt() {
  setCurrentControl(CONTROL_SPOTLIGHTPOINT);
}

void D3SceneEditor::OnLightControlHide() {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  getScene().setLightControlVisible(lc->getLightIndex(), false);
  render(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnLightRemove() {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  setCurrentObj(NULL);
  const int lightIndex = lc->getLightIndex();
  getScene().removeLight(lightIndex);
  render(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnEditAmbientLight() {
  CHECKINVARIANT();
  resetCurrentControl();
  const D3DCOLOR oldColor = getScene().getAmbientColor();
  CColorDlg dlg(_T("Ambient color"), SP_AMBIENTCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_AMBIENTLIGHTCOLOR);
  if(dlg.DoModal() != IDOK) {
    getScene().setAmbientColor(oldColor);
    getScene().render();
  }
  resetCurrentControl();
  m_currentEditor = NULL;
  CHECKINVARIANT();
}

void D3SceneEditor::OnEditBackgroundColor() {
  CHECKINVARIANT();
  resetCurrentControl();
  const D3DCOLOR oldColor = getScene().getBackgroundColor();
  CColorDlg dlg(_T("Background color"), SP_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_BACKGROUNDCOLOR);
  if(dlg.DoModal() != IDOK) {
    getScene().setBackgroundColor(oldColor);
  }
  resetCurrentControl();
  m_currentEditor = NULL;
  CHECKINVARIANT();
}

void D3SceneEditor::OnFillmodePoint()     { getCurrentObj()->setFillMode(D3DFILL_POINT     ); render(SE_RENDERALL); }
void D3SceneEditor::OnFillmodeWireframe() { getCurrentObj()->setFillMode(D3DFILL_WIREFRAME ); render(SE_RENDERALL); }
void D3SceneEditor::OnFillmodeSolid()     { getCurrentObj()->setFillMode(D3DFILL_SOLID     ); render(SE_RENDERALL); }
void D3SceneEditor::OnShadingFlat()       { getCurrentObj()->setShadeMode(D3DSHADE_FLAT    ); render(SE_RENDERALL); }
void D3SceneEditor::OnShadingGouraud()    { getCurrentObj()->setShadeMode(D3DSHADE_GOURAUD ); render(SE_RENDERALL); }
void D3SceneEditor::OnShadingPhong()      { getCurrentObj()->setShadeMode(D3DSHADE_PHONG   ); render(SE_RENDERALL); }

void D3SceneEditor::setCoordinateSystemVisible(bool visible) {
  if(visible) {
    if(m_coordinateSystem == NULL) {
      m_coordinateSystem = new D3SceneObjectCoordinateSystem(getScene()); TRACE_NEW(m_coordinateSystem);
      getScene().addSceneObject(m_coordinateSystem);
    } else {
      m_coordinateSystem->setVisible(true);
    }
  } else {
    if(isCoordinateSystemVisible()) {
      m_coordinateSystem->setVisible(false);
    }
  }
  render(SE_RENDER3D);
}

void D3SceneEditor::setSelectedCubeVisible(bool visible) {
  if(visible) {
    if(m_selectedCube == NULL) {
      m_selectedCube = new D3SelectedCube(getScene()); TRACE_NEW(m_selectedCube);
      getScene().addSceneObject(m_selectedCube);
    } else {
      m_selectedCube->setVisible(true);
    }
  } else {
    if(isSelectedCubeVisible()) {
      m_selectedCube->setVisible(false);
    }
  }
  render(SE_RENDER3D);
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
    showException(e);
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
    render(SE_RENDERALL);
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch(Exception e) {
    showException(e);
  }
}

#ifdef _DEBUG

#ifdef verify
#undef verify
#endif
#define verify(exp) (void)((exp) || (showError(_T("%s line %d:%s"), method, line, _T(#exp)), 0))

void D3SceneEditor::checkInvariant(const TCHAR *method, int line) const {
  const SceneObjectType objType = getCurrentObjType();
/*
  case SOTYPE_NULL          :
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_LIGHTCONTROL  :
  case SOTYPE_ANIMATEDOBJECT:;
*/

  switch(m_currentControl) {
  case CONTROL_IDLE             :
  case CONTROL_CAMERA_WALK      :
    break;
  case CONTROL_OBJECT_POS       :
    verify((objType == SOTYPE_VISUALOBJECT) || (objType == SOTYPE_ANIMATEDOBJECT));
    break;
  case CONTROL_OBJECT_SCALE     :
  case CONTROL_ANIMATION_SPEED  :
    verify(objType == SOTYPE_ANIMATEDOBJECT);
    break;
  case CONTROL_MATERIAL         :
    verify((objType == SOTYPE_VISUALOBJECT) || (objType == SOTYPE_ANIMATEDOBJECT));
    verify(m_propertyDialogMap.getVisibleDialogId() == SP_MATERIALPARAMETERS);
    break;
  case CONTROL_LIGHT            :
    verify(objType == SOTYPE_LIGHTCONTROL);
    break;
  case CONTROL_LIGHTCOLOR:
    verify(objType                                  == SOTYPE_LIGHTCONTROL);
    verify(m_propertyDialogMap.getVisibleDialogId() == SP_LIGHTPARAMETERS );
    break;
  case CONTROL_SPOTLIGHTPOINT   :
  case CONTROL_SPOTLIGHTANGLES  :
    verify(objType == SOTYPE_LIGHTCONTROL);
    verify(getCurrentLightControl()->getLightType() == D3DLIGHT_SPOT);
    break;
  case CONTROL_BACKGROUNDCOLOR  :
  case CONTROL_AMBIENTLIGHTCOLOR:
    break;
  }

  switch(m_currentControl) {
  case CONTROL_IDLE             :
    break;
  case CONTROL_BACKGROUNDCOLOR  :
  case CONTROL_AMBIENTLIGHTCOLOR:
    verify(m_currentEditor != NULL);
    break;
  default                       :
    verify(m_currentEditor == NULL);
    break;
  }
}

#endif // _DEBUG
