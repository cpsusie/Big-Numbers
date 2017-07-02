//
//   makewav.cpp - by Don Cross <dcross@intersrv.com>
//
//   Get latest version of this file at:
//   http://www.intersrv.com/~dcross/makewav
//
//   Get latest version of C++ code for class WaveFile at:
//   http://www.intersrv.com/~dcross/wavio.html
//

#include "stdafx.h"

using namespace std;

#include "riff.h"

static void usage() {
  tcerr << _T("Usage:wavconc [-s] [-t] [-p] infiles... outfile") << endl;
  exit(-1);
}

#define ERRORSTR _T(":") << DDCRET_String(rc) << endl

bool SampleIsNull(WaveFileSample &sample) {
  return sample.chan[0] == 0 && sample.chan[1] == 0;
}

void SampleSpline(WaveFile &outfile, int count, WaveFileSample &prev, WaveFileSample &next) {
  DDCRET rc ;
  for(int i = 0; i < count; i++) {
    WaveFileSample res;
    res.chan[0] = ((UINT)prev.chan[0] * i + (UINT)next.chan[0] * (count-i)) / count;
    res.chan[1] = ((UINT)prev.chan[1] * i + (UINT)next.chan[1] * (count-i)) / count;
    tcout << res.chan[1] << endl;
    rc = outfile.WriteStereoSample(res.chan[0],res.chan[1]);
    if( rc != DDC_SUCCESS ) {
      tcerr << "Error writing spline" << ERRORSTR;
      exit(-1);
    }
  }
}

int main ( int argc, char **argv) {
  char *cp;
  const char *outFilename = argv[argc - 1];
  bool remspace   = false;
  bool usespline  = false;
  bool dumpastext = false;
  for(argv++; *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++)
      switch(*cp) {
      case 's':
        remspace  = true;
        continue;
      case 'p':
        usespline = true;
        continue;
      case 't':
        dumpastext = true;
        continue;
      default:
        usage();
    }
  }

  DDCRET rc ;
  WaveFile outfile;

  bool firstfile = true;
  WaveFileSample firstsamp,lastsamp;
  int nummer = 0;
  for(;*argv && *argv != outFilename;argv++) {
    char *inFilename = *argv;
    WaveFile infile;
    DDCRET ret = infile.OpenForRead(inFilename);
    if ( ret != DDC_SUCCESS) {
      cerr << "Error opening infile " << inFilename << ERRORSTR;
      exit(-1);
    }
    UINT32 numsamp = infile.NumSamples();
    WaveFileSample *data = new WaveFileSample[numsamp] ;

    if(firstfile) {
      rc = outfile.OpenForWrite(outFilename,infile );

      if( rc != DDC_SUCCESS ) {
        cerr << "Error creating " << outFilename << ERRORSTR;
        exit(-1);
      }
    }

/*
    firstsamp.chan[0] = 0; firstsamp.chan[1] = 0;
    lastsamp.chan[0] = (INT16)0xefff; lastsamp.chan[1] = (INT16)0xefff;
    SampleSpline(outfile, 100, firstsamp,lastsamp);
*/

    cout << "Number of samples(" << inFilename << "):" << numsamp << endl;
    if(infile.ReadSamples(numsamp,data) != DDC_SUCCESS) {
      cerr << "Error reading " << inFilename << ERRORSTR;
      exit(-1);
    }

    UINT32 i, startsamp = 0,endsamp = numsamp - 1,totalsamp;

    if(remspace) {
      for(startsamp = 0; startsamp < numsamp; startsamp++)
        if(!SampleIsNull(data[startsamp])) break;
      for(endsamp = numsamp - 1; endsamp > startsamp; endsamp--)
        if(!SampleIsNull(data[endsamp]))   break;
      cerr << "skipping (" << startsamp << "," << numsamp - endsamp << ") samples" << endl;
    }

    firstsamp = data[startsamp];
    totalsamp = endsamp - startsamp;
    if(usespline) {
      if(!firstfile) {
        SampleSpline(outfile, 100, firstsamp,lastsamp);
      }
    }
    int skip[] = { 3400, 3288, 3000, 3613 };
    endsamp -= skip[nummer];

    FILE *f;
    if(dumpastext) {
      char fname[100];
      sprintf(fname,"c:\\temp\\%s",*argv);
      f = fopen(fname,"w");
    }
    for(i = startsamp; i <= endsamp; i++) {
//      cout << data[i].chan[0] << " " <<  data[i].chan[1] << endl;
      if(dumpastext)
        fprintf(f,"%d %d\n",i,data[i].chan[0]);
      rc = outfile.WriteStereoSample(data[i].chan[0],data[i].chan[1]);
      if( rc != DDC_SUCCESS ) {
        cerr << "Error writing " << outFilename << ERRORSTR;
        exit(-1);
      }
      if(i % 100000 == 0)
        cerr << i * 100 / totalsamp << "%\r";
    }
    if(dumpastext)
      fclose(f);
    nummer++;
    infile.Close();
    lastsamp = data[endsamp];
    delete[] data;
    firstfile = false;
  }

  outfile.Close();

  return 0;
}



