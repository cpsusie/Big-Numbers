#include "stdafx.h"

void SyntaxNode::init(int token) {
  m_token = token;
  m_userattributes.m_data = nullptr;
}

SyntaxNode::SyntaxNode(const TCHAR *s, int token) {
  init(token);
  m_attributes.m_str = new String(s);
}

SyntaxNode::SyntaxNode(double number) {
  init(NUMBER);
  m_attributes.m_number = number;
}

SyntaxNode::SyntaxNode(const Date &d) {
  init(DATECONST);
  m_attributes.m_date   = new Date(d);
}

SyntaxNode::SyntaxNode(const Time &d) {
  init(TIMECONST);
  m_attributes.m_time   = new Time(d);
}

SyntaxNode::SyntaxNode(const Timestamp &d) {
  init(TIMESTAMPCONST);
  m_attributes.m_timestamp = new Timestamp(d);
}

SyntaxNode::SyntaxNode(int token, va_list argptr) {
  SyntaxNode *tmp[100],**children,*help; // we expect no more than 100 parameters !!
  init(token);
  int n = 0;
  for(help = va_arg(argptr,SyntaxNode *); help; help = va_arg(argptr,SyntaxNode *))
    tmp[n++] = help;
  children = n ? new SyntaxNode*[n] : nullptr;
  for(int i = 0; i < n; i++)
    children[i] = tmp[i];
  m_attributes.m_children.m_count = n;
  m_attributes.m_children.m_child = children;
}

SyntaxNode::SyntaxNode(const SyntaxNode *src) {
  m_token          = src->m_token;
  m_pos            = src->m_pos;
  m_userattributes = src->m_userattributes;
  int n,i;
  switch(src->token()) {
  case NAME          :
  case PARAM         :
  case HOSTVAR       :
  case STRING        :
    m_attributes.m_str       = new String(*src->m_attributes.m_str);
    break;
  case NUMBER        :
    m_attributes.m_number    = src->m_attributes.m_number;
    break;
  case DATECONST     :
    m_attributes.m_date      = new Date(*src->m_attributes.m_date);
    break;
  case TIMECONST     :
    m_attributes.m_time      = new Time(*src->m_attributes.m_time);
    break;
  case TIMESTAMPCONST:
    m_attributes.m_timestamp = new Timestamp(*src->m_attributes.m_timestamp);
    break;
  default            :
    n = src->childCount();
    m_attributes.m_children.m_count = n;
    if(n == 0)
      m_attributes.m_children.m_child = nullptr;
    else {
      m_attributes.m_children.m_child = new SyntaxNode*[n];
      for(i = 0; i < n; i++)
        m_attributes.m_children.m_child[i] = src->m_attributes.m_children.m_child[i];
    }
    break;
  }
}

SyntaxNode::~SyntaxNode() {
  switch(m_token) {
  case NAME          :
  case PARAM         :
  case HOSTVAR       :
  case STRING        :
    delete m_attributes.m_str;
    break;
  case NUMBER        :
    break;
  case DATECONST     :
    delete m_attributes.m_date;
    break;
  case TIMECONST     :
    delete m_attributes.m_time;
    break;
  case TIMESTAMPCONST:
    delete m_attributes.m_timestamp;
    break;
  default            :
    delete[] m_attributes.m_children.m_child; // NB! do NOT delete children
    break;
  }
}

const TCHAR *SyntaxNode::name() const {
  if(m_token != NAME)
    throwSqlError( SQL_FATAL_ERROR, _T("name:SyntaxNode not type NAME"));
  return m_attributes.m_str->cstr();
}

const TCHAR *SyntaxNode::param() const {
  if(m_token != PARAM)
    throwSqlError( SQL_FATAL_ERROR, _T("param:SyntaxNode not type PARAM"));
  return m_attributes.m_str->cstr();
}

const TCHAR *SyntaxNode::hostvar() const {
  if(m_token != HOSTVAR)
    throwSqlError( SQL_FATAL_ERROR, _T("hostvar:SyntaxNode not type HOSTVAR"));
  return m_attributes.m_str->cstr();
}

const TCHAR *SyntaxNode::str() const {
  if(m_token != STRING)
    throwSqlError(SQL_FATAL_ERROR, _T("str:SyntaxNode not type STRING"));
  return m_attributes.m_str->cstr();
}

double SyntaxNode::number() const {
  if(m_token != NUMBER)
    throwSqlError(SQL_FATAL_ERROR, _T("number:SyntaxNode not type NUMBER"));
  return m_attributes.m_number;
}

const Date &SyntaxNode::getDate() const {
  if(m_token != DATECONST)
    throwSqlError(SQL_FATAL_ERROR, _T("getDate:SyntaxNode not type DATECONST"));
  return *m_attributes.m_date;
}

const Time &SyntaxNode::getTime() const {
  if(m_token != TIMECONST)
    throwSqlError(SQL_FATAL_ERROR, _T("number:SyntaxNode not type TIMECONST"));
  return *m_attributes.m_time;
}

const Timestamp &SyntaxNode::getTimestamp() const {
  if(m_token != TIMESTAMPCONST)
    throwSqlError(SQL_FATAL_ERROR, _T("getTimestamp:SyntaxNode not type TIMESTAMPCONST"));
  return *m_attributes.m_timestamp;
}

void SyntaxNode::setHostVarIndex(int hostvarindex) {
  if(m_token != HOSTVAR)
    throwSqlError(SQL_FATAL_ERROR, _T("setHostVarIndex:SyntaxNode not type HOSTVAR"));
  m_userattributes.m_hostvarindex = hostvarindex;
}

int SyntaxNode::getHostVarIndex() const {
  if(m_token != HOSTVAR)
    throwSqlError(SQL_FATAL_ERROR, _T("getHostVarIndex:SyntaxNode not type HOSTVAR"));
  return m_userattributes.m_hostvarindex;
}

void SyntaxNode::setLikeOperatorIndex(int index) {
  if(m_token != LIKE)
    throwSqlError(SQL_FATAL_ERROR, _T("setLikeOperatorIndex:SyntaxNode not LIKE"));
  m_userattributes.m_likeoperatorindex = index;
}

int SyntaxNode::getLikeOperatorIndex() const {
  if(m_token != LIKE)
    throwSqlError(SQL_FATAL_ERROR, _T("getLikeOperatorIndex:SyntaxNode not LIKE"));
  return m_userattributes.m_likeoperatorindex;
}

void SyntaxNode::setData(SyntaxNodeData *p) {
  m_userattributes.m_data = p;
}

SyntaxNodeData *SyntaxNode::getData() const {
  return m_userattributes.m_data;
}

SyntaxNodeData::SyntaxNodeData(SyntaxNode *node) {
  m_node = node;
  m_node->setData(this);
}

static void thrownochilderror(const SyntaxNode *n, int i) {
  throwSqlError(SQL_FATAL_ERROR,_T("No child(%d) in SyntaxNode (line %d, token %d)")
                               ,i,n->pos().getLineNumber(),n->token());
}

SyntaxNode *SyntaxNode::child(UINT i) const {
  switch(m_token) {
  case NAME      :
  case PARAM     :
  case HOSTVAR   :
  case STRING    :
  case NUMBER    :
  case DATECONST :
  case TIMECONST :
  case TIMESTAMPCONST:
    thrownochilderror(this,i);
    break;
  }
  if(i >= m_attributes.m_children.m_count) {
    thrownochilderror(this,i);
  }
  return m_attributes.m_children.m_child[i];
}

SyntaxNode *SyntaxNode::findChild(int searchtoken) const {
  switch(m_token) {
  case NAME      :
  case PARAM     :
  case HOSTVAR   :
  case STRING    :
  case NUMBER    :
  case DATECONST :
  case TIMECONST :
  case TIMESTAMPCONST:
    throwSqlError(SQL_FATAL_ERROR,_T("No children in findChild(%d) in SyntaxNode (line %d, token %d)")
                                 ,searchtoken,m_pos.getLineNumber(),m_token);
    break;
  }
  for(int i = 0; i < m_attributes.m_children.m_count; i++) {
    SyntaxNode *p = m_attributes.m_children.m_child[i];
    if(p->token() == searchtoken)
      return p;
  }
  return nullptr;
}

UINT SyntaxNode::childCount() const {
  switch(m_token) {
  case NAME      :
  case PARAM     :
  case HOSTVAR   :
  case STRING    :
  case NUMBER    :
  case DATECONST :
  case TIMECONST :
  case TIMESTAMPCONST:
    return 0;
  default:
    return m_attributes.m_children.m_count;
  }
}
