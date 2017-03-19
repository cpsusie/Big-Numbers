#pragma once

#include <PropertyContainer.h>
#include <MFCUtil/GifCtrl.h>

typedef enum {
  GIF_LOADSTATUS
 ,GIF_PLAYSTATUS
 ,GIF_SHOWALLSTATUS
 ,GIF_VISIBLESTATUS
} GifCtrlProperty;


class CGifCtrlWithProperties : public CGifCtrl, public PropertyContainer {
private:
  int getStatusFlags() const;
  bool isVisible() const;;
  void notifyStatusChanged(int oldFlags);
  void notifyIfFlagChange(int oldFlags, int newFlags, int mask, GifCtrlProperty propertyId);
public:
  void load(const String &fileName);
  void loadFromGifFile(const GifFileType *gifFile);
  void unload();
  void play(bool force = false);
  void stop();
  void paintFrame(unsigned int index);
  void paintAllFrames();
  void hide();
};

