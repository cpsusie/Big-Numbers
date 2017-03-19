#pragma once

#include "..\lib\Src\GifLib\giflib-5.1.1\lib\gif_lib.h"

GifFileType *allocateGifFile(int w, int h);
void         deallocateGif(GifFileType *);

SavedImage *allocateSavedImage(int w, int h, unsigned int colorCount);
void        deallocateSavedImage(      SavedImage *image);
void        setGCB(                    SavedImage *image, const GraphicsControlBlock &gcb);
GraphicsControlBlock  getGCB(    const SavedImage *image);
void        addNetscapeBlock(          SavedImage *image);
