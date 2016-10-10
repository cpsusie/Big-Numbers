#include "stdafx.h"
#include <MyUtil.h>
#include <Math.h>
#include <ByteFile.h>
#include <ByteMemoryStream.h>
#include "MFCUtil/AnimatedImage.h"
#include <..\..\GifLib\giflib-5.1.1\lib\gif_lib.h>

#pragma warning(disable : 4244)

void checkGifErrorCode(int errorCode, const TCHAR *fileName, int line);
#define THROWGIFERROR(code) throwGifErrorCode(code, _T(__FILE__), __LINE__)

String gifErrorCodeToString(int errorCode) {
  const char *errorMsg = GifErrorString(errorCode);
  if(errorMsg == NULL) {
    return "No error";
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
  m_parent            = NULL;
  m_workPr            = NULL;
  m_background        = NULL;
  m_lastPaintedFrame  = NULL;
}

AnimatedImage::~AnimatedImage() {
  unload();
  m_animator.kill();
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
  if(gifFile == NULL) {
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
  PixelAccessor *pa = NULL;

  try {
    m_size.cx = gifFile->SWidth;
    m_size.cy = gifFile->SHeight;
    m_workPr  = new PixRect(m_device, PIXRECT_TEXTURE, m_size);

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
      GifFrame &frame = m_frameTable.last();

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

      frame.m_owner        = this;
      frame.m_rect.left    = image.ImageDesc.Left;
      frame.m_rect.top     = image.ImageDesc.Top;
      frame.m_rect.right   = image.ImageDesc.Left + image.ImageDesc.Width;
      frame.m_rect.bottom  = image.ImageDesc.Top  + image.ImageDesc.Height;
      frame.m_srcRect      = CRect(0,0, image.ImageDesc.Width, image.ImageDesc.Height);

      int transparentColor = NO_TRANSPARENT_COLOR;

      GraphicsControlBlock gcb;
      if(DGifSavedExtensionToGCB((GifFileType*)gifFile, i, &gcb) == GIF_OK) {
        frame.m_disposalMode = gcb.DisposalMode;
        frame.m_delayTime    = max(60,10*gcb.DelayTime);
        if((transparentColor = gcb.TransparentColor) != NO_TRANSPARENT_COLOR) {
          frame.m_useTransparency = true;
        }
      }

      const CSize sz = frame.m_rect.Size();
      frame.m_pr = new PixRect(m_device, PIXRECT_TEXTURE, sz, D3DPOOL_SYSTEMMEM, D3DFMT_A8R8G8B8);

      const ColorMapObject *colorMap = image.ImageDesc.ColorMap ? image.ImageDesc.ColorMap : gfColorMap;
      const GifColorType   *colors   = colorMap->Colors;
      const GifPixelType   *pixelp   = image.RasterBits;

      pa = frame.m_pr->getPixelAccessor(0 /*D3DLOCK_NOSYSLOCK | D3DLOCK_NO_DIRTY_UPDATE | D3DLOCK_NOOVERWRITE*/);
      for(CPoint p(0,0); p.y < sz.cy; p.y++) {
        for(p.x = 0; p.x < sz.cx; p.x++) {
          const int entry = *(pixelp++);
          if((entry == transparentColor) && frame.m_useTransparency) {
            pa->setPixel(p, D3DCOLOR_RGBA(0,0,0,0));
          } else {
            const GifColorType &mapEntry = colors[entry];
            pa->setPixel(p, D3DCOLOR_RGBA(mapEntry.Red, mapEntry.Green, mapEntry.Blue, 255));
          }
        }
      }
      delete pa;
      pa = NULL;
//      frame.m_pr->moveToPool(D3DPOOL_DEFAULT);
    }
  } catch(...) {
    if(pa) {
      delete pa;
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
  return (m_background != NULL) && (m_parent != NULL);
}

void AnimatedImage::saveBackground(const CPoint &p, const CSize *size) {
  releaseBackground();
  const CSize sz = size ? *size : m_size;
  m_background = new PixRect(m_device, PIXRECT_PLAINSURFACE, sz);
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
  if(m_background) {
    delete m_background;
    m_background = NULL;
  }
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
  m_comment = "";
  if(m_workPr) {
    delete m_workPr;
    m_workPr = NULL;
  }
  if(m_background) {
    delete m_background;
    m_background = NULL;
  }
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
          delete pr;
          m_disposeTable[i] = NULL;
        }
      }
    }
  }
}

const PixRect *AnimatedImage::findLastNonDisposed() const {
  for(int i = (int)m_disposeTable.size()-1; i >= 0; i--) {
    if(m_disposeTable[i] != NULL) {
      return m_disposeTable[i];
    }
  }
  return NULL;
}

void AnimatedImage::clearDisposeTable() {
  for(size_t i = 0; i < m_disposeTable.size(); i++) {
    PixRect *pr = m_disposeTable[i];
    if(pr) delete pr;
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
  if(m_lastPaintedFrame != NULL) {
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
  if(!isLoaded() || isPlaying() || (m_workPr == NULL)) {
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
    throwException("No images loaded");
  }
  hide();
  saveBackground(p);
  m_animator.startAnimation(this, p);
}

void AnimatedImage::stopAnimation() {
  m_animator.stopAnimation();
}

void AnimatedImage::hide() {
  stopAnimation();
  restoreBackground();
  m_lastPaintedFrame = NULL;
}

AnimationThread::AnimationThread() : m_delaySem(0) {
  setDeamon(true);
  m_owner      = NULL;
  m_point      = CPoint(-1,-1);
  m_running    = false;
  m_killed     = false;
  m_stopSignal = false;
  m_frameIndex = 0;
}

void AnimationThread::startAnimation(AnimatedImage *image, const CPoint &p) {
  if(!isRunning()) {
    m_owner   = image;
    m_point   = p;
    m_running = true;
    resume();
  }
}

void AnimationThread::stopAnimation() {
  if(isRunning()) {
    stopLoop();
  }
}

void AnimationThread::kill() {
  m_killed = true;
  if(isRunning()) {
    stopLoop();
  } else {
    resume();
  }
  for(int i = 0; i < 10 && stillActive(); i++) {
    Sleep(10);
  }
  if(stillActive()) {
    AfxMessageBox(_T("Cannot stop animationThread!"));
  }
}

void AnimationThread::stopLoop() {
  if(isRunning()) {
    m_stopSignal = true;
    m_delaySem.signal();
    while(isRunning()) {
      Sleep(30);
    }
  }
}

UINT AnimationThread::run() {
  while(!m_killed) {
    const int imageCount = m_owner->getFrameCount();
    for(m_stopSignal = false, m_frameIndex = 0; !m_stopSignal; m_frameIndex = (m_frameIndex + 1) % imageCount) {
      const GifFrame &frame = m_owner->getFrame(m_frameIndex);
      frame.paint();
      m_owner->flushWork(m_point);
      m_delaySem.wait(frame.m_delayTime);
      frame.dispose();
    }
    m_running = false;
    if(m_killed) break;
    suspend();
  }
  return 0;
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
  m_owner->m_lastPaintedFrame = NULL;

  switch(m_disposalMode) {
  case DISPOSAL_UNSPECIFIED:       // No disposal specified
  case DISPOSE_DO_NOT      :       // Leave image in place
    if(!m_owner->isDisposeTableFull()) {
      m_owner->addToDisposeTable(m_owner->m_workPr->clone(m_owner->m_workPr->getType(), true));
    }
    return;
  case DISPOSE_BACKGROUND  :       // Set area to background color
    { if(!m_owner->isDisposeTableFull()) {
        m_owner->addToDisposeTable(NULL);
      }
      if(m_owner->hasSavedBackground()) {
        m_owner->m_workPr->rop(ORIGIN,m_owner->m_size,SRCCOPY, m_owner->m_background, ORIGIN);
      }
    }
    break;

  case DISPOSE_PREVIOUS    :       // Restore to previous non-disposd content content
    { if(!m_owner->isDisposeTableFull()) {
        m_owner->addToDisposeTable(NULL);
      }
      if(m_disposeTo == NULL) {
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
  m_disposeTo        = NULL;
  m_owner            = NULL;
  m_disposalMode     = DISPOSAL_UNSPECIFIED;
  m_delayTime        = 0;
  m_useTransparency  = false;
  m_srcRect = m_rect = CRect(0,0,0,0);
  m_pr               = NULL;
}

void GifFrame::cleanup() {
  m_disposeTo        = NULL;
  m_owner            = NULL;
  m_disposalMode     = DISPOSAL_UNSPECIFIED;
  m_delayTime        = 0;
  m_useTransparency  = false;
  m_srcRect = m_rect = CRect(0,0,0,0);
  if(m_pr) {
    delete m_pr;
    m_pr = NULL;
  }
}
