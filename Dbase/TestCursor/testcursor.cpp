static char sqlapi_program_id[240] = {
116,  0,101,  0,115,  0,116,  0, 99,  0,117,  0,114,  0,115,  0,111,  0,114,  0,
 46,  0,115,  0,113,  0, 99,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 84,  0,104,  0,117,  0, 32,  0, 78,  0,111,  0,118,  0, 32,  0, 32,  0, 53,  0,
 32,  0, 49,  0, 55,  0, 58,  0, 49,  0, 54,  0, 58,  0, 50,  0, 54,  0, 32,  0,
 50,  0, 48,  0, 50,  0, 48,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

};
/* testcursor.sqc Thu Nov  5 17:16:26 2020 */
#line 1 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
#include "stdafx.h"


#if defined(__NEVER__)
  EXEC SQL INCLUDE SQLCA;
#endif
#line 3 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
#include <sqlapi.h>
sqlca sqlca;
#line 3 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"



#if defined(__NEVER__)
  EXEC SQL BEGIN DECLARE SECTION;
#endif
#line 5 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"

  static char	          dbname[20];
  static short	        dbnameindi;
  static char	          username[30];
  static char	          password[30];
  static char	          firstcolumn[30];
  static short	        s;
  static unsigned short s1;
  static short	        s2;
  static float	        f;
  static double         d;
  static int	          i;
  static unsigned int   i1;
  static int	          i2;
  static long	          l;
  static unsigned long  l1;
  static long	          l2;
  static char	          varch1[20];
  static char	          longss[27];
  static varchar        myvarchar(512);
  static char	          nullcolumn1[7];
  static double         dd;
  static int	          ii;
  static int            mm;
  static Date           dddd1;
  static Date           dddd2;

#if defined(__NEVER__)
  EXEC SQL END DECLARE SECTION;
#endif
#line 31 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"


static void xchecksql(TCHAR *file,int line) {
  if(sqlca.sqlcode != SQL_OK && sqlca.sqlcode != SQL_NOT_FOUND) {
	  _tprintf(_T("sql failure in %s line %d\n"), file, line);
	  sqlca.dump();
	  exit(-1);
  }
}

#define checkSql() xchecksql(_T(__FILE__),__LINE__)

static void openCursor() {
  dd = 12;

#if defined(__NEVER__)
  EXEC SQL
    DECLARE curs cursor for
	  select
	    cast(firstcolumn as int),
		firstcolumn
	  from lilletable

	  where "12" <> :myvarchar
  END-EXEC
#endif
#line 53 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"



#if defined(__NEVER__)
  EXEC SQL
    OPEN curs
  END-EXEC
#endif
#line 57 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
{
  SqlApiVarList sqlapi_varl[1];
#line 57 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[0].sqltype = 31;
  sqlapi_varl[0].sqllen  = myvarchar.len();
  sqlapi_varl[0].sqldata = myvarchar.data();
  sqlapi_varl[0].sqlind = 0L;
#line 57 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_call(6,1,sqlapi_program_id,1,0,sqlapi_varl,sqlca);
#line 57 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
}
#line 56 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"

}

static bool fetchcursor() {

#if defined(__NEVER__)
  EXEC SQL
    FETCH curs
	into
	  :dd :s,
	  :firstcolumn

  END-EXEC
#endif
#line 67 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
{
  SqlApiVarList sqlapi_varl[2];
#line 67 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[0].sqltype = 0;
  sqlapi_varl[0].sqllen  = 8;
  sqlapi_varl[0].sqldata = (void*)&dd;
  sqlapi_varl[0].sqlind = &s;
#line 67 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[1].sqltype = 27;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)firstcolumn;
  sqlapi_varl[1].sqlind = 0L;
#line 67 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_call(8,1,sqlapi_program_id,0,2,sqlapi_varl,sqlca);
#line 67 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
}
#line 66 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"

  checkSql();
  return sqlca.sqlcode == 0;
}

static void closeCursor() {

#if defined(__NEVER__)
  EXEC SQL
    CLOSE curs
  END-EXEC
#endif
#line 75 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
{
  sqlapi_call(7,1,sqlapi_program_id,0,0,nullptr,sqlca);
#line 75 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
}
#line 74 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"

}

static void dbConnect() {

#if defined(__NEVER__)
  EXEC SQL CONNECT TO :dbname user :username using :password;
#endif
#line 79 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
{
  SqlApiVarList sqlapi_varl[3];
#line 80 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[0].sqltype = 29;
  sqlapi_varl[0].sqllen  = 20;
  sqlapi_varl[0].sqldata = (void*)dbname;
  sqlapi_varl[0].sqlind = 0L;
#line 80 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[1].sqltype = 29;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)username;
  sqlapi_varl[1].sqlind = 0L;
#line 80 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_varl[2].sqltype = 29;
  sqlapi_varl[2].sqllen  = 30;
  sqlapi_varl[2].sqldata = (void*)password;
  sqlapi_varl[2].sqlind = 0L;
#line 80 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
  sqlapi_call(1,5,sqlapi_program_id,3,0,sqlapi_varl,sqlca);
#line 80 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
}
#line 79 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
#line 78 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"


  checkSql();
}

static void dbDisconnect() {

#if defined(__NEVER__)
  EXEC SQL CONNECT RESET ;
#endif
#line 85 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
{
  sqlapi_call(1,3,sqlapi_program_id,0,0,nullptr,sqlca);
#line 85 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
}
#line 84 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"
#line 84 "C:/Mytools2015/Dbase/testcursor/testcursor.sqc"

}

int main(int argc, char **argv) {
  strcpy(dbname  , "fisk");
  strcpy(username, "ADM" );
  strcpy(password, "ADM" );

  dbConnect();
  checkSql();

  openCursor();
  checkSql();

  while(fetchcursor()) {
    _tprintf(_T("<%s>, <%d>\n"), firstcolumn, ii);
  }

  closeCursor();
  checkSql();

  dbDisconnect();
  checkSql();

  return 0;
}
