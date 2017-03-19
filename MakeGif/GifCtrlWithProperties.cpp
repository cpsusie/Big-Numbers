#include "stdafx.h"
#include "GifCtrlWithProperties.h"

#define FLAG_VISIBLE 0x01
#define FLAG_LOADED  0x02
#define FLAG_PLAYING 0x04
#define FLAG_SHOWALL 0x08

bool CGifCtrlWithProperties::isVisible() const {
  return isSuspended() || isPlaying() || isPainted();
}

int CGifCtrlWithProperties::getStatusFlags() const {
  return (isVisible() ? FLAG_VISIBLE : 0) 
       | (isLoaded()  ? FLAG_LOADED  : 0) 
       | (isPlaying() ? FLAG_PLAYING : 0)
       | (isPainted() ? FLAG_SHOWALL : 0)
       ;
}

void CGifCtrlWithProperties::load(const String &fileName) {
  const int oldFlags = getStatusFlags();
  CGifCtrl::load(fileName);
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::notifyStatusChanged(int oldFlags) {
  const int newFlags = getStatusFlags();
  const int diff = oldFlags ^ newFlags;
  if(diff) {
    notifyIfFlagChange(oldFlags, newFlags, FLAG_LOADED , GIF_LOADSTATUS   );
    notifyIfFlagChange(oldFlags, newFlags, FLAG_PLAYING, GIF_PLAYSTATUS   );
    notifyIfFlagChange(oldFlags, newFlags, FLAG_SHOWALL, GIF_SHOWALLSTATUS);
    notifyIfFlagChange(oldFlags, newFlags, FLAG_VISIBLE, GIF_VISIBLESTATUS);
  }
}

void CGifCtrlWithProperties::notifyIfFlagChange(int oldFlags, int newFlags, int mask, GifCtrlProperty propertyId) {
  bool oldValue = (oldFlags & mask) ? true : false;
  bool newValue = (newFlags & mask) ? true : false;
  if(newValue != oldValue) {
    notifyPropertyChanged(propertyId, &oldValue, &newValue);
  }
}

void CGifCtrlWithProperties::loadFromGifFile(const GifFileType *gifFile) {
  const int oldFlags = getStatusFlags();
  CGifCtrl::loadFromGifFile(gifFile);
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::unload() {
  const int oldFlags = getStatusFlags();
  CGifCtrl::unload();
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::play(bool force) {
  const int oldFlags = getStatusFlags();
  CGifCtrl::play(force);
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::stop() {
  const int oldFlags = getStatusFlags();
  CGifCtrl::stop();
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::paintAllFrames() {
  const int oldFlags = getStatusFlags();
  CGifCtrl::paintAllFrames();
  notifyStatusChanged(oldFlags);
}

void CGifCtrlWithProperties::hide() {
  const int oldFlags = getStatusFlags();
  CGifCtrl::hide();
  notifyStatusChanged(oldFlags);
}

