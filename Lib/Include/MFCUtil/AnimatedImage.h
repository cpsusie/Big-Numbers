#pragma once

#include <PropertyContainer.h>
#include <ByteStream.h>
#include "PixRect.h"

class AnimatedImage;

class GifFrame {
private:
  mutable const PixRect *m_disposeTo;
public:
  AnimatedImage *m_owner;
  int            m_disposalMode;
  int            m_delayTime;           // milliseconds
  bool           m_useTransparency;
  CRect          m_rect, m_srcRect;
  PixRect       *m_pr;
  GifFrame();
  void cleanup();
  void paint() const;                   // paint image to m_owner->m_workPr
  void dispose() const;
  const PixRect *getDisposeTo() const {
    return m_disposeTo;
  }
};

struct GifFileType;
class Animator;
class AnimatedImage : private PropertyChangeListener {
  friend class Animator;
  friend class GifFrame;
private:
  Animator              *m_animator;
  CSize                  m_size;
  D3DCOLOR               m_backgroundColor;
  CompactArray<GifFrame> m_frameTable;
  CompactArray<PixRect*> m_disposeTable;
  CWnd                  *m_parent;
  PixRectDevice          m_device;
  PixRect               *m_workPr, *m_background;
  const GifFrame        *m_lastPaintedFrame;
  CPoint                 m_savedPosition;
  String                 m_comment;
  void throwGifError(int errorCode);
  void parseApplicationBlock(const unsigned char *bytes, int n);
  void extractGifData(const GifFileType *gifFile);                           // gifFile actually GifFileType
  void releaseBackground();
  void saveBackground(const CPoint &p, const CSize *size = NULL); // if size=NULL, then m_size
  void restoreBackground();
  bool hasSavedBackground() const;
  inline void flushWork(const CPoint &p) const {
    flushPr(p, m_workPr, 1.0);
  }
  void flushPr(         const CPoint &p, PixRect *src, double scaleFactor) const; // flushPr(CClientDC(m_wnd),...)
  void flushPr(CDC &dc, const CPoint &p, PixRect *src, double scaleFactor) const;
  void addToDisposeTable(PixRect *pr);
  const PixRect *findLastNonDisposed() const;
  void clearDisposeTable();
  inline bool isDisposeTableFull() const {
    return m_disposeTable.size() == getFrameCount();
  }
  void handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue);
public:
  AnimatedImage();
  virtual ~AnimatedImage();
  void load(CWnd *parent, const String &fileName);
  void loadFromResource(CWnd *parent, int resId, const String &typeName);
  void load(CWnd *parent, ByteInputStream &in);
  void createFromGifFile(CWnd *parent, const GifFileType *gifFile);
  void unload();
  void startAnimation(const CPoint &p);
  void stopAnimation();
  void paintFrames(   const CPoint &p, UINT last);
  // do internal paint of frame 0..last on m_workPr (without delays) so we can call paintWork and show the specified frame
  void paintWork(     CDC  &dc,  const CPoint &p);
  void paintAllFrames(const CRect  &r);
  void hide();
  inline int getFrameCount() const {
    return (int)m_frameTable.size();
  }
  inline const GifFrame &getFrame(int index) const {
    return m_frameTable[index];
  }
  inline const CSize &getSize() const {
    return m_size;
  }
  inline bool isLoaded() const {
    return !m_frameTable.isEmpty();
  }
  inline const String &getComment() const {
    return m_comment;
  }
  bool isPlaying() const;
};
