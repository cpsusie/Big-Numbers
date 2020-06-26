#include "pch.h"
#include <ByteFile.h>
#include <ExternProcess.h>
#include <MFCUtil/ColorDlg.h>
#include <D3DGraphics/D3Device.h>
#include <D3DGraphics/D3ToString.h>
#include <D3DGraphics/LightDlg.h>
#include <D3DGraphics/MaterialDlg.h>
#include <D3DGraphics/D3Camera.h>
#include <D3DGraphics/D3LightControl.h>
#include <D3DGraphics/D3SceneObjectAnimatedVisual.h>
#include <D3DGraphics/D3SceneObjectCoordinateSystem.h>
#include <D3DGraphics/D3AbstractTextureFactory.h>
#include <D3DGraphics/D3SelectedCube.h>
#include <D3DGraphics/D3SceneEditor.h>

D3SceneEditor::D3SceneEditor()
: m_sceneContainer(        NULL )
, m_currentPropertyDialog( NULL )
, m_selectedCamera(        NULL )
, m_selectedCameraIndex(   -1   )
, m_currentObj(            NULL )
, m_currentControl(CONTROL_IDLE )
, m_coordinateSystem(      NULL )
, m_paramFileName(_T("Untitled"))
, m_stateFlags(  SE_MOUSEVISIBLE)
{
}

#define sCAM   (*getSelectedCAM())
#define SCENE  (*getScene())

D3SceneEditor::~D3SceneEditor() {
  close();
}

D3Scene *D3SceneEditor::getScene() const {
  return hasSceneContainer() ? &m_sceneContainer->getScene() : NULL;
}
D3Device *D3SceneEditor::getDevice() const {
  D3Scene *scene = getScene();
  return scene ? &scene->getDevice() : NULL;
}

CameraSet D3SceneEditor::getActiveCameraSet() const {
  return SCENE.getCameraArray().getActiveCameraSet();
}

CameraSet D3SceneEditor::getSelectedCameraSet() const {
  CameraSet set;
  if(m_selectedCameraIndex >= 0) {
    set.add(m_selectedCameraIndex);
  }
  return set;
}

void D3SceneEditor::render(BYTE flags, CameraSet cameraSet) const {
  if(isSet(flags)) {
    m_sceneContainer->render(flags, cameraSet);
  }
}

void D3SceneEditor::init(D3SceneContainer *sceneContainer) {
  m_sceneContainer   = sceneContainer;
  CLightDlg    *dlg1 = new CLightDlg();    TRACE_NEW(dlg1);
  CMaterialDlg *dlg2 = new CMaterialDlg(); TRACE_NEW(dlg2);
  m_propertyDialogMap.addDialog(dlg1).addDialog(dlg2).addPropertyChangeListener(this);
  const UINT count = sceneContainer->get3DWindowCount();
  for(UINT i = 0; i < count; i++) {
    SCENE.addCamera(sceneContainer->get3DWindow(i));
  }
  m_currentControl = CONTROL_IDLE;
  selectCAM((count >= 1) ? 0 : -1);
  SCENE.addPropertyChangeListener(this);
  setFlag(SE_INITDONE);
}

void D3SceneEditor::close() {
  if(isInitDone()) {
    SCENE.removePropertyChangeListener(this);
    m_propertyDialogMap.removeAllDialogs();
    SCENE.removeVisual(m_coordinateSystem);
    SAFEDELETE(m_coordinateSystem);
    SCENE.close();
    clrFlag(SE_INITDONE);
  }
}

D3SceneEditor &D3SceneEditor::setEnabled(bool enabled, BYTE flags) {
  if(isInitDone()) {
    flags &= ~SE_INITDONE;
    if(enabled) {
      setFlag(flags);
      renderInfo();
    } else {
      clrFlag(flags);
      if(!isSet(SE_LIGHTCONTROLS)) {
        OnLightControlHide();
        mapDialogHide();
      }
      renderInfo();
    }
    setMouseVisible(true);
  }
  return *this;
}

D3SceneEditor &D3SceneEditor::pushStateFlags(bool enabled, BYTE flags) {
  m_stateFlagsStack.push(m_stateFlags);
  return setEnabled(enabled, flags);
}

D3SceneEditor &D3SceneEditor::popStateFlags() {
  return setEnabled(true, m_stateFlagsStack.pop());
}

D3SceneEditor &D3SceneEditor::popAllStateFlags() {
  while(!m_stateFlagsStack.isEmpty()) {
    popStateFlags();
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
    setFlag(SE_MOUSEVISIBLE);
  } else {
    ShowCursor(FALSE);
    clrFlag(SE_MOUSEVISIBLE);
  }
}

void D3SceneEditor::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(!isSet(SE_PROPCHANGES)) {
    return;
  }
  if(source == getScene()) {
    switch(id) {
    case SP_MATERIALPARAMETERS    :
    case SP_LIGHTPARAMETERS       :
    case SP_RIGHTHANDED           :
      renderActiveCameras(SC_RENDERALL);
      break;
    case SP_VISUALCOUNT           :
      { const UINT oldCount = *(UINT*)oldValue, newCount = *(UINT*)newValue;
        if(newCount < oldCount) {
          if(!SCENE.isVisual(m_currentObj)) {
            resetControl();
          }
        } else {
          setControl(CONTROL_IDLE, SCENE.getLastVisual());
        }
        if(!m_centerOfRotation.isEmpty() && !SCENE.isVisual(m_centerOfRotation.m_obj)) {
          m_centerOfRotation.reset();
        }
        renderActiveCameras(SC_RENDERALL);
      }
      break;
    case SP_ANIMATIONFRAMEINDEX   :
      renderActiveCameras(SC_RENDER3D);
      break;
    case SP_AMBIENTCOLOR          :
      renderActiveCameras(SC_RENDERALL | SC_RENDERNOW);
      break;
    case SP_LIGHTCOUNT            :
      { const UINT oldCount = *(UINT*)oldValue, newCount = *(UINT*)newValue;
        if(newCount < oldCount) {
          D3Light light;
          if((m_propertyDialogMap.getVisiblePropertyId() == SP_LIGHTPARAMETERS)
          &&  m_propertyDialogMap.getVisibleProperty(light)
          && (!SCENE.isLightDefined(light.getIndex())))
          {
            mapDialogHide();
          }
        }
      }
      break;
    case SP_MATERIALCOUNT         :
      { const UINT oldCount = *(UINT*)oldValue, newCount = *(UINT*)newValue;
        if(newCount < oldCount) {
          D3Material material;
          if((m_propertyDialogMap.getVisiblePropertyId() == SP_MATERIALPARAMETERS)
          &&  m_propertyDialogMap.getVisibleProperty(material)
          && (!SCENE.isMaterialDefined(material.getId())))
          {
            mapDialogHide();
          }
        }
      }
      break;
    } // switch
  } else if(source == getSelectedCAM()) {
    switch(id) {
    case CAM_VIEW                 :  // D3DXMATRIX
    case CAM_PROJECTION           :  // D3DXMATRIX
    case CAM_LIGHTCONTROLSVISIBLE :  // BitSet
      renderSelectedCamera(SC_RENDERALL);
      break;
    case CAM_BACKGROUNDCOLOR      :  // D3DCOLOR
      renderSelectedCamera(SC_RENDERALL|SC_RENDERNOW);
      break;
    }
  } else if(source == &m_propertyDialogMap) {
    switch(id) {
    case PDM_VISIBLE_PROPERTYID   :  // int    - notification id, when active dialog changes, or hide/show
      { const int newId = *(int*)newValue;
        if(newId == -1) {
          resetControl();
        }
      }
      break;
    case PDM_CURRENT_DLGVALUE     :  // void * - notification id, when value in active dialog changes
      { switch(m_propertyDialogMap.getVisiblePropertyId()) {
        case SP_LIGHTPARAMETERS   :
          { const D3Light &newLight = *(D3Light*)newValue;
            D3Light        tmp      = SCENE.getLight(newLight.getIndex()); // to keep pos and direction as they are
            if(tmp.getIndex() != newLight.getIndex()) {
              showWarning(_T("%s:Light %d is undefined"), __TFUNCTION__,newLight.getIndex());
            } else {
              SCENE.setLight(CLightDlg::copyModifiableValues(tmp, newLight));
            }
          }
          break;
        case SP_MATERIALPARAMETERS:
          SCENE.setMaterial(*(D3Material*)newValue);
          break;
        }
      }
      break;
    }
  } else if(source == m_currentPropertyDialog) {
    switch(id) {
    case CAM_BACKGROUNDCOLOR      :
      sCAM.setBackgroundColor(*(D3DCOLOR*)newValue);
      break;
    case SP_AMBIENTCOLOR          :
      SCENE.setAmbientColor(*(D3DCOLOR*)newValue);
      break;
    }
  }
}

D3SceneObjectVisual *D3SceneEditor::getPickedVisual(const CPoint &point, long mask, D3Ray &ray, D3SceneEditorPickedInfo &info) const {
  return sCAM.getPickedVisual(point, mask, &info.m_hitPoint, &ray, &info.m_dist, &info.m_info);
}

void D3SceneEditor::OnLButtonDown(UINT nFlags, CPoint point) {
  m_lastMouse = point;
  switch(getCurrentControl()) {
  case CONTROL_SPOTLIGHTPOINT:
    setSpotToPointAt(point);
    break;

  default:
    { D3SceneObjectVisual *pickedObj = getPickedVisual(point, OBJMASK_ALL, m_pickedRay, m_pickedInfo);
      if(pickedObj == NULL) {
        if(getCurrentControl() == CONTROL_CAMERA_WALK) {
          setMouseVisible(false);
          SetCapture(m_sceneContainer->getMessageWindow());
          m_pickedInfo.clear();
        } else {
          resetControl().setControl(CONTROL_CAMERA_WALK, NULL);
        }
      } else {
        switch(pickedObj->getType()) {
        case SOTYPE_VISUALOBJECT  :
        case SOTYPE_ANIMATEDOBJECT:
          setControl(CONTROL_OBJECT_POS, pickedObj);
          pickedObj->OnLButtonDown(nFlags, point);
          break;
        case SOTYPE_LIGHTCONTROL  :
          setControl(CONTROL_LIGHT, pickedObj);
          break;
        }
      }
    }
    break;
  }
}

void D3SceneEditor::OnLButtonUp(UINT nFlags, CPoint point) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnLButtonUp(nFlags, point)) {
    return;
  }

  switch(getCurrentControl()) {
  case CONTROL_SPOTLIGHTPOINT:
    setControl(CONTROL_IDLE, m_currentObj);
    break;
  case CONTROL_OBJECT_POS    :
  case CONTROL_CAMERA_WALK   :
    setMouseVisible(true);
    ReleaseCapture();
    renderInfo();
    break;
  }
}

void D3SceneEditor::OnContextMenu(HWND pwnd, MSG *pMsg) {
  m_lastMouse = sCAM.screenToWin(pMsg->pt);
  D3SceneObjectVisual *pickedObj = getPickedVisual(m_lastMouse, OBJMASK_ALL, m_pickedRay,m_pickedInfo);
  if(pickedObj == NULL) {
    m_pickedInfo.clear();
    OnContextMenuBackground(pMsg->pt);
  } else {
    setControl(CONTROL_IDLE, pickedObj);
    OnContextMenuObj(pMsg->pt);
  }
}

void D3SceneEditor::OnMouseMove(UINT nFlags, CPoint point) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnMouseMove(nFlags, point)) {
    return;
  }
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
}

void D3SceneEditor::OnLButtonDblClk(UINT nFlags, CPoint point) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnLButtonDblClk(nFlags, point)) {
    return;
  }
}

// pt in window-coordinates
BOOL D3SceneEditor::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnMouseWheel(nFlags, zDelta, pt)) {
    return TRUE;
  }
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

bool D3SceneEditor::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnKeyDown(nChar, nRepCnt, nFlags)) {
    return TRUE;
  }
  switch(nChar) {
  case VK_ESCAPE:
    setEnabled(false).setEnabled(true, SC_RENDERINFO).renderActiveCameras(SC_RENDERALL);
    return true;
  default:
    return false;
  }
}

bool D3SceneEditor::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) {
  D3SceneObjectVisual *visual = getCurrentVisual();
  if(visual && visual->OnKeyUp(nChar, nRepCnt, nFlags)) {
    return TRUE;
  }
  return false;
}

HWND D3SceneEditor::getCurrentHwnd() const {
  return hasCAM() ? sCAM.getHwnd() : (HWND)INVALID_HANDLE_VALUE;
}

int D3SceneEditor::findCameraIndex(CPoint p) const {
  return SCENE.getCameraArray().findCameraIndex(p);
}

bool D3SceneEditor::selectCAM(int index) {
  if(index != m_selectedCameraIndex) {
    if(hasCAM()) {
      sCAM.removePropertyChangeListener(this);
    }
    if(index >= 0) {
      m_selectedCamera = SCENE.getCameraArray()[index];
    } else {
      m_selectedCamera = NULL;
      m_pickedRay.clear();
    }
    if(hasCAM()) {
      sCAM.addPropertyChangeListener(this);
    }
    m_selectedCameraIndex = index;
  }
  return hasCAM();
}

bool D3SceneEditor::selectCAM(CPoint point) {
  return selectCAM(findCameraIndex(point));
}

bool D3SceneEditor::isSameCAM(CPoint point) const {
  return hasCAM() && sCAM.ptInRect(point);
}

bool D3SceneEditor::isCoordinateSystemVisible() const {
  return m_coordinateSystem && m_coordinateSystem->isVisible();
}

BOOL D3SceneEditor::PreTranslateMessage(MSG *pMsg) {
  if(!isEnabled() || (m_sceneContainer->getMessageWindow() != GetActiveWindow())) {
    return false;
  }
  switch(pMsg->message) {
  case WM_KEYDOWN:
    return OnKeyDown((UINT)pMsg->wParam, (UINT)(pMsg->lParam & 0xffff), (UINT)(pMsg->lParam>>16));
  case WM_KEYUP  :
    return OnKeyUp((UINT)pMsg->wParam, (UINT)(pMsg->lParam & 0xffff), (UINT)(pMsg->lParam >> 16));
  case WM_LBUTTONDOWN:
    if(selectCAM(pMsg->pt)) {
      OnLButtonDown((UINT)pMsg->wParam, sCAM.screenToWin(pMsg->pt));
      return true;
    }
    break;
  case WM_LBUTTONUP  :
    if(isSameCAM(pMsg->pt)) {
      OnLButtonUp((UINT)pMsg->wParam, sCAM.screenToWin(pMsg->pt));
      return true;
    }
    return selectCAM(-1);
  case WM_MOUSEMOVE  :
    if(isSameCAM(pMsg->pt)) {
      OnMouseMove((UINT)pMsg->wParam, sCAM.screenToWin(pMsg->pt));
      return true;
    }
    break;
  case WM_MOUSEWHEEL :
    if(isSameCAM(pMsg->pt)) {
      OnMouseWheel((UINT)(pMsg->wParam&0xffff), (short)(pMsg->wParam >> 16), sCAM.screenToWin(pMsg->pt));
      return true;
    }
    break;
  case WM_RBUTTONUP:
    if(selectCAM(pMsg->pt)) {
      OnContextMenu(m_sceneContainer->getMessageWindow(), pMsg);
      return true;
    }
    break;
  case WM_COMMAND:
    switch(pMsg->wParam) {
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
    case ID_OBJECT_SHOWDATA               : OnObjectShowData()                  ; return true;
    case ID_OBJECT_SHOWNORMALS            : OnObjectShowNormals(true)           ; return true;
    case ID_OBJECT_HIDENORMALS            : OnObjectShowNormals(false)          ; return true;
    case ID_OBJECT_SELECTTEXTURE          : OnObjectSelectTexture()             ; return true;
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
    case ID_CAMERA_SPLITVERTICAL          : OnCameraSplit(true)                 ; return true;
    case ID_CAMERA_SPLITHORIZONTAL        : OnCameraSplit(false)                ; return true;
    case ID_CAMERA_REMOVE                 : OnCameraRemove()                    ; return true;
    case ID_RIGHTHANDED                   : SetRightHanded(true)                ; return true;
    case ID_LEFTHANDED                    : SetRightHanded(false)               ; return true;
    case ID_SCENE_EDIT_AMBIENTLIGHT       : OnSceneEditAmbientLight()           ; return true;
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
    case ID_LIGHT_SHOWCONTROLS            : setAllLightControlsVisible(true)    ; return true;
    case ID_LIGHT_HIDECONTROLS            : setAllLightControlsVisible(false)   ; return true;
    case ID_LIGHTCONTROL_HIDE             : OnLightControlHide()                ; return true;
    case ID_LIGHTCONTROL_SPOTAT           : OnLightControlSpotAt()              ; return true;
    case ID_LIGHTCONTROL_ENABLEEFFECT     : setLightControlRenderEffect(true)   ; return true;
    case ID_LIGHTCONTROL_DISABLEEFFECT    : setLightControlRenderEffect(false)  ; return true;
    case ID_SCENE_SAVEPARAMETERS          : OnSceneSaveParameters()             ; return true;
    case ID_SCENE_LOADPARAMETERS          : OnSceneLoadParameters()             ; return true;

    default:
      if(hasCAM() && (ID_SELECT_LIGHT0 <= pMsg->wParam) && (pMsg->wParam <= ID_SELECT_LIGHT20)) {
        const UINT index = (int)pMsg->wParam - ID_SELECT_LIGHT0;
        setControl(CONTROL_IDLE, sCAM.setLightControlVisible(index, true));
        return true;
      }
    }
  }
  return false;
}

// ---------------------------------- Controlling object -----------------------------------------
void D3SceneEditor::OnMouseMoveObjPos(UINT nFlags, CPoint pt) {
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
}

void D3SceneEditor::moveCurrentObjXY(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentObjPos() - m_pickedInfo.m_hitPoint;
  const float       dist           = length(m_pickedInfo.m_hitPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = sCAM.getPickedRay(pt);
  const D3DXVECTOR3 newPickedPoint = newPickedRay.getHitPoint(dist);
  setCurrentObjPos(newPickedPoint  + dp);
  m_pickedRay                      = newPickedRay;
  m_pickedInfo.m_hitPoint          = newPickedPoint;
  m_pickedInfo.m_dist              = length(newPickedPoint - newPickedRay.m_orig);
}

void D3SceneEditor::moveCurrentObjXZ(CPoint pt) {
  const D3DXVECTOR3 dp             = getCurrentObjPos() - m_pickedInfo.m_hitPoint;
  const float       dist           = length(m_pickedInfo.m_hitPoint - m_pickedRay.m_orig);
  const D3Ray       newPickedRay   = sCAM.getPickedRay(pt);
  const D3Ray       ray1           = sCAM.getPickedRay(CPoint(pt.x,pt.y+1));
  const float       dRaydPixel     = length(newPickedRay.getHitPoint(dist) - ray1.getHitPoint(dist));
  const CSize       dMouse         = pt - m_lastMouse;
  const D3DXVECTOR3 camDir         = sCAM.getDir(), camRight = sCAM.getRight();
  const D3DXVECTOR3 newPickedPoint = m_pickedInfo.m_hitPoint
                                   - (dRaydPixel * dMouse.cy) * camDir
                                   + (dRaydPixel * dMouse.cx) * camRight;
  setCurrentObjPos(newPickedPoint + dp);
  m_pickedRay             = newPickedRay;
  m_pickedInfo.m_hitPoint = newPickedPoint;
  m_pickedInfo.m_dist     = length(newPickedPoint - newPickedRay.m_orig);
}

D3DXVECTOR3 D3SceneEditor::getCurrentObjPos() {
  switch(getCurrentObjType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_LIGHTCONTROL  :
  case SOTYPE_ANIMATEDOBJECT:
    return ((D3World)(*getCurrentObj())).getPos();
  default:
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), getCurrentObjType());
    return D3DXVECTOR3(0,0,0);
  }
}

void D3SceneEditor::setCurrentObjPos(const D3DXVECTOR3 &pos) {
  D3SceneObjectVisual *obj = getCurrentObj();
  if(obj == NULL) return;
  switch(obj->getType()) {
  case SOTYPE_LIGHTCONTROL  :
    SCENE.setLightPosition(getCurrentLightControl()->getLightIndex(), pos);
    // continue case
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    obj->getWorld() = D3World(*obj).setPos(pos);
    renderActiveCameras(SC_RENDERALL);
    break;
  default                   :
    throwInvalidArgumentException(__TFUNCTION__, _T("type=%d"), obj->getType());
  }
}

void D3SceneEditor::setCurrentVisualWorld(const D3DXMATRIX &world) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj) {
    obj->getWorld() = world;
    renderActiveCameras(SC_RENDERALL);
  }
}

const D3DXMATRIX *D3SceneEditor::getCurrentVisualWorld() const {
  D3SceneObjectVisual *obj = getCurrentVisual();
  return (obj == NULL) ? NULL : &obj->getWorld();
}

void D3SceneEditor::setCurrentVisualOrientation(const D3DXQUATERNION &q, const D3DXVECTOR3 &centerOfRotation) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).setOrientation(q, centerOfRotation));
}

void D3SceneEditor::setCurrentVisualOrientation(const D3DXQUATERNION &q) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  if(obj != m_centerOfRotation.m_obj) {
    setCurrentVisualWorld(D3World(*obj).setOrientation(q));
  } else {
    setCurrentVisualOrientation(q, m_centerOfRotation.getWorldPoint());
  }
}

void D3SceneEditor::setCurrentVisualScale(const D3DXVECTOR3 &scale) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).setScale(scale));
}

void D3SceneEditor::OnObjectResetPosition() {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).resetPos());
}

void D3SceneEditor::OnObjectResetScale() {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).resetScale());
}
void D3SceneEditor::OnObjectResetOrientation() {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).resetOrientation());
}

void D3SceneEditor::OnObjectResetAll() {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World());
}

#define signDelta ((float)sign(zDelta))

void D3SceneEditor::OnMouseWheelObjPos(UINT nFlags, short zDelta, CPoint pt) {
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    { D3Ray                   ray;
      D3SceneEditorPickedInfo info;
      D3SceneObjectVisual    *obj = getPickedVisual(pt, OBJMASK_VISUALOBJECT | OBJMASK_ANIMATEDOBJECT, ray, info);
      if(obj != getCurrentVisual()) return;
      const float       factor = 1.0f + signDelta / 30.0f;
      const D3DXVECTOR3 v0     = info.m_hitPoint - ray.m_orig;
      const D3DXVECTOR3 newPos = ray.m_orig + v0 * factor;
      const D3DXVECTOR3 dp     = newPos - info.m_hitPoint;
      const D3DXVECTOR3 objPos = getCurrentObjPos();
      setCurrentObjPos(objPos + dp);
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
}

void D3SceneEditor::rotateCurrentVisualFrwBckw(float angle1, float angle2) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  const D3DXQUATERNION rot = createRotation(sCAM.getUp(), angle1) * createRotation(sCAM.getRight(), angle2);
  setCurrentVisualOrientation(D3World(*obj).getOrientation() * rot);
}

void D3SceneEditor::rotateCurrentVisualLeftRight(float angle) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualOrientation(D3World(*obj).getOrientation() * createRotation(sCAM.getDir(), angle));
}

void D3SceneEditor::OnMouseWheelObjScale(UINT nFlags, short zDelta, CPoint pt) {
  const float factor = (1.0f+ 0.04f*signDelta);
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           : adjustCurrentVisualScale(VADJUST_ALL, factor); break;
  case MK_CONTROL  : adjustCurrentVisualScale(VADJUST_X  , factor); break;
  case MK_SHIFT    : adjustCurrentVisualScale(VADJUST_Y  , factor); break;
  case MK_CTRLSHIFT: adjustCurrentVisualScale(VADJUST_Z  , factor); break;
  }
}

void D3SceneEditor::adjustCurrentVisualScale(int component, float factor) {
  D3SceneObjectVisual *obj = getCurrentVisual();
  if(obj == NULL) return;
  setCurrentVisualWorld(D3World(*obj).multiplyScale(factor, component));
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
    { const D3Ray ray = sCAM.getPickedRay(pt);
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
    { const float a = sCAM.getViewAngle();
      const float d = ((a > D3DX_PI/2) ? (D3DX_PI - a) : a) / (D3DX_PI/2);
      sCAM.setViewAngle(a + d * 0.04f * signDelta);
    }
    break;
  case MK_CONTROL  :
    sCAM.setNearViewPlane(sCAM.getNearViewPlane() * (1 + 0.05f*signDelta));
    break;
  case MK_SHIFT:
    sCAM.setFarViewPlane(sCAM.getFarViewPlane() * (1 + 0.05f*signDelta));
    break;
  }
}

void D3SceneEditor::walkWithCamera(float dist, float angle) {
  D3World cw = sCAM.getD3World();
  sCAM.setD3World(cw.setPos(cw.getPos() + cw.getDir()*dist).rotate(createRotation(cw.getUp(), angle)));
}

void D3SceneEditor::sidewalkWithCamera(float upDist, float rightDist) {
  const D3World &cw = sCAM.getD3World();
  sCAM.setPos(cw.getPos() + cw.getUp()*upDist + cw.getRight()*rightDist);
}

void D3SceneEditor::moveCamera(const D3DXVECTOR3 &dir, float dist) {
  const D3World &cw = sCAM.getD3World();
  sCAM.setPos(cw.getPos() + unitVector(dir) * dist);
}

void D3SceneEditor::rotateCameraUpDown(float angle) {
  D3World cw = sCAM.getD3World();
  sCAM.setD3World(cw.rotate(createRotation(cw.getRight(), angle)));
}

void D3SceneEditor::rotateCameraLeftRight(float angle) {
  D3World cw = sCAM.getD3World();
  sCAM.setD3World(cw.rotate(createRotation(cw.getDir(), angle)));
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
}

void D3SceneEditor::OnMouseMoveLightSpot(UINT nFlags, CPoint pt) {
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
      const D3DXVECTOR3 newDir = rotate(rotate(dir,sCAM.getUp(),angle1), sCAM.getRight(),angle2);
      SCENE.setLightDirection(lc->getLightIndex(), newDir);
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
  D3LightControl            *lc   = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_DIRECTIONAL));
  D3LightControlDirectional &ctrl = *(D3LightControlDirectional*)lc;
  const D3DXVECTOR3          dir  = ctrl.getLight().Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    ctrl.setSphereRadius(ctrl.getSphereRadius() * (1.0f-0.04f*signDelta));
    renderActiveCameras(SC_RENDERALL);
    break;
  case MK_CONTROL  :
    SCENE.setLightDirection(ctrl.getLightIndex(), rotate(dir, sCAM.getRight(), -0.06f*signDelta));
    break;
  case MK_SHIFT    :
    SCENE.setLightDirection(ctrl.getLightIndex(), rotate(dir, sCAM.getUp(),    -0.06f*signDelta));
    break;
  case MK_CTRLSHIFT:
    SCENE.setLightDirection(ctrl.getLightIndex(), rotate(dir, sCAM.getDir(),    0.06f*signDelta));
    break;
  }
}

void D3SceneEditor::OnMouseWheelLightPoint(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl      *lc   = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_POINT));
  D3LightControlPoint &ctrl = *(D3LightControlPoint*)lc;
  const D3DXVECTOR3    pos  = ctrl.getLight().Position;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    SCENE.setLightPosition(ctrl.getLightIndex(), pos + sCAM.getDir()   * 0.04f*signDelta);
    break;
  case MK_CONTROL  :
    SCENE.setLightPosition(ctrl.getLightIndex(), pos + sCAM.getUp()    * 0.04f*signDelta);
    break;
  case MK_SHIFT    :
    SCENE.setLightPosition(ctrl.getLightIndex(), pos + sCAM.getRight() * 0.04f*signDelta);
    break;
  case MK_CTRLSHIFT:
    break;
  }
}

void D3SceneEditor::OnMouseWheelLightSpot(UINT nFlags, short zDelta, CPoint pt) {
  D3LightControl    *lc    = getCurrentLightControl();
  assert(lc && (lc->getLightType() == D3DLIGHT_SPOT));
  D3LightControlSpot &ctrl = *(D3LightControlSpot*)lc;
  const D3Light      param = ctrl.getLight();
  const D3DXVECTOR3  pos   = param.Position;
  const D3DXVECTOR3  dir   = param.Direction;
  switch(nFlags & MK_CTRLSHIFT) {
  case 0           :
    SCENE.setLightPosition( ctrl.getLightIndex(), pos + sCAM.getUp() * 0.04f * signDelta);
    break;
  case MK_CONTROL  :
    SCENE.setLightDirection(ctrl.getLightIndex(), rotate(dir, sCAM.getRight(), -0.06f * signDelta));
    break;
  case MK_SHIFT    :
    SCENE.setLightPosition( ctrl.getLightIndex(), pos + sCAM.getRight() * 0.04f * signDelta);
    break;
  case MK_CTRLSHIFT:
    SCENE.setLightDirection(ctrl.getLightIndex(), rotate(dir, sCAM.getUp(), 0.06f * signDelta));
    break;
  }
}

void D3SceneEditor::OnMouseWheelLightSpotAngle(UINT nFlags, short zDelta, CPoint pt) {
  const D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
    resetControl();
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
  default          :
    return;
  }
  SCENE.setLight(light);
}
// ------------------------------------------------------------------------------------------------------------

void D3SceneEditor::OnLightAddDirectional() { addLight(D3DLIGHT_DIRECTIONAL); }
void D3SceneEditor::OnLightAddPoint()       { addLight(D3DLIGHT_POINT);       }
void D3SceneEditor::OnLightAddSpot()        { addLight(D3DLIGHT_SPOT);        }

void D3SceneEditor::addLight(D3DLIGHTTYPE type) {
  D3DLIGHT lp = D3Light::createDefaultLight(type);
  switch(type) {
  case D3DLIGHT_DIRECTIONAL:
    break;
  case D3DLIGHT_POINT      :
    lp.Position  = m_pickedRay.getHitPoint(2);
    break;
  case D3DLIGHT_SPOT       :
    lp.Position  = m_pickedRay.getHitPoint(3);
    lp.Direction = unitVector((sCAM.getPos() + 5 * sCAM.getDir()) - lp.Position);
    break;
  }
  try {
    sCAM.setLightControlVisible(SCENE.addLight(lp), true);
  } catch(Exception e) {
    showException(e);
  }
}

SceneObjectType D3SceneEditor::getCurrentObjType() const {
  return hasObj() ? getCurrentObj()->getType() : SOTYPE_NULL;
}

D3SceneObjectVisual *D3SceneEditor::getCurrentVisual() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
  case SOTYPE_VISUALOBJECT  :
    return getCurrentObj();
  }
  return NULL;
}

D3SceneObjectAnimatedVisual *D3SceneEditor::getCurrentAnimatedObj() const {
  switch(getCurrentObjType()) {
  case SOTYPE_ANIMATEDOBJECT:
    return (D3SceneObjectAnimatedVisual*)getCurrentObj();
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

void D3SceneEditor::setAllLightControlsVisible(bool visible) {
  sCAM.setLightControlsVisible(visible ? SCENE.getLightsDefined() : BitSet(10));
}

void D3SceneEditor::setLightControlRenderEffect(bool enabled) {
  if(enabled != D3LightControl::isRenderEffectEnabled()) {
    D3LightControl::enableRenderEffect(enabled);
    renderActiveCameras(SC_RENDER3D);
  }
}

void D3SceneEditor::setSpotToPointAt(CPoint point) {
  D3LightControl *lc = getCurrentLightControl();
  if((lc == NULL) || (lc->getLightType() != D3DLIGHT_SPOT)) {
    resetControl();
    return;
  }
  D3SceneObjectVisual *obj = getPickedVisual(point, ~OBJMASK_LIGHTCONTROL, m_pickedRay, m_pickedInfo);
  if(obj == NULL) {
    m_pickedInfo.clear();
  } else {
    D3Light param = lc->getLight();
    param.Direction = unitVector(m_pickedInfo.m_hitPoint - param.Position);
    SCENE.setLight(param);
  }
}

void D3SceneEditor::OnContextMenuObj(CPoint point) {
  switch(getCurrentObjType()) {
  case SOTYPE_VISUALOBJECT  :
  case SOTYPE_ANIMATEDOBJECT:
    OnContextMenuVisualObj(point);
    break;
  case SOTYPE_LIGHTCONTROL  :
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
  const int visibleLightCount = (int)sCAM.getLightControlsVisible().size();
  if(visibleLightCount == 0) {
    removeMenuItem(menu,ID_LIGHT_HIDECONTROLS);
  }
  if(visibleLightCount == SCENE.getLightCount()) {
    removeMenuItem(menu, ID_LIGHT_SHOWCONTROLS);
  }
  const LightArray lights = SCENE.getAllLights();
  BitSet definedLights(SCENE.getMaxLightCount());
  for(D3Light l : lights) {
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
  removeMenuItem(menu, SCENE.getRightHanded()
                      ?ID_RIGHTHANDED
                      :ID_LEFTHANDED);

  if(!m_sceneContainer->canSplit3DWindow(sCAM.getHwnd())) {
    removeSubMenuContainingId(menu, ID_CAMERA_SPLITVERTICAL);
  }
  if(!m_sceneContainer->canDelete3DWindow(sCAM.getHwnd())) {
    removeMenuItem(menu, ID_CAMERA_REMOVE);
  }
  sCAM.modifyContextMenu(*menu.GetSubMenu(0));
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
  case SOTYPE_VISUALOBJECT     :
    removeSubMenuContainingId(menu, ID_OBJECT_STARTANIMATION   );
    break;

  case SOTYPE_ANIMATEDOBJECT:
    { D3SceneObjectAnimatedVisual *obj = getCurrentAnimatedObj();
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
  if(!getCurrentObj()->hasFillMode()) {
    removeSubMenuContainingId(menu, ID_OBJECT_FILLMODE_WIREFRAME);
  } else {
    switch(getCurrentObj()->getFillMode()) {
    case D3DFILL_SOLID      : removeMenuItem(menu, ID_OBJECT_FILLMODE_SOLID    ); break;
    case D3DFILL_WIREFRAME  : removeMenuItem(menu, ID_OBJECT_FILLMODE_WIREFRAME); break;
    case D3DFILL_POINT      : removeMenuItem(menu, ID_OBJECT_FILLMODE_POINT    ); break;
    }
  }
  if(!getCurrentObj()->hasShadeMode()) {
    removeSubMenuContainingId(menu, ID_OBJECT_SHADING_FLAT);
  } else {
    switch(getCurrentObj()->getShadeMode()) {
    case D3DSHADE_FLAT      : removeMenuItem(menu, ID_OBJECT_SHADING_FLAT    ); break;
    case D3DSHADE_GOURAUD   : removeMenuItem(menu, ID_OBJECT_SHADING_GOURAUD ); break;
    case D3DSHADE_PHONG     : removeMenuItem(menu, ID_OBJECT_SHADING_PHONG   ); break;
    }
  }
  if(!getCurrentObj()->hasNormals()) {
    removeMenuItem(menu, ID_OBJECT_SHOWNORMALS);
    removeMenuItem(menu, ID_OBJECT_HIDENORMALS);
  } else if(getCurrentObj()->isNormalsVisible()) {
    removeMenuItem(menu, ID_OBJECT_SHOWNORMALS);
  } else {
    removeMenuItem(menu, ID_OBJECT_HIDENORMALS);
  }
  if(!getCurrentObj()->hasMutableTexture()) {
    removeMenuItem(menu, ID_OBJECT_SELECTTEXTURE);
  }
  getCurrentObj()->modifyContextMenu(*menu.GetSubMenu(0));
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

void D3SceneEditor::OnControlCameraWalk()       { setControl(CONTROL_CAMERA_WALK      , NULL); }
void D3SceneEditor::OnControlCameraProjection() { setControl(CONTROL_CAMERA_PROJECTION, NULL); }
void D3SceneEditor::OnControlObjPos()           { setControl(CONTROL_OBJECT_POS       , NULL); }
void D3SceneEditor::OnControlObjScale()         { setControl(CONTROL_OBJECT_SCALE     , NULL); }

void D3SceneEditor::OnControlObjMoveRotate() {
  setControl(CONTROL_OBJECT_POS,getCurrentVisual());
}

void D3SceneEditor::OnObjectControlSpeed() {
  setControl(CONTROL_ANIMATION_SPEED, getCurrentAnimatedObj());
}

void D3SceneEditor::OnObjectCreateCube() {
  resetControl();
}

D3SceneEditor &D3SceneEditor::resetControl() {
  if(isSet(SE_RESETCONTROLACTIVE)) {
    return *this;
  }
  setFlag(SE_RESETCONTROLACTIVE);
  setControl(CONTROL_IDLE, NULL);
  m_pickedInfo.clear();
  clrFlag(SE_RESETCONTROLACTIVE);
  return *this;
}

bool D3SceneEditor::setControl(D3EditorControl control, D3SceneObjectVisual *visual) {
  if(isSet(SE_SETCONTROLACTIVE)) {
    return false;
  }
  if((control == m_currentControl) && (visual == m_currentObj)) {
    return false;
  }
  setFlag(SE_SETCONTROLACTIVE);
  m_currentControl = control;
  m_currentObj     = visual;
  switch(control) {
  case CONTROL_OBJECT_POS    :
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_HAND  ));
    break;
  case CONTROL_CAMERA_WALK   :
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  case CONTROL_SPOTLIGHTPOINT:
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_CROSS ));
    break;
  default                    :
    setWindowCursor(getCurrentHwnd(), MAKEINTRESOURCE(OCR_NORMAL));
    break;
  }

  D3LightControl *lc = getCurrentLightControl();
  if(m_propertyDialogMap.isDialogVisible()) {
    if(lc) {
      mapDialogShow(lc);
    } else if(visual && visual->hasMaterial()) {
      mapDialogShow(visual);
    } else {
      mapDialogHide();
    }
  }
  clrFlag(SE_SETCONTROLACTIVE);
  renderInfo();
  return true;
}

void D3SceneEditor::selectPropertyDialog(PropertyDialog *dlg, D3EditorControl control) {
  m_currentPropertyDialog = dlg;
  setControl(control, NULL);
}
void D3SceneEditor::unselectPropertyDialog() {
  m_currentPropertyDialog = NULL;
  setControl(CONTROL_IDLE, NULL);
}

void D3SceneEditor::mapDialogShow(D3LightControl *lc) {
  m_propertyDialogMap.showDialog(SP_LIGHTPARAMETERS, lc->getLight());
  setControl(CONTROL_LIGHTCOLOR, lc);
}
void D3SceneEditor::mapDialogShow(D3SceneObjectVisual *obj) {
  m_propertyDialogMap.showDialog(SP_MATERIALPARAMETERS, obj->getMaterial());
  setControl(CONTROL_MATERIAL, obj);
}
void D3SceneEditor::mapDialogHide() {
  m_propertyDialogMap.hideDialog();
}

void D3SceneEditor::OnObjectEditMaterial() {
  if(hasObj() && getCurrentObj()->hasMaterial()) {
    mapDialogShow(getCurrentObj());
  }
}

static void showText(const String &str) {
  const String fileName = _T("c:\\temp\\fisk.txt");
  FILE *f = MKFOPEN(fileName, _T("w"));
  _ftprintf(f, _T("%s"), str.cstr());
  fclose(f);
  ExternProcess::runNoWait(_T("c:\\windows\\system32\\notepad.exe"), fileName.cstr(), NULL);
}

void D3SceneEditor::OnObjectShowData() {
  if(hasObj() && getCurrentObj()->hasMesh()) {
    showText(::toString(getCurrentObj()->getMesh()));
  }
}

void D3SceneEditor::OnObjectShowNormals(bool show) {
  if(hasObj() && getCurrentObj()->hasNormals()) {
    if(show != getCurrentObj()->isNormalsVisible()) {
      getCurrentObj()->setNormalsVisible(show);
      renderActiveCameras(SC_RENDERALL);
    }
  }
}

void D3SceneEditor::OnObjectSelectTexture() {
  try {
    if(hasObj() && getCurrentObj()->hasMutableTexture()) {
      D3Device *device = getDevice();
      if(device) {
        const String fileName = selectAndValidateTextureFile(*device);
        if(fileName.length() == 0) {
          return;
        }
        LPDIRECT3DTEXTURE texture = device->loadTextureFromFile(fileName);
        D3Scene          *scene   = getScene();
        const UINT        id      = scene->addTexture(texture);
        getCurrentObj()->setTextureId(id);
        renderActiveCameras(SC_RENDERALL);
      }
    }
  } catch(Exception e) {
    showWarning(_T("Exception:%s"), e.what());
  }
}

void D3SceneEditor::OnCameraResetPosition()    { sCAM.resetPos();         }
void D3SceneEditor::OnCameraResetOrientation() { sCAM.resetOrientation(); }
void D3SceneEditor::OnCameraResetProjection()  { sCAM.resetProjection();  }
void D3SceneEditor::OnCameraResetAll()         { sCAM.resetAll();         }

void D3SceneEditor::SetRightHanded(bool rightHanded) {
  SCENE.setRightHanded(rightHanded);
}

typedef enum {
  ANIMATION_STOPPED
 ,ANIMATION_FORWARD
 ,ANIMATION_BACKWARDS
 ,ANIMATION_ALTERNATING
} AnimationState;

static void setCurrentAnimationState(D3SceneObjectAnimatedVisual *obj, AnimationState newState) {
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
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_FORWARD);
}

void D3SceneEditor::OnObjectStartBckAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_BACKWARDS);
}

void D3SceneEditor::OnObjectStartAltAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_ALTERNATING);
}

void D3SceneEditor::OnObjectReverseAnimation() {
  D3SceneObjectAnimatedVisual *obj = getCurrentAnimatedObj();
  if((obj == NULL) || !obj->isRunning()) return;
  switch(obj->getAnimationType()) {
  case ANIMATE_FORWARD    : setCurrentAnimationState(obj, ANIMATION_BACKWARDS); break;
  case ANIMATE_BACKWARD   : setCurrentAnimationState(obj, ANIMATION_FORWARD  ); break;
  }
}

void D3SceneEditor::OnObjectStopAnimation() {
  setCurrentAnimationState(getCurrentAnimatedObj(), ANIMATION_STOPPED);
}

void D3SceneEditor::OnMouseWheelAnimationSpeed(UINT nFlags, short zDelta, CPoint pt) {
  const float factor = (1.0f + 0.04f*signDelta);
  D3SceneObjectAnimatedVisual *obj = getCurrentAnimatedObj();
  if(obj && obj->isRunning()) {
    obj->scaleSpeed(factor);
    renderInfo();
  }
}

void D3SceneEditor::OnObjectRemove() {
  if(!hasObj()) return;
  D3SceneObjectVisual *obj = m_currentObj;
  SCENE.removeVisual(m_currentObj);
  SAFEDELETE(obj);
  renderActiveCameras(SC_RENDERALL);
}

void D3SceneEditor::OnObjectSetCenterOfRotation() {
  m_centerOfRotation.set(m_currentObj, m_pickedInfo.m_info.getMeshPoint());
  renderInfo();
}

void D3SceneEditor::OnObjectResetCenterOfRotation() {
  m_centerOfRotation.reset();
  renderInfo();
}

void D3SceneEditor::setLightEnabled(bool enabled) {
  SCENE.setLightEnabled(getCurrentLightControl()->getLightIndex(), enabled);
  renderActiveCameras(SC_RENDERALL);
}

void D3SceneEditor::OnLightAdjustColors() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  mapDialogShow(lc);
}

void D3SceneEditor::OnLightAdjustSpotAngles() {
  setControl(CONTROL_SPOTLIGHTANGLES, getCurrentLightControl());
}

void D3SceneEditor::OnLightControlSpotAt() {
  setControl(CONTROL_SPOTLIGHTPOINT, getCurrentLightControl());
}

void D3SceneEditor::OnLightControlHide() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  sCAM.setLightControlVisible(lc->getLightIndex(), false);
  resetControl().renderActiveCameras(SC_RENDERALL);
}

void D3SceneEditor::OnLightRemove() {
  D3LightControl *lc = getCurrentLightControl();
  if(lc == NULL) return;
  SCENE.removeLight(lc->getLightIndex());
  resetControl().renderActiveCameras(SC_RENDERALL);
}

void D3SceneEditor::OnSceneEditAmbientLight() {
  resetControl();
  const D3DCOLOR oldColor = SCENE.getAmbientColor();
  CColorDlg dlg(_T("Ambient color"), SP_AMBIENTCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  selectPropertyDialog(&dlg, CONTROL_AMBIENTLIGHTCOLOR);
  if(dlg.DoModal() != IDOK) {
    SCENE.setAmbientColor(oldColor);
    renderActiveCameras(SC_RENDERALL);
  }
  unselectPropertyDialog();
}

void D3SceneEditor::OnCameraEditBackgroundColor() {
  resetControl();
  const D3DCOLOR oldColor = sCAM.getBackgroundColor();
  CColorDlg dlg(_T("Background color"), CAM_BACKGROUNDCOLOR, oldColor);
  dlg.addPropertyChangeListener(this);
  selectPropertyDialog(&dlg, CONTROL_BACKGROUNDCOLOR);
  if(dlg.DoModal() != IDOK) {
    sCAM.setBackgroundColor(oldColor);
  }
  unselectPropertyDialog();
}

void D3SceneEditor::OnObjectFillmodePoint()     { getCurrentObj()->setFillMode(D3DFILL_POINT     ); renderActiveCameras(SC_RENDERALL); }
void D3SceneEditor::OnObjectFillmodeWireframe() { getCurrentObj()->setFillMode(D3DFILL_WIREFRAME ); renderActiveCameras(SC_RENDERALL); }
void D3SceneEditor::OnObjectFillmodeSolid()     { getCurrentObj()->setFillMode(D3DFILL_SOLID     ); renderActiveCameras(SC_RENDERALL); }
void D3SceneEditor::OnObjectShadingFlat()       { getCurrentObj()->setShadeMode(D3DSHADE_FLAT    ); renderActiveCameras(SC_RENDERALL); }
void D3SceneEditor::OnObjectShadingGouraud()    { getCurrentObj()->setShadeMode(D3DSHADE_GOURAUD ); renderActiveCameras(SC_RENDERALL); }
void D3SceneEditor::OnObjectShadingPhong()      { getCurrentObj()->setShadeMode(D3DSHADE_PHONG   ); renderActiveCameras(SC_RENDERALL); }

void D3SceneEditor::setCoordinateSystemVisible(bool visible) {
  if(visible) {
    if(m_coordinateSystem == NULL) {
      m_coordinateSystem = new D3SceneObjectCoordinateSystem(SCENE); TRACE_NEW(m_coordinateSystem);
      SCENE.addVisual(m_coordinateSystem);
    } else {
      m_coordinateSystem->setVisible(true);
    }
  } else {
    if(isCoordinateSystemVisible()) {
      m_coordinateSystem->setVisible(false);
    }
  }
  renderActiveCameras(SC_RENDER3D);
}

static const TCHAR *extensions = _T("Scene-files (*.scn)\0*.scn\0\0");

void D3SceneEditor::OnSceneSaveParameters() {
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
    SCENE.save(dlg.m_ofn.lpstrFile);
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch(Exception e) {
    showException(e);
  }
}

void D3SceneEditor::OnSceneLoadParameters() {
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
    SCENE.load(dlg.m_ofn.lpstrFile);
    renderActiveCameras(SC_RENDERALL);
    m_paramFileName = dlg.m_ofn.lpstrFile;
  } catch(Exception e) {
    showException(e);
  }
}
