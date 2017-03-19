#define _GETOPT_H // dont need it
#undef  _MSC_VER
#define _MSC_VER 1600 // to include stdint.h

#include <stdio.h>

#define _BASICINTEGERTYPES_DEFINED
typedef unsigned __int32  uint32_t;

#include "..\lib\Src\GifLib\giflib-5.1.1\lib\gif_lib_private.h"
#include "GifUtil.h"
#include <Myutil.h>

GifFileType *allocateGifFile(int w, int h) {
  GifFileType        *GifFile = NULL;
  GifFilePrivateType *Private = NULL;

  try {
    GifFile = MALLOC(GifFileType,1);
    memset(GifFile, 0, sizeof(GifFileType));

    GifFile->SavedImages = NULL;
    GifFile->SColorMap   = NULL;

    Private = MALLOC(GifFilePrivateType, 1);
    memset(Private, 0, sizeof(GifFilePrivateType));

    GifFile->SWidth           = w;
    GifFile->SHeight          = h;
    GifFile->SColorResolution = 8;
    GifFile->UserData         = NULL;
    GifFile->Error            = 0;
    GifFile->Private          = Private;
    Private->FileHandle       = 0;
    Private->File             = NULL;
    Private->FileState        = 0;
    Private->gif89            = true;

    return GifFile;
  } catch(...) {
    if(Private) {
      FREE(Private);
    }
    if(GifFile) {
      FREE(GifFile);
    }
    throw;
  }
}

void deallocateGif(GifFileType *gif) {
  if(gif->Image.ColorMap) {
    GifFreeMapObject(gif->Image.ColorMap);
    gif->Image.ColorMap = NULL;
  }

  if(gif->SColorMap) {
    GifFreeMapObject(gif->SColorMap);
    gif->SColorMap = NULL;
  }

  if(gif->SavedImages) {
    GifFreeSavedImages(gif);
    gif->SavedImages = NULL;
  }

  GifFreeExtensions(&gif->ExtensionBlockCount, &gif->ExtensionBlocks);

  if(gif->Private) {
    FREE(gif->Private);
  }
  FREE(gif);
}

SavedImage *allocateSavedImage(int w, int h, unsigned int colorCount) {
  SavedImage *image          = MALLOC(SavedImage, 1);
  image->ImageDesc.Left      = image->ImageDesc.Top = 0;
  image->ImageDesc.Width     = w;
  image->ImageDesc.Height    = h;
  image->ImageDesc.Interlace = false;
  image->ImageDesc.ColorMap  = colorCount ? GifMakeMapObject(colorCount, NULL) : NULL;
  image->RasterBits          = MALLOC(GifByteType, w*h);
  image->ExtensionBlockCount = 0;     // TODO
  image->ExtensionBlocks     = NULL;

  return image;
}

void deallocateSavedImage(SavedImage *image) {
  if(image->ImageDesc.ColorMap != NULL) {
    GifFreeMapObject(image->ImageDesc.ColorMap);
  }
  FREE(image->RasterBits);
  FREE(image);
}

void setGCB(SavedImage *image, const GraphicsControlBlock &gcb) {
  GifByteType buf[sizeof(GraphicsControlBlock)];

  for(int i = 0; i < image->ExtensionBlockCount; i++) {
    ExtensionBlock *ep = &image->ExtensionBlocks[i];
    if(ep->Function == GRAPHICS_EXT_FUNC_CODE) {
      EGifGCBToExtension(&gcb, ep->Bytes);
      return;
    }
  }

  const UINT len = (UINT)EGifGCBToExtension(&gcb, buf);
  if(GifAddExtensionBlock(&image->ExtensionBlockCount
                         ,&image->ExtensionBlocks
                         ,GRAPHICS_EXT_FUNC_CODE
                         ,len
                         ,(BYTE*)buf) == GIF_ERROR) {
    throwException(_T("setGCB failed"));
  }
}

GraphicsControlBlock getGCB(const SavedImage *image) {
  GraphicsControlBlock gcb;
  gcb.DisposalMode     = DISPOSAL_UNSPECIFIED;
  gcb.UserInputFlag    = false;
  gcb.DelayTime        = 0;
  gcb.TransparentColor = NO_TRANSPARENT_COLOR;

  for(int i = 0; i < image->ExtensionBlockCount; i++) {
    const ExtensionBlock *ep = &image->ExtensionBlocks[i];
    if(ep->Function == GRAPHICS_EXT_FUNC_CODE) {
      if(DGifExtensionToGCB(ep->ByteCount, ep->Bytes, &gcb) != GIF_OK) {
        throwException(_T("DGifExtensionToGCB failed"));
      }
      return gcb;
    }
  }
  throwException(_T("No GraphicsControlBlock for SavedImage"));
  return gcb;
}

void addNetscapeBlock(SavedImage *image) {
  const char *buf1 = "NETSCAPE2.0";
  UINT  len1 = (UINT)strlen(buf1);
  if(GifAddExtensionBlock(&image->ExtensionBlockCount
                         ,&image->ExtensionBlocks
                         ,APPLICATION_EXT_FUNC_CODE
                         ,len1
                         ,(BYTE*)buf1) == GIF_ERROR) {
    throwException(_T("GifAddExtensionBlock failed"));
  }

  const BYTE buf2[3] = { 1, 0xe8, 0 };
  if(GifAddExtensionBlock(&image->ExtensionBlockCount
                         ,&image->ExtensionBlocks
                         ,CONTINUE_EXT_FUNC_CODE
                         ,ARRAYSIZE(buf2)
                         ,(BYTE*)buf2) == GIF_ERROR) {
    throwException(_T("GifAddExtensionBlock failed"));
  }
}
