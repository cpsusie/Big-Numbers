#include "stdafx.h"
#include "GRAMMARS.h"

// if changed, modify GRAMMARS.h too

#if USE_GRAMMAR == JAVA5_GRAMMAR

#include <C:/mytools2015/parsergen/java/Java5Lex.cpp>
#include <C:/mytools2015/parsergen/java/Java5Tables.cpp>

#elif USE_GRAMMAR == EXPR_GRAMMAR

#include <C:/mytools2015/Lib/src/Expression/ExpressionLex.cpp>
#include <C:/mytools2015/Lib/src/Expression/ExpressionTables.cpp>

#elif USE_GRAMMAR == EVAL_GRAMMAR

#include <C:/mytools2015/Math/Eval/EvalLex.cpp>
#include <C:/mytools2015/Math/Eval/EvalTables.cpp>

#elif USE_GRAMMAR == SQL_GRAMMAR

#include <C:/mytools2015/DBASE/SQLPARSE/SqlLex.cpp>
#include <C:/mytools2015/DBASE/SQLPARSE/SqlTables.cpp>

#elif USE_GRAMMAR == OLM641_GRAMMAR

#include <C:/mytools2015/parsergen/olm641/Olm641Lex.cpp>
#include <C:/mytools2015/parsergen/olm641/Olm641Tables.cpp>

#elif USE_GRAMMAR == EXCEL_GRAMMAR

#include <C:/mytools2015/convexcel/ExcelLex.cpp>
#include <C:/mytools2015/convexcel/ExcelTables.cpp>

#elif USE_GRAMMAR == ACCESS_GRAMMAR

#include <C:/mytools2015/sqlbeauty/SqlLex.cpp>
#include <C:/mytools2015/sqlbeauty/SqlTables.cpp>

#elif USE_GRAMMAR == CPP_GRAMMAR

#include <C:/mytools2015/parsergen/cpp/CppLex.cpp>
#include <C:/mytools2015/parsergen/cpp/CppTables.cpp>

#else

#error No lexical scanner og grammar selected

#endif
