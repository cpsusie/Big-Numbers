/*
/--------------------------------------------------------------------
|
|      $Id: plpgmdec.cpp,v 1.10 2004/09/11 12:41:35 uzadow Exp $
|      portable graymap Decoder Class
|
|      Original author: Jose Miguel Buenaposada Biencinto.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plpgmdec.h"
#include "plexcept.h"

// Creates a decoder
PLPGMDecoder::PLPGMDecoder() : m_LastByte(0), m_UseLastByte(false) {
}

PLPGMDecoder::~PLPGMDecoder() {
}

void PLPGMDecoder::Open(PLDataSource *pDataSrc) {
  Trace (2, "Decoding PGM.\n");

  readPgmHeader(&m_PGMHeader, pDataSrc);
  SetBmpInfo (PLPoint(m_PGMHeader.ImageWidth, m_PGMHeader.ImageHeight), PLPoint (0,0), PLPixelFormat::L8);
}

void PLPGMDecoder::GetImage(PLBmpBase &Bmp) {
  readImage(&Bmp, m_pDataSrc);
}


void PLPGMDecoder::readPgmHeader(PGMHEADER *pPgmHead, PLDataSource *pDataSrc) {
  int current = 0;
  bool HeaderComplete = false;

  // Read type
  m_LastByte = ReadByte (pDataSrc);
  if (m_LastByte !='P')
      raiseError (PL_ERRFORMAT_UNKNOWN, "PGM decoder: Is not the correct identifier P5 or P2.");

  m_LastByte = ReadByte (pDataSrc);
  switch(m_LastByte) {
    case '2':
	  pPgmHead->ImageType = PGM_P2;
      break;
    case '5':
	  pPgmHead->ImageType = PGM_P5;
      break;
    default:
      raiseError (PL_ERRFORMAT_UNKNOWN,"PGM decoder: Is not the correct identifier P5 or P2.");
  }

  m_LastByte = ReadByte (pDataSrc);

  // Search for the with, height and Max gray value
  while (current<3) {
    if (m_LastByte=='#') // # Starts a comment
		skipComment(pDataSrc);
	else if(m_LastByte >= '0' && m_LastByte <= '9') // A digit
      switch (current) {
		case 0: // looking for the width
		  {
		  pPgmHead->ImageWidth = readASCIIDecimal(pDataSrc);
		  current++;
		  }
		  break;
		case 1: // looking for the height
		  {
		  pPgmHead->ImageHeight = readASCIIDecimal(pDataSrc);
		  current++;
		  }
		  break;
		case 2: // looking for the max gray value
		  {
		  pPgmHead->MaxGrayValue  = readASCIIDecimal(pDataSrc);
          if ((pPgmHead->MaxGrayValue>255)||(pPgmHead->MaxGrayValue<=0))
	        pPgmHead->MaxGrayValue=255;
		  current++;
		  }
		  break;
		default:
          continue;
		}
	else
      skipPgmASCIISeparators(pDataSrc);
  }
}

PLBYTE *PLPGMDecoder::readASCIILine(PLDataSource *pDataSrc) {
  PLBYTE buffer[PGM_MAXLINESIZE];

  for(int i = 0;; i++) {
    if(i==PGM_MAXLINESIZE)
      raiseError(PL_ERRFORMAT_UNKNOWN, "PPM decoder: File Line to long.");

    PLBYTE byte = ReadByte(pDataSrc);
    buffer[i] = byte;
    if(byte == '\r' || byte == '\n') {
      buffer[i] = 0;
      break;
    }
  }

  PLBYTE *result = new PLBYTE[PGM_MAXLINESIZE];
  memcpy(result,buffer,sizeof(PLBYTE)*PGM_MAXLINESIZE);
  return result;
}


int PLPGMDecoder::readASCIIDecimal(PLDataSource *pDataSrc) {
  int Value = 0;

  while (m_LastByte >= '0' && m_LastByte <= '9') { // Is ASCII digit
    int digit = m_LastByte - '0';
    Value = Value * 10+digit;
    m_LastByte = ReadByte(pDataSrc);
  }

  return Value;
}

void PLPGMDecoder::skipComment(PLDataSource * pDataSrc) {
  while(m_LastByte != '\r' && m_LastByte != '\n')
    m_LastByte = ReadByte(pDataSrc);
}

void PLPGMDecoder::skipPgmASCIISeparators(PLDataSource * pDataSrc) {
  while(m_LastByte == ' ' || m_LastByte == '\n' || m_LastByte == '\r')
    m_LastByte = ReadByte(pDataSrc);
}

void PLPGMDecoder::readImage(PLBmpBase *pBmp, PLDataSource *pDataSrc) {
  switch (m_PGMHeader.ImageType) {
    case PGM_P5:
    case PGM_P2:
      readData(pBmp, pDataSrc);
	  break;
    default:
      raiseError (PL_ERRFORMAT_UNKNOWN, "Unknown PGM image type.");
  }
}

void PLPGMDecoder::readData(PLBmpBase *pBmp, PLDataSource *pDataSrc) {
  int Width  = m_PGMHeader.ImageWidth;
  int Height = m_PGMHeader.ImageHeight;

  PLBYTE **pLineArray = pBmp->GetLineArray();

  if(m_PGMHeader.ImageType == PGM_P2) {
    skipPgmASCIISeparators(pDataSrc);
    m_UseLastByte = true;
  }

  for(int y = 0; y < Height; y++) {
    PLBYTE *pDest = pLineArray[y];
	if(m_PGMHeader.ImageType==PGM_P5) // P5
      expandByteLine(pDest, m_PGMHeader.MaxGrayValue, Width, pDataSrc);
	else // P2
      expandASCIILine(pDest, m_PGMHeader.MaxGrayValue, Width, pDataSrc);
  }
}

void PLPGMDecoder::expandASCIILine(PLBYTE *pDest, int MaxGrayValue, int Width, PLDataSource *pDataSrc) {
  for (int x=0; x<Width; x++) {
    *pDest = readASCIIPixel8(MaxGrayValue, pDataSrc);
    pDest++;
  }
}

void PLPGMDecoder::expandByteLine(PLBYTE *pDest, int MaxGrayValue, int Width, PLDataSource * pDataSrc) {
  PLBYTE *pLine = pDataSrc->ReadNBytes(Width);
  if(pLine==NULL)
	  return;

  for(int x = 0; x < Width; x++) {
    *pDest = (PLBYTE)(((int)pLine[x]*255)/MaxGrayValue);
    pDest++;
  }
}

PLBYTE PLPGMDecoder::readASCIIPixel8(int MaxGrayValue, PLDataSource *pDataSrc) {
  skipPgmASCIISeparators(pDataSrc);
  m_UseLastByte = true;
  int Value     = readASCIIDecimal(pDataSrc);
  return (PLBYTE)((Value*255)/MaxGrayValue);
}


/*
/--------------------------------------------------------------------
|
|      $Log: plpgmdec.cpp,v $
|      Revision 1.10  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.9  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.8  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.7  2002/08/04 21:20:41  uzadow
|      no message
|
|      Revision 1.6  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.5  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.4  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.3  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.4  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.3  2000/05/27 16:34:05  Ulrich von Zadow
|      Linux compatibility changes
|
|      Revision 1.2  2000/05/23 10:19:11  Ulrich von Zadow
|      Minor unix compatibility changes.
|
|      Revision 1.1  2000/03/16 13:56:37  Ulrich von Zadow
|      Added pgm decoder by Jose Miguel Buenaposada Biencinto
|
|
\--------------------------------------------------------------------
*/
