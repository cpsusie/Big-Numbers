#include "stdafx.h"
#include "GRAMMARS.h"

// if changed, modify GRAMMARS.h too

#if USE_GRAMMAR == EXPR_GRAMMAR

#include <C:\mytools\Lib\src\Math\Expression\ExpressionLex.cpp>
#include <C:\mytools\Lib\src\Math\Expression\ExpressionTables.cpp>

#elif USE_GRAMMAR == EVAL_GRAMMAR

#include <C:\mytools\Math\Eval\EvalLex.cpp>
#include <C:\mytools\Math\Eval\EvalTables.cpp>

#elif USE_GRAMMAR == SQL_GRAMMAR

#include <C:\mytools\DBASE\SQLPARSE\SqlLex.cpp>
#include <C:\mytools\DBASE\SQLPARSE\SqlTables.cpp>

#elif USE_GRAMMAR == OLM641_GRAMMAR

#include <C:\mytools\parsergen\olm641\Olm641Lex.cpp>
#include <C:\mytools\parsergen\olm641\Olm641Tables.cpp>

#elif USE_GRAMMAR == EXCEL_GRAMMAR

#include <C:\mytools\convexcel\ExcelLex.cpp>
#include <C:\mytools\convexcel\ExcelTables.cpp>

#elif USE_GRAMMAR == ACCESS_GRAMMAR

#include <C:\mytools\sqlbeauty\SqlLex.cpp>
#include <C:\mytools\sqlbeauty\SqlTables.cpp>

#elif USE_GRAMMAR == CPP_GRAMMAR

#include <C:\mytools\parsergen\cpp\CppLex.cpp>
#include <C:\mytools\parsergen\cpp\CppTables.cpp>

#elif USE_GRAMMAR == JAVA5_GRAMMAR

#include <C:\mytools2015\parsergen\java\Java5Lex.cpp>
#include <C:\mytools2015\parsergen\java\Java5Tables.cpp>

#else

#error No lexical scanner og grammar selected

#endif
