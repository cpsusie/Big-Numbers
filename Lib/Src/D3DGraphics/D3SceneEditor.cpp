#include "pch.h"

#include <ByteFile.h>
#include <MFCUtil/ColorDlg.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3SceneObjectAnimatedMesh.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>
#include <D3DGraphics/D3SelectedCube.h>
#include <D3DGraphics/D3SceneEditor.h>

D3SceneEditor::D3SceneEditor()
: m_sceneContainer(        NULL )
, m_currentPropertyDialog( NULL )
, m_currentCamera(         NULL )
, m_currentCameraIndex(    -1   )
, m_currentObj(            NULL )
, m_currentControl(CONTROL_IDLE )
, m_coordinateSystem(      NULL )
, m_selectedCube(          NULL )
, m_paramFileName(_T("Untitled"))
, m_stateFlags(  SE_MOUSEVISIBLE)
{
  CHECKINVARIANT();
}

D3SceneEditor::~D3SceneEditor() {
  close();
}

CameraSet D3SceneEditor::getVisibleCameraSet() const {
  return getScene().getCameraArray().getVisibleCameraSet();
}

CameraSet D3SceneEditor::getCurrentCameraSet() const {
  CameraSet set;
  if(m_currentCameraIndex >= 0) {
    set.add(m_currentCameraIndex);
  }
  return set;
}

void D3SceneEditor::init(D3SceneContainer *sceneContainer) {
  m_sceneContainer    = sceneContainer;
  CLightDlg    *dlg1  = new CLightDlg(   this); TRACE_NEW(dlg1);
  m_propertyDialogMap.addDialog(dlg1);
  CMaterialDlg *dlg2  = new CMaterialDlg(this); TRACE_NEW(dlg2);
  m_propertyDialogMap.addDialog(dlg2);
  const UINT count = sceneContainer->get3DWindowCount();
  for(UINT i = 0; i < count; i++) {
    getScene().addCamera(sceneContainer->get3DWindow(i));
  }
  m_currentControl    = CONTROL_IDLE;
  selectCamera((count >= 1) ? 0 : -1);
  getScene().addPropertyChangeListener(this);
  setFlags(SE_INITDONE);
  CHECKINVARIANT();
}

void D3SceneEditor::close() {
  CHECKINVARIANT();
  getScene().removePropertyChangeListener(this);
  m_propertyDialogMap.clear();
  getScene().removeVisual(m_coordinateSystem);
  SAFEDELETE(m_coordinateSystem);
  getScene().removeVisual(m_selectedCube);
  SAFEDELETE(m_selectedCube);
  getScene().close();
  clrFlags(SE_INITDONE);
}

D3SceneEditor &D3SceneEditor::setEnabled(bool enabled, BYTE flags) {
  if(isInitDone()) {
    flags &= ~SE_INITDONE;
    if(enabled) {
      setFlags(flags);
      renderInfo();
    } else {
      clrFlags(flags);
      if(!isSet(SE_LIGHTCONTROLS)) {
        OnLightControlHide();
        m_propertyDialogMap.hideDialog();
      }
      renderInfo();
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
    case SP_MATERIALPARAMETERS:
    case SP_LIGHTPARAMETERS:
    case SP_VISUALCOUNT:
    case SP_RIGHTHANDED:
      renderVisible(SE_RENDERALL);
      break;
    case SP_ANIMATIONFRAMEINDEX:
      renderVisible(SE_RENDER3D);
      break;
    case SP_AMBIENTCOLOR:
      renderVisible(SE_RENDERALL | SE_RENDERNOW);
      break;
    }
  } else if(source == m_currentCamera) {
    switch (id) {
    case CAM_VIEW           :           // D3DXMATRIX
    case CAM_PROJECTION     :           // D3DXMATRIX
      renderCurrent(SE_RENDERALL);
      break;
    case CAM_BACKGROUNDCOLOR:           // D3DCOLOR
      renderCurrent(SE_RENDERALL|SE_RENDERNOW);
      break;
    }
  } else if(m_propertyDialogMap.hasPropertyContainer(source)) {
    switch(id) {
    case SP_LIGHTPARAMETERS:
      { const D3Light &newLight = *(D3Light*)newValue;
        D3Light        tmp      = getScene().getLight(newLight.getIndex()); // to keep pos and direction as they are
        if(tmp.getIndex() != newLight.getIndex()) {
          showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__,newLight.getIndex());
        } else {
          getScene().setLight(CLightDlg::copyModifiableValues(tmp, newLight));
        }
      }
      break;
    case SP_MATERIALPARAMETERS:
      getScene().setMaterial(*(D3Material*)newValue);
      break;
    }
  } else if(source == m_currentPropertyDialog) {
    switch(id) {
    case CAM_BACKGROUNDCOLOR   :
      m_currentCamera->setBackgroundColor(*(D3DCOLOR*)newValue);
      break;
    case SP_AMBIENTCOLOR      :
      getScene().setAmbientColor(*(D3DCOLOR*)newValue);
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
    { D3SceneObjectVisual *pickedObj = m_currentCamera->getPickedVisual(point, OBJMASK_ALL, &m_pickedPoint, &m_pickedRay, NULL, &m_pickedInfo);
      if(pickedObj == NULL) {
        m_pickedInfo.clear();
        if(getCurrentControl() == CONTROL_CAMERA_WALK) {
          setMouseVisible(false);
          SetCapture(m_sceneContainer->getMessageWindow());
        } else {
          resetCurrentControl();
          setCurrentControl(CONTROL_CAMERA_WALK);
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
    renderInfo();
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnContextMenu(HWND pwnd, CPoint point) {
  m_lastMouse = m_currentCamera->screenToWin(point);
  D3SceneObjectVisual *pickedObj = getScene().getPickedVisual(point, OBJMASK_ALL, &m_pickedPoint, &m_pickedRay,NULL,&m_pickedInfo);
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

// pt in window-coordinates
BOOL D3SceneEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  switch(getCurrentControl()) {
  case CONTROL_CAMERA_WALK           :
    OnMouseWheelCameraWalk(          nFlags, zDelta, pt);
    return TRUE;
  case CONTROL_CAMERA_PROJECTION     :
    OnMouseWheelCameraProjection(    nFlags, zDelta, pt);
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

HWND D3SceneEditor::getCurrentHwnd() const {
  return m_currentCamera ? m_currentCamera->getHwnd() : (HWND)INVALID_HANDLE_VALUE;
}

int D3SceneEditor::findCameraIndex(CPoint p) const {
  return getScene().getCameraArray().findCameraIndex(p);
}

void D3SceneEditor::selectCamera(int index) {
  if(index == m_currentCameraIndex) {
    return;
  }
  if(m_currentCamera) {
    m_currentCamera->removePropertyChangeListener(this);
  }
  if(index >= 0) {
    m_currentCamera = getScene().getCameraArray()[index];
  } else {
    m_currentCamera = NULL;
    m_pickedRay.clear();
  }
  if(m_currentCamera) {
    m_currentCamera->addPropertyChangeListener(this);
  }
  m_currentCameraIndex = index;
}

bool D3SceneEditor::isCoordinateSystemVisible() const {
  return m_coordinateSystem && m_coordinateSystem->isVisible();
}
bool D3SceneEditor::isSelectedCubeVisible() const {
  return m_selectedCube && m_selectedCube->isVisible();
}

BOOL D3SceneEditor::PreTranslateMessage(MSG *pMsg) {
  if(!isEnabled()) return false;
  int cameraIndex;
  switch(pMsg->message) {
  case WM_KEYDOWN:
    switch(pMsg->wParam) {
    case VK_ESCAPE:
      setEnabled(false).setEnabled(true,SE_RENDERINFO).renderVisible(SE_RENDERALL);
      return true;
    }
    break;
  case WM_LBUTTONDOWN:
    if((cameraIndex = findCameraIndex(pMsg->pt)) < 0) return false;
    selectCamera(cameraIndex);
    OnLButtonDown((UINT)pMsg->wParam, m_currentCamera->screenToWin(pMsg->pt));
    return true;
  case WM_LBUTTONUP  :
    if(((cameraIndex = findCameraIndex(pMsg->pt)) < 0) || (cameraIndex != m_currentCameraIndex)) {
      selectCamera(-1);
      return false;
    }
    OnLButtonUp((UINT)pMsg->wParam, m_currentCamera->screenToWin(pMsg->pt));
    return true;
  case WM_MOUSEMOVE  :
    if(((cameraIndex = findCameraIndex(pMsg->pt)) < 0) || (cameraIndex != m_currentCameraIndex)) return false;
    OnMouseMove((UINT)pMsg->wParam, m_currentCamera->screenToWin(pMsg->pt));
    return true;
  case WM_MOUSEWHEEL :
    if(((cameraIndex = findCameraIndex(pMsg->pt)) < 0) || (cameraIndex != m_currentCameraIndex)) return false;
    OnMouseWheel((UINT)(pMsg->wParam&0xffff), (short)(pMsg->wParam >> 16), m_currentCamera->screenToWin(pMsg->pt));
    return true;
  case WM_RBUTTONUP:
    if((cameraIndex = findCameraIndex(pMsg->pt)) < 0) return false;
    selectCamera(cameraIndex);
    OnContextMenu(m_sceneContainer->getMessageWindow(), pMsg->pt);
    return true;
  case WM_COMMAND:
    switch (pMsg->wParam) {
    case ID_CONTROL_OBJECT_POS            : OnControlObjPos()                   ; return true;
    case ID_CONTROL_OBJECT_SCALE          : OnControlObjScale()                 ; return true;
    case ID_CONTROL_OBJECT_MOVEROTATE     : OnControlObjMoveRotate()            ; return true;
    case ID_OBJECT_FILLMODE_POINT         : OnObjectFillmodePoint()             ; return true;
    case ID_OBJECT_FILLMODE_WIREFRAME     : OnObjectFillmodeWireframe()         ; return true;
    case ID_OBJECT_FILLMODE_SOLID         : OnObjectFillmodeSolid()             ; return true;
    case ID_OBJECT_SHADING_FLAT           : OnObjectShadingFlat()               ; return true;
    case ID_OBJECT_SHADING_GOURAUD        : OnObjectShadingGouraud()            ; return true;
    case ID_OBJECT_SHADING_PHONG          : OnObjectShadingPhong()              ; return true;
    case ID_OBJECT_RESETPOSITION          : OnObjectResetPosition()             ; return true;
    case ID_OBJECT_RESETSCALE             : OnObjectResetScale()                ; return true;
    case ID_OBJECT_RESETORIENTATION       : OnObjectResetOrientation()          ; return true;
    case ID_OBJECT_RESETALL               : OnObjectResetAll()                  ; return true;
    case ID_OBJECT_ADJUSTMATERIAL         : OnObjectEditMaterial()              ; return true;
    case ID_OBJECT_REMOVE                 : OnObjectRemove()                    ; return true;
    case ID_OBJECT_SETCENTEROFROTATION    : OnObjectSetCenterOfRotation()       ; return true;
    case ID_OBJECT_RESETCENTEROFROTATION  : OnObjectResetCenterOfRotation()     ; return true;
    case ID_OBJECT_STARTANIMATION         : OnObjectStartAnimation()            ; return true;
    case ID_OBJECT_STARTBCKANIMATION      : OnObjectStartBckAnimation()         ; return true;
    case ID_OBJECT_STARTALTANIMATION      : OnObjectStartAltAnimation()         ; return true;
    case ID_OBJECT_REVERSEANIMATION       : OnObjectReverseAnimation()          ; return true;
    case ID_OBJECT_STOPANIMATION          : OnObjectStopAnimation()             ; return true;
    case ID_OBJECT_CONTROL_SPEED          : OnObjectControlSpeed()              ; return true;
    case ID_OBJECT_CREATECUBE             : OnObjectCreateCube()                ; return true;
    case ID_CONTROL_CAMERA_WALK           : OnControlCameraWalk()               ; return true;
    case ID_CONTROL_CAMERA_PROJECTION     : OnControlCameraProjection()         ; return true;
    case ID_CAMERA_RESETPOSITION          : OnCameraResetPosition()             ; return true;
    case ID_CAMERA_RESETORIENTATION       : OnCameraResetOrientation()          ; return true;
    case ID_CAMERA_RESETPROJECTION        : OnCameraResetProjection()           ; return true;
    case ID_CAMERA_RESETALL               : OnCameraResetAll()                  ; return true;
    case ID_CAMERA_EDIT_BACKGROUNDCOLOR   : OnCameraEditBackgroundColor()       ; return true;
    case ID_RIGHTHANDED                   : SetRightHanded(true)                ; return true;
    case ID_LEFTHANDED                    : SetRightHanded(false)               ; return true;
    case ID_SCENE_EDIT_AMBIENTLIGHT       : OnSceneEditAmbientLight()           ; return true;
    case ID_ENABLE_SPECULARHIGHLIGHT      : setSpecularEnable(true)             ; return true;
    case ID_DISABLE_SPECULARHIGHLIGHT     : setSpecularEnable(false)            ; return true;
    case ID_SHOWCOORDINATESYSTEM          : setCoordinateSystemVisible(true)    ; return true;
    case ID_HIDECOORDINATESYSTEM          : setCoordinateSystemVisible(false)   ; return true;
    case ID_LIGHT_ADDDIRECTIONAL          : OnLightAddDirectional()             ; return true;
    case ID_LIGHT_ADDPOINT                : OnLightAddPoint()                   ; return true;
    case ID_LIGHT_ADDSPOT                 : OnLightAddSpot()                    ; return true;
    case ID_LIGHT_REMOVE                  : OnLightRemove()                     ; return true;
    case ID_LIGHT_ENSABLE                 : setLightEnabled(true)               ; return true;
    case ID_LIGHT_DISABLE                 : setLightEnabled(false)              ; return true;
    case ID_LIGHT_ADJUSTCOLORS            : OnLightAdjustColors()               ; return true;
    case ID_LIGHT_ADJUSTSPOTANGLES        : OnLightAdjustSpotAngles()           ; return true;
    case ID_LIGHT_SHOWCONTROLS            : setLightControlsVisible(true)       ; return true;
    case ID_LIGHT_HIDECONTROLS            : setLightControlsVisible(false)      ; return true;
    case ID_LIGHTCONTROL_HIDE             : OnLightControlHide()                ; return true;
    case ID_LIGHTCONTROL_SPOTAT           : OnLightControlSpotAt()              ; return true;
    case ID_LIGHTCONTROL_ENABLEEFFECT     : setLightControlRenderEffect(true)   ; return true;
    case ID_LIGHTCONTROL_DISABLEEFFECT    : setLightControlRenderEffect(false)  ; return true;
    case ID_SAVESCENEPARAMETERS           : OnSaveSceneParameters()             ; return true;
    case ID_LOADSCENEPARAMETERS           : OnLoadSceneParameters()             ; return true;
    case ID_CAMERA_SPLITVERTICAL          : OnCameraSplit(true)                 ; return true;
    case ID_CAMERA_SPLITHORIZONTAL        : OnCameraSplit(false)                ; return true;
    case ID_CAMERA_REMOVE                 : OnCameraRemove()                    ; return true;

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
  const D3Ray       newPickedRay   = m_currentCamera->getPickedRay(pt);
  D3DXVECTOR3       newPickedPoint = newPickedRay.getHitPoint(dist);
  setCurrentObjPos(newPickedPoint  + dp);
  m_pickedRay   = newPickedRay;
  m_pickedPoint = newPickedPoint;
}

void D3SceneEditor::moveCurrentObjXZ(CPoint pt) {
  const D3DXVECTOR3      dp             = getCurrentObjPos() - m_pickedPoint;
  const float            dist           = length(m_pickedPoint - m_pickedRay.m_orig);
  const D3Ray            newPickedRay   = m_currentCamera->getPickedRay(pt);
  const D3Ray            ray1           = m_currentCamera->getPickedRay(CPoint(pt.x,pt.y+1));
  const float            dRaydPixel     = length(newPickedRay.getHitPoint(dist) - ray1.getHitPoint(dist));
  const CSize            dMouse         = pt - m_lastMouse;
  const D3DXVECTOR3      camDir         = m_currentCamera->getDir(), camRight = m_currentCamera->getRight();
  D3DXVECTOR3            newPickedPoint = m_pickedPoint
                                        - (dRaydPixel * dMouse.cy) * camDir
                                        + (dRaydPixel * dMouse.cx) * camRight;
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
    return ((D3World)(*getCurrentObj())).getPos();
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), getCurrentObjType());
    return D3DXVECTOR3(0,0,0);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentObjPos(const D3DXVECTOR3 &pos) {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentObj();
  if(obj == NULL) return;
  switch(obj->getType()) {
  case SOTYPE_LIGHTCONTROL  :
    getScene().setLightPosition(getCurrentLightControl()->getLightIndex(), pos);
    // continue case
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    obj->getWorld() = D3World(*obj).setPos(pos);
    renderVisible(SE_RENDERALL);
    break;
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualWorld(const D3DXMATRIX &world) {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj) {
    obj->getWorld() = world;
    renderVisible(SE_RENDERALL);
  }
  CHECKINVARIANT();
}

const D3DXMATRIX *D3SceneEditor::getCurrentVisualWorld() const {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  return (obj == NULL) ? NULL : &obj->getWorld();
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualOrientation(const D3DXQUATERNION &q) {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  if(obj != m_centerOfRotation.m_obj) {
    setCurrentVisualWorld(w.setOrientation(q));
  } else {
    const D3DXVECTOR3 crOldPos = m_centerOfRotation.getWorldPoint();
    const D3DXVECTOR3 crNewPos = w.setOrientation(q) * m_centerOfRotation.getMeshPoint();
    setCurrentVisualWorld(w.setPos(w.getPos() + crOldPos - crNewPos));
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentVisualScale(const D3DXVECTOR3 &scale) {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  setCurrentVisualWorld(w.setScale(scale));
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectResetPosition() {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  setCurrentVisualWorld(w.resetPos());
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectResetScale() {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  setCurrentVisualWorld(w.resetScale());
  CHECKINVARIANT();
}
void D3SceneEditor::OnObjectResetOrientation() {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  setCurrentVisualWorld(w.resetOrientation());
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectResetAll() {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World());
  CHECKINVARIANT();
}

#define signDelta ((float)sign(zDelta))

void D3SceneEditor::OnMouseWheelObjPos(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const D3DXVECTOR3 objPos = getCurrentObjPos();
      const D3DXVECTOR3 dir    = objPos - m_currentCamera->getPos();
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
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  const D3DXVECTOR3      camUp    = m_currentCamera->getUp(), camRight = m_currentCamera->getRight();
  const D3DXQUATERNION   rot      = createRotation(camUp, angle1) * createRotation(camRight, angle2);
  setCurrentVisualOrientation(w.getOrientation() * rot);
  CHECKINVARIANT();
}

void D3SceneEditor::rotateCurrentVisualLeftRight(float angle) {
  CHECKINVARIANT();
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  const D3DXVECTOR3     camDir = m_currentCamera->getDir();
  const D3DXQUATERNION  rot    = createRotation(camDir, angle);
  setCurrentVisualOrientation(w.getOrientation() * rot);
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
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  D3World w(*obj);
  D3DXVECTOR3 scale = w.getScale();
  if(component & VADJUST_X) scale.x *= factor;
  if(component & VADJUST_Y) scale.y *= factor;
  if(component & VADJUST_Z) scale.z *= factor;
  setCurrentVisualWorld(w.setScale(scale));
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
    { const D3Ray ray = m_currentCamera->getPickedRay(pt);
      moveCamera(ray.m_dir, 0.25f*signDelta);
    }
    break;
  case MK_CONTROL  :
    rotateCameraUpDown(signDelta / -50.0f);
    break;
  case MK_SHIFT    :
    walkWithCamera(1.5f * signDelta, 0);
    break;
  case MK_CTRLSHIFT:
    rotateCameraLeftRight(signDelta / 50.0f);
    break;
  }
}

// pt in window-coordinates
void D3SceneEditor::OnMouseWheelCameraProjection(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { const float a = m_currentCamera->getViewAngle();
      const float d = ((a > D3DX_PI/2) ? (D3DX_PI - a) : a) / (D3DX_PI/2);
      m_currentCamera->setViewAngle(a + d * 0.04f * signDelta);
    }
    break;
  case MK_CONTROL  :
    m_currentCamera->setNearViewPlane(m_currentCamera->getNearViewPlane() * (1 + 0.05f*signDelta));
    break;
  }
}

void D3SceneEditor::walkWithCamera(float dist, float angle) {
  D3World w = m_currentCamera->getWorld();
  w.setPos(w.getPos() + w.getDir()*dist)
       .setOrientation(w.getOrientation()*createRotation(w.getUp(), angle));
  m_currentCamera->setWorld(w);
}

void D3SceneEditor::sidewalkWithCamera(float upDist, float rightDist) {
  const D3World &cw = m_currentCamera->getWorld();
  m_currentCamera->setPos(cw.getPos()
                        + cw.getUp()    * upDist
                        + cw.getRight() * rightDist);
}

void D3SceneEditor::moveCamera(const D3DXVECTOR3 &dir, float dist) {
  const D3World &cw = m_currentCamera->getWorld();
  m_currentCamera->setPos(cw.getPos() + unitVector(dir) * dist);
}

void D3SceneEditor::rotateCameraUpDown(float angle) {
  const D3World &cw = m_currentCamera->getWorld();
  m_currentCamera->setOrientation(cw.getOrientation() * createRotation(cw.getRight(), angle));
}

void D3SceneEditor::rotateCameraLeftRight(float angle) {
  const D3World &cw = m_currentCamera->getWorld();
  m_currentCamera->setOrientation(cw.getOrientation() * createRotation(cw.getDir(), angle));
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
  D3LightControl   *lc    = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  const D3Light     param = lc->getLight();
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
      const D3DXVECTOR3 newDir = rotate(rotate(dir,m_currentCamera->getUp(),angle1), m_currentCamera->getRight(),angle2);
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
  D3LightControl            *lc  = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_DIRECTIONAL));
  D3LightControlDirectional &ctrl = *(D3LightControlDirectional*)lc;
  const D3DXVECTOR3          dir  = ctrl.getLight().Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    ctrl.setSphereRadius(ctrl.getSphereRadius() * (1.0f-0.04f*signDelta));
    renderVisible(SE_RENDERALL);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, m_currentCamera->getRight(), -0.06f*signDelta));
    break;
  case MK_SHIFT    :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, m_currentCamera->getUp(),    -0.06f*signDelta));
    break;
  case MK_CTRLSHIFT:
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, m_currentCamera->getDir(),    0.06f*signDelta));
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightPoint(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl      *lc   = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  D3LightControlPoint &ctrl = *(D3LightControlPoint*)lc;
  const D3DXVECTOR3    pos  = ctrl.getLight().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + m_currentCamera->getDir()   * 0.04f*signDelta);
    break;
  case MK_CONTROL  :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + m_currentCamera->getUp()    * 0.04f*signDelta);
    break;
  case MK_SHIFT    :
    getScene().setLightPosition(ctrl.getLightIndex(), pos + m_currentCamera->getRight() * 0.04f*signDelta);
    break;
  case MK_CTRLSHIFT:
    break;
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnMouseWheelLightSpot(UINT nFlags, short zDelta, CPoint pt) {
  CHECKINVARIANT();
  D3LightControl    *lc    = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  D3LightControlSpot &ctrl = *(D3LightControlSpot*)lc;
  const D3Light      param = ctrl.getLight();
  const D3DXVECTOR3  pos   = param.Position;
  const D3DXVECTOR3  dir   = param.Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    getScene().setLightPosition( ctrl.getLightIndex(), pos + m_currentCamera->getUp() * 0.04f*signDelta);
    break;
  case MK_CONTROL  :
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, m_currentCamera->getRight(), -0.06f*signDelta));
    break;
  case MK_SHIFT    :
    getScene().setLightPosition( ctrl.getLightIndex(), pos + m_currentCamera->getRight()   * 0.04f*signDelta);
    break;
  case MK_CTRLSHIFT:
    getScene().setLightDirection(ctrl.getLightIndex(), rotate(dir, m_currentCamera->getUp(), 0.06f*signDelta));
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
  D3Light light = lc->getLight();
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

void D3SceneEditor::OnLightAddDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void D3SceneEditor::OnLightAddPoint()       { addLight(D3DLIGHT_POINT);       }
void D3SceneEditor::OnLightAddSpot()        { addLight(D3DLIGHT_SPOT);        }

void D3SceneEditor::addLight(D3DLIGHTTYPE type) {
  CHECKINVARIANT();
  D3DLIGHT lp = D3Light::createDefaultLight(type);
  switch(type) {
  case D3DLIGHT_DIRECTIONAL:
    break;
  case D3DLIGHT_POINT      :
    m_pickedPoint = lp.Position = m_pickedRay.getHitPoint(2);
    break;
  case D3DLIGHT_SPOT       :
    m_pickedPoint = lp.Position = m_pickedRay.getHitPoint(3);
    lp.Direction  = unitVector((m_currentCamera->getPos() + 5 * m_currentCamera->getDir()) - lp.Position);
    break;
  }
  getScene().setLightControlVisible(getScene().addLight(lp), true);
  CHECKINVARIANT();
}

void D3SceneEditor::setCurrentObj(D3SceneObjectVisual *obj) {
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
      m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, lc->getLight());
    } else if(obj && obj->hasMaterial()) {
      m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, obj->getMaterial());
    }
  }
  renderInfo();
  CHECKINVARIANT();
}

SceneObjectType D3SceneEditor::getCurrentObjType() const {
  return m_currentObj ? m_currentObj->getType() : SOTYPE_NULL;
}

D3SceneObjectVisual *D3SceneEditor::getCurrentVisual() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return getCurrentObj();
  }
  return NULL;
}

D3SceneObjectAnimatedMesh *D3SceneEditor::getCurrentAnimatedObj() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
    return (D3SceneObjectAnimatedMesh*)getCurrentObj();
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
  renderVisible(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::setLightControlRenderEffect(bool enabled) {
  CHECKINVARIANT();
  if(enabled != D3LightControl::isRenderEffectEnabled()) {
    D3LightControl::enableRenderEffect(enabled);
    renderVisible(SE_RENDER3D);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::setSpecularEnable(bool enabled) {
  getScene().setSpecularEnable(enabled);
  renderVisible(SE_RENDER3D);
}

void D3SceneEditor::setSpotToPointAt(CPoint point) {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
    resetCurrentControl();
    CHECKINVARIANT();
    return;
  }
  D3SceneObjectVisual *obj = m_currentCamera->getPickedVisual(point, ~OBJMASK_LIGHTCONTROL, &m_pickedPoint, &m_pickedRay, NULL, &m_pickedInfo);
  if(obj == NULL) {
    m_pickedInfo.clear();
    CHECKINVARIANT();
    return;
  }
  D3Light param = lc->getLight();
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
  TrackPopupMenu(*menu.GetSubMenu(0), TPM_LEFTALIGN|TPM_RIGHTBUTTON, point.x,point.y, 0, m_sceneContainer->getMessageWindow(),NULL);
}

static String lightMenuText(const D3Light &light) {
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
    removeMenuItem(menu,ID_LIGHT_HIDECONTROLS);
  }
  if(visibleLightCount == getScene().getLightCount()) {
    removeMenuItem(menu, ID_LIGHT_SHOWCONTROLS);
  }
  const LightArray lights = getScene().getAllLights();
  BitSet definedLights(getScene().getMaxLightCount());
  for(size_t i = 0; i < lights.size(); i++) {
    const D3Light &l = lights[i];
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
  removeMenuItem(menu, getScene().getSpecularEnable()
                      ?ID_ENABLE_SPECULARHIGHLIGHT
                      :ID_DISABLE_SPECULARHIGHLIGHT);
  removeMenuItem(menu, getScene().getRightHanded()
                      ?ID_RIGHTHANDED
                      :ID_LEFTHANDED);

  if(!m_sceneContainer->canSplit3DWindow(m_currentCamera->getHwnd())) {
    removeSubMenuContainingId(menu, ID_CAMERA_SPLITVERTICAL);
  }
  if(!m_sceneContainer->canDelete3DWindow(m_currentCamera->getHwnd())) {
    removeMenuItem(menu, ID_CAMERA_REMOVE);
  }
  m_currentCamera->modifyContextMenu(*menu.GetSubMenu(0));
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
    { D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
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
  if(m_centerOfRotation.isEmpty()) {
    removeMenuItem(menu, ID_OBJECT_RESETCENTEROFROTATION);
  }
  if(!m_currentObj->hasFillMode()) {
    removeSubMenuContainingId(menu, ID_OBJECT_FILLMODE_WIREFRAME);
  } else {
    switch(m_currentObj->getFillMode()) {
    case D3DFILL_SOLID     : removeMenuItem(menu, ID_OBJECT_FILLMODE_SOLID    ); break;
    case D3DFILL_WIREFRAME : removeMenuItem(menu, ID_OBJECT_FILLMODE_WIREFRAME); break;
    case D3DFILL_POINT     : removeMenuItem(menu, ID_OBJECT_FILLMODE_POINT    ); break;
    }
  }
  if(!m_currentObj->hasShadeMode()) {
    removeSubMenuContainingId(menu, ID_OBJECT_SHADING_FLAT);
  } else {
    switch(m_currentObj->getShadeMode()) {
    case D3DSHADE_FLAT     : removeMenuItem(menu, ID_OBJECT_SHADING_FLAT    ); break;
    case D3DSHADE_GOURAUD  : removeMenuItem(menu, ID_OBJECT_SHADING_GOURAUD ); break;
    case D3DSHADE_PHONG    : removeMenuItem(menu, ID_OBJECT_SHADING_PHONG   ); break;
    }
  }
  m_currentObj->modifyContextMenu(*menu.GetSubMenu(0));
  showContextMenu(menu, point);
}

void D3SceneEditor::OnContextMenuLightControl(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  const D3Light light = lc->getLight();
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

void D3SceneEditor::OnControlCameraWalk()       { setCurrentControl(CONTROL_CAMERA_WALK      ); }
void D3SceneEditor::OnControlCameraProjection() { setCurrentControl(CONTROL_CAMERA_PROJECTION); }
void D3SceneEditor::OnControlObjPos()           { setCurrentControl(CONTROL_OBJECT_POS       ); }
void D3SceneEditor::OnControlObjScale()         { setCurrentControl(CONTROL_OBJECT_SCALE     ); }

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
  unselectPropertyDialog();
  setCurrentControl(CONTROL_IDLE);
}

void D3SceneEditor::setCurrentControl(D3EditorControl control) {
  switch(control) {
  case CONTROL_OBJECT_POS:
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_HAND));
    break;
  case CONTROL_CAMERA_WALK     :
    setCurrentObj(NULL);
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_CROSS));
    break;
  default:
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  }
  m_currentControl = control;
  CHECKINVARIANT();
}

SceneObjectType D3SceneEditor::getCurrentControlObjType() const {
  switch(getCurrentControl()) {
  case CONTROL_IDLE             :
  case CONTROL_CAMERA_WALK      :
  case CONTROL_CAMERA_PROJECTION: return SOTYPE_NULL;
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
    m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, m_currentObj->getMaterial());
    setCurrentControl(CONTROL_MATERIAL);
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnCameraResetPosition()    {  m_currentCamera->resetPos();         }
void D3SceneEditor::OnCameraResetOrientation() {  m_currentCamera->resetOrientation(); }
void D3SceneEditor::OnCameraResetProjection()  {  m_currentCamera->resetProjection();  }
void D3SceneEditor::OnCameraResetAll()         {  m_currentCamera->resetAll();         }

void D3SceneEditor::SetRightHanded(bool rightHanded) {
  getScene().setRightHanded(rightHanded);
}

typedef enum {
  ANIMATION_STOPPED
 ,ANIMATION_FORWARD
 ,ANIMATION_BACKWARDS
 ,ANIMATION_ALTERNATING
} AnimationState;

static void setCurrentAnimationState(D3SceneObjectAnimatedMesh *obj, AnimationState newState) {
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
  D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
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
  D3SceneObjectAnimatedMesh *obj = getCurrentAnimatedObj();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
    renderInfo();
  }
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectRemove() {
  CHECKINVARIANT();
  if(m_currentObj == NULL) return;
  D3SceneObjectVisual *obj = m_currentObj;
  getScene().removeVisual(m_currentObj);
  setCurrentObj(NULL);
  m_pickedInfo.clear();
  SAFEDELETE(obj);
  renderVisible(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectSetCenterOfRotation() {
  m_centerOfRotation.set(m_currentObj, m_pickedInfo.getMeshPoint());
  renderInfo();
}

void D3SceneEditor::OnObjectResetCenterOfRotation() {
  m_centerOfRotation.reset();
  renderInfo();
}

void D3SceneEditor::setLightEnabled(bool enabled) {
  CHECKINVARIANT();
  getScene().setLightEnabled(getCurrentLightControl()->getLightIndex(), enabled);
  renderVisible(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnLightAdjustColors() {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, lc->getLight());
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
  renderVisible(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnLightRemove() {
  CHECKINVARIANT();
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  setCurrentObj(NULL);
  const int lightIndex = lc->getLightIndex();
  getScene().removeLight(lightIndex);
  renderVisible(SE_RENDERALL);
  CHECKINVARIANT();
}

void D3SceneEditor::OnSceneEditAmbientLight() {
  CHECKINVARIANT();
  resetCurrentControl();
  const D3DCOLOR oldColor = getScene().getAmbientColor();
  CColorDlg dlg(_T("Ambient color"), SP_AMBIENTCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  selectPropertyDialog(&dlg, CONTROL_AMBIENTLIGHTCOLOR);
  if(dlg.DoModal() != IDOK) {
    getScene().setAmbientColor(oldColor);
    renderVisible(SE_RENDERALL);
  }
  unselectPropertyDialog();
  CHECKINVARIANT();
}

void D3SceneEditor::OnCameraEditBackgroundColor() {
  CHECKINVARIANT();
  resetCurrentControl();
  const D3DCOLOR oldColor = m_currentCamera->getBackgroundColor();
  CColorDlg dlg(_T("Background color"), CAM_BACKGROUNDCOLOR, oldColor);

  dlg.addPropertyChangeListener(this);
  selectPropertyDialog(&dlg, CONTROL_BACKGROUNDCOLOR);
  if(dlg.DoModal() != IDOK) {
    m_currentCamera->setBackgroundColor(oldColor);
  }
  unselectPropertyDialog();
  CHECKINVARIANT();
}

void D3SceneEditor::OnObjectFillmodePoint()     { getCurrentObj()->setFillMode(D3DFILL_POINT     ); renderVisible(SE_RENDERALL); }
void D3SceneEditor::OnObjectFillmodeWireframe() { getCurrentObj()->setFillMode(D3DFILL_WIREFRAME ); renderVisible(SE_RENDERALL); }
void D3SceneEditor::OnObjectFillmodeSolid()     { getCurrentObj()->setFillMode(D3DFILL_SOLID     ); renderVisible(SE_RENDERALL); }
void D3SceneEditor::OnObjectShadingFlat()       { getCurrentObj()->setShadeMode(D3DSHADE_FLAT    ); renderVisible(SE_RENDERALL); }
void D3SceneEditor::OnObjectShadingGouraud()    { getCurrentObj()->setShadeMode(D3DSHADE_GOURAUD ); renderVisible(SE_RENDERALL); }
void D3SceneEditor::OnObjectShadingPhong()      { getCurrentObj()->setShadeMode(D3DSHADE_PHONG   ); renderVisible(SE_RENDERALL); }

void D3SceneEditor::setCoordinateSystemVisible(bool visible) {
  if(visible) {
    if(m_coordinateSystem == NULL) {
      m_coordinateSystem = new D3SceneObjectCoordinateSystem(getScene()); TRACE_NEW(m_coordinateSystem);
      getScene().addVisual(m_coordinateSystem);
    } else {
      m_coordinateSystem->setVisible(true);
    }
  } else {
    if(isCoordinateSystemVisible()) {
      m_coordinateSystem->setVisible(false);
    }
  }
  renderVisible(SE_RENDER3D);
}

void D3SceneEditor::setSelectedCubeVisible(bool visible) {
  if(visible) {
    if(m_selectedCube == NULL) {
      m_selectedCube = new D3SelectedCube(getScene()); TRACE_NEW(m_selectedCube);
      getScene().addVisual(m_selectedCube);
    } else {
      m_selectedCube->setVisible(true);
    }
  } else {
    if(isSelectedCubeVisible()) {
      m_selectedCube->setVisible(false);
    }
  }
  renderVisible(SE_RENDER3D);
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
    getScene().save(dlg.m_ofn.lpstrFile);
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
    getScene().load(dlg.m_ofn.lpstrFile);
    renderVisible(SE_RENDERALL);
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
  case CONTROL_CAMERA_PROJECTION:
    break;
  case CONTROL_OBJECT_POS       :
  case CONTROL_OBJECT_SCALE     :
    verify((objType == SOTYPE_VISUALOBJECT) || (objType == SOTYPE_ANIMATEDOBJECT));
    break;
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
    verify(m_currentPropertyDialog != NULL);
    break;
  default                       :
    verify(m_currentPropertyDialog == NULL);
    break;
  }
}

#endif // _DEBUG
