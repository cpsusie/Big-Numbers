#pragma once

#include <Thread.h>
#include <ByteStream.h>
#include "Wintools.h"
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

class AnimationThread : public Thread {
private:
  AnimatedImage *m_owner;
  CPoint         m_point;
  int            m_frameIndex;
  Semaphore      m_delaySem;
  bool           m_running    : 1;
  bool           m_killed     : 1;
  bool           m_stopSignal : 1;
  void stopLoop();
public:
  unsigned int run();
  AnimationThread();
  void startAnimation(AnimatedImage *image, const CPoint &p);
  void stopAnimation();
  void kill();
  inline bool isRunning() const {
    return m_running;
  }
};

struct GifFileType;

class AnimatedImage {
private:
  AnimationThread        m_animator;
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
  friend class AnimationThread;
  friend class GifFrame;

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
  void paintFrames(   const CPoint &p, unsigned int last);
  // do internal paint of frame 0..last on m_workPr (without delays) so we can call paintWork and show the specified frame
  void paintWork(     CDC  &dc,  const CPoint &p);
  void paintAllFrames(const CRect  &r);
  void hide();
  inline int getFrameCount() const {
    return m_frameTable.size();
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
  const String &getComment() const {
    return m_comment;
  }
  inline bool isPlaying() const {
    return m_animator.isRunning();
  }
};
