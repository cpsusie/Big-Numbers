#include "stdafx.h"
#include "SqlCom.h"

SqlApiVarList &copy(SqlApiVarList &dst, const SqlApiVarList &src) {
  memcpy(dst.sqldata,src.sqldata,dst.sqllen);
  if(dst.sqlind && src.sqlind) *dst.sqlind = *src.sqlind;
  return dst;
}

HostVarDescription::HostVarDescription(DbFieldType type, unsigned long len) {
  sqltype = type;
  sqllen  = len;
}

Packer &operator<<(Packer &p, const HostVarDescription &h) {
  p << h.sqltype
    << h.sqllen;
  return p;
}

Packer &operator>>(Packer &p, HostVarDescription &h) {
  p >> h.sqltype
    >> h.sqllen;
  return p;
}

String HostVarDescription::toString() const {
  return format(_T("<Type:%s, len:%u>"), getTypeString((DbFieldType)sqltype));
}

// ----------------------------------------------------------------------------------

HostVarList::HostVarList() {
  m_n       = 0;
  m_hostvar = NULL;
}

HostVarList::HostVarList(const HostVarDescriptionList &desc) {
  m_n       = 0;
  m_hostvar = NULL;
  describe(desc);
}

HostVarList::HostVarList(const HostVarList &src) {
  m_n       = 0;
  m_hostvar = NULL;
  describe(src.getdescription());
}

HostVarList &HostVarList::operator=(const HostVarList &src) {
  clear();
  describe(src.getdescription());
  for(unsigned int i = 0; i < m_n; i++)
    copy(m_hostvar[i],src.m_hostvar[i]);
  return *this;
}

void HostVarList::describe(const HostVarDescriptionList &desc) {
  clear();
  m_n       = (UINT)desc.size();
  m_hostvar = new SqlApiVarList[m_n];
  for(UINT i = 0; i < m_n; i++) {
    const HostVarDescription &d = desc[i];
    m_hostvar[i].sqltype = d.sqltype;
    m_hostvar[i].sqldata = new char[d.sqllen];
    m_hostvar[i].sqllen  = d.sqllen;
    m_hostvar[i].sqlind  = new short;
  }
}

// ----------------------------------------------------------------------------------------------

HostVarDescriptionList::HostVarDescriptionList(const SqlApiBindStmt &bndstmt, bool input, bool output) {
  if(input) {
    for(UINT i = 0; i < bndstmt.m_stmtHead.m_ninput; i++) {
      add(HostVarDescription(bndstmt.m_inHost[i].getType(),bndstmt.m_inHost[i].sqllen));
    }
  }
  if(output) {
    for(UINT i = 0; i < bndstmt.m_stmtHead.m_noutput; i++) {
      add(HostVarDescription(bndstmt.m_outHost[i].getType(),bndstmt.m_outHost[i].sqllen));
    }
  }
}

void HostVarDescriptionList::dump(FILE *f) const {
  _ftprintf(f,_T("HostVarDescriptionList:\n"));
  for(size_t i = 0; i < size(); i++) {
    const HostVarDescription &h = (*this)[i];
    _ftprintf(f,_T("%-20s %d\n"),getTypeString(h.getType()),h.sqllen);
  }
}

Packer &operator<<(Packer &p, const HostVarDescriptionList &hl) {
  p << hl.size();
  for(size_t i = 0; i < hl.size(); i++) {
    p << hl[i];
  }
  return p;
}

Packer &operator>>(Packer &p, HostVarDescriptionList &hl) {
  hl.clear();
  UINT n;
  p >> n;
  for(UINT i = 0; i < n; i++) {
    HostVarDescription hd;
    p >> hd;
    hl.add(hd);
  }
  return p;
}

HostVarDescriptionList HostVarList::getdescription() const {
  HostVarDescriptionList desc;
  for(UINT i = 0; i < m_n; i++) {
    desc.add(HostVarDescription(m_hostvar[i].getType(),m_hostvar[i].sqllen));
  }
  return desc;
}

// --------------------------------------------------------------------------------------------

HostVarList::~HostVarList() {
  clear();
}

void HostVarList::clear() {
  if(m_hostvar != NULL) {
    for(unsigned int i = 0; i < m_n; i++) {
      delete[] m_hostvar[i].sqldata;
      delete   m_hostvar[i].sqlind;
    }
    delete[] m_hostvar;
  }
  m_hostvar = NULL;
  m_n       = 0;
}

Packer &operator>>(Packer &p, HostVarList &h) {
  h.clear();
  p >> h.m_n;
  if(h.m_n > 0) {
    h.m_hostvar = new SqlApiVarList[h.m_n];
    SqlApiVarList *v = h.m_hostvar;
    for(unsigned int i = 0; i < h.m_n; i++, v++) {
      v->sqlind = new short;
      p >>  v->sqltype
        >>  v->sqllen
        >> *v->sqlind;
      v->sqldata = (void*)new char[v->sqllen];
      p.getElement(Packer::E_VOID,v->sqldata,v->sqllen);
    }
  }
  return p;
}

Packer &operator<<(Packer &p, const HostVarList &h) {
  p << h.m_n;
  for(unsigned int i = 0; i < h.m_n; i++) {
    p << h.m_hostvar[i];
  }
  return p;
}

// --------------------------------------------------------------------------------------------

Packer &operator<<(Packer &p, const SqlApiVarList &v) {
  p << v.sqltype // rækkefølge skal matche den i HostVarList::receive
    << v.sqllen
    << (v.sqlind ? *v.sqlind : (short)0);
  p.addElement(Packer::E_VOID, v.sqldata, v.sqllen);
  return p;
}

Packer &operator>>(Packer &p,       SqlApiVarList &v) {
  p >> v.sqltype // rækkefølge skal matche den i HostVarList::receive
    >> v.sqllen
    >> *v.sqlind;
  p.getElement(Packer::E_VOID, v.sqldata, v.sqllen);
  return p;
}

void HostVarList::dump(FILE *f) {
  _ftprintf(f,_T("n:%d\n"),m_n);
  for(UINT i = 0; i < m_n; i++) {
    SqlApiVarList &hv = m_hostvar[i];
    _ftprintf(f,_T("  hv[%d]:type:%3d len:%d ind:%hx data:%p\n")
               ,i
               ,hv.sqltype
               ,hv.sqllen
               ,*hv.sqlind
               ,hv.sqldata
             );
  }
}
