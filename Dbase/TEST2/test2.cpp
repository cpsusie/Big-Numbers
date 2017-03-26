static char sqlapi_program_id[240] = {
116,  0,101,  0,115,  0,116,  0, 50,  0, 46,  0,115,  0,113,  0, 99,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 83,  0,117,  0,110,  0, 32,  0, 77,  0, 97,  0,114,  0, 32,  0, 50,  0, 54,  0,
 32,  0, 49,  0, 52,  0, 58,  0, 53,  0, 48,  0, 58,  0, 50,  0, 51,  0, 32,  0,
 50,  0, 48,  0, 49,  0, 55,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

};
/* test2.sqc Sun Mar 26 14:50:23 2017 */
#line 1 "C:/mytools2015/Dbase/TEST2/test2.sqc"
#include "stdafx.h"


#ifdef __NEVER__
  EXEC SQL INCLUDE SQLCA;
#endif
#line 3 "C:/mytools2015/Dbase/TEST2/test2.sqc"
#include <sqlapi.h>
sqlca sqlca;
#line 3 "C:/mytools2015/Dbase/TEST2/test2.sqc"



#ifdef __NEVER__
  EXEC SQL BEGIN DECLARE SECTION;
#endif
#line 5 "C:/mytools2015/Dbase/TEST2/test2.sqc"

  static char   dbname[20];
  static short  dbnameindi;
  static char   username[30];
  static char   password[30];
  static char   firstcolumn[30];
  static short  s;
  static unsigned short s1;
  static short  s2;
  static float  f;
  static double d;
  static int    i;
  static unsigned int i1;
  static int    i2;
  static long   l;
  static unsigned long l1;
  static long   l2;
  static char   varch1[20];
  static char   ch[1];
  static char   outstr[30];
  static char   outint;

#ifdef __NEVER__
  EXEC SQL END   DECLARE SECTION;
#endif
#line 26 "C:/mytools2015/Dbase/TEST2/test2.sqc"


static void selectMyTable() {

  
#ifdef __NEVER__
  exec sql
    select
      c1
     ,longss
    from mytable a 
    where
      a.i = cast("12" as int)
    and
      d = 1
  union
    select
     c1
    ,FIRSTCOLUMN
    into
     :s
    ,:outstr
    from mytable a
    where
      a.firstcolumn = :varch1
  intersect
    select
     :s
     ,firstcolumn
    from mytable a
    where
      a.i = :d
    and
      a.firstcolumn = :varch1
  end-exec
#endif
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
{
  SqlApiVarList sqlapi_varl[6];
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[0].sqltype = 27;
  sqlapi_varl[0].sqllen  = 20;
  sqlapi_varl[0].sqldata = (void*)varch1;
  sqlapi_varl[0].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[1].sqltype = 7;
  sqlapi_varl[1].sqllen  = 2;
  sqlapi_varl[1].sqldata = (void*)&s;
  sqlapi_varl[1].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[2].sqltype = 25;
  sqlapi_varl[2].sqllen  = 8;
  sqlapi_varl[2].sqldata = (void*)&d;
  sqlapi_varl[2].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[3].sqltype = 27;
  sqlapi_varl[3].sqllen  = 20;
  sqlapi_varl[3].sqldata = (void*)varch1;
  sqlapi_varl[3].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[4].sqltype = 7;
  sqlapi_varl[4].sqllen  = 2;
  sqlapi_varl[4].sqldata = (void*)&s;
  sqlapi_varl[4].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[5].sqltype = 27;
  sqlapi_varl[5].sqllen  = 30;
  sqlapi_varl[5].sqldata = (void*)outstr;
  sqlapi_varl[5].sqlind = 0L;
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_call(9,1,sqlapi_program_id,4,2,sqlapi_varl,sqlca);
#line 58 "C:/mytools2015/Dbase/TEST2/test2.sqc"
}
#line 57 "C:/mytools2015/Dbase/TEST2/test2.sqc"

}

static void dbConnect() {
  
#ifdef __NEVER__
  EXEC SQL CONNECT To :dbname user :username using :password;
#endif
#line 62 "C:/mytools2015/Dbase/TEST2/test2.sqc"
{
  SqlApiVarList sqlapi_varl[3];
#line 63 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[0].sqltype = 29;
  sqlapi_varl[0].sqllen  = 20;
  sqlapi_varl[0].sqldata = (void*)dbname;
  sqlapi_varl[0].sqlind = 0L;
#line 63 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[1].sqltype = 29;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)username;
  sqlapi_varl[1].sqlind = 0L;
#line 63 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_varl[2].sqltype = 29;
  sqlapi_varl[2].sqllen  = 30;
  sqlapi_varl[2].sqldata = (void*)password;
  sqlapi_varl[2].sqlind = 0L;
#line 63 "C:/mytools2015/Dbase/TEST2/test2.sqc"
  sqlapi_call(1,5,sqlapi_program_id,3,0,sqlapi_varl,sqlca);
#line 63 "C:/mytools2015/Dbase/TEST2/test2.sqc"
}
#line 62 "C:/mytools2015/Dbase/TEST2/test2.sqc"
#line 61 "C:/mytools2015/Dbase/TEST2/test2.sqc"


  if(sqlca.sqlcode != SQL_OK) {
    printf("Cannot connect to database <%s>. sqlcode:%ld\n",dbname,sqlca.sqlcode);
    exit(-1);
  }
}


static void dbDisconnect() {
  
#ifdef __NEVER__
  EXEC SQL CONNECT RESET;
#endif
#line 72 "C:/mytools2015/Dbase/TEST2/test2.sqc"
{
  sqlapi_call(1,3,sqlapi_program_id,0,0,NULL,sqlca);
#line 72 "C:/mytools2015/Dbase/TEST2/test2.sqc"
}
#line 71 "C:/mytools2015/Dbase/TEST2/test2.sqc"
#line 71 "C:/mytools2015/Dbase/TEST2/test2.sqc"

}

int main(int argc, char **argv) {
  strcpy(dbname   , "fisk");
  strcpy(username , "ADM" );
  strcpy(password , "ADM" );
  dbConnect();
  selectMyTable();
  if(sqlca.sqlcode != SQL_OK) {
    sqlca.dump();
  }
  return 0;
}
