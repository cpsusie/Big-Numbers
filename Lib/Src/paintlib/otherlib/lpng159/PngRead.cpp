//an example of how to read a PNG file (extract from my image class)

#include <png.h> 


enum colorType
{
  COLOR_GRAY,
  COLOR_RGB,
  COLOR_RGBA
};

//image data
int 	    m_width;
int 	    m_height;
BYTE* 	  m_pData = NULL; //image buffer
int       m_bytesPerPixel;
bool	    m_bForce8BitDepth = true;
colorType m_colorType;


bool ReadPNG(char* in_sName)
{
  FILE *pFile = NULL;	
  BYTE** pRowPointers = NULL;
  pFile = fopen(in_sName,"rb");

  if(!pFile)
    return false;

  BYTE header[8]; 
  
  //confirm png header
  fread(header, 1, 8, pFile);
  if (!png_check_sig(header, 8))
  {
    fclose(pFile);
    return false;
  }

  //create pPng and info_png
  png_structp pPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!pPng)
  {
    fclose(pFile);
    return false;
  }

  png_infop pPngInfo = png_create_info_struct(pPng);
  if (!pPngInfo)
  {
    fclose(pFile);
    png_destroy_read_struct(&pPng,NULL, NULL); 
    return false;
  }

  // it's a goto (in case png lib hits the bucket)
  if(setjmp(png_jmpbuf(pPng))) 
  {  
    fclose(pFile);
    png_destroy_read_struct(&pPng, &pPngInfo, NULL);
    if(pRowPointers)
      delete [] pRowPointers;
    return false;
  }

  png_init_io(pPng, pFile);
  png_set_sig_bytes(pPng, 8);  // we already read the 8 signature bytes 
  png_read_info(pPng, pPngInfo);  // read all PNG info up to image data 

  int bitDepth, colorType;
  ULONG width, height;
  png_get_IHDR(pPng, pPngInfo, &width, &height, &bitDepth, &colorType,NULL, NULL, NULL);

  m_width = width;
  m_height = height;  

  // apply filters to image so we can get proper image data format
  if (colorType == PNG_COLOR_TYPE_PALETTE)
      png_set_palette_to_rgb(pPng);
  if (colorType == PNG_COLOR_TYPE_GRAY && bitDepth < 8)
      png_set_gray_1_2_4_to_8(pPng);
  if (png_get_valid(pPng, pPngInfo, PNG_INFO_tRNS))
      png_set_tRNS_to_alpha(pPng);

  if(bitDepth == 16)
  {
    if(m_bForce8BitDepth)
      png_set_strip_16(pPng); //make 8-bit
    else
      png_set_swap(pPng); //swap endian order (PNG is Big Endian)
  }

  if(m_bForce8BitDepth) // force8bit depth per channel
    bitDepth = 8;

  int byteDepth = bitDepth/8;

  switch(colorType)
  {
  case  PNG_COLOR_TYPE_GRAY:
    m_colorType = COLOR_GRAY;
    m_bytesPerPixel = byteDepth;
    break;

  case PNG_COLOR_TYPE_RGB_ALPHA:
    m_colorType = COLOR_RGBA;
    m_bytesPerPixel = byteDepth*4;
    break;

  case PNG_COLOR_TYPE_RGB:
  default:
    m_colorType = COLOR_RGB; 
    m_bytesPerPixel = byteDepth*3;
    break;
  }

  png_read_update_info(pPng, pPngInfo);

  int rowbytes = png_get_rowbytes(pPng, pPngInfo);

  m_pData = new BYTE[rowbytes*m_height];
  pRowPointers = new BYTE*[m_height];

  for (int i = 0;  i < m_height;  ++i)
    pRowPointers[i] = m_pData + i*rowbytes;

  png_read_image(pPng, pRowPointers);

  png_read_end(pPng, NULL);

  png_destroy_read_struct(&pPng, &pPngInfo, NULL);

  fclose(pFile);
  delete [] pRowPointers;

  return true;
}