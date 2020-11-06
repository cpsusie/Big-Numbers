#include "pch.h"
#include <Math.h>
#include <ByteFile.h>
#include <ByteMemoryStream.h>
#include <ThreadPool.h>
#include <Thread.h>
#include <InterruptableRunnable.h>
#include <MFCUtil/PixRect.h>
#include <MFCUtil/AnimatedImage.h>
#include <..\..\GifLib\giflib-5.1.1\lib\gif_lib.h>

#pragma warning(disable : 4244)

typedef enum {
  PLAYING
 ,STOPPED
 ,KILLED
} AnimatorState;

class Animator : private InterruptableRunnable {
private:
  AnimatedImage &m_owner;
  String         m_description;
  TimedSemaphore m_delaySem;
  Semaphore      m_animatorStopped;
  AnimatorState  m_state;
  CPoint         m_point;
  UINT           m_frameIndex;
  void waitUntilStopped() {
    m_animatorStopped.wait();
  }
  void setState(AnimatorState newState);
public:
  Animator(AnimatedImage *owner, const String &description = "Animator");
  inline AnimatorState getState() const {
    return m_state;
  }
  void startAnimation(const CPoint &p);
  void stopAnimation();
  void kill();
  UINT safeRun();
};

Animator::Animator(AnimatedImage *owner, const String &description)
: m_owner(*owner)
, m_description(description)
, m_delaySem(0)
, m_point(-1,-1)
, m_frameIndex(0)
{
  setSuspended();
  m_state = PLAYING;
  m_animatorStopped.wait();
  ThreadPool::executeNoWait(*this);
}

void Animator::setState(AnimatorState newState) {
  if(newState == m_state) {
    return;
  }
  switch(getState()) {
  case STOPPED:
    switch(newState) {
    case PLAYING:
    case KILLED:
      m_state = newState;
      break;
    }
    break;
  case PLAYING:
    switch(newState) {
    case STOPPED:
      m_state = newState;
      m_animatorStopped.notify();
      suspend();
      break;
    case KILLED:
      m_state = newState;
      m_animatorStopped.notify();
      break;
    }
    break;
  case KILLED:
    switch(newState) {
    case STOPPED:
      throwInvalidArgumentException(__TFUNCTION__, _T("m_state=KILLED,newState=STOPPED"));
      break;
    case PLAYING:
      throwInvalidArgumentException(__TFUNCTION__, _T("m_state=KILLED,newState=PLAYING"));
      break;
    }
    break;
  }
}

void Animator::startAnimation(const CPoint &p) {
  switch(getState()) {
  case STOPPED  :
    m_point = p;
    resume();
  }
}

void Animator::stopAnimation() {
  switch(getState()) {
  case PLAYING:
    setSuspended();
    waitUntilStopped();
    break;
  }
}

void Animator::kill() {
  switch(getState()) {
  case PLAYING:
  case STOPPED  :
    setInterrupted();
    waitUntilJobDone();
    break;
  }
}

UINT Animator::safeRun() {
  SETTHREADDESCRIPTION(m_description);
  for(;;) {
    const UINT imageCount = m_owner.getFrameCount();
    for(m_frameIndex = 0; !isInterruptedOrSuspended(); m_frameIndex = (m_frameIndex + 1) % imageCount) {
      setState(PLAYING);
      const GifFrame &frame = m_owner.getFrame(m_frameIndex);
      frame.paint();
      m_owner.flushWork(m_point);
      m_delaySem.wait(frame.m_delayTime);
      frame.dispose();
    }
    if(isInterrupted()) {
      break;
    }
    setState(STOPPED);
  }
  setState(KILLED);
  return 0;
}

#define THROWGIFERROR(code) throwGifErrorCode(code, __TFILE__, __LINE__)

String gifErrorCodeToString(int errorCode) {
  const char *errorMsg = GifErrorString(errorCode);
  if(errorMsg == nullptr) {
    return _T("No error");
  } else {
    return errorMsg;
  }
}

void throwGifErrorCode(int errorCode, const TCHAR *fileName, int line) {
  throwException(_T("Gif error in %s line %d: error=%s")
                ,fileName, line
                ,gifErrorCodeToString(errorCode).cstr()
                );
}

AnimatedImage::AnimatedImage() {
  m_size              = CSize(0,0);
  m_parent            = nullptr;
  m_workPr            = nullptr;
  m_background        = nullptr;
  m_lastPaintedFrame  = nullptr;
  m_animator          = new Animator(this); TRACE_NEW(m_animator);
  ThreadPool::addListener(this);
}

AnimatedImage::~AnimatedImage() {
  ThreadPool::removeListener(this);
  m_animator->kill();
  unload();
  SAFEDELETE(m_animator);
}

void AnimatedImage::handlePropertyChanged(const PropertyContainer *source, int id, const void *oldValue, const void *newValue) {
  if(ThreadPool::isPropertyContainer(source)) {
    switch(id) {
    case THREADPOOL_SHUTTINGDDOWN:
      m_animator->kill();
      break;
    }
  }
}

void AnimatedImage::load(CWnd *parent, const String &fileName) {
  load(parent, ByteInputFile(fileName));
}

void AnimatedImage::loadFromResource(CWnd *parent, int resId, const String &typeName) {
  load(parent, ByteMemoryInputStream(ByteArray().loadFromResource(resId, typeName.cstr())));
}

void AnimatedImage::createFromGifFile(CWnd *parent, const GifFileType *gifFile) {
  unload();
  m_parent = parent;
  m_device.attach(*m_parent);
  extractGifData(gifFile);
}

static int readGifStreamFunction(GifFileType *gifFile, GifByteType *buffer, int n) {
  return ((ByteInputStream*)gifFile->UserData)->getBytes((BYTE*)buffer, n);
}

void AnimatedImage::load(CWnd *parent, ByteInputStream &in) {
  int error;
  GifFileType *gifFile = DGifOpen(&in, readGifStreamFunction, &error);
  if(gifFile == nullptr) {
    THROWGIFERROR(error);
  }
  try {
    if(DGifSlurp(gifFile) != GIF_OK) {
      THROWGIFERROR(gifFile->Error);
    }
    createFromGifFile(parent, gifFile);
    DGifCloseFile(gifFile, &error);
  } catch(...) {
    DGifCloseFile(gifFile, &error);
    throw;
  }
}

void AnimatedImage::extractGifData(const GifFileType *gifFile) {
  PixelAccessor *pa    = nullptr;
  GifFrame      *frame = nullptr;
  try {
    m_size.cx = gifFile->SWidth;
    m_size.cy = gifFile->SHeight;
    m_workPr  = new PixRect(m_device, PIXRECT_TEXTURE, m_size); TRACE_NEW(m_workPr);

    const int           imageCount   = gifFile->ImageCount;
    ColorMapObject     *gfColorMap   = gifFile->SColorMap;
    const GifColorType &colorType    = gfColorMap->Colors[gifFile->SBackGroundColor];
    const int           bitsPerPixel = gfColorMap->BitsPerPixel;

    m_backgroundColor      = D3DCOLOR_RGBA(colorType.Red, colorType.Green, colorType.Blue,255);

    for(int k = 0; k < gifFile->ExtensionBlockCount; k++) {
      const ExtensionBlock &block = gifFile->ExtensionBlocks[k];
      switch(block.Function) {
      case COMMENT_EXT_FUNC_CODE:
        m_comment += format(_T("%*.*s"), block.ByteCount, block.ByteCount, block.Bytes);
        break;
      case APPLICATION_EXT_FUNC_CODE:
        parseApplicationBlock(block.Bytes, block.ByteCount);
        break;
      }
    }
    m_frameTable.setCapacity(imageCount);
    for(int i = 0; i < imageCount; i++) {
      SavedImage &image = gifFile->SavedImages[i];
      m_frameTable.add(GifFrame());
      frame = &m_frameTable.last();

      String imageComment;
      for(int k = 0; k < image.ExtensionBlockCount; k++) {
        const ExtensionBlock &block = image.ExtensionBlocks[k];
        switch(block.Function) {
        case COMMENT_EXT_FUNC_CODE:
          imageComment += format(_T("%*.*s"), block.ByteCount, block.ByteCount, block.Bytes);
          break;

        case APPLICATION_EXT_FUNC_CODE:
          parseApplicationBlock(block.Bytes, block.ByteCount);
          break;
        }
      }
      if(imageComment.length() > 0) {
        m_comment += _T("\n") + imageComment;
      }

      frame->m_owner        = this;
      frame->m_rect.left    = image.ImageDesc.Left;
      frame->m_rect.top     = image.ImageDesc.Top;
      frame->m_rect.right   = image.ImageDesc.Left + image.ImageDesc.Width;
      frame->m_rect.bottom  = image.ImageDesc.Top  + image.ImageDesc.Height;
      frame->m_srcRect      = CRect(0,0, image.ImageDesc.Width, image.ImageDesc.Height);

      int transparentColor = NO_TRANSPARENT_COLOR;

      GraphicsControlBlock gcb;
      if(DGifSavedExtensionToGCB((GifFileType*)gifFile, i, &gcb) == GIF_OK) {
        frame->m_disposalMode = gcb.DisposalMode;
        frame->m_delayTime    = max(60,10*gcb.DelayTime);
        if((transparentColor = gcb.TransparentColor) != NO_TRANSPARENT_COLOR) {
          frame->m_useTransparency = true;
        }
      }

      const CSize sz = frame->m_rect.Size();
      frame->m_pr = new PixRect(m_device, PIXRECT_TEXTURE, sz, D3DPOOL_SYSTEMMEM, D3DFMT_A8R8G8B8); TRACE_NEW(frame->m_pr);

      const ColorMapObject *colorMap = image.ImageDesc.ColorMap ? image.ImageDesc.ColorMap : gfColorMap;
      const GifColorType   *colors   = colorMap->Colors;
      const GifPixelType   *pixelp   = image.RasterBits;

      pa = frame->m_pr->getPixelAccessor(0 /*D3DLOCK_NOSYSLOCK | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOOVERWRITE*/);
      for(CPoint p(0,0); p.y < sz.cy; p.y++) {
        for(p.x = 0; p.x < sz.cx; p.x++) {
          const int entry = *(pixelp++);
          if((entry == transparentColor) && frame->m_useTransparency) {
            pa->setPixel(p, D3DCOLOR_RGBA(0,0,0,0));
          } else {
            const GifColorType &mapEntry = colors[entry];
            pa->setPixel(p, D3DCOLOR_RGBA(mapEntry.Red, mapEntry.Green, mapEntry.Blue, 255));
          }
        }
      }
      frame->m_pr->releasePixelAccessor();
      pa = nullptr;
//      frame.m_pr->moveToPool(D3DPOOL_DEFAULT);
    }
  } catch(...) {
    if(pa) {
      frame->m_pr->releasePixelAccessor();
    }
    unload();
    throw;
  }
}

void AnimatedImage::parseApplicationBlock(const unsigned char *bytes, int n) {
/*
  int loopCount = 0;
  if(memcmp(bytes, "NETSCAPE2.0", 11) == 0) {
    char bytes1[100];
    memset(bytes1, 0, sizeof(bytes1));
    n = min(n, sizeof(bytes1));
    memcpy(bytes1, bytes, n);
  }
*/
}

bool AnimatedImage::hasSavedBackground() const {
  return (m_background != nullptr) && (m_parent != nullptr);
}

void AnimatedImage::saveBackground(const CPoint &p, const CSize *size) {
  releaseBackground();
  const CSize sz = size ? *size : m_size;
  m_background = new PixRect(m_device, PIXRECT_PLAINSURFACE, sz); TRACE_NEW(m_background);
  CClientDC dc(m_parent);
  PixRect::bitBlt(m_background, ORIGIN, sz, SRCCOPY, dc, p);
  m_savedPosition = p;
  m_workPr->rop(ORIGIN, m_size, SRCCOPY, m_background, ORIGIN);
}

void AnimatedImage::restoreBackground() {
  if(hasSavedBackground()) {
    m_workPr->rop(ORIGIN, m_size, SRCCOPY, m_background, ORIGIN);
    flushPr(m_savedPosition, m_background, 1.0);
  }
  releaseBackground();
}

void AnimatedImage::releaseBackground() {
  SAFEDELETE(m_background);
}

void AnimatedImage::flushPr(const CPoint &p, PixRect *src, double scaleFactor) const {
  flushPr(CClientDC(m_parent), p, src, scaleFactor);
}

void AnimatedImage::flushPr(CDC &dc, const CPoint &p, PixRect *src, double scaleFactor) const {
  if(scaleFactor == 1.0) {
    PixRect::bitBlt(dc, p, src->getSize(), SRCCOPY, src, ORIGIN);
  } else {
    const CSize sz = src->getSize();
    const CSize dstSize(scaleFactor*sz.cx, scaleFactor*sz.cy);
    dc.SetStretchBltMode(COLORONCOLOR /*HALFTONE*/);
    PixRect::stretchBlt(dc, p,dstSize, SRCCOPY, src, ORIGIN, sz);
  }
}

void AnimatedImage::unload() {
  hide();
  for(size_t i = 0; i < m_frameTable.size(); i++) {
    m_frameTable[i].cleanup();
  }
  m_frameTable.clear();
  clearDisposeTable();
  m_size = CSize(0,0);
  m_comment = EMPTYSTRING;
  SAFEDELETE(m_workPr);
  SAFEDELETE(m_background);
}

void AnimatedImage::addToDisposeTable(PixRect *pr) {
  m_disposeTable.add(pr);
  if(isDisposeTableFull()) {
    const int n = getFrameCount();
    for(int i = 0; i < n; i++) {
      PixRect *pr = m_disposeTable[i];
      if(pr) {
        bool used = false;
        for(int k = i+1; k < n; k++) {
          if(m_frameTable[k].getDisposeTo() == pr) {
            used = true;
            break;
          }
        }
        if(!used) {
          SAFEDELETE(pr);
          m_disposeTable[i] = nullptr;
        }
      }
    }
  }
}

const PixRect *AnimatedImage::findLastNonDisposed() const {
  for(int i = (int)m_disposeTable.size()-1; i >= 0; i--) {
    if(m_disposeTable[i] != nullptr) {
      return m_disposeTable[i];
    }
  }
  return nullptr;
}

void AnimatedImage::clearDisposeTable() {
  for(size_t i = 0; i < m_disposeTable.size(); i++) {
    PixRect *pr = m_disposeTable[i];
    SAFEDELETE(pr);
  }
  m_disposeTable.clear();
}

void AnimatedImage::paintFrames(const CPoint &p, UINT last) {
  if(!isLoaded() || isPlaying()) {
    return;
  }
  if(last >= (UINT)m_frameTable.size()) {
    last = (int)m_frameTable.size()-1;
  }
  if(hasSavedBackground() && (m_background->getSize() != m_size)) {
    hide();
  }
  if(!hasSavedBackground()) {
    saveBackground(p);
  }
  if(m_lastPaintedFrame != nullptr) {
    m_lastPaintedFrame->dispose();
  }
  for(UINT i = 0; i <= last; i++) {
    const GifFrame &frame = m_frameTable[i];
    frame.paint();
    if(i < last) {
      frame.dispose();
    }
  }
}

void AnimatedImage::paintWork(CDC &dc, const CPoint &p) {
  if(!isLoaded() || isPlaying() || (m_workPr == nullptr)) {
    return;
  }
  flushPr(dc, p, m_workPr, 1.0);
}

void AnimatedImage::paintAllFrames(const CRect &r) {
  if(!isLoaded()) {
    return;
  }
  hide();
  const CSize rSize         = r.Size();
  const CSize prSize        = getSize();
  const int   n             = getFrameCount();
  int         availableArea = getArea(rSize);

  double       scaleFactor;
  CSize        newSize;
  int          fpl;
  UINT         maxArea = 0;

//  m_comment = format("imageCount:%2d  rSize:(%3d,%3d)\r\n", n, rSize.cx, rSize.cy);

  for(int framesPerLine = n; framesPerLine >= 1; framesPerLine--) {
    const int lineCount      = (n-1) / framesPerLine + 1;
    const int lastFrameCount = n%framesPerLine;
    const int fullLines      = lineCount - (lastFrameCount?1:0);

    assert((fullLines >= 1) && (lastFrameCount >= 0) && (fullLines * framesPerLine + lastFrameCount == n));

    const double sfx = min(1.0, (double)(rSize.cx-1) / ((prSize.cx+1) * framesPerLine));
    double       sfy = min(1.0, (double)(rSize.cy-1) / ((prSize.cy+1) * lineCount    ));
    double       sf  = min(sfx, sfy);

    const CSize  psz  = CSize(floor(sf * prSize.cx) + 1, floor(sf * prSize.cy) + 1);
    const UINT   area = getArea(psz);;
    const CSize dstSz(framesPerLine * psz.cx-1, lineCount * psz.cy-1);
/*
    const CSize rightWaste( rSize.cx - dstSz.cx             , fullLines * psz.cy  );
    const CSize bottomWaste(rSize.cx                        , rSize.cy - dstSz.cy );
    const int   totalWaste = getArea(rightWaste) + getArea(bottomWaste);
*/
    if((dstSz.cx <= rSize.cx) && (dstSz.cy <= rSize.cy) && (area > maxArea)) {
      maxArea     = area;
      newSize     = psz;
      scaleFactor = sf;
      fpl         = framesPerLine;
    }
/*
    m_comment += format("f/l %2d r:%3d,%3d) a %6s, waste %8s (fx,fy,f):(%.3lf,%.3lf,%.6lf) sz:(%3d,%3d)\r\n"
                       , framesPerLine
                       , dstSz.cx, dstSz.cy
                       , format1000(area).cstr()
                       , format1000(totalWaste).cstr()
                       , sfx,sfy,sf
                       , psz.cx,psz.cy
                       );
*/
  }

  CPoint p = r.TopLeft();
  saveBackground(p, &rSize);
  for(int i = 0, lc = 0; i < n; i++) {
    const GifFrame &frame = m_frameTable[i];
    if(lc == fpl) {
      p.x = r.left;
      p.y += newSize.cy;
      lc = 0;
    }
    frame.paint();
    flushPr(p,  m_workPr, scaleFactor);
    frame.dispose();
    p.x += newSize.cx;
    lc++;
  }
}

void AnimatedImage::startAnimation(const CPoint &p) {
  if(!isLoaded()) {
    throwException(_T("No images loaded"));
  }
  hide();
  saveBackground(p);
  m_animator->startAnimation(p);
}

void AnimatedImage::stopAnimation() {
  m_animator->stopAnimation();
}

bool AnimatedImage::isPlaying() const {
  return m_animator->getState() == PLAYING;
}

void AnimatedImage::hide() {
  stopAnimation();
  restoreBackground();
  m_lastPaintedFrame = nullptr;
}

void GifFrame::paint() const {
  if(m_useTransparency) {
    PixRect::alphaBlend(*m_owner->m_workPr, m_rect, *m_pr, m_srcRect, 255);
  } else {
    m_owner->m_workPr->rop(m_rect, SRCCOPY, m_pr, ORIGIN);
  }
  m_owner->m_lastPaintedFrame = this;
}

void GifFrame::dispose() const {
  m_owner->m_lastPaintedFrame = nullptr;

  switch(m_disposalMode) {
  case DISPOSAL_UNSPECIFIED:       // No disposal specified
  case DISPOSE_DO_NOT      :       // Leave image in place
    if(!m_owner->isDisposeTableFull()) {
      m_owner->addToDisposeTable(m_owner->m_workPr->clone(true));
    }
    return;
  case DISPOSE_BACKGROUND  :       // Set area to background color
    { if(!m_owner->isDisposeTableFull()) {
        m_owner->addToDisposeTable(nullptr);
      }
      if(m_owner->hasSavedBackground()) {
        m_owner->m_workPr->rop(ORIGIN,m_owner->m_size,SRCCOPY, m_owner->m_background, ORIGIN);
      }
    }
    break;

  case DISPOSE_PREVIOUS    :       // Restore to previous non-disposd content content
    { if(!m_owner->isDisposeTableFull()) {
        m_owner->addToDisposeTable(nullptr);
      }
      if(m_disposeTo == nullptr) {
        m_disposeTo = m_owner->findLastNonDisposed();
      }
      if(m_disposeTo) {
        m_owner->m_workPr->rop(ORIGIN,m_owner->m_size,SRCCOPY, m_disposeTo, ORIGIN);
      }
    }
    break;
  }
//  flush(wnd, p);
}

GifFrame::GifFrame() {
  m_disposeTo        = nullptr;
  m_owner            = nullptr;
  m_disposalMode     = DISPOSAL_UNSPECIFIED;
  m_delayTime        = 0;
  m_useTransparency  = false;
  m_srcRect = m_rect = CRect(0,0,0,0);
  m_pr               = nullptr;
}

void GifFrame::cleanup() {
  m_disposeTo        = nullptr;
  m_owner            = nullptr;
  m_disposalMode     = DISPOSAL_UNSPECIFIED;
  m_delayTime        = 0;
  m_useTransparency  = false;
  m_srcRect = m_rect = CRect(0,0,0,0);
  SAFEDELETE(m_pr);
}
