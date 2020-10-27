#include "stdafx.h"
#include "ShowGrafDoc.h"

#if defined(_DEBUG)
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

AxisOptions::AxisOptions() {
  m_type            = AXIS_LINEAR;
  m_color           = BLACK;
  m_showValues      = true;
  m_showValueMarks  = true;
  m_showGridLines   = false;
  m_relativeToFirst = false;
  m_reader          = &DataReader::LinearDataReader;
}

InitialOptions::InitialOptions() {
  m_backgroundColor  = WHITE;
  m_graphStyle       = GSCURVE;
  m_trigoMode        = RADIANS;
  m_ignoreErrors     = false;
  m_onePerLine       = false;
  m_rangeSpecified   = false;
  m_explicitRange.init(-10,10,-10,10);
  m_rollingAvg       = RollingAvg::s_default;
}

void CShowGrafDoc::init() {
  TCHAR **argv       = __targv, *cp;
  int     argc       = __argc;
  argvExpand(argc,argv);

#define AXISOPT(ch)         m_options.m_axisOptions[((#@ch)=='x')?XAXIS_INDEX:YAXIS_INDEX]
#define SETBOTHAXISOPT(f,v) AXISOPT(x).f = AXISOPT(y).f = (v)

  for(argv++;*argv && *(cp = *argv) == '-'; argv++) {
    for(cp++;*cp;cp++) {
      bool axisOptSet = false;
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
        if(cp[1] == 'x') {
          AXISOPT(x).m_showGridLines = true; cp++; axisOptSet = true;
        }
        if(cp[1] == 'y') {
          AXISOPT(y).m_showGridLines = true; cp++; axisOptSet = true;
        }
        if(!axisOptSet) {
          SETBOTHAXISOPT(m_showGridLines, true);
        }
      case 'R':
        if(cp[1] == 'x') {
          AXISOPT(x).m_relativeToFirst = true;
          cp++;
        }
        if(cp[1] == 'y') {
          AXISOPT(y).m_relativeToFirst = true;
          cp++;
        };
        continue;

      case 'r':
        { m_options.m_rollingAvg.setEnabled(true);
          UINT queueSize;
          if(_stscanf(cp + 1, _T("%u"), &queueSize) == 1) {
            m_options.m_rollingAvg.setQueueSize(queueSize);
            break;
          }
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
          AxisOptions &opt = AXISOPT(x);
          opt.m_type   = AXIS_LOGARITHMIC;
          opt.m_reader = &DataReader::LogarithmicDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          AxisOptions &opt = AXISOPT(y);
          opt.m_type   = AXIS_LOGARITHMIC;
          opt.m_reader = &DataReader::LogarithmicDataReader;
          cp++;
        };
        continue;
      case 'N':
        if(cp[1] == 'x') {
          AxisOptions &opt = AXISOPT(x);
          opt.m_type   = AXIS_NORMAL_DISTRIBUTION;
          opt.m_reader = &DataReader::NormalDistributionDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          AxisOptions &opt = AXISOPT(y);
          opt.m_type   = AXIS_NORMAL_DISTRIBUTION;
          opt.m_reader = &DataReader::NormalDistributionDataReader;
          cp++;
        };
        continue;

      case 'D':
        if(cp[1] == 'x') {
          AxisOptions &opt = AXISOPT(x);
          opt.m_type   = AXIS_DATE;
          opt.m_reader = &DataReader::DateTimeDataReader;
          cp++;
        }
        if(cp[1] == 'y') {
          AxisOptions &opt = AXISOPT(y);
          opt.m_type   = AXIS_DATE;
          opt.m_reader = &DataReader::DateTimeDataReader;
          cp++;
        };
        continue;

      case 'c':
        if(!(*(++argv))) {
          usage();
        } else {
          const ColorName *bc = findColorByName(*argv);
          if(bc == nullptr) {
            usage();
          }
          __assume(bc);
          m_options.m_backgroundColor = bc->m_color;
        }
        if(!(*(++argv))) {
          usage();
        } else {
          const ColorName *ac = findColorByName(*argv);
          if(ac == nullptr) {
            usage();
          }
          __assume(ac);
          SETBOTHAXISOPT(m_color, ac->m_color);
        }
        break;

      default:
        usage();
      }
      break;
    }
  }
  m_argvFileNames = argv;
}

void CShowGrafDoc::addArgvGraphs() {
  TCHAR **argv = m_argvFileNames;
  if(*argv == nullptr) {
    addInitialDataGraph(_T("stdin"), getColor(0));
  } else {
    for(int colorIndex = 0; *argv && isOK(); argv++) {
      if (addInitialDataGraph(*argv, getColor(colorIndex))) {
        colorIndex++;
      }
    }
  }
  if(m_graphArray.size() == 1) {
    m_graphArray.select(GRAPHSELECTED, 0);
  }
}

bool CShowGrafDoc::addInitialDataGraph(const String &name, COLORREF color) {
  DataGraphParameters param(name
                           ,color
                           ,m_options.m_onePerLine
                           ,m_options.m_ignoreErrors
                           ,AXISOPT(x).m_relativeToFirst
                           ,AXISOPT(y).m_relativeToFirst
                           ,*AXISOPT(x).m_reader
                           ,*AXISOPT(y).m_reader
                           ,m_options.m_rollingAvg
                           ,m_options.m_graphStyle);
  Graph *g = new DataGraph(getSystem(), param); TRACE_NEW(g);
  if(!g->isEmpty()) {
    m_graphArray.add(g);
    return true;
  } else {
    SAFEDELETE(g);
    return false;
  }
}

void CShowGrafDoc::initScaleIfSingleGraph() {
  if(m_graphArray.size() == 1) {
    getView()->initScale();
  }
}

CShowGrafView *CShowGrafDoc::getView() const {
  return theApp.getMainWindow()->getView();
}

CCoordinateSystem &CShowGrafDoc::getSystem() const {
  return getView()->getCoordinateSystem();
}

CShowGrafDoc::~CShowGrafDoc() {
  clear();
}

bool CShowGrafDoc::refreshFiles() {
  if(m_graphArray.needRefresh()) {
    m_graphArray.refresh();
    return true;
  }
  return false;
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
  showWarning(_T("%s is not a valid format"), fileName.cstr());
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
  Graph *g = new DataGraph(param); TRACE_NEW(g);
  if(!g->isEmpty()) {
    m_graphArray.add(g);
    initScaleIfSingleGraph();
  } else {
    SAFEDELETE(g);
  }
*/
}

void CShowGrafDoc::readFunctionFile(const String &fileName) {
  FunctionGraphParameters param;
  param.load(fileName);
  Graph *g = new FunctionGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readParametricFile(const String &fileName) {
  ParametricGraphParameters param;
  param.load(fileName);
  Graph *g = new ParametricGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readIsoFile(const String &fileName) {
  IsoCurveGraphParameters param;
  param.load(fileName);
  Graph *g = new IsoCurveGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::readDiffEqFile(const String &fileName) {
  DiffEquationGraphParameters param;
  param.load(fileName);
  Graph *g = new DiffEquationGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addFunctionGraph(const FunctionGraphParameters &param) {
  Graph *g = new FunctionGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  m_graphArray.select(GRAPHSELECTED, m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addParametricGraph(const ParametricGraphParameters &param) {
  Graph *g = new ParametricGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  m_graphArray.select(GRAPHSELECTED, m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addIsoCurveGraph(const IsoCurveGraphParameters &param) {
  Graph *g = new IsoCurveGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  m_graphArray.select(GRAPHSELECTED, m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::addDiffEquationGraph(const DiffEquationGraphParameters &param) {
  Graph *g = new DiffEquationGraph(getSystem(), param); TRACE_NEW(g);
  m_graphArray.add(g);
  m_graphArray.select(GRAPHSELECTED, m_graphArray.size()-1);
  initScaleIfSingleGraph();
}

void CShowGrafDoc::setRollingAvg(const RollingAvg &rollingAvg) {
  m_options.m_rollingAvg = rollingAvg;
  m_graphArray.setRollingAvg(m_options.m_rollingAvg);
}

#if defined(_DEBUG)
void CShowGrafDoc::AssertValid() const {
  CDocument::AssertValid();
}

void CShowGrafDoc::Dump(CDumpContext& dc) const {
  CDocument::Dump(dc);
}
#endif //_DEBUG
