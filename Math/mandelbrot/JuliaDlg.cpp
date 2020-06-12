#include "stdafx.h"
#include "JuliaDlg.h"

#if defined(_DEBUG)
#define new DEBUG_NEW
#endif

CJuliaDlg::CJuliaDlg(const RealPoint2D &point, CWnd *pParent /*=NULL*/)
: m_point(point)
, CDialog(CJuliaDlg::IDD, pParent)
, m_job(NULL)
{
}

CJuliaDlg::~CJuliaDlg() {
  SAFEDELETE(m_job);
}

BEGIN_MESSAGE_MAP(CJuliaDlg, CDialog)
END_MESSAGE_MAP()

BOOL CJuliaDlg::OnInitDialog() {
  __super::OnInitDialog();

  const RealRectangle2D rr(-2,-2,4,4);

  m_transform.setFromRectangle(rr);
  adjustToRectangle();

  m_job = new JuliaCalculatorJob(*this); TRACE_NEW(m_job);
  ThreadPool::executeNoWait(*m_job);
  return TRUE;
}

void CJuliaDlg::adjustToRectangle() {
  CRect cl = getClientRect(this);
//  const int t = cl.bottom;
//  cl.bottom = cl.top;
//  cl.top = t;
  m_transform.setToRectangle(toRealRect(cl));
}

JuliaCalculatorJob::JuliaCalculatorJob(CJuliaDlg &dlg) : m_dlg(dlg) {
  m_killed = false;
}

JuliaCalculatorJob::~JuliaCalculatorJob() {
  kill();
  waitUntilJobDone();
}

void JuliaCalculatorJob::kill() {
  m_killed = true;
};

UINT JuliaCalculatorJob::safeRun() {
  const RealPoint2D                  c  = m_dlg.getC();
  const RealRectangleTransformation &tr = m_dlg.getTransform();

  RealPoint2D z = c;
#define POINTBUFFERSIZE 10000
  CPoint pointBuffer[POINTBUFFERSIZE];
  int index = 0;
  while(!m_killed) {
    pointBuffer[index++] = toCPoint(tr.forwardTransform(z));
    if(index == POINTBUFFERSIZE) {
      CClientDC dc(&m_dlg);
      for(const CPoint *p = pointBuffer; index--; p++) {
        dc.SetPixel(p->x,p->y, RGB(0,0,255));
      }
      index = 0;
    }

    const Real x2 = z.x*z.x;
    const Real y2 = z.y*z.y;
    if(x2+y2 > 4) {
      break;
    }
    const Real tmp = x2-y2-c.x;
    z.y = 2*z.x*z.y-c.y;
    z.x = tmp;
  }
  if(!m_killed && (index > 0)) {
    CClientDC dc(&m_dlg);
    for(const CPoint *p = pointBuffer; index--; p++) {
      dc.SetPixel(p->x,p->y, RGB(0,0,0));
    }
  }
  return 0;
}
