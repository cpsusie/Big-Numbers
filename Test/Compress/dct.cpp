#include "stdafx.h"

#ifdef _USE_DCT

#include <Math.h>
#include <CountedByteStream.h>
#include "DCT.h"

//#define DEBUGMODULE

typedef struct {
  int row;
  int col;
} PixelPosition;

static PixelPosition pixelPosition[] = {
  {0,0}
 ,{0,1},{1,0}
 ,{2,0},{1,1},{0,2}
 ,{0,3},{1,2},{2,1},{3,0}
 ,{4,0},{3,1},{2,2},{1,3},{0,4}
 ,{0,5},{1,4},{2,3},{3,2},{4,1},{5,0}
 ,{6,0},{5,1},{4,2},{3,3},{2,4},{1,5},{0,6}
 ,{0,7},{1,6},{2,5},{3,4},{4,3},{5,2},{6,1},{7,0}
 ,{7,1},{6,2},{5,3},{4,4},{3,5},{2,6},{1,7}
 ,{2,7},{3,6},{4,5},{5,4},{6,3},{7,2}
 ,{7,3},{6,4},{5,5},{4,6},{3,7}
 ,{4,7},{5,6},{6,5},{7,4}
 ,{7,5},{6,6},{5,7}
 ,{6,7},{7,6}
 ,{7,7}
};

void DCT::init(int quality) {
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      m_quantum[i][j] = 1+(1+i+j) * quality;
    }
  }

  for(int j = 0; j < DCTBlockSize; j++) {
    C[0][j] = Ct[j][0] = 1.0/sqrt(DCTBlockSize);
  }
  for(int i = 1; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      C[i][j] = Ct[j][i] = sqrt(2.0/DCTBlockSize) * cos(M_PI * (2*j+1)*i/(2.0*DCTBlockSize));
    }
  }
  m_inputRunLength  = 0;
  m_outputRunLength = 0;
}

int DCT::inputCode(BitInputStream &input) {
  if(m_inputRunLength > 0) {
    m_inputRunLength--;
    return 0;
  }
  int bitCount = input.getBits(2);
  if(bitCount == 0) {
    m_inputRunLength = input.getBits(4);
    return 0;
  }
  if(bitCount == 1) {
    bitCount = input.getBit() + 1;
  } else {
    bitCount = input.getBits(2) + (bitCount << 2) - 5;
  }
  int result = input.getBits(bitCount);
  if(result & (1 << (bitCount-1))) {
    return result;
  }
  return result - (1 << bitCount) + 1;
}

void DCT::outputCode(BitOutputStream &output, int code) {
  if(code == 0) {
    m_outputRunLength++;
    return;
  }
  if(m_outputRunLength != 0) {
    while(m_outputRunLength > 0) {
      output.putBits(0,2);
      if(m_outputRunLength <= 16) {
        output.putBits(m_outputRunLength-1,4);
        m_outputRunLength = 0;
      } else {
        output.putBits(15,4);
        m_outputRunLength -= 16;
      }
    }
  }
  int absCode = (code < 0) ? -code : code;
  int topOfRange = 1;
  int bitCount = 1;
  while(absCode > topOfRange) {
    bitCount++;
    topOfRange = (topOfRange+1)*2 - 1;
  }

  if(bitCount < 3) {
    output.putBits(bitCount+1,3);
  } else {
    output.putBits(bitCount+5,4);
  }
  if(code > 0) {
    output.putBits(code,bitCount);
  } else {
    output.putBits(code+topOfRange,bitCount);
  }
}

void DCT::quantify(int data[DCTBlockSize][DCTBlockSize]) {
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      data[i][j] /= m_quantum[i][j];
    }
  }
}

void DCT::dequantify(int data[DCTBlockSize][DCTBlockSize]) {
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      data[i][j] *= m_quantum[i][j];
    }
  }
}

void DCT::readDCTData(BitInputStream &input, int data[DCTBlockSize][DCTBlockSize]) {
  for(int i = 0; i < DCTBlockSize*DCTBlockSize; i++) {
    const PixelPosition &p = pixelPosition[i];
    data[p.row][p.col] = inputCode(input);
  }
}

#define ROUND(a) (((a)<0)?(int)((a)-0.5) : ((int)((a)+0.5)))

void DCT::writeDCTData(BitOutputStream &output, int data[DCTBlockSize][DCTBlockSize]) {
  for(int i = 0; i < DCTBlockSize*DCTBlockSize; i++) {
    const PixelPosition &p = pixelPosition[i];
    double result = data[p.row][p.col];
    outputCode(output,ROUND(result));
  }
}

void DCT::forwardDCT(const BYTE input[DCTBlockSize][DCTBlockSize], int output[DCTBlockSize][DCTBlockSize]) {
  double tmp[DCTBlockSize][DCTBlockSize];
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      double sum = 0;
      for(int k = 0; k < DCTBlockSize; k++) {
        sum += ((int)input[i][k]-128) * Ct[k][j];
      }
      tmp[i][j] = sum;
    }
  }
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      double sum = 0;
      for(int k = 0; k < DCTBlockSize; k++) {
        sum += C[i][k] * tmp[k][j];
      }
      output[i][j] = ROUND(sum);
    }
  }
}

void DCT::inverseDCT(const int input[DCTBlockSize][DCTBlockSize], BYTE output[DCTBlockSize][DCTBlockSize]) {
  double tmp[DCTBlockSize][DCTBlockSize];
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      double sum = 0;
      for(int k = 0; k < DCTBlockSize; k++) {
        sum += ((int)input[i][k]) * C[k][j];
      }
      tmp[i][j] = sum;
    }
  }
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      double sum = 128;
      for(int k = 0; k < DCTBlockSize; k++) {
        sum += Ct[i][k] * tmp[k][j];
      }
      if(sum < 0) {
        output[i][j] = 0;
      } else if(sum > 255) {
        output[i][j] = 255;
      } else {
        output[i][j] = (BYTE)(ROUND(sum));
      }
    }
  }
}

class PixelBlock {
public:
  unsigned char m_pixels[DCTBlockSize][DCTBlockSize];
  void getPixels(PixelAccessor *src, int row, int col, int part);
  void putPixels(PixelAccessor *dst, int row, int col, int part);
  void dump(int part, int row, int col, FILE *f = stdout) const;
};

void PixelBlock::getPixels(PixelAccessor *src, int row, int col, int part) {
  const int w     = src->getPixRect()->getWidth();
  const int h     = src->getPixRect()->getHeight();
  const int shift = 8 * part;
  const int maxI  = min(DCTBlockSize, h - row);
  const int maxJ  = min(DCTBlockSize, w - col);

  memset(m_pixels,0,sizeof(m_pixels));
  for(int i = 0, y = row; i < maxI; i++, y++) {
    for(int j = 0, x = col; j < maxJ; j++, x++) {
      D3DCOLOR value = src->getPixel(x,y);
      m_pixels[i][j] = (BYTE)((value >> shift)&0xff);
    }
  }
}

void PixelBlock::putPixels(PixelAccessor *dst, int row, int col, int part) {
  const int w     = dst->getPixRect()->getWidth();
  const int h     = dst->getPixRect()->getHeight();
  const int shift = 8 * part;
  const int maxI  = min(DCTBlockSize, h - row);
  const int maxJ  = min(DCTBlockSize, w - col);

  for(int i = 0, y = row; i < maxI; i++, y++) {
    for(int j = 0, x = col; j < maxJ; j++, x++) {
      D3DCOLOR value = dst->getPixel(x,y) | (m_pixels[i][j] << shift);
      dst->setPixel(x,y,value);
    }
  }
}

void PixelBlock::dump(int part, int row, int col, FILE *f) const {
  _ftprintf(f,_T("---------Pixels (%d,%d,%d)----------------\n"), part, row, col);
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      _ftprintf(f,_T("%4d"),m_pixels[i][j]);
    }
    _ftprintf(f,_T("\n"));
  }
  _ftprintf(f,_T("-------------------------------\n"));
}

void dump(const String &label, const int data[DCTBlockSize][DCTBlockSize], FILE *f) {
  _ftprintf(f,_T("---------%s--------------------\n"),label.cstr());
  for(int i = 0; i < DCTBlockSize; i++) {
    for(int j = 0; j < DCTBlockSize; j++) {
      _ftprintf(f,_T("%4d"),data[i][j]);
    }
    _ftprintf(f,_T("\n"));
  }
  _ftprintf(f,_T("-------------------------------\n"));
}

void DCT::writeImage(PixRect *p, ByteOutputStream &output, BYTE quality) {
  ByteCounter counter;
  CountedByteOutputStream out1(counter, output);
  BitOutputStream out(out1);
  PixelAccessor *pa = p->getPixelAccessor();
  init(quality);
  const int w = p->getWidth();
  const int h = p->getHeight();

  out.putBits(quality,8);
  out.putBits((USHORT)w,16);
  out.putBits((USHORT)h,16);
  for(int part = 0; part < 3; part++) {
    for(int row = 0; row < h; row += DCTBlockSize) {
      for(int col = 0; col < w; col += DCTBlockSize) {
        PixelBlock block;
        block.getPixels(pa,row,col,part);

        int outputArray[DCTBlockSize][DCTBlockSize];
        forwardDCT(block.m_pixels,outputArray);

#ifdef DEBUGMODULE
        block.dump(part,row,col);
        dump(format("Before quantify (%d,%d,%d)", part,row,col),outputArray);
#endif
        quantify(outputArray);

#ifdef DEBUGMODULE
        dump(format("After quantify (%d,%d,%d)", part,row,col),outputArray);
#endif

        writeDCTData(out,outputArray);
      }
    }
  }
  delete pa;
  outputCode(out,1);
  m_compressedSize = (int)counter.getCount();
}

PixRect *DCT::readImage(ByteInputStream &input) {
  ByteCounter counter;
  CountedByteInputStream in1(counter, input);
  BitInputStream in(in1);
  const int quality = in.getBits(8);
  init(quality);
  const long w = in.getBits(16);
  const long h = in.getBits(16);
  PixRect *result = new PixRect(m_device, PIXRECT_PLAINSURFACE, w,h);
  result->fillRect(0,0,w,h,BLACK);
  PixelAccessor *pa = result->getPixelAccessor();
  for(int part = 0; part < 3; part++) {
    for(int row = 0; row < h; row += DCTBlockSize) {
      for(int col = 0; col < w; col += DCTBlockSize) {
        int inputArray[DCTBlockSize][DCTBlockSize];
        readDCTData(in,inputArray);

#ifdef DEBUGMODULE
        dump(format("Before dequantify (%d,%d,%d)", part,row,col), inputArray);
#endif
        dequantify(inputArray);

        PixelBlock block;
        inverseDCT(inputArray,block.m_pixels);

#ifdef DEBUGMODULE
        dump(format("After dequantify (%d,%d,%d)", part,row,col), inputArray);
        block.dump(part,row,col);
#endif

        block.putPixels(pa,row,col,part);
      }
    }
  }
  delete pa;
  m_compressedSize = (int)counter.getCount();
  return result;
}

PictureCoder::PictureCoder(HWND hwnd) {
  m_device.attach(hwnd);
}

void PictureCoder::compress(ResetableByteInputStream &input, ByteOutputStream &output, int quality) {
  ByteCounter counter;
  CountedByteInputStream in(counter, input);
  PixRect *p = PixRect::load(m_device, in);
  DCT dct(m_device);
  dct.writeImage(p,output,quality);
  delete p;

  m_rawSize        = counter.getCount();
  m_compressedSize = dct.getCompressedSize();
}

void PictureCoder::expand(ByteInputStream &input, ByteOutputStream &output) {
  DCT dct(m_device);
  PixRect *p = dct.readImage(input);
  ByteCounter counter;
  CountedByteOutputStream out(counter, output);
  p->writeAsBMP(out);
  delete p;

  m_rawSize        = counter.getCount();
  m_compressedSize = dct.getCompressedSize();
}

#endif
