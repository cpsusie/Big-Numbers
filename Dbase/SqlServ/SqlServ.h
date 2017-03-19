#pragma once

#include <MyUtil.h>
#include <Math.h>
#include <BitSet.h>
#include <Thread.h>
#include <Semaphore.h>
#include <Stack.h>
#include <SqlApi.h>
#include <Btree.h>
#include <SqlSymbol.h>
#include <SqlParser.h>
#include <SqlCom.h>
#include "VirtCode.h"
#include "SqlExec.h"

class SqlCompiler;
class SelectStmt;
class SelectSetOperator;
class FromTable;

class Predicate {
public:
  CompactShortArray          m_colIndex;
  int                        m_relOpToken;     // the relation-operator used in this Predicate
  bool                       m_keyPredicate;   // is this a KeyPredicate
  double                     m_setSize;        // only used for INSYM Predicate
  const SyntaxNode          *m_pred;
  Predicate(int relOpToken, const SyntaxNode *n, double setSize = 0) {
    m_relOpToken   = relOpToken;
    m_pred         = n;
    m_keyPredicate = false;
    m_setSize      = setSize;
  }
  bool columnIsMember(short int col) const;
  SyntaxNode *findOpposite(FromTable *fromtable, int col);
  void dump(FILE *f = stdout) const;
};

class PredicateList : public Array<Predicate> {
public:
  void dump(FILE *f = stdout) const;
};

class KeyPredicate {
public:
  bool                       m_begin; // true if this is beginkeypredicate, false for endkeypred
  FromTable                 *m_fromTable;
  CompactArray<SyntaxNode*>  m_expr;
  int                        m_relOpToken;
  void dump(FILE *f = stdout) const;
  void getKeyPredicateExpression(int length, const IndexDefinition &indexDef, PredicateList &predicateList, BitSet &set);
};

class KeyPredicates {
public:
  KeyPredicate               m_beginKeyPredicate;
  KeyPredicate               m_endKeyPredicate;
  void find(const IndexDefinition &indexDef, PredicateList &predicateList);
  void dump(FILE *f = stdout) const;
  void init();
};

class IndexStatistic {
public:
  IndexDefinition            m_indexDef;
  SysTableStatData           m_stat;
  bool                       m_indexOnly;
  double selectivity(PredicateList &predlist, KeyPredicates &predicates) const;
};

class ColumnAttributes {
public:
  bool m_whereUnaggr : 1; // is column unaggregated in the where clause
  bool m_whereAggr   : 1; // is column aggregated   in the where clause
  bool m_selUnaggr   : 1; // is column unaggregated in selectlist
  bool m_selAggr     : 1; // is column aggregated   in selectlist
  bool m_havUnaggr   : 1; // is column unaggregated in having clause
  bool m_havAggr     : 1; // is column aggregated   in having clause
  bool m_inGroupBy   : 1; // is column              in the group by clause
  bool m_orderAggr   : 1; // is column aggregated   in order by clause
  bool m_orderUnaggr : 1; // is column unaggregated in order by clause
  SyntaxNode *m_fixedBy;
  ColumnAttributes();
  bool used() const;
  void setAttribute(int clause, bool inaggr, bool value);
  void dump(const FromTable *t, FILE *f = stdout, int level = 0) const;
};

class ProjectMap {
public:
  FromTable                 &m_table;
  FieldSet                   m_fieldSet;
  CompactIntArray            m_fieldMap;
  ProjectMap(FromTable &table);
};

class StatementTable : public TableDefinition {
private:
  void calculateTableSize();
  double m_tableSize;                          // estimated number of rows in table
public:
  SqlCompiler               &m_compiler;
  BitSet                    *m_fixedByConst; // is this fixed by a const_expression. ie f = :hostvar etc.
  SyntaxNode                *m_node;         // root SyntaxNode of this. only used for errormessages
  Array<IndexStatistic>      m_indexStat;

  StatementTable(SqlCompiler &compiler, SyntaxNode *n);
  ~StatementTable();
  double getTableSize() const {  // estimated number of rows in table
    return m_tableSize;
  }
};

class StatementSymbolInfo : public SyntaxNodeData {
public:
  StatementTable            &m_table;
  unsigned short             m_colIndex;     // columnindex in m_fromTable
  char                       m_currentTupleReg;
  int                        m_currentTupleIndex;
  StatementSymbolInfo(StatementTable &ft, unsigned short colIndex, SyntaxNode *n);
  void dump(FILE *f = stdout, int level = 0) const;
  bool operator==(const StatementSymbolInfo &rhs) {
    return (&m_table == &rhs.m_table) && (m_colIndex == rhs.m_colIndex);
  }
  const ColumnDefinition &getColumn() const { return m_table.getColumn(m_colIndex);                }
  DbFieldType getType()               const { return getColumn().getType();                        }
  bool isNullAllowed()                const { return getColumn().m_nullAllowed;                    }
  int len()                           const { return getColumn().m_len;                            }
  bool isFixedByConst()               const { return m_table.m_fixedByConst->contains(m_colIndex); }
};

class FromTable : public StatementTable {
private:
  void findEqualPredicates(       const SyntaxNode *n,     const NodeList &left, const NodeList &right,                 PredicateList &predicateList) const;
  void findInEqualPredicates(     const SyntaxNode *n,     const NodeList &left, const NodeList &right, int relOpToken, PredicateList &predicateList) const;
  void findSetPredicate(          const SyntaxNode *n,     PredicateList &predicateList) const;
  void findPredicates(            const NodeList &andlist, PredicateList &predicateList) const;
  void booleanTermSelectivity(    const SyntaxNode *n,     double &indexSelectivity, double &selectivity);
  void predicateSelectivity(            NodeList &andlist, double &indexSelectivity, double &selectivity);
  void searchConditionSelectivity(const SyntaxNode *n,     double &indexSelectivity, double &selectivity);
public:
  String                     m_correlationName;
  SelectStmt                &m_belongsTo;    // the SelectStmt this table belongs to
  short int                  m_joinSequence; // the best position in the joinsequence. first is 0
  Array<ColumnAttributes>    m_attributes;   // attributes for each field in table
  bool                       m_countAggr;    // are we using a count(*). Cannot be contained in m_attributes
  int                        m_usedIndex;    // index into m_indexStat.
  KeyPredicates              m_keyPredicates;
  bool                       m_fixed;        // true if there exists a unique index with all fields fixed by const_expressions
  bool                       m_asc;          // asc or desc redirection on this table
  double                     m_indexSelectivity;     // estimated fraction (0..1) of table that will be read. Calculated by optimizer
  double                     m_selectivity;  // estimated fraction (0..1) of table that will be selected. Calculated by optimizer
  short                      m_readOperator; // in which QueryOperator is this table read
  short                      m_cursorReg;    // and which cursor
  FromTable(SelectStmt &belongsTo, SyntaxNode *n, const TCHAR *correlationName);
  void checkFixedByUniqueKey();
  int isFixedByUniqueKey() const { // returns index-number if fixed by constant. -1 if not.
    return m_fixed ? m_usedIndex : -1;
  }
  int getNumberOfUsedColumns() const;
  void calulateIndexOnly();
  void findTableSelectivity(const SyntaxNode *n, double &indexSelectivity, double &selectivity);
  const IndexStatistic &getUsedIndex() const {
    return m_indexStat[m_usedIndex];
  }
  void dump(FILE *f = stdout, int level = 0) const;
};

class SelectSymbolInfo : public StatementSymbolInfo {
public:
  FromTable &m_fromTable;
  SelectSymbolInfo(FromTable &ft, unsigned short colindex, SyntaxNode *n);
  ColumnAttributes &getAttributes() {
    return m_fromTable.m_attributes[m_colIndex];
  }
  const String &getName() const {
    return m_fromTable.getColumn(m_colIndex).m_name;
  }
  void dump(FILE *f, int level) const;
};

class InsertColumnExpression : public StatementSymbolInfo {
public:
  SyntaxNode *m_expr;
  InsertColumnExpression(StatementTable &ft, unsigned short colIndex, SyntaxNode *expr);
  void dump(FILE *f = stdout) const;
};

class SSIMap : public Array<SelectSymbolInfo> {
public:
  void append(FromTable &ft, unsigned short colIndex, SyntaxNode *n);
  void dump(FILE *f = stdout, int level = 0) const;
};

class OrderByExpression {
public:
  SyntaxNode *m_expr;
  bool        m_asc;
  OrderByExpression(SyntaxNode *expr, bool asc) { 
    m_expr = expr;
    m_asc  = asc;
  }
  void dump(FILE *f = stdout) const;
};

typedef enum {
  MAINSELECT
 ,SUBSELECT_PURPOSE_EXIST
 ,SUBSELECT_PURPOSE_NOT_EXIST
 ,SUBSELECT_IN_SET
 ,SUBSELECT_NOT_IN_SET
 ,SUBSELECT_EXPRESSION
 ,UNION_PART
 ,DIFFERENCE_PART
 ,INTERSECT_PART
} SelectStmtPurpose;

typedef CompactArray<SelectStmt*> SelectStmtArray;

class UniqueSelectStmtArray : public SelectStmtArray {
public:
  void add(SelectStmt *stmt);
};

class TableReadMethod {
public:
  FromTable                 *m_table;
  int                        m_index;
  bool                       m_asc;
  double                     m_indexSelectivity;  
  double                     m_selectivity;
  KeyPredicates              m_keyPredicates;
  TableReadMethod(FromTable *table);
};

class KeyPredicatesHashMap : public HashMap<SyntaxNodeP,int> {
public:
  KeyPredicatesHashMap();
};

class SelectExpression {
public:
  SyntaxNode                *m_expr;
  DbMainType                 m_type; // indicates type of m_expr
  SelectExpression(SyntaxNode *expr) {
    m_expr = expr;
    m_type = MAINTYPE_VOID;
  }
  void dump(FILE *f = stdout);
};

class SelectExpressionList : public Array<SelectExpression> {
public:
  SelectExpressionList(const NodeList &list);
  SelectExpressionList();
  void dump(const SqlCompiler &compiler, FILE *f = stdout, int level = 0) const;
};

typedef enum {
  predfalse = 1,
  predundef = 2,
  predtrue  = 4
} PredicateValue;

class PossiblePredicateValues {
private:
  char        map;
  static char bitCount[];
  int  count()                        const { return bitCount[map];        }
public:
  PossiblePredicateValues()                 { map = 0;                     }
  bool contains(PredicateValue v)     const { return (map & v) != 0;       }
  void insert(PredicateValue v)             { map |= v;                    }
  void clear()                              { map = 0;                     }
  PossiblePredicateValues &operator=(int v) { map = (char)v; return *this; }
  bool isConst()                      const { return count() == 1;         }
  bool falsePossible()                const { return contains(predfalse);  }
  bool nullPossible()                 const { return contains(predundef);  }
  bool truePossible()                 const { return contains(predtrue );  }
  void dump(FILE *f = stdout);
};

typedef CompactArray<SelectSymbolInfo*> PipeAllocation;

class SelectSetOperator : public SyntaxNodeData {
protected:
  mutable HostVarDescriptionList *m_desc;
public:
  SqlCompiler       &m_compiler;
  SelectStmtPurpose  m_purpose;
  SelectSetOperator *m_son1;
  SelectSetOperator *m_son2;
  SelectSetOperator(SqlCompiler &compiler, SelectStmtPurpose purpose, SyntaxNode *n, SelectSetOperator *son1, SelectSetOperator *son2);
  SelectSetOperator(SqlCompiler &compiler, SelectStmtPurpose purpose, SyntaxNode *n);
  void checkCompatibleSelectLists();
  virtual double rowsSelected(double &total) const;
  virtual bool   isEmptySelect() const;
  DbMainType getExpressionType(int i);
  virtual const HostVarDescriptionList &getDescription() const;
  virtual void  dump(FILE *f = stdout, int level = 0) const;
  virtual ~SelectSetOperator();
};

class SelectStmt : public SelectSetOperator {
private:
  CompactArray<FromTable*>        m_fromTable;         // tables, used directly in this select
  SSIMap                          m_ssiMap;            // symboltable
  Array<OrderByExpression>        m_orderBy;           // order by expr1,expr2....exprn all asc or desc
  CompactArray<SyntaxNode*>       m_groupBy;           // group by expr1,expr2,...exprn
  NodeList                        m_fromList;
  NodeList                        m_groupByList;
  NodeList                        m_orderByList;
  NodeList                        m_selectIntoList;
  int                             m_currentClause;     // SELECT,WHERE,HAVING. which clause are we checking
  SyntaxNode                     *m_whereClause;
  SyntaxNode                     *m_havingClause;
  SelectStmtArray                 m_subSelects;        // all direct subselects
  UniqueSelectStmtArray           m_corSubSelects;     // subselects correlated to this
  UniqueSelectStmtArray           m_usedParentSelects; // parent-selects, this is correlated to
  SelectStmt                     *m_parent;
  String                          m_name;
  double                          m_indexSelectivity;
  double                          m_selectivity;
  double                          m_totalCost;         // total cost of this select
  double                          m_orderCost;         // cost of order by
  Array<TableReadMethod>          m_bestReadMethod;    // current best accessplan
  int                             m_outputPipe;
  int                             m_noOfLike;          // should be in operator-scope
  PossiblePredicateValues         m_possibleWhere,m_possibleHaving;
  void        findColumnName(      SyntaxNode *n);
  FromTable  *findCorrelationName( SyntaxNode *n, bool searchParent);
  void        findCorrelationTable(SyntaxNode *n);
  void   expandStar(        SelectExpressionList &expandList, const FromTable *ft);
  void   selectExpandStar(UINT i);
  void   getTableDefinition(       SyntaxNode *n);
  void   checkTypeIsNumeric(       SyntaxNode *expr, bool inaggr); // check recursive if typeof(expr)=numeric and generate syntaxError SQL_INVALID_EXPR_TYPE if not
  void   checkTypeIsString(        SyntaxNode *expr, bool inaggr); // check recursive if typeof(expr)=String  and generate syntaxError SQL_INVALID_EXPR_TYPE if not
  DbMainType checkExpressionType(  SyntaxNode *expr, bool inaggr); // check expr recursive and returns the typeof(epxr)
  void   checkExpression(          SyntaxNode *expr);
  void   aggrInAggrError(          SyntaxNode *expr);   // generates syntaxError SQL_AGGR_IN_AGGR
  void   aggrInWhereError(         SyntaxNode *expr);   // generates syntaxError SQL_AGGR_IN_WHERE
  void   checkIsOuterRef(          SyntaxNode *expr);
  void   checkAggrInWhere(         SyntaxNode *expr);
  void   checkCountArgument(       SyntaxNode *expr); // check the expression in a aggr.function count
  void   checkSetExpr(             SyntaxNode *expr, DbMainType exprtype, bool neg);
  void   checkRelOpSubSelect(                         const SyntaxNode *pred, bool neg);
  void   checkGroupedOrAggregatedExpression(          const SyntaxNode *expr, const TCHAR *clause);
  void   checkGroupedOrAggregatedExpressionList(      const NodeList   &list, const TCHAR *clause);
  void   checkGroupedOrAggregatedPredicate(           const SyntaxNode *pred, const TCHAR *clause);
  bool   isInGroupBy(                                 const SyntaxNode *expr) const;
  bool   isTreeEqual(                                 const SyntaxNode *t1  , const SyntaxNode *t2) const;
  const  SyntaxNode *findUnGroupedUnAggregatedColumn( const SyntaxNode *expr);  // returns a NAME-syntax-node with attr.m_ingroup = false and not contained in an aggregate-func. NULL if none
  const  SyntaxNode *findUnAggregatedSyntaxNode(      const SyntaxNode *expr, const FromTable *table, int colIndex);
  const  SyntaxNode *findUnAggregatedSelectNode(      const FromTable *table, int colIndex); // find the syntaxnode in m_selectexpr   that is not aggregated and SSIMap match table,colindex
  const  SyntaxNode *findUnAggregatedHavingNode(      const FromTable *table, int colIndex); // find the syntaxnode in m_havingclause that is not aggregated and SSIMap match table,colindex
  const  SyntaxNode *findUnAggregatedOrderByNode(     const FromTable *table, int colIndex); // find the syntaxnode in m_orderclause  that is not aggregated and SSIMap match table,colindex
  void   markColumnAttributes( int clause, const SyntaxNode *expr, bool inaggr = false); // recursive set attributes for specified clause
  void   clearColumnAttributes(int clause, bool inaggr);
  bool   hasAggregation() const;
  void   checkSelectList();
  void   checkExpressionList(NodeList &left, NodeList &right);
  void   checkPredicate(SyntaxNode *pred, bool neg = false);
  void   checkOrderBy();
  void   checkGroupBy();
  void   checkAggregation();
  BitSet findFieldsFixedByConstPredicate(FromTable *table, const SyntaxNode *pred, bool neg); // returns the set of fields in table, that are fixed by a constant expression
  bool   addFieldsFixedByConstPredicate(const SyntaxNode *pred);
  void   statementSelectivity( double &indexSelectivity, double &selectivity);
  double estimateGroupByCost(  double elementsSelected);
  double estimateOrderByCost(  double elementsSelected);
  double estimateCost(double &orderCost); // returns the total cost (and cost of orderby) of the current joinsequence
  void   estimateAndSaveCost();
  void   combinateUsedIndex(     int tableIndex);
  void   combinateReadDirection( int tableIndex);
  void   permuterJoinSequence(   int nelem );
  void   findJoinSequence();
  void   reduceByKeyPredicates();
  void   setCurrentTupleReg(const ProjectMap &map, int tuplereg);
  DbMainType checkSimpleSubSelect(SelectStmtPurpose purpose, SyntaxNode *expr); // expr = SELECT returns DbMainType of exprlist[0] of subselect
  DbMainType checkSubSelect(      SelectStmtPurpose purpose, SyntaxNode *expr); // expr = SELECT,UNION,INTERSECT,SETDIFFERENCE
  SelectSetOperator *checkSubSelectOperator(SelectStmtPurpose purpose, SyntaxNode *expr, bool allowmany = false);

public:
  SelectExpressionList            m_selectExprList;    // select expr1,expr2, ... exprn
  int                             m_noOfFixedTables;   // counts the number of fixed from_tables, used as joinsequence
  SelectStmt( SqlCompiler        &compiler,
              SyntaxNode         *n,                   // n == SelectStmt
              SelectStmt         *parent,
              SelectStmtPurpose   purpose,
              int                 outputPipe
             );
  ~SelectStmt();
  const SyntaxNode *whereClause()  const {
    return m_whereClause;
  }
  const SyntaxNode *havingClause() const {
    return m_havingClause;
  }

  bool   isFixedExpression(            const SyntaxNode *expr);
  int    expressionMaxJoinSequence(    const SyntaxNode *expr) const; // returns the higest joinsequencenumber of an expression. const and parent-table-columns are -1
  int    subSelectMaxJoinSequence(     SelectStmt *subselect ) const; // returns the higest joinsequencenumber of a subselect relative to this
  int    selectOperatorMaxJoinSequence(SelectSetOperator *op ) const; // returns the higest joinsequencenumber of a subselect relative to this
  int    setMaxJoinSequence(                 SyntaxNode *n   ) const; // returns the higest joinsequencenumber of a setexpr   relative to this
  double getSetSize(                         SyntaxNode *n   ) const; // estimates size of setexpr
  void   typeCheck();
  void   syntaxError(const SyntaxNode *n, long sqlcode, TCHAR *format, ...);
  const  HostVarDescriptionList &getDescription() const;
  bool   isEmptySelect() const;
  void   genCode();
  void   genScanOperator(FromTable &t , int outputPipe);
  void   genJoinOperator(FromTable &t1, FromTable &t2, int outputPipe);
  void   genJoinOperator(FromTable &t , int inputpipe, int outputPipe);
  void   genNullOperator(                              int outputPipe);
  double rowsSelected(double &total) const;
  // should be called after reducePredicate has been called

  SelectSymbolInfo *getInfo(const SyntaxNode *n) const;
  int    expressionColumnIndex(FromTable *table, const SyntaxNode *n);
  void   dump(FILE *f = stdout, int level = 0) const;

};

typedef Array<int> JumpCodeAdressList;


class JumpCodeTable {
public:
  JumpCodeAdressList m_truePart;
  JumpCodeAdressList m_falsePart;
  JumpCodeAdressList m_undefPart;
};

#define HOSTVARSCANNERFLAG_INTO        0x1
#define HOSTVARSCANNERFLAG_ALLBUTINTO  0x2
#define HOSTVARSCANNERFLAG_ALL         HOSTVARSCANNERFLAG_INTO | HOSTVARSCANNERFLAG_ALLBUTINTO

class SqlCompiler : public ParserTree {
private:
  void parse();
  void genUpdate(       SyntaxNode *n);
  void genInsert(       SyntaxNode *n);
  void genInsertValues( const SyntaxNode *n, StatementTable &table);
  void checkExpressionType(InsertColumnExpression &col);
  DbMainType checkValueExpressionMainType(const SyntaxNode *expr);

  void genInsertSelect(SyntaxNode *n);

// end of genInsert

  void genDelete(       SyntaxNode *n);
  SelectSetOperator    *genSelect1(SyntaxNode *n);
  void genSelect(       SyntaxNode *n);
  SelectSetOperator    *genSimpleSelect(SyntaxNode *n);
  void genDeclare(      SyntaxNode *n);
  void genFetch(        SyntaxNode *n);

  void genKeyDefinition( const SyntaxNode *n, const  TableDefinition &tableDef,
                                              IndexDefinition        &indexDef);
  void getType(          const SyntaxNode *n, ColumnDefinition  &col);
  void checkDefault(    SyntaxNode *n,        ColumnDefinition  &col);
  void checkNumberRange(SyntaxNode *n,       unsigned char dbtype );
  void checkNumberRange(SyntaxNode *n,       double min, double max, bool integer);
  void genColumnDefinition(const SyntaxNode *n, TableDefinition &tableDef);
  void genCreateTableAs(SyntaxNode *n);
  void genCreateTableLike(SyntaxNode *n);
  void genCreateTable(  SyntaxNode *n);
  void genCreateIndex(  SyntaxNode *n);
  void genCreate(       SyntaxNode *n);
  void genDrop(         SyntaxNode *n);
  void genExplain(      SyntaxNode *n);
  void genCode();
  CastParameter genCastParameter(const SyntaxNode *n);
  void genTypeCast(       const SyntaxNode *n);
  void genExpression(     const SyntaxNode *expr);
  void genPredicate(      const SyntaxNode *predicate);
  SyntaxNode            *fetchDefaultValueNode(const ColumnDefinition &col);
  SyntaxNode            *reducePredicate1(     SyntaxNode *n, bool &isConst);
  SyntaxNode            *reducePredicate(      SyntaxNode *n, PossiblePredicateValues &values);
  bool isNullPossible(const SyntaxNode *n); // n is expr
public:
  const Database            &m_db;
  const SqlApiBindProgramId &m_programid;
  const SqlApiBindStmt      &m_bndstmt;
  int                        m_noofpipes;
  JumpCodeAdressList         m_operatorStart;
  CodeGeneration             m_code;
  Array<PipeAllocation>      m_pipeAllocation;
  Array<SelectSetOperator*>  m_selectOperators;
  SqlCompiler(const Database &db, const SqlApiBindProgramId &programid, const SqlApiBindStmt &bndstmt);
  ~SqlCompiler();
  void   syntaxError( const SyntaxNode *n  , long sqlcode, const TCHAR *format, ...);
  void   vSyntaxError(const SyntaxNode *n  , long sqlcode, const TCHAR *format, va_list argptr);
  void   findHostVarIndex( SyntaxNode *expr, int &hostvarcounter, int flags = HOSTVARSCANNERFLAG_ALL);
  void   getCode(VirtualCode &vc);
  SyntaxNode *reduceExpression(SyntaxNode *n, bool &isconst);
//  const  SyntaxNode *reducePredicate(      const SyntaxNode *n);
  SyntaxNode *reduceByKeyPredicates(SyntaxNode *n, KeyPredicatesHashMap &hash);
  double evaluateConstNumericExpression(const SyntaxNode *expr, bool &valueKnown); // evaluate a const number-expression at compile-time
  int    evaluateMaxStringLength(       const SyntaxNode *n);                      // n is stringexpression. returns -1 on undefined
  int    newPipe() { return m_noofpipes++; }
  void   fixJumpAdressList(const JumpCodeAdressList &jumpcodes, UINT jumpAddress);
  friend class SelectStmt;
  StatementSymbolInfo *getInfo(const SyntaxNode *n) const;
  bool isSimpleIndexField(const SyntaxNode *n);
};

class IndexColumnName {
public:
  String m_colName;
  bool   m_asc;
};

class CreateIndexData { // data til CODECRINX
public:
  String         m_tableName;
  String         m_indexName;
  bool           m_unique;
  Array<IndexColumnName> m_columns;
  CreateIndexData() {
    m_unique = false;
  }
  String toString() const;
  inline void dump(FILE *f = stdout) const {
    _ftprintf(f,_T("%s"), toString().cstr());
  }
};
Packer &operator<<(Packer &p, const CreateIndexData &inx);
Packer &operator>>(Packer &p,       CreateIndexData &inx);

void sqlCreateIndex(Database &db, CreateIndexData &data);

void xstopcomp(const SyntaxNode *n, TCHAR *file, int line);

#define DEBUG_SQLCOMP

#ifdef DEBUG_SQLCOMP
#define stopcomp(n) xstopcomp(n,_T(__FILE__),__LINE__)
#else
#define stopcomp(n) 
#endif


String    sqlSubString(const String &str, int from, int len);  // calculates substring
bool      isLike(const String &str, const String &pattern);
bool      isConstExpression(    const SyntaxNode *expr);         // check if an expression is constant, ie. known at compiletime
int       constExpressionCmp(   const SyntaxNode *n1, const SyntaxNode *n2); // n1,n2 contains valuenode
bool      isEmptySelect(        const SyntaxNode *n);
const SyntaxNode *findFirstTableReference(    const SyntaxNode *expr);
const SyntaxNode *findFirstAggregateFunction( const SyntaxNode *expr); // return the first occurrence of an aggregate function in expr


bool sqlCompile(   const Database            &db        ,
                   SqlApiBindProgramId       &programid ,
                   const SqlApiBindStmt      &bndstmt   , 
                   VirtualCode               &vc        ,
                   StringArray               &errmsg    , 
                   sqlca                     &ca        );

bool sqlCompile(   const Database            &db        ,
                   const String              &stmt      , 
                   VirtualCode               &vc        ,
                   StringArray               &errmsg    , 
                   sqlca                     &ca        );

void sqlExecute(   Database                  &db        ,
                   const VirtualCode         &vc        ,
                   const HostVarList         &hostvar   ,
                   sqlca                     &ca        );

void sqlLoadCode(  const Database            &db        ,
                   const SqlApiBindProgramId &programid ,
                   int                        nr        ,
                   VirtualCode               &vc        );

void sqlSaveCode(  Database                  &db        ,
                   SqlApiBindProgramId       &programid ,
                   int                        nr        ,
                   VirtualCode               &vc        );

void sqlDeleteCode(Database                  &db        , 
                   const String              &filename  );

void sqlExecute(   Database                  &db        ,
                   const String              &stmt      ,
                   StringArray               &errmsg    ,
                   sqlca                     &ca        );

#define TRACECOMP

