#pragma once

#include <MFCUtil/Viewport2D.h>
#include <D3DGraphics/Profile2D.h>

typedef enum {
  NORMALS_INVISIBLE
 ,NORMALS_FLAT
 ,NORMALS_SMOOTH
} NormalsMode;

typedef enum {
  IDLE
 ,DRAGGING
 ,MOVING
 ,STRETCHING
 ,ROTATING
} ProfileEditorState;

const TCHAR *stateToString(ProfileEditorState state);

class ProfileEditor {
public:
  virtual Viewport2D &getViewport()      = 0;
  virtual Profile2D  &getProfile()       = 0;
  virtual void        repaintViewport()  = 0;
  virtual NormalsMode getNormalsMode()   = 0;
  virtual bool        getShowPoints()    = 0;
  virtual void        setMousePosition(const Point2D &p) = 0;
  virtual HWND        getWindow()        = 0;
};
