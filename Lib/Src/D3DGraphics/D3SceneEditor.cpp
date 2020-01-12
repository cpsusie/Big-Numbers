#include "pch.h"
#include <ByteFile.h>
#include <MFCUtil/ColorDlg.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/D3SceneEditor.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>
#include <D3DGraphics/D3SelectedCube.h>

D3SceneEditor::D3SceneEditor() {
  m_sceneContainer     = NULL;
  m_currentEditor      = NULL;
  m_currentSceneObject = NULL;
  m_coordinateSystem   = NULL;
  m_selectedCube       = NULL;
  m_paramFileName      = _T("Untitled");
  m_stateFlags.add(SE_MOUSEVISIBLE);
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
}

void D3SceneEditor::close() {
  getScene().removePropertyChangeListener(this);
  m_propertyDialogMap.clear();
  getScene().removeSceneObject(m_coordinateSystem);
  SAFEDELETE(m_coordinateSystem);
  getScene().removeSceneObject(m_selectedCube);
  SAFEDELETE(m_selectedCube);
}

void D3SceneEditor::setEnabled(bool enabled) {
  if (enabled) {
    m_stateFlags.add(SE_ENABLED);
    enablePropertyChanges();
    enableRender();
    render(RENDER_INFO);
  } else {
    OnLightControlHide();
    m_propertyDialogMap.hideDialog();
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
    case SP_CAMERAPDUS              :
    case SP_PROJECTIONTRANSFORMATION:
    case SP_LIGHTPARAMETERS         :
    case SP_MATERIALPARAMETERS      :
    case SP_OBJECTCOUNT             :
      render(RENDER_ALL);
      break;
    case SP_ANIMATIONFRAMEINDEX     :
      render(RENDER_3D);
      break;
    }
  } else if(m_propertyDialogMap.hasPropertyContainer(source)) {
    switch(id) {
    case SP_LIGHTPARAMETERS:
      { const LIGHT &lp = *(LIGHT*)newValue;
        LIGHT tmp = getScene().getLight(lp.m_index); // to keep pos and direction as they are
        if(tmp.m_index != lp.m_index) {
          showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__,lp.m_index);
        }
        getScene().setLight(CLightDlg::copyModifiableValues(tmp, lp));
      }
      break;
    case SP_MATERIALPARAMETERS:
      getScene().setMaterial(*(MATERIAL*)newValue);
      break;
    }
  } else if(source == m_currentEditor) {
    switch(id) {
    case SP_BACKGROUNDCOLOR:
      getScene().setBackgroundColor(*(D3DCOLOR*)newValue);
      render(RENDER_ALL);
      break;
    case SP_AMBIENTLIGHT           :
      getScene().setAmbientColor(*(D3DCOLOR*)newValue);
      render(RENDER_ALL);
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
    { D3SceneObject *pickedObj = getScene().getPickedObject(point, OBJMASK_ALL, &m_pickedPoint, &m_pickedInfo);
      if(pickedObj == NULL) {
        if(m_currentControl != CONTROL_CAMERA_WALK) {
          setCurrentSceneObject(NULL);
          setCurrentControl(CONTROL_IDLE);
        } else {
          setMouseVisible(false);
          getMessageWindow()->SetCapture();
        }
        m_pickedInfo.clear();
      } else {
        setCurrentSceneObject(pickedObj);
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
  D3SceneObject *pickedObject = getScene().getPickedObject(m_lastMouse, OBJMASK_ALL, &m_pickedPoint, &m_pickedInfo);
  if(pickedObject == NULL) {
    OnContextMenuBackground(point);
    m_pickedInfo.clear();
  } else {
    setCurrentSceneObject(pickedObject);
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
      setEnabled(false);
      render(RENDER_ALL);
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
    case ID_CONTROL_OBJECT_POS            : OnControlObjectPos()                ; return true;
    case ID_CONTROL_OBJECT_SCALE          : OnControlObjectScale()              ; return true;
    case ID_CONTROL_OBJECT_MOVEROTATE     : OnControlObjectMoveRotate()         ; return true;
    case ID_OBJECT_RESETSCALE             : OnObjectResetScale()                ; return true;
    case ID_OBJECT_ADJUSTMATERIAL         : OnObjectEditMaterial()              ; return true;
    case ID_OBJECT_REMOVE                 : OnObjectRemove()                    ; return true;
    case ID_OBJECT_SETCENTEROFROTATION    : OnObjectSetCenterOfRotation()       ; return true;
    case ID_OBJECT_STARTANIMATION         : OnObjectStartAnimation()            ; return true;
    case ID_OBJECT_STARTBCKANIMATION      : OnObjectStartBckAnimation()         ; return true;
    case ID_OBJECT_STARTALTANIMATION      : OnObjectStartAltAnimation()         ; return true;
    case ID_OBJECT_REVERSEANIMATION       : OnObjectReverseAnimation()          ; return true;
    case ID_OBJECT_STOPANIMATION          : OnObjectStopAnimation()             ; return true;
    case ID_OBJECT_CONTROL_SPEED          : OnObjectControlSpeed()              ; return true;
    case ID_CONTROL_CAMERA_WALK           : OnControlCameraWalk()               ; return true;
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
    case ID_SAVESCENEPARAMETERS           : OnSaveSceneParameters()             ; return true;
    case ID_LOADSCENEPARAMETERS           : OnLoadSceneParameters()             ; return true;
    default:
      if((ID_SELECT_LIGHT0 <= pMsg->wParam) && (pMsg->wParam <= ID_SELECT_LIGHT20)) {
        const int index = (int)pMsg->wParam - ID_SELECT_LIGHT0;
        setCurrentSceneObject(getScene().setLightControlVisible(index, true));
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
    moveCurrentSceneObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentSceneObjectXZ(pt);
    break;
  case MK_CONTROL  :
    rotateCurrentVisualFrwBckw(double(pt.x - m_lastMouse.x) / 100.0
                              ,double(pt.y - m_lastMouse.y) / 100.0);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentVisualLeftRight(double(pt.x - m_lastMouse.x) / 100.0);
    break;
  }
}

void D3SceneEditor::moveCurrentSceneObjectXY(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentSceneObjectPos() - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.m_orig + newPickedRay.m_dir * dist;
  setCurrentSceneObjectPos(newPickedPoint + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void D3SceneEditor::moveCurrentSceneObjectXZ(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentSceneObjectPos() - m_pickedPoint;
  const float       dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = getScene().getPickRay(pt);
  const D3Ray       ray1           = getScene().getPickRay(CPoint(pt.x,pt.y+1));
  const double      dRaydPixel     = length((newPickedRay.m_orig + dist * newPickedRay.m_dir) - (ray1.m_orig + dist * ray1.m_dir));
  const CSize       dMouse         = pt - m_lastMouse;
  const D3PosDirUpScale &camPDUS   = getScene().getCameraPDUS();
  D3DXVECTOR3       newPickedPoint = m_pickedPoint
                                   + (float)(-dMouse.cy * dRaydPixel) * camPDUS.getDir()
                                   + (float)( dMouse.cx * dRaydPixel) * camPDUS.getRight();

  setCurrentSceneObjectPos(newPickedPoint + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

D3DXVECTOR3 D3SceneEditor::getCurrentSceneObjectPos() {
  D3SceneObject *obj = getCurrentSceneObject();
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

void D3SceneEditor::setCurrentSceneObjectPos(const D3DXVECTOR3 &pos) {
  D3SceneObject *obj = getCurrentSceneObject();
  if(obj == NULL) return;
  switch(obj->getType()) {
  case SOTYPE_LIGHTCONTROL  :
    getScene().setLightPosition(getCurrentLightControl()->getLightIndex(), pos);
    // continue case
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    obj->setPos(pos);
    render(RENDER_ALL);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
  }
}

void D3SceneEditor::setCurrentVisualOrientation(const D3DXVECTOR3 &dir, const D3DXVECTOR3 &up) {
  D3SceneObject *obj = getCurrentVisual();
  if(obj == NULL) return;
  const D3DXVECTOR3 centerOfRot = getCenterOfRotation();
  if(centerOfRot == D3DXORIGIN) {
    obj->getPDUS().setOrientation(dir, up);
  } else {
    const D3DXVECTOR3 crMesh = getCenterOfRotation();
    const D3DXVECTOR3 crOldWorld = getCurrentVisual()->getWorldMatrix() * crMesh;
    obj->getPDUS().setOrientation(dir, up);
    const D3DXVECTOR3 crNewWorld = getCurrentVisual()->getWorldMatrix() * crMesh;
    obj->setPos(obj->getPos() + crOldWorld - crNewWorld);
  }
  render(RENDER_ALL);
}

void D3SceneEditor::setCurrentVisualScale(const D3DXVECTOR3 &scale) {
  D3SceneObject *obj = getCurrentVisual();
  if(obj == NULL) return;
  obj->getPDUS().setScale(scale);
  render(RENDER_ALL);
}

void D3SceneEditor::OnMouseWheelObjectPos(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = getCurrentSceneObjectPos();
      const D3DXVECTOR3 dir    = objPos - getScene().getCameraPos();
      const float       dist   = length(dir);
      if(dist > 0) {
        setCurrentSceneObjectPos(objPos + unitVector(dir) * dist / 30.0 * (float)sign(zDelta));
      }
    }
    break;
  case MK_CONTROL  :
    rotateCurrentVisualFrwBckw(0, (double)sign(zDelta) / -50.0);
    break;
  case MK_SHIFT    :
    rotateCurrentVisualFrwBckw((double)sign(zDelta) / -50, 0);
    break;
  case MK_CTRLSHIFT:
    rotateCurrentVisualLeftRight((double)sign(zDelta) / 50.0);
    break;
  }
}

void D3SceneEditor::rotateCurrentVisualFrwBckw(double angle1, double angle2) {
  D3SceneObject *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();
  const D3DXVECTOR3 cameraUp    = getScene().getCameraUp();
  const D3DXVECTOR3 cameraRight = getScene().getCameraRight();
  const D3DXVECTOR3 newDir      = rotate(rotate(pdus.getDir(),cameraUp,angle1),cameraRight,angle2);
  const D3DXVECTOR3 newUp       = rotate(rotate(pdus.getUp() ,cameraUp,angle1),cameraRight,angle2);
  setCurrentVisualOrientation(newDir, newUp);
}

void D3SceneEditor::rotateCurrentVisualLeftRight(double angle) {
  D3SceneObject *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3PosDirUpScale &pdus = obj->getPDUS();
  const D3DXVECTOR3 cameraDir = getScene().getCameraDir();
  setCurrentVisualOrientation(rotate(pdus.getDir(),cameraDir, angle)
                             ,rotate(pdus.getUp(), cameraDir, angle));
}

#define VADJUST_X    0x01
#define VADJUST_Y    0x02
#define VADJUST_Z    0x04
#define VADJUST_ALL  (VADJUST_X | VADJUST_Y | VADJUST_Z)

void D3SceneEditor::OnMouseWheelObjectScale(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustCurrentVisualScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustCurrentVisualScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustCurrentVisualScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustCurrentVisualScale(VADJUST_Z  , factor); break;
  }
}

void D3SceneEditor::OnMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  const double factor = (1.0+(double)sign(zDelta)*0.04);
  D3AnimatedSurface *obj = getCurrentAnimatedObject();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
    render(RENDER_INFO);
  }
}

void D3SceneEditor::adjustCurrentVisualScale(int component, double factor) {
  D3SceneObject *obj = getCurrentVisual();
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
  setCurrentVisualScale(scale);
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
    moveCurrentSceneObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentSceneObjectXZ(pt);
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
  const LIGHT       param = lc->getLight();
  const D3DXVECTOR3 pos   = param.Position;
  const D3DXVECTOR3 dir   = param.Direction;

  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    moveCurrentSceneObjectXY(pt);
    break;
  case MK_SHIFT    :
    moveCurrentSceneObjectXZ(pt);
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
  const D3DXVECTOR3 dir = ctrl.getLight().Direction;
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
  const D3DXVECTOR3 pos = ctrl.getLight().Position;
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
  const LIGHT       param = ctrl.getLight();
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
    setCurrentControl(CONTROL_IDLE);
    return;
  }
  LIGHT light = lc->getLight();
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
  getScene().setLight(light);
}
// ------------------------------------------------------------------------------------------------------------

void D3SceneEditor::OnAddLightDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void D3SceneEditor::OnAddLightPoint()       { addLight(D3DLIGHT_POINT);       }
void D3SceneEditor::OnAddLightSpot()        { addLight(D3DLIGHT_SPOT);        }

void D3SceneEditor::addLight(D3DLIGHTTYPE type) {
  D3DLIGHT lp = D3Scene::getDefaultLight(type);
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
  getScene().setLightControlVisible(getScene().addLight(lp), true);
}

void D3SceneEditor::setCurrentSceneObject(D3SceneObject *obj) {
  m_currentSceneObject = obj;
  const D3LightControl *lc = getCurrentLightControl();
  if(m_propertyDialogMap.isDialogVisible()) {
    if(lc) {
      m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, &lc->getLight());
    } else if(obj && obj->hasMaterial()) {
      m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, &obj->getMaterial());
    }
  }
  render(RENDER_INFO);
}

D3SceneObject *D3SceneEditor::getCurrentVisual() const {
  D3SceneObject *obj = getCurrentSceneObject();
  if(obj == NULL) return NULL;
  switch(obj->getType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return getCurrentSceneObject();
  default:
    return NULL;
  }
}

D3AnimatedSurface *D3SceneEditor::getCurrentAnimatedObject() const {
  D3SceneObject *obj = getCurrentVisual();
  if((obj == NULL) || (obj->getType() != SOTYPE_ANIMATEDOBJECT)) return NULL;
  return (D3AnimatedSurface*)obj;
}

D3LightControl *D3SceneEditor::getCurrentLightControl() {
  D3SceneObject *obj = getCurrentSceneObject();
  if((obj == NULL) || (obj->getType() != SOTYPE_LIGHTCONTROL)) return NULL;
  return (D3LightControl*)obj;
}

const D3LightControl *D3SceneEditor::getCurrentLightControl() const {
  D3SceneObject *obj = getCurrentSceneObject();
  if((obj == NULL) || (obj->getType() != SOTYPE_LIGHTCONTROL)) return NULL;
  return (D3LightControl*)obj;
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

void D3SceneEditor::setLightControlRenderEffect(bool enabled) {
  if (enabled != D3LightControl::isRenderEffectEnabled()) {
    D3LightControl::enableRenderEffect(enabled);
    render(RENDER_3D);
  }
}

void D3SceneEditor::setSpecularEnable(bool enabled) {
  getScene().setSpecularEnable(enabled);
  render(RENDER_3D);
}

void D3SceneEditor::setSpotToPointAt(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
    setCurrentControl(CONTROL_IDLE);
    return;
  }
  D3DXVECTOR3 pointInSpace;
  D3SceneObject *obj = getScene().getPickedObject(point, ~OBJMASK_LIGHTCONTROL, &pointInSpace);
  if(obj == NULL) return;
  LIGHT param = lc->getLight();
  param.Direction = unitVector(pointInSpace - param.Position);
  getScene().setLight(param);
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
    removeSubMenuContainingId(menu, ID_OBJECT_STARTANIMATION   );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3AnimatedSurface *obj = getCurrentAnimatedObject();
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
  if(!m_currentSceneObject->hasFillMode()) {
    removeSubMenuContainingId(menu, ID_FILLMODE_WIREFRAME);
  } else {
    switch(m_currentSceneObject->getFillMode()) {
    case D3DFILL_SOLID     : removeMenuItem(menu, ID_FILLMODE_SOLID    ); break;
    case D3DFILL_WIREFRAME : removeMenuItem(menu, ID_FILLMODE_WIREFRAME); break;
    case D3DFILL_POINT     : removeMenuItem(menu, ID_FILLMODE_POINT    ); break;
    }
  }
  if(!m_currentSceneObject->hasShadeMode()) {
    removeSubMenuContainingId(menu, ID_SHADING_FLAT);
  } else {
    switch (m_currentSceneObject->getShadeMode()) {
    case D3DSHADE_FLAT     : removeMenuItem(menu, ID_SHADING_FLAT    ); break;
    case D3DSHADE_GOURAUD  : removeMenuItem(menu, ID_SHADING_GOURAUD ); break;
    case D3DSHADE_PHONG    : removeMenuItem(menu, ID_SHADING_PHONG   ); break;
    }
  }
  if(m_currentSceneObject) {
    m_currentSceneObject->modifyContextMenu(*menu.GetSubMenu(0));
  }
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

void D3SceneEditor::OnControlCameraWalk()      {  setCurrentControl(CONTROL_CAMERA_WALK        ); }
void D3SceneEditor::OnControlObjectPos()       {  setCurrentControl(CONTROL_OBJECT_POS         ); }
void D3SceneEditor::OnControlObjectScale()     {  setCurrentControl(CONTROL_OBJECT_SCALE       ); }

void D3SceneEditor::OnControlObjectMoveRotate() {
//  setSelectedObject(m_calculatedObject);
  setCurrentControl(CONTROL_OBJECT_POS);
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
  case CONTROL_CAMERA_WALK     :
    setCurrentSceneObject(NULL);
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
  if(m_currentSceneObject && m_currentSceneObject->hasMaterial()) {
    m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, &m_currentSceneObject->getMaterial());
    setCurrentControl(CONTROL_MATERIAL);
  }
}

void D3SceneEditor::OnObjectResetScale() {
  getScene().getObjPDUS().setScaleAll(1);
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
  setCurrentAnimationState(getCurrentAnimatedObject(), ANIMATION_FORWARD);
}

void D3SceneEditor::OnObjectStartBckAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObject(), ANIMATION_BACKWARDS);
}

void D3SceneEditor::OnObjectStartAltAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObject(), ANIMATION_ALTERNATING);
}

void D3SceneEditor::OnObjectReverseAnimation() {
  D3AnimatedSurface *obj = getCurrentAnimatedObject();
  if((obj == NULL) || !obj->isRunning()) return;
  switch(obj->getAnimationType()) {
  case ANIMATE_FORWARD    : setCurrentAnimationState(obj, ANIMATION_BACKWARDS); break;
  case ANIMATE_BACKWARD   : setCurrentAnimationState(obj, ANIMATION_FORWARD  ); break;
  }
}

void D3SceneEditor::OnObjectStopAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObject(), ANIMATION_STOPPED);
}

void D3SceneEditor::OnObjectRemove() {
  if(m_currentSceneObject == NULL) return;
  getScene().removeSceneObject(m_currentSceneObject);
  SAFEDELETE(m_currentSceneObject);
  setCurrentSceneObject(NULL);
  render(RENDER_ALL);
}

void D3SceneEditor::OnObjectSetCenterOfRotation() {
  setCenterOfRotation();
}

void D3SceneEditor::resetCenterOfRotation() {
  m_centerOfRotation.reset();
  render(RENDER_INFO);
}

void D3SceneEditor::setCenterOfRotation() {
  m_centerOfRotation.set(m_currentSceneObject, m_pickedInfo.getMeshPoint());
  render(RENDER_INFO);
}

void D3SceneEditor::setLightEnabled(bool enabled) {
  getScene().setLightEnabled(getCurrentLightControl()->getLightIndex(), enabled);
  render(RENDER_ALL);
}

void D3SceneEditor::OnLightAdjustColors() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, &lc->getLight());
  setCurrentControl(CONTROL_LIGHTCOLOR);
}

void D3SceneEditor::OnLightAdjustSpotAngles() {
  setCurrentControl(CONTROL_SPOTLIGHTANGLES);
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
  setCurrentSceneObject(NULL);
  render(RENDER_ALL);
}

void D3SceneEditor::OnEditAmbientLight() {
  const D3DCOLOR oldColor = getScene().getAmbientColor();
  CColorDlg dlg(_T("Ambient color"), SP_AMBIENTLIGHT, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_AMBIENTLIGHTCOLOR);
  if(dlg.DoModal() != IDOK) {
    getScene().setAmbientColor(oldColor);
    getScene().render();
  }
  m_currentEditor = NULL;
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::OnEditBackgroundColor() {
  const D3DCOLOR oldColor = getScene().getBackgroundColor();
  CColorDlg dlg(_T("Background color"), SP_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  m_currentEditor = &dlg;
  setCurrentControl(CONTROL_BACKGROUNDCOLOR);

  if(dlg.DoModal() != IDOK) {
    getScene().setBackgroundColor(oldColor);
  }
  m_currentEditor = NULL;
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::OnFillmodePoint()     { getCurrentSceneObject()->setFillMode(D3DFILL_POINT     ); render(RENDER_ALL); }
void D3SceneEditor::OnFillmodeWireframe() { getCurrentSceneObject()->setFillMode(D3DFILL_WIREFRAME ); render(RENDER_ALL); }
void D3SceneEditor::OnFillmodeSolid()     { getCurrentSceneObject()->setFillMode(D3DFILL_SOLID     ); render(RENDER_ALL); }
void D3SceneEditor::OnShadingFlat()       { getCurrentSceneObject()->setShadeMode(D3DSHADE_FLAT    ); render(RENDER_ALL); }
void D3SceneEditor::OnShadingGouraud()    { getCurrentSceneObject()->setShadeMode(D3DSHADE_GOURAUD ); render(RENDER_ALL); }
void D3SceneEditor::OnShadingPhong()      { getCurrentSceneObject()->setShadeMode(D3DSHADE_PHONG   ); render(RENDER_ALL); }

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
  render(RENDER_3D);
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
    render(RENDER_ALL);
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch(Exception e) {
    showException(e);
  }
}

const TCHAR *controlString(CurrentObjectControl control) {
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
  String result = _T("(");
  const TCHAR *delim = NULL;
#define ADDDELIM { if(delim) result += delim; else delim=_T(" "); }
  if(isEnabled())                { ADDDELIM; result += _T("enabled"); }
  if(isPropertyChangesEnabled()) { ADDDELIM; result += _T("prop");    }
  if(isMouseVisible())           { ADDDELIM; result += _T("mouse");   }
  result += _T(")");
  return result;
}

String D3SceneEditor::getSelectedString() const {
  if(m_currentSceneObject == NULL) {
    return _T("--");
  } else {
    String result = m_currentSceneObject->getName();
    if(m_currentSceneObject->hasFillMode()) {
      result += format(_T(" %s"), ::toString(m_currentSceneObject->getFillMode()).cstr());
    }
    if(m_currentSceneObject->hasShadeMode()) {
      result += format(_T(" %s"), ::toString(m_currentSceneObject->getShadeMode()).cstr());
    }
    return result;
  }
}

String D3SceneEditor::toString() const {
  if(!isEnabled()) return EMPTYSTRING;

  String result = format(_T("Current Motion:%s State:%s Selected:%s")
                        ,controlString(m_currentControl)
                        ,stateFlagsToString().cstr()
                        ,getSelectedString().cstr()
                        );
  if(m_pickedRay.isSet()) {
    result += format(_T("\nPicked ray:%s"), m_pickedRay.toString().cstr());
    if (m_pickedInfo.isSet()) {
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
    result += format(_T("\nDlg-light:%s")
                    ,((LIGHT*)m_propertyDialogMap.getProperty(SP_LIGHTPARAMETERS))->toString().cstr());
    break;
  case SP_MATERIALPARAMETERS:
    result += format(_T("\nDlg-material:%s")
                    ,((MATERIAL*)m_propertyDialogMap.getProperty(SP_MATERIALPARAMETERS))->toString().cstr());
    break;
  }

  switch (m_currentControl) {
  case CONTROL_IDLE                  :
    return result;
  case CONTROL_CAMERA_WALK           :
    return result + format(_T("\n%s"), getScene().getCameraString().cstr());

  case CONTROL_OBJECT_POS            :
  case CONTROL_OBJECT_SCALE          :
    if(m_currentSceneObject != NULL) {
      result += format(_T("\nObject:\n%s")
                      ,m_currentSceneObject->getPDUS().toString().cstr());
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
    { D3AnimatedSurface *obj = getCurrentAnimatedObject();
      if(obj) {
        result += format(_T("\nFrames/sec:%.2lf"), obj->getFramePerSec());
      }
      return result;
    }
  case CONTROL_MATERIAL              :
    if(m_currentSceneObject && m_currentSceneObject->hasMaterial()) {
      result += format(_T("\nMaterial:%s"), m_currentSceneObject->getMaterial().toString().cstr());
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
