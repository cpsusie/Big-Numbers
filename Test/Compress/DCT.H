#pragma once

#ifdef _USE_DCT

#include "Compressor.h"
#include <MFCUtil/WinTools.h>
#include <MFCUtil/PixRect.h>

#define DCTBlockSize 8

void dump(const String &label, const int data[DCTBlockSize][DCTBlockSize], FILE *f = stdout);

class DCT {
private:
  PixRectDevice &m_device;
  int m_quantum[DCTBlockSize][DCTBlockSize];
  double C[DCTBlockSize][DCTBlockSize];
  double Ct[DCTBlockSize][DCTBlockSize];
  int m_inputRunLength;
  int m_outputRunLength;
  int m_compressedSize;
  void init(int quality);
  int  inputCode( BitInputStream  &input);
  void outputCode(BitOutputStream &output,int code);
  void quantify(  int data[DCTBlockSize][DCTBlockSize]);
  void dequantify(int data[DCTBlockSize][DCTBlockSize]);
  void readDCTData( BitInputStream  &input , int data[DCTBlockSize][DCTBlockSize]);
  void writeDCTData(BitOutputStream &output, int data[DCTBlockSize][DCTBlockSize]);
  void forwardDCT(const BYTE input[DCTBlockSize][DCTBlockSize], int  output[DCTBlockSize][DCTBlockSize]);
  void inverseDCT(const int  input[DCTBlockSize][DCTBlockSize], BYTE output[DCTBlockSize][DCTBlockSize]);
public:
  DCT(PixRectDevice &device) : m_device(device) {
  }
  void     writeImage(PixRect *p, ByteOutputStream &output, BYTE quality = 3);
  PixRect *readImage(ByteInputStream &input);
  int getCompressedSize() const {
    return m_compressedSize;
  }
};

class PictureCoder : public Compressor {
private:
  PixRectDevice m_device;
public:
  PictureCoder(HWND hwnd);
  void compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality);
  void expand(  ByteInputStream          &input, ByteOutputStream &output);
};

#endif