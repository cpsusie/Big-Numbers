#include "stdafx.h"
#include "ShowGrafDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CShowGrafDoc, CDocument)

BEGIN_MESSAGE_MAP(CShowGrafDoc, CDocument)
END_MESSAGE_MAP()

CShowGrafDoc::CShowGrafDoc() {
  try {
    init();
  } catch (Exception e) {
    m_errorMsg = e.what();
  }
}

void CShowGrafDoc::setTrigoMode(TrigonometricMode mode) {
  m_options.m_trigoMode = mode;
  getGraphArray().setTrigoMode(mode);
}

InitialOptions::InitialOptions() {
  m_backgroundColor  = RGB(255,255,255);
  m_axisColor        = RGB(0,0,0);
  m_graphStyle       = GSCURVE;
  m_XAxisType        = AXIS_LINEAR;
  m_YAxisType        = AXIS_LINEAR;
  m_xRelativeToFirst = false;
  m_yRelativeToFirst = false;
  m_xReader          = &DataReader::LinearDataReader;
  m_yReader          = &DataReader::LinearDataReader;
  m_trigoMode        = RADIANS;
  m_ignoreErrors     = false;
  m_grid             = false;
  m_onePerLine       = false;
  m_rangeSpecified   = false;
  m_explicitRange.init(-10,10,-10,10);
  m_rollAvg          = false;
  m_rollAvgSize      = 10;
}

void CShowGrafDoc::init() {
  TCHAR **argv       = __targv, *cp;
  int     argc       = __argc;
  argvExpand(argc,argv);

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'p':
        m_options.m_graphStyle   = GSPOINT; continue;
      case 'k':
        m_options.m_graphStyle   = GSCROSS; continue;
      case 'i':
        m_options.m_ignoreErrors = true;    continue;
      case '1':
        m_options.m_onePerLine   = true;    continue;
      case 'g':
        m_options.m_grid         = true;    continue;
      case 'R':
        if(cp[1] == 'x') {
          m_options.m_xRelativeToFirst = true;
          cp++;
        }
        if(cp[1] == 'y') {
          m_options.m_yRelativeToFirst = true;
          cp++;
        };
        continue;

      case 'r':
        m_options.m_rollAvg = true;
        if(_stscanf(cp+1,_T("%d"), &m_options.m_rollAvgSize) == 1) {
          if((m_options.m_rollAvgSize < 1) || (m_options.m_rollAvgSize > 10000)) {
            usage();
          }
          break;
        }
        continue;
      case 'I':
        { double minX, maxX, minY, maxY;
          if(_stscanf(cp+1,_T("%le,%le,%le,%le"),&minX,&maxX,&minY,&maxY) != 4) {
            usage();
          }
          m_options.m_explicitRange.init(minX,maxX,minY,maxY);
          m_options.m_rangeSpecified = true;
          break;
        }
      case 'L':
        if(cp[1] == 'x') {
          m_options.m_XAxisType = AXIS_LOGARITHMIC;
          m_options.m_xReader   = &DataReader::LogarithmicDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_options.m_YAxisType = AXIS_LOGARITHMIC;
          m_options.m_yReader   = &DataReader::LogarithmicDataReader;
          cp++;
        };
        continue;
      case 'N':
        if(cp[1] == 'x') {
          m_options.m_XAxisType = AXIS_NORMAL_DISTRIBUTION;
          m_options.m_xReader   = &DataReader::NormalDistributionDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_options.m_YAxisType = AXIS_NORMAL_DISTRIBUTION;
          m_options.m_yReader   = &DataReader::NormalDistributionDataReader;
          cp++;
        };
        continue;

      case 'D':
        if(cp[1] == 'x') {
          m_options.m_XAxisType = AXIS_DATE;
          m_options.m_xReader   = &DataReader::DateTimeDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          m_options.m_YAxisType = AXIS_DATE;
          m_options.m_yReader   = &DataReader::DateTimeDataReader;
          cp++;
        };
        continue;

      case 'c':
        if(!(*(++argv))) {
          usage();
        } else {
          const ColorName *bc = findColorByName(*argv);
          if(bc == NULL) {
            usage();
          }
          m_options.m_backgroundColor = bc->m_color;
        }
        if(!(*(++argv))) {
          usage(); 
        } else {
          const ColorName *ac = findColorByName(*argv);
          if(ac == NULL) {
            usage();
          }
          m_options.m_axisColor = ac->m_color;
        }
        break;

      default:
        usage();
      }
      break;
    }
  }
  if(*argv == NULL) {
    addInitialDataGraph(_T("stdin"), getColor(0));
  } else {
    for(int colorIndex = 0; *argv && isOK(); argv++) {
      if(addInitialDataGraph(*argv, getColor(colorIndex))) {
        colorIndex++;
      }
    }
  }
  if(m_graphArray.size() == 1) {
    m_graphArray.select(0);
  }
}

bool CShowGrafDoc::addInitialDataGraph(const String &name, COLORREF color) {
  DataGraphParameters param(name
                            ,color
                            ,m_options.m_onePerLine
                            ,m_options.m_ignoreErrors
                            ,m_options.m_xRelativeToFirst
                            ,m_options.m_yRelativeToFirst 
                            ,*m_options.m_xReader
                            ,*m_options.m_yReader
                            ,m_options.m_rollAvg ? m_options.m_rollAvgSize : 0
                            ,m_options.m_graphStyle);
  Graph *g = new DataGraph(param);
  if(!g->isEmpty()) {
    m_graphArray.add(g);
    return true;
  } else {
    delete g;
    return false;
  }
}

void CShowGrafDoc::initScaleIfSingleGraph() {
  if(m_graphArray.size() == 1) {
    getView()->initScale();
  }
}

CShowGrafView *CShowGrafDoc::getView() {
  return theApp.getMainWindow()->getView();
}

CShowGrafDoc::~CShowGrafDoc() {
}

bool CShowGrafDoc::refreshFiles() {
  if(m_graphArray.needRefresh()) {
    m_graphArray.refresh();
    return true;
  }
  return false;
}

BOOL CShowGrafDoc::OnNewDocument() {
  if (!CDocument::OnNewDocument())
    return FALSE;

  return TRUE;
}

void CShowGrafDoc::Serialize(CArchive& ar) {
  if (ar.IsStoring()) {
  } else {
  }
}

void CShowGrafDoc::addGraphFromFile(const String &fileName) {
  for (int i = 0; i < 5; i++) {
    try {
      switch(i) {
      case 0: readParametricFile(fileName); break;
      case 1: readFunctionFile(  fileName); break;
      case 2: readIsoFile(       fileName); break;
      case 3: readDataFile(      fileName); break;
      case 4: readDiffEqFile(    fileName); break;
      }
      return;
    } catch(Exception e) {
    }
  }
  Message(_T("%s is not a valid format"), fileName.cstr());
}

void CShowGrafDoc::readDataFile(const String &fileName) {
  throwUnsupportedOperationException(__TFUNCTION__);
/*
  DataGraphParameters param(fileName
                           ,randomColor()
                           ,m_onePerLine
                           ,isMenuItemChecked(ID_OPTIONS_IGNOREERRORS)
                           ,m_xRelativeToFirst
                           ,m_yRelativeToFirst 
                           ,*m_xReader
                           ,*m_yReader
                           ,isMenuItemChecked(ID_VIEW_ROLLAG) ? m_rollAvgSize : 0
                           ,theApp.getMainWindow()->getGraphStyle());
  Graph *g = new DataGraph(param);
  if(!g->isEmpty()) {
    m_graphArray.add(g);
    initScaleIfSingleGraph();
  } else {
    delete g;
  }
*/
}

void CShowGrafDoc::readFunctionFile(const String &fileName) {
  FunctionGraphParameters param;
  param.load(fileName);
  Graph *g = new FunctionGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readParametricFile(const String &fileName) {
  ParametricGraphParameters param;
  param.load(fileName);
  Graph *g = new ParametricGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readIsoFile(const String &fileName) {
  IsoCurveGraphParameters param;
  param.load(fileName);
  Graph *g = new IsoCurveGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readDiffEqFile(const String &fileName) {
  DiffEquationGraphParameters param;
  param.load(fileName);
  Graph *g = new DiffEquationGraph(param);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addFunctionGraph(const FunctionGraphParameters &param) {
  m_graphArray.add(new FunctionGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addParametricGraph(const ParametricGraphParameters &param) {
  m_graphArray.add(new ParametricGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addIsoCurveGraph(const IsoCurveGraphParameters &param) {
  m_graphArray.add(new IsoCurveGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addDiffEquationGraph(const DiffEquationGraphParameters &param) {
  m_graphArray.add(new DiffEquationGraph(param));
  m_graphArray.select(m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::setRollAvg(bool on) {
  m_options.m_rollAvg = on;
  m_graphArray.setRollAvgSize(on ? m_options.m_rollAvgSize : 0);
}

void CShowGrafDoc::setRollAvgSize(int size) {
  m_options.m_rollAvgSize = size;
  setRollAvg(true);
}

#ifdef _DEBUG
void CShowGrafDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CShowGrafDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
