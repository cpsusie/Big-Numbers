#pragma once

#ifdef _USE_DCT

#include "Compressor.h"
#include <MFCUtil/PixRect.h>

#define DCTBlockSize 8

void dump(const String &label, const int data[DCTBlockSize][DCTBlockSize], FILE *f = stdout);

class DCT {
private:
  int m_quantum[DCTBlockSize][DCTBlockSize];
  double C[DCTBlockSize][DCTBlockSize];
  double Ct[DCTBlockSize][DCTBlockSize];
  int m_inputRunLength;
  int m_outputRunLength;
  int m_compressedSize;
  void init(int quality);
  int  inputCode(BitFile &input);
  void outputCode(BitFile &output,int code);
  void quantify(  int data[DCTBlockSize][DCTBlockSize]);
  void dequantify(int data[DCTBlockSize][DCTBlockSize]);
  void readDCTData( BitFile &input , int data[DCTBlockSize][DCTBlockSize]);
  void writeDCTData(BitFile &output, int data[DCTBlockSize][DCTBlockSize]);
  void forwardDCT(const unsigned char input[DCTBlockSize][DCTBlockSize], int output[DCTBlockSize][DCTBlockSize]);
  void inverseDCT(const int input[DCTBlockSize][DCTBlockSize], unsigned char output[DCTBlockSize][DCTBlockSize]);
public:
  void     writeImage(PixRect *p, FILE *output, unsigned char quality = 3);
  PixRect *readImage(FILE *input);
  int getCompressedSize() const {
    return m_compressedSize;
  }
};

class PictureCoder : public Compressor {
public:
  void compress(FILE *input, FILE *output, int quality);
  void expand(  FILE *input, FILE *output);
};

#endif