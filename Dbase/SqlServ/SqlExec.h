#pragma once

#include "SqlRegex.h"

class DbEngine;

class VirtualMachine {
private:
  DbEngine          &m_engine;
  Stack<TupleField>  m_stack;
  Tuple             *m_tuplereg[2];
  TableCursor       *m_cursor[2];
  Array<SqlRegex>    m_sqlregex;
  int                m_status;
  int                popInt();
  String             popString();
  void               tupleInit(  int reg, UINT size);
  TupleField         isLike(     UINT adr, const TupleField &str, const TupleField &pattern);
  void               tupleDestroy( int reg);
  void               tupleInsert(  int reg, int sequenceno);
  void               openCursor(   int reg, const void *adr);
  void               closeCursor(  int reg);
  void               fetchCursor(  int reg, int tup);
  void               checkRange(double minvalue, double maxvalue);
  void               cleanup();
public:
  VirtualMachine(DbEngine &engine);
  ~VirtualMachine();
  void run(int entrypoint = 0);
};

class QueryOperator : public Thread {
private:
  VirtualMachine      m_vm;
  int                 m_entrypoint;
  QueryOperator(const QueryOperator &src);            // Not defined. Class not cloneable
  QueryOperator &operator=(const QueryOperator &src); // Not defined. Class not cloneable
public:
  QueryOperator(DbEngine &engine, int entrypoint = 0);
  UINT run();
};

class PipeLine {
private:
  Tuple *m_buffer;
  int m_head,m_tail,m_count;
  FastSemaphore m_gate,m_fullsem,m_emptysem;
  PipeLine(const PipeLine &pipe);                     // Not defined. Class not cloneable
  PipeLine &operator=(const PipeLine &src);           // Not defined. Class not cloneable
public:
  PipeLine();
  ~PipeLine();
  void write(const Tuple &v);
  void writeeof();
  bool read(Tuple &v);
  bool full() const;
  bool isEmpty() const {
    return m_count == 0;
  }
};

class PipeLineArray : public Array<PipeLine*> {
private:
  void clear();
public:
  void init(UINT n);
  ~PipeLineArray();
};

class OperatorArray : public Array<QueryOperator*> {
private:
  void clear();
public:
  ~OperatorArray();
};

class DbEngine {
private:
  Database           &m_db;
  const VirtualCode  &m_vc;
  const HostVarList  &m_hostvar;
  OperatorArray       m_operators;
  PipeLineArray       m_pipelines;
  friend class VirtualMachine;
  friend class DbCursor;
public:
  DbEngine(Database &db, const VirtualCode &vc, const HostVarList &hostvar) : m_db(db), m_vc(vc), m_hostvar(hostvar) {}
  void run();
  sqlca               m_sqlca;
};

class DbCursor : public DbEngine {
public:
  DbCursor(Database &db, const VirtualCode &vc, const HostVarList &hostvar) : DbEngine(db,vc,hostvar) { run(); };
  ~DbCursor();
  bool fetch(HostVarList &hostvar);
  bool fetch(Tuple &tuple);
  HostVarDescriptionList getDescription() const { return m_vc.getDescription(); }
};

class SqlCursor {
private:
  DbCursor         *m_cursor;
  Database         &m_db;
  HostVarList       m_hostvar;
  VirtualCode       m_vc;
public:
  StringArray       m_errmsg;
  sqlca             m_sqlca;
  SqlCursor(Database &db, const String &stmt);
  ~SqlCursor();
  bool fetch(Tuple &tuple);
  HostVarDescriptionList getDescription() const { return m_vc.getDescription(); }
  bool ok() const { return m_errmsg.size() == 0; }
};
