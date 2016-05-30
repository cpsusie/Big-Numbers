/*****************************************************************************

gif_err.c - handle error reporting for the GIF library.

****************************************************************************/

#include <stdio.h>

#include "gif_lib.h"
#include "gif_lib_private.h"

/*****************************************************************************
 Return a string description of  the last GIF error
*****************************************************************************/
const char *GifErrorString(int ErrorCode) {
  switch (ErrorCode) {
  case E_GIF_ERR_OPEN_FAILED   :    return "Failed to open given file";
  case E_GIF_ERR_WRITE_FAILED  :    return "Failed to write to given file";
  case E_GIF_ERR_HAS_SCRN_DSCR :    return "Screen descriptor has already been set";
  case E_GIF_ERR_HAS_IMAG_DSCR :    return "Image descriptor is still active";
  case E_GIF_ERR_NO_COLOR_MAP  :    return "Neither global nor local color map";
  case E_GIF_ERR_DATA_TOO_BIG  :    return "Number of pixels bigger than width * height";
  case E_GIF_ERR_NOT_ENOUGH_MEM:    return "Failed to allocate required memory";
  case E_GIF_ERR_DISK_IS_FULL  :    return "Write failed (disk full?)";
  case E_GIF_ERR_CLOSE_FAILED  :    return "Failed to close given file";
  case E_GIF_ERR_NOT_WRITEABLE :    return "Given file was not opened for write";
  case D_GIF_ERR_OPEN_FAILED   :    return "Failed to open given file";
  case D_GIF_ERR_READ_FAILED   :    return "Failed to read from given file";
  case D_GIF_ERR_NOT_GIF_FILE  :    return "Data is not in GIF format";
  case D_GIF_ERR_NO_SCRN_DSCR  :    return "No screen descriptor detected";
  case D_GIF_ERR_NO_IMAG_DSCR  :    return "No Image Descriptor detected";
  case D_GIF_ERR_NO_COLOR_MAP  :    return "Neither global nor local color map";
  case D_GIF_ERR_WRONG_RECORD  :    return "Wrong record type detected";
  case D_GIF_ERR_DATA_TOO_BIG  :    return "Number of pixels bigger than width * height";
  case D_GIF_ERR_NOT_ENOUGH_MEM:    return "Failed to allocate required memory";
  case D_GIF_ERR_CLOSE_FAILED  :    return "Failed to close given file";
  case D_GIF_ERR_NOT_READABLE  :    return "Given file was not opened for read";
  case D_GIF_ERR_IMAGE_DEFECT  :    return "Image is defective, decoding aborted";
  case D_GIF_ERR_EOF_TOO_SOON  :    return "Image EOF detected before image complete";
  default                      :    return NULL;
  }
}

/* end */
