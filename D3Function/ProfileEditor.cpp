#include "stdafx.h"
#include "ProfileEditor.h"

const TCHAR *stateToString(ProfileEditorState state) {
  switch(state) {
  case IDLE       : return _T("idle"         );
  case DRAGGING   : return _T("dragging"     );
  case MOVING     : return _T("moving"       );
  case STRETCHING : return _T("stretching"   );
  case ROTATING   : return _T("rotating"     );
  default         : return _T("unknown state");
  }
}
