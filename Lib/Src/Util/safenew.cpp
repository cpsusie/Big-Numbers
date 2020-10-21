#include "pch.h"
#include <new.h>

static _PNH old_handler = nullptr;

static int SafeNewHandler(size_t size) {
  throwException(_T("out of memory"));
  return 0;
}

class InstallSafeNewHandler {
public:
  InstallSafeNewHandler();
  ~InstallSafeNewHandler();
};

int UseSafeNew;
static InstallSafeNewHandler installer;

InstallSafeNewHandler::InstallSafeNewHandler() {
  old_handler = _set_new_handler( SafeNewHandler );
  _set_new_mode(1);

}

InstallSafeNewHandler::~InstallSafeNewHandler() {
  _set_new_handler(old_handler);
}
