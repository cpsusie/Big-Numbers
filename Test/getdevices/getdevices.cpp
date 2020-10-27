#include "stdafx.h"

int main(int argc, char **argv) {
  void **buffer = nullptr;
  DWORD needed;
  EnumDeviceDrivers(buffer,0,&needed);

  int count = needed / sizeof(void*);
  buffer = new void*[count];
  EnumDeviceDrivers(buffer,needed,&needed);
  _tprintf(_T("Devices drivers:\n"));
  for(int i = 0; i < count; i++) {
    TCHAR name[256];
    TCHAR fileName[256];
    if(GetDeviceDriverBaseName(buffer[i],name,ARRAYSIZE(name)) == 0) {
      _tprintf(_T("error:%s\n"),getLastErrorText().cstr());
    }
    GetDeviceDriverFileName(buffer[i],fileName, ARRAYSIZE(fileName));
    _tprintf(_T("device[%-3d]:%-20s file:%s\n"),i,name,fileName);
  }

  _tprintf(_T("Display devices:\n"));
  for(DWORD index = 0; ; index++) {
    DISPLAY_DEVICE device, monitor;
    device.cb  = sizeof(device);
    monitor.cb = sizeof(monitor);
    if(EnumDisplayDevices(nullptr,index,&device,0) == 0) {
      break;
    }
    if(EnumDisplayDevices(device.DeviceName, 0, &monitor, 0) == 0) {
      monitor.DeviceName[0] = 0;
    }

    _tprintf(_T("DisplayDevice:%s %s.\n"), device.DeviceName,device.DeviceString);
    if(_tcslen((TCHAR*)monitor.DeviceName) > 0) {
      _tprintf(_T("  Monitor name:%s\n"), monitor.DeviceName);
    }
    if(device.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
      _tprintf(_T("  Attached to desktop\n"));
    }
    if(device.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) {
      _tprintf(_T("  Mirroring driver\n"));
    }
    if(device.StateFlags & DISPLAY_DEVICE_MODESPRUNED) {
      _tprintf(_T("  Nodes pruned\n"));
    }
    if(device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) {
      _tprintf(_T("  Primary device\n"));
    }
    if(device.StateFlags & DISPLAY_DEVICE_REMOVABLE) {
      _tprintf(_T("  Removable\n"));
    }
    if(device.StateFlags & DISPLAY_DEVICE_VGA_COMPATIBLE) {
      _tprintf(_T("  VGA compatible\n"));
    }
  }
  return 0;
}
