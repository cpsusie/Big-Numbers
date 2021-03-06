#pragma once

#include <LRparser.h>

#define JAVA5_GRAMMAR   1
#define EXPR_GRAMMAR    2
#define EVAL_GRAMMAR    3
#define SQL_GRAMMAR     4
#define OLM641_GRAMMAR  5
#define EXCEL_GRAMMAR   6
#define ACCESS_GRAMMAR  7
#define CPP_GRAMMAR     8

#define USE_GRAMMAR  JAVA5_GRAMMAR

// if names are changed, modify GRAMMARS.cpp too

#if USE_GRAMMAR == JAVA5_GRAMMAR

#include "../java/Java5lex.h"
#include "../java/Java5Parser.h"
typedef      Java5Lex      ScannerToTest;
#define      tablesToTest  Java5Parser::Java5Tables

#elif USE_GRAMMAR == EXPR_GRAMMAR

#include <Math/Expression/ExpressionParser.h>
#include <c:/Mytools2015/lib/src/Expression/ExpressionLex.h>
#include <c:/Mytools2015/lib/src/Expression/ExpressionLex.h>
using namespace Expr;
typedef      ExpressionLex          ScannerToTest;
#define      tablesToTestRef        (ExpressionParser::getTables())
#define      tablesToTest           (&tablesToTestRef)

#elif USE_GRAMMAR == EVAL_GRAMMAR

#include "../../Math/Eval/EvalLex.h"
typedef      EvalLex               ScannerToTest;
const extern AbstractParserTables *EvalTables;
#define      tablesToTest          EvalTables

#elif USE_GRAMMAR == SQL_GRAMMAR

#include "../../Dbase/SqlParse/SqlLex.h"
typedef      SqlLex                ScannerToTest;
const extern AbstractParserTables *SqlTables;
#define      tablesToTest          SqlTables

#elif USE_GRAMMAR == OLM641_GRAMMAR

#include <../Olm641/Olm641Lex.h>
typedef      Olm641Lex             ScannerToTest;
const extern AbstractParserTables *Olm641Tables;
#define      tablesToTest          Olm641Tables

#elif USE_GRAMMAR == EXCEL_GRAMMAR

#include <../../ConvExcel/ExcelLex.h>

typedef      ExcelLex              ScannerToTest;
const extern AbstractParserTables *ExcelTables;
#define      tablesToTest          ExcelTables

#elif USE_GRAMMAR == ACCESS_GRAMMAR

#include "../../SqlBeauty/SqlLex.h"
typedef      SqlLex                ScannerToTest;
const extern AbstractParserTables *SqlTables;
#define      tablesToTest          SqlTables

#elif USE_GRAMMAR == CPP_GRAMMAR

#include "../Cpp/CppLex.h"
typedef      CppLex                ScannerToTest;
const extern AbstractParserTables *CppTables;
#define      tablesToTest          CppTables

#else

#error No lexical scanner og grammar selected

#endif
