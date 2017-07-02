#include "stdafx.h"

static void usage() {
  _ftprintf(stderr,_T("Usage:average files...\n"));
  exit(-1);
}

class Point {
public:
  double x,y;
  Point() {
    x = y = 0;
  }
  Point(double x, double y) {
    this->x = x;
    this->y = y;
  }
};

class DataSet {
public:
  String fileName;
  Array<Point> data;
};

int _tmain(int argc, TCHAR **argv) {
  argvExpand(argc,argv);

  TCHAR *cp;
  for(argv++; *argv && *(cp = *argv) == '-'; argv++) {
    for(cp++; *cp; cp++) {
      switch(*cp) {
      case ' ':
      default:
        usage();
      }
    }
  }

  Array<DataSet> data;
  for(;*argv; argv++) {
    FILE *f = FOPEN(*argv,"r");

    TCHAR line[100];
    int lineCount = 0;

    DataSet fileData;
    fileData.fileName = *argv;

    while(FGETS(line,ARRAYSIZE(line),f) != NULL) {
      lineCount++;
      Point p;
      if(_stscanf(line,_T("%le %le"),&p.x,&p.y) != 2) {
        _ftprintf(stderr,_T("Error in %s line %d:%s"),*argv,lineCount,line);
        exit(-1);
      }
      fileData.data.add(p);
    }
    data.add(fileData);
    fclose(f);
  }

  if(data.size() == 0) {
    _ftprintf(stderr,_T("No files specified\n"));
    exit(-1);
  }

  const size_t dataCountPerFile = data[0].data.size();

  Array<Point> result = data[0].data;
  int dataSetCount = 1;

  for(size_t i = 1; i < data.size(); i++) {
    const DataSet &dataSet = data[i];
    const Array<Point> &points = dataSet.data;
    if(points.size() != dataCountPerFile) {
      _ftprintf(stderr,_T("Wrong number of data specified for file <%s>. Data ignored\n"), dataSet.fileName.cstr());
      continue;
    }
    for(int d = 0; d < dataCountPerFile; d++) {
      if(points[d].x != result[d].x) {
        _ftprintf(stderr,_T("Warning:x-value[%d] for file <%s> is not equal to x-value[%d] for file <%s>\n"),d,dataSet.fileName.cstr(),d,data[0].fileName.cstr());
      }
      result[d].y += points[d].y;
    }
    dataSetCount++;
  }
  for(int d = 0; d < dataCountPerFile; d++) {
    result[d].y /= dataSetCount;
  }

  for(size_t i = 0; i < result.size(); i++) {
    _tprintf(_T("%.8le %.8le\n"), result[i].x, result[i].y);
  }

  return 0;
}
