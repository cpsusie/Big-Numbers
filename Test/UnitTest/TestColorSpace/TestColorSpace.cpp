#include "stdafx.h"
#include "CppUnitTest.h"
#include <Random.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestColorSpace {

#include <UnitTestTraits.h>

  static double dist(const LSHColor &l1, const LSHColor &l2) {
    return sqrt(sqr(l1.m_hue-l2.m_hue) + sqr(l1.m_saturation-l2.m_saturation) + sqr(l1.m_lumination-l2.m_lumination));
  }

  TEST_CLASS(TestColorSpace)	{
	public:

    TEST_METHOD(RGBLSHColorSpace) {
      int printCount = 0;
      for(float hue = 0.01f; hue <= 0.99f; hue += 0.002f) {
        if(printCount++ % 20 == 0) {
          OUTPUT(_T("1/2:%5.0lf%%"), ((double)hue-0.01) * 100.0 / (0.99-0.01));
        }
        for(float satur = 0.01f; satur <= 0.99; satur += 0.005f) {
          for(float lum = 0.01f; lum <= 0.99; lum += 0.005f) {
            const LSHColor lsh(hue, satur, lum);
            const RGBColor color = getRGBColor(lsh);
            const LSHColor lsh1  = getLSHColor(color);
            verify(dist(lsh1, lsh) <= 0.001);
/*
              _tprintf(_T("lsh :(%.4f,%.4f,%.4f)\n"),lsh.m_hue,lsh.m_saturation,lsh.m_lumination);
              _tprintf(_T("lsh1:(%.4f,%.4f,%.4f)  "),lsh1.m_hue,lsh1.m_saturation,lsh1.m_lumination);
              _tprintf(_T("color:(%.4f,%.4f,%.4f)\n"),color.m_red,color.m_green,color.m_blue);
              _tprintf(_T("_____________________\n"));
              pause();
              RGBColor color = getRGBColor(lsh);
              LSHColor lsh1  = getLSHColor(color);
            }
*/
          }
        }
      }

      for(int red = 0; red < 256; red++) {
        if(red % 10 == 0) {
          OUTPUT(_T("2/2:%5.0lf%%"), ((double)red) * 100.0 / 255.0);
        }
        for(int green = 0; green < 256; green++) {
          for(int blue = 0; blue < 256; blue++) {
            const RGBColor color = D3DCOLOR_XRGB(red,green,blue);

    //        LSHColor lsh = getLSH(RGB(red,green,blue));
            const D3DCOLOR c = color;
            const int red1   = ARGB_GETRED(  c);
            const int green1 = ARGB_GETGREEN(c);
            const int blue1  = ARGB_GETBLUE( c);
            verify(red1   == red  );
            verify(green1 == green);
            verify(blue1  == blue );
/*
            if(red1 != red || green1 != green || blue1 != blue) {
              ok = false;
              _tprintf(_T("rgb  = (%3d,%3d,%3d)\n"),red ,green ,blue );
              _tprintf(_T("rgb1 = (%3d,%3d,%3d)\n"),red1,green1,blue1);
    //          _tprintf(_T("rgb1 = (%3d,%3d,%3d)  lsh:(%.4f,%.4f,%.4f)\n"),red1,green1,blue1,lsh.m_hue,lsh.m_saturation,lsh.m_lumination);
              _tprintf(_T("_____________________\n"));
              pause();
            }
*/
          }
        }
      }
    }

    TEST_METHOD(RGBsRGBColorSpace) {
      for(int red = 0; red < 256; red++) {
        if(red % 10 == 0) {
          OUTPUT(_T("2/2:%5.0lf%%"), ((double)red) * 100.0 / 255.0);
        }
        for(int green = 0; green < 256; green++) {
          for(int blue = 0; blue < 256; blue++) {
            const RGBColor color = D3DCOLOR_XRGB(red,green,blue);

            sRGBColor sRGB(color);
        
            const RGBColor color1 = sRGB;

            const D3DCOLOR c = color1;

            const int red1   = ARGB_GETRED(  c);
            const int green1 = ARGB_GETGREEN(c);
            const int blue1  = ARGB_GETBLUE( c);
            verify(red1   == red  );
            verify(green1 == green);
            verify(blue1  == blue );
/*
            if(red1 != red || green1 != green || blue1 != blue) {
              ok = false;
              _tprintf(_T("rgb  = (%3d,%3d,%3d)\n"),red ,green ,blue );
              _tprintf(_T("rgb1 = (%3d,%3d,%3d)\n"),red1,green1,blue1);
    //          _tprintf(_T("rgb1 = (%3d,%3d,%3d)  lsh:(%.4f,%.4f,%.4f)\n"),red1,green1,blue1,lsh.m_hue,lsh.m_saturation,lsh.m_lumination);
              _tprintf(_T("_____________________\n"));
              pause();
            }
*/
          }
        }
      }
    }

    TEST_METHOD(RGB_CIEXYZColorSpace) {
      for(int red = 0; red < 256; red++) {
        if(red % 10 == 0) {
          OUTPUT(_T("2/2:%5.0lf%%"), ((double)red) * 100.0 / 255.0);
        }
        for(int green = 0; green < 256; green++) {
          for(int blue = 0; blue < 256; blue++) {
            const RGBColor color = D3DCOLOR_XRGB(red,green,blue);

            CIEXYZ1931Color cieXYZ(color);
        
            const RGBColor color1 = cieXYZ;

            const D3DCOLOR c = color1;

            const int red1   = ARGB_GETRED(  c);
            const int green1 = ARGB_GETGREEN(c);
            const int blue1  = ARGB_GETBLUE( c);
            verify(red1   == red  );
            verify(green1 == green);
            verify(blue1  == blue );
/*
            if(red1 != red || green1 != green || blue1 != blue) {
              ok = false;
              _tprintf(_T("rgb  = (%3d,%3d,%3d)\n"),red ,green ,blue );
              _tprintf(_T("rgb1 = (%3d,%3d,%3d)\n"),red1,green1,blue1);
    //          _tprintf(_T("rgb1 = (%3d,%3d,%3d)  lsh:(%.4f,%.4f,%.4f)\n"),red1,green1,blue1,lsh.m_hue,lsh.m_saturation,lsh.m_lumination);
              _tprintf(_T("_____________________\n"));
              pause();
            }
*/
          }
        }
      }
    }

    TEST_METHOD(RGB_CIEXYZ_CIELABColorSpace) {
      int printCount = 0;
      float minL = 1000, maxL = -1000;
      for(int red = 0; red < 256; red++) {
        if(red % 10 == 0) {
          OUTPUT(_T("2/2:%5.0lf%%"), ((double)red) * 100.0 / 255.0);
        }
        for(int green = 0; green < 256; green++) {
          for(int blue = 0; blue < 256; blue++) {
            const RGBColor color = D3DCOLOR_XRGB(red,green,blue);

            CIEXYZ1931Color cieXYZ(color);
        
            CIELABColor cieLabColor(cieXYZ, CIEXYZ1931Color::monitorWhite);

            minL = min(minL, cieLabColor.m_L);
            maxL = max(maxL, cieLabColor.m_L);

            CIEXYZ1931Color cieXYZ1 = cieLabColor.getCIEXYZ1931Color(CIEXYZ1931Color::monitorWhite);

            const RGBColor color1 = cieXYZ1;

            const D3DCOLOR c = color1;

            const int red1   = ARGB_GETRED(  c);
            const int green1 = ARGB_GETGREEN(c);
            const int blue1  = ARGB_GETBLUE( c);
            verify(red1   == red  );
            verify(green1 == green);
            verify(blue1  == blue );
/*
            if(red1 != red || green1 != green || blue1 != blue) {
              ok = false;
              _tprintf(_T("rgb  = (%3d,%3d,%3d)\n"),red ,green ,blue );
              _tprintf(_T("rgb1 = (%3d,%3d,%3d)\n"),red1,green1,blue1);
    //          _tprintf(_T("rgb1 = (%3d,%3d,%3d)  lsh:(%.4f,%.4f,%.4f)\n"),red1,green1,blue1,lsh.m_hue,lsh.m_saturation,lsh.m_lumination);
              _tprintf(_T("_____________________\n"));
              pause();
            }
*/
          }
        }
      }
      OUTPUT(_T("minL,maxL:(%f, %f)"), minL, maxL);
      OUTPUT(_T("monitorWhite:%s"), CIEXYZ1931Color::monitorWhite.toString().cstr());
    }
	};
}

