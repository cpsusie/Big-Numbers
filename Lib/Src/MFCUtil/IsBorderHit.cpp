#include "stdafx.h"
#include <MFCUtil/WinTools.h>

bool isBorderHit(UINT hitTestCode) {
  switch(hitTestCode) {
  case HTBOTTOM      :
  case HTBOTTOMLEFT  :
  case HTBOTTOMRIGHT :
  case HTLEFT        :
  case HTRIGHT       :
  case HTTOP         :
  case HTTOPLEFT     :
  case HTTOPRIGHT    :
    return true;
  default            :
    return false;
  }
}

