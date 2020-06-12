#include "stdafx.h"
#include <ByteMemoryStream.h>
#include "SqlRegex.h"

bool isLike(const SqlRegex &regex, const String &pattern) {
  return regex.match(pattern);
}

bool isLike(const String &str, const String &pattern) {
  SqlRegex regex(pattern);
  return regex.match(str);
}

String sqlSubString(const String &str, int from, int len) {
  if(len <= 0) {
    return EMPTYSTRING;
  }
  from--; // characterposition is indexed from 0 in C++ and 1 in SQL
  if(from < 0) {
    from = 0;
  }
  if(from > (int)str.length()) {
    return EMPTYSTRING;
  }

  return substr(str,from,len);
}

TupleField sqlSubString(const TupleField &v1, const TupleField &v2, const TupleField &v3) {
  if(!v1.isDefined() || !v2.isDefined() || !v3.isDefined()) {
    return TupleField();
  }
  String str;
  int from, len;
  v1.get(str);
  v2.get(from);
  v3.get(len);
  TupleField result;
  result = sqlSubString(str,from,len);
  return result;
}

TupleField sqldate(const TupleField &v1, const TupleField &v2, const TupleField &v3) {
  if(!v1.isDefined() || !v2.isDefined() || !v3.isDefined()) {
    return TupleField();
  }
  int dd, mm, yyyy;
  v1.get(dd);
  v2.get(mm);
  v3.get(yyyy);
  TupleField result;
  result = Date(dd,mm,yyyy);
  return result;
}

TupleField sqltime(const TupleField &v1, const TupleField &v2, const TupleField &v3) {
  if(!v1.isDefined() || !v2.isDefined() || !v3.isDefined()) {
    return TupleField();
  }
  int hh, mm, ss;
  v1.get(hh);
  v2.get(mm);
  v3.get(ss);
  TupleField result;
  result = Time(hh,mm,ss);
  return result;
}

TupleField sqltimestamp(const TupleField &v1, const TupleField &v2, const TupleField &v3,
                        const TupleField &v4, const TupleField &v5, const TupleField &v6) {
  if(!v1.isDefined() || !v2.isDefined() || !v3.isDefined() || !v4.isDefined() || !v5.isDefined() || !v6.isDefined()) {
    return TupleField();
  }
  int dd, MM,yyyy, hh, mm, ss;
  v1.get(dd);
  v2.get(MM);
  v3.get(yyyy);
  v4.get(hh);
  v5.get(mm);
  v6.get(ss);
  TupleField result;
  result = Timestamp(dd, MM, yyyy, hh, mm, ss);
  return result;
}

TupleField sqlcast(const TupleField &v, const CastParameter &p) {
  if(!v.isDefined()) {
    return TupleField();
  }

  TupleField result(v);
  result.setType(p.getType());
  switch(p.m_type) {
  case DBTYPE_CSTRING :
  case DBTYPE_WSTRING :
    { String s;
      result.get(s);
      if((int)s.length() > p.m_len) {
        result = left(s,p.m_len);
      }
      break;
    }
  case DBTYPE_VARCHAR:
    { varchar vch,tmp;
      result.get(vch);
      if((int)vch.len() > p.m_len) {
        tmp.setdata(p.m_len,vch.data());
        result = tmp;
      }
      break;
    }
  }
  return result;
}

TupleField sqlcast(const TupleField &v, const void *adr) {
  Packer pack;
  pack.read(ByteMemoryInputStream((const BYTE*)adr));
  CastParameter p;
  pack >> p;
  return sqlcast(v,p);
}

int VirtualMachine::popInt() {
  TupleField f = m_stack.pop();
  int i;
  f.get(i);
  return i;
}

String VirtualMachine::popString() {
  TupleField f = m_stack.pop();
  String s;
  f.get(s);
  return s;
}

TupleField VirtualMachine::isLike(UINT adr, const TupleField &str, const TupleField &pattern) {
  if(adr >= m_sqlregex.size()) {
    for(size_t i = m_sqlregex.size(); i <= adr; i++) {
      m_sqlregex.add(SqlRegex());
    }
  }
  if(!str.isDefined() || !pattern.isDefined()) {
    return TupleField();
  }
  String pat,s;
  pattern.get(pat);
  str.get(s);
  bool result = m_sqlregex[adr].isLike(s.cstr(),pat.cstr());
  return TupleField(result?1:0);
}

VirtualMachine::VirtualMachine(DbEngine &engine) : m_engine(engine) {
  m_tuplereg[0] = m_tuplereg[1] = NULL;
  m_cursor[0]   = m_cursor[1]   = NULL;
}

VirtualMachine::~VirtualMachine() {
  for(int i = 0; i < ARRAYSIZE(m_tuplereg);i++) {
    tupleDestroy(i);
  }
  for(int i = 0; i < ARRAYSIZE(m_cursor); i++) {
    closeCursor(i);
  }
}

void VirtualMachine::closeCursor(int reg) {
  if(m_cursor[reg] != NULL) {
    delete m_cursor[reg];
    m_cursor[reg] = NULL;
  }
}

void VirtualMachine::openCursor(int reg, const void *adr) {
  closeCursor(reg);
  Packer pack;
  pack.read(ByteMemoryInputStream((const BYTE*)adr));
  TableCursorParam param;
  pack >> param;
  m_cursor[reg] = new TableCursor(m_engine.m_db,param,m_tuplereg[0],m_tuplereg[1]);
}

void VirtualMachine::fetchCursor(int reg, int tup) {
  if(!m_cursor[reg]->hasNext()) {
    m_status = 1;
  } else {
    m_cursor[reg]->next(*m_tuplereg[tup]);
    m_status = 0;
  }
//  _tprintf(_T("fetched:<")); m_tuplereg[tup]->dump(); _tprintf(_T(">\n "));
}


void VirtualMachine::checkRange(double minvalue, double maxvalue) {
  double v;
  m_stack.top().get(v);
  if(v < minvalue || v > maxvalue) {
    throwSqlError(SQL_DOMAIN_ERROR,_T("Expression-overflow/underflow"));
  }
}

#if defined(__NEVER__)
static void checkhostvartype(sqlapi_varlist &hostvar, DbFieldType expectedtype) {
  /* It should be checked, that the hostvar-type is the same as in the
     SqlApiBindStmt. The user, could have changed the type, in the file generated
     by sqlprep, or generated wrong code with his own preprocessor.
     So we have to check it here too
  */
  if(hostvar.getType() != expectedtype && hostvar.getType() != expectedtype + 1) // the NULL-allowed-type
   throwSqlError(SQL_INVALID_HOSTVAR_TYPE,_T("Invalid type of hostvar (=%d. expected %d)"),
                                          hostvar.getType(),expectedtype);
}
#endif

static void executeTableCreate(Database &db, const void *adr) {
  Packer pack;
  pack.read(ByteMemoryInputStream((const BYTE *)adr));
  TableDefinition tableDef;
  IndexDefinition indexDef;
  pack >> tableDef >> indexDef;
  tableDef.dump();
  indexDef.dump();
  db.tableCreate(tableDef);
  db.indexCreate(indexDef);
}

static void executeIndexCreate(Database &db, const void *adr) {
  Packer pack;
  pack.read(ByteMemoryInputStream((const BYTE*)adr));
  CreateIndexData inx;
  pack >> inx;

  sqlCreateIndex(db,inx);
}

void VirtualMachine::tupleDestroy(int reg) {
  if(m_tuplereg[reg] != NULL) {
    delete m_tuplereg[reg];
    m_tuplereg[reg] = NULL;
  }
}

void VirtualMachine::tupleInit(int reg, UINT size) {
  tupleDestroy(reg);
  m_tuplereg[reg] = new Tuple(size);
}

void VirtualMachine::tupleInsert(int reg, int sequenceNo) {
  TableRecord record(m_engine.m_db,sequenceNo);
  record.insert(*m_tuplereg[reg]);
}

void VirtualMachine::cleanup() {
  if(m_engine.m_db.inTMF()) {
    try {
      m_engine.m_db.trabort();
    } catch(sqlca) {}
  }
  if(m_engine.m_pipelines.size() > 0) {
    m_engine.m_pipelines[0]->writeeof();
  }
}

void VirtualMachine::run(int entrypoint) {
  TupleField v1,v2,v3,v4,v5,v6;
  const VirtualCode &vc    = m_engine.m_vc;
  Database          &db    = m_engine.m_db;
  UINT               pcreg = entrypoint;
  for(;;) {
    Instruction ins          = *(Instruction*)vc.getCode(pcreg);
    UINT        currentpcreg = pcreg;
    pcreg += ins.size();
    try {
      switch(ins.opcode()) {
      case CODECRTAB      :
        executeTableCreate(db,vc.data(popInt()));
        break;

      case CODECRLIK      :
        { String src = popString();
          String dst = popString();
          db.tableCreateLike(dst,src,EMPTYSTRING);
          break;
        }

      case CODEDRTAB      :
        db.tableDrop(popString());
        break;

      case CODECRINX      :
        executeIndexCreate(db,vc.data(popInt()));
        break;

      case CODEDRINX      :
        db.indexDrop(popString());
        break;

      case CODERETURN     :
        return;

      case CODETRBEGIN    :
        db.trbegin();
        break;

      case CODETRCOMMIT   :
        db.trcommit();
        break;

      case CODETRABORT    :
        db.trabort();
        break;

      case CODETUPINIT    :
        tupleInit(ins.reg(),ins.index());
        break;

      case CODETUPINSERT  :
        tupleInsert(ins.reg(),popInt());
        break;

      case CODEPOPTUP     :
        { Tuple &tuple = *(m_tuplereg[ins.reg()]);
          tuple[ins.index()] = m_stack.pop();
          break;
        }

      case CODEPUSHTUP    :
        { Tuple &tuple = *(m_tuplereg[ins.reg()]);
          m_stack.push(tuple[ins.index()]);
          break;
        }

      case CODEPUSHHV     :
        v1 = m_engine.m_hostvar[ins.addr()];
        m_stack.push(v1);
        break;

      case CODEPUSHCONST  :
        m_stack.push(vc.getConst(ins.addr()));
        break;

      case CODEPUSHADR    :
        v1 = ins.addr();
        m_stack.push(v1);
        break;

      case CODEAND        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 && v1);
        break;

      case CODEOR         :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 || v1);
        break;

      case CODENOT        :
        m_stack.top() = !m_stack.top();
        break;

      case CODEADD        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 + v1);
        break;

      case CODESUB         :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 - v1);
        break;

      case CODEMULT        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 * v1);
        break;

      case CODEDIV         :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 / v1);
        break;

      case CODEMOD         :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 % v1);
        break;

      case CODEEXPO        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(sqlPow(v2,v1));
        break;

      case CODENEG         :
        m_stack.top() = -m_stack.top();
        break;

      case CODECONCAT      :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(concat(v2, v1));
        break;

      case CODESUBSTR      :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        v3 = m_stack.pop();
        m_stack.push(sqlSubString(v3,v2,v1));
        break;

      case CODEDATE        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        v3 = m_stack.pop();
        m_stack.push(sqldate(v3,v2,v1));
        break;

      case CODETIME        :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        v3 = m_stack.pop();
        m_stack.push(sqltime(v3,v2,v1));
        break;

      case CODETIMESTAMP   :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        v3 = m_stack.pop();
        v4 = m_stack.pop();
        v5 = m_stack.pop();
        v6 = m_stack.pop();
        m_stack.push(sqltimestamp(v6,v5,v4,v3,v2,v1));
        break;

      case CODECAST        :
        m_stack.top() = sqlcast(m_stack.top(),vc.data(ins.addr()));
        break;

      case CODEPUSHNULL        :
        m_stack.push(TupleField());
        break;

      case CODEPUSHCURRENTDATE:
        v1 = Date();
        m_stack.push(v1);
        break;

      case CODEPUSHCURRENTTIME:
        v1 = Time();
        m_stack.push(v1);
        break;

      case CODEPUSHCURRENTTIMESTAMP:
        v1 = Timestamp();
        m_stack.push(v1);
        break;

      case CODEOPENCURSOR  :
        openCursor(ins.reg(),vc.data(popInt()));
        break;

      case CODEFETCHCURSOR :
        fetchCursor(ins.reg(),ins.index());
        break;

      case CODESENDTUP     :
//        _tprintf(_T("sendtup %d til pipe %d\n"),ins.reg(),ins.index());
        m_engine.m_pipelines[ins.index()]->write(*m_tuplereg[ins.reg()]);
        break;

      case CODESENDEOF     :
//        _tprintf(_T("sendeof til pipe %d\n"),ins.addr());
        m_engine.m_pipelines[ins.addr()]->writeeof();
        break;

      case CODERECEIVETUP  :
        m_status = m_engine.m_pipelines[ins.index()]->read(*m_tuplereg[ins.reg()]) ? 0 : 1;
        break;

      case CODENEWOPERATOR :
//        _tprintf(_T("starting new operator at address %d\n"),ins.addr());
        m_engine.m_operators.add(new QueryOperator(m_engine,ins.addr()));
        break;

      case CODEINITPIPES   :
        m_engine.m_pipelines.init(ins.addr());
        break;

      case CODECMP         :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_status = compare(v2,v1);
//        _tprintf(_T("pcreg:%2d: compare("),pcreg); v2.dump(); _tprintf(_T(",")); v1.dump(); _tprintf(_T("):%d\n"),m_status);
        if(m_status == -2) pcreg = ins.addr();
        break;

      case CODECMPTRUE     :
        v1 = m_stack.pop();
        v2 = 1;
        m_status = compare(v1,v2);
//        _tprintf(_T("pcreg:%2d: compare("),pcreg); v2.dump(); _tprintf(_T(",")); v1.dump(); _tprintf(_T("):%d\n"),m_status);
        if(m_status == -2) pcreg = ins.addr();
        break;

      case CODEEQ          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 == v1);
        break;

      case CODENQ          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 != v1);
        break;

      case CODEGE          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 >= v1);
        break;

      case CODEGT          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 > v1);
        break;

      case CODELE          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 <= v1);
        break;

      case CODELT          :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(v2 < v1);
        break;

      case CODEISNULL      :
        v1 = m_stack.pop();
        m_stack.push(v1.isDefined() ? 0 : 1);
        break;

      case CODEISLIKE      :
        v1 = m_stack.pop();
        v2 = m_stack.pop();
        m_stack.push(isLike(ins.addr(),v2,v1));
        break;

      case CODEJMPONDEFINED:
        v1 = m_stack.pop();
        if(v1.isDefined()) pcreg = ins.addr();
        break;

      case CODEJMP         :
        pcreg = ins.addr();
        break;

      case CODEJMPEQ       :
        if(m_status == 0) pcreg = ins.addr();
        break;

      case CODEJMPNQ       :
        if(m_status == 1 || m_status == -1) pcreg = ins.addr();
        break;

      case CODEJMPGE       :
        if(m_status >= 0) pcreg = ins.addr();
        break;

      case CODEJMPGT       :
        if(m_status >  0) pcreg = ins.addr();
        break;

      case CODEJMPLE       :
        if(m_status == 0 || m_status == -1) pcreg = ins.addr();
        break;

      case CODEJMPLT       :
        if(m_status == -1) pcreg = ins.addr();
        break;

      default:
        throwSqlError(SQL_FATAL_ERROR,_T("Unknown opcode:%d\n"),ins.opcode());
        return;
      }
    } catch(sqlca error) {
      m_engine.m_sqlca = error;
      cleanup();
      return;
    } catch(Exception e) {
      m_engine.m_sqlca.seterror(SQL_FATAL_ERROR,_T("Exception in vm.run, pcreg:%d:%s"),currentpcreg,e.what());
      cleanup();
      return;
    } catch(...) {
      m_engine.m_sqlca.seterror(SQL_FATAL_ERROR,_T("Unknown Exception in vm.run, pcreg:%d"),currentpcreg);
      cleanup();
      return;
    }
  }
}

UINT QueryOperator::run() {
  m_vm.run(m_entrypoint);
  return 0;
}

QueryOperator::QueryOperator(DbEngine &engine, int entrypoint) : m_vm(engine) {
  m_entrypoint = entrypoint;
  resume();
}

void OperatorArray::clear() {
  for(size_t i = 0; i < size(); i++)
    delete (*this)[i];
  Array<QueryOperator*>::clear();
}

OperatorArray::~OperatorArray() {
  clear();
}

void PipeLineArray::clear() {
  for(size_t i = 0; i < size(); i++)
    delete (*this)[i];
  Array<PipeLine*>::clear();
}

void PipeLineArray::init(UINT n) {
  clear();
  for(UINT i = 0; i < n; i++)
    add(new PipeLine);
}

PipeLineArray::~PipeLineArray() {
  clear();
}

void DbEngine::run() {
  VirtualMachine vm(*this);
  vm.run();
}

bool DbCursor::fetch(Tuple &tuple) {
  bool ret = m_pipelines[0]->read(tuple);
  if(!ret && m_sqlca.sqlcode == SQL_OK) {
    m_sqlca.seterror(SQL_NOT_FOUND);
  }
  return ret;
}

bool DbCursor::fetch(HostVarList &hostvar) {
  HostVarDescriptionList desc = getDescription();
  if(desc.size() != hostvar.size()) {
    m_sqlca.seterror(SQL_INVALID_NO_OF_COLUMNS,_T("SqlCursor::fetch:Invalid number of columns specified in HostVarList (=%d) expected %zd"),
    hostvar.size(),desc.size());
    return false;
  }
  Tuple t((UINT)desc.size());
  if(!fetch(t)) {
    return false;
  }
  for(UINT i = 0; i < hostvar.size(); i++) {
    t[i].get(hostvar[i]);
  }
  return true;
}

DbCursor::~DbCursor() {
}

void sqlExecute(   Database                  &db        ,
                   const VirtualCode         &vc        ,
                   const HostVarList         &hostvar   ,
                   sqlca                     &ca        ) {

  DbEngine engine(db, vc, hostvar);
  engine.run();
  ca = engine.m_sqlca;
}

void sqlExecute(   Database                  &db        ,
                   const String              &stmt      ,
                   StringArray               &errmsg    ,
                   sqlca                     &ca        ) {

  VirtualCode vc;
  HostVarList hostvar;
  sqlCompile(db, stmt, vc, errmsg, ca);
  if(ca.sqlcode == SQL_OK) {
    DbEngine engine(db, vc, hostvar);
    engine.run();
    ca = engine.m_sqlca;
  }
}

SqlCursor::SqlCursor(Database &db, const String &stmt) : m_db(db) {
  m_cursor = NULL;
  try {
    sqlCompile(db, stmt, m_vc, m_errmsg, m_sqlca);
    if(m_sqlca.sqlcode == SQL_OK) {
      m_cursor = new DbCursor(m_db, m_vc, m_hostvar);
    }
  } catch(sqlca ca) {
    m_sqlca = ca;
  }
}

SqlCursor::~SqlCursor() {
  delete m_cursor;
}

bool SqlCursor::fetch(Tuple &tuple) {
  if(m_cursor != NULL) {
    if(m_cursor->m_sqlca.sqlcode != SQL_OK) {
      m_sqlca = m_cursor->m_sqlca;
      return false;
    } else {
      bool ret = m_cursor->fetch(tuple);
      if(!ret) {
        m_sqlca = m_cursor->m_sqlca;
      }
      return ret;
    }
  } else {
    m_sqlca.seterror(SQL_CURSOR_NOT_OPEN,_T("DbCursor not opened"));
    return false;
  }
}
