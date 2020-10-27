#include "stdafx.h"
#include <DebugLog.h>

void checkGifError(const char *file, int line, int errorCode) {
  if(errorCode == GIF_OK) {
    return;
  }
  printf("Error in %s line %d:%s\n", file, line, GifErrorString(errorCode));
  exit(-1);
}

#define CHECKGIFOK(errorCode) checkGifError(__FILE__, __LINE__, errorCode)

static void usage() {
  _ftprintf(stderr, _T("Usage: dumpGif:fileName\n"));
  exit(-1);
}

void dumpContinuationBlock(const ExtensionBlock &ep) {
  _tprintf(_T("ContinuationBlock:\n"));
  hexdump(ep.Bytes, ep.ByteCount);
}

void dumpCommentBlock(const ExtensionBlock &ep) {
  printf("CommentBlock:<%*.*s>\n", ep.ByteCount,ep.ByteCount,ep.Bytes);
}

void dumpGCBBlock(const ExtensionBlock &ep) {
  GraphicsControlBlock gcb;
  CHECKGIFOK(DGifExtensionToGCB(ep.ByteCount, ep.Bytes, &gcb));
  _tprintf(_T("GraphicsControlBlock : Dispose:%d, Delay:%d, Transparent color:%d, Userinput:%s\n")
         ,gcb.DisposalMode, gcb.DelayTime, gcb.TransparentColor, boolToStr(gcb.UserInputFlag));
}

void dumpPlainTextBloxk(const ExtensionBlock &ep) {
  _tprintf(_T("PainTextBlock:\n"));
  hexdump(ep.Bytes, ep.ByteCount);
}

void dumpApplicationblock(const ExtensionBlock &ep){
  _tprintf(_T("ApplicationBlock:\n"));
  hexdump(ep.Bytes, ep.ByteCount);
}

void dumpExtensionBlocks(const ExtensionBlock *blockArray, int n) {
  for(int i = 0; i < n; i++) {
    const ExtensionBlock &ep = blockArray[i];
    switch(ep.Function) {
    case CONTINUE_EXT_FUNC_CODE   :            /* continuation subblock                 */
      dumpContinuationBlock(ep);
      break;
    case COMMENT_EXT_FUNC_CODE    :            /* comment                               */
      dumpCommentBlock(ep);
      break;
    case GRAPHICS_EXT_FUNC_CODE   :            /* graphics control (GIF89)              */
      dumpGCBBlock(ep);
      break;
    case PLAINTEXT_EXT_FUNC_CODE  :            /* plaintext                             */
      dumpPlainTextBloxk(ep);
      break;
    case APPLICATION_EXT_FUNC_CODE:            /* application block                     */
      dumpApplicationblock(ep);
      break;
    default:
      _tprintf(_T("Invalid Functioncode in extensionBlock:%d\n"), ep.Function);
      continue;
    }
  }
}

void dumpColorMap(ColorMapObject *map) {
  if(map == nullptr) {
    _tprintf(_T("ColorMap=nullptr\n"));
  } else {
    _tprintf(_T("ColorMap:ColorCount:%d, BitsPerPixel:%d, SortFlag:%s\n")
            ,map->ColorCount, map->BitsPerPixel, boolToStr(map->SortFlag));
  }
}

void dumpImageDesc(const GifImageDesc &desc) {
  _tprintf(_T("ImageDesc:Rect(%d,%d,%d,%d), interlaced:%s\n")
          ,desc.Left, desc.Top, desc.Width, desc.Height
          ,boolToStr(desc.Interlace)
        );
  dumpColorMap(desc.ColorMap);
}

void dumpSavedImage(int i, const SavedImage &image) {
  _tprintf(_T("Image %d\n"), i);
  dumpExtensionBlocks(image.ExtensionBlocks, image.ExtensionBlockCount);
  dumpImageDesc(image.ImageDesc);
  _tprintf(_T("----------------- End of image %d---------------------\n"), i);
}

int main(int argc, char **argv) {
  argv++;
  if(!*argv) usage();

  int errorCode;
  GifFileType *gifFile = DGifOpenFileName(*argv, &errorCode);
  if(gifFile == nullptr) {
    printf("Error:%s\n", GifErrorString(errorCode));
    exit(-1);
  }
  CHECKGIFOK(DGifSlurp(gifFile));

  _tprintf(_T("GifFile:Size:(%d,%d)\n"), gifFile->SWidth, gifFile->SHeight);
  _tprintf(_T("GifFile:BackgroundColor:%d, AspectByte:%d, ColorResultion:%d\n"), gifFile->SBackGroundColor, gifFile->AspectByte, gifFile->SColorResolution);
  _tprintf(_T("GifFile ")); dumpImageDesc(gifFile->Image);
  _tprintf(_T("GifFile:")); dumpColorMap(gifFile->SColorMap);
  _tprintf(_T("GifFile.ExtentionBlocks:\n"));
  dumpExtensionBlocks(gifFile->ExtensionBlocks, gifFile->ExtensionBlockCount);

  for(int i = 0; i < gifFile->ImageCount; i++) {
    dumpSavedImage(i, gifFile->SavedImages[i]);
  }

  if(DGifCloseFile(gifFile, &errorCode) != GIF_OK) {
    CHECKGIFOK(errorCode);
  }

  return 0;
}
