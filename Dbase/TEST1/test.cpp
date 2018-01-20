static char sqlapi_program_id[240] = {
116,  0,101,  0,115,  0,116,  0, 46,  0,115,  0,113,  0, 99,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
 83,  0, 97,  0,116,  0, 32,  0, 74,  0, 97,  0,110,  0, 32,  0, 50,  0, 48,  0,
 32,  0, 49,  0, 53,  0, 58,  0, 51,  0, 52,  0, 58,  0, 52,  0, 55,  0, 32,  0,
 50,  0, 48,  0, 49,  0, 56,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0

};
/* test.sqc Sat Jan 20 15:34:47 2018 */
#line 1 "C:/mytools2015/Dbase/TEST1/test.sqc"
#include "stdafx.h"


#ifdef __NEVER__
  EXEC SQL INCLUDE SQLCA;
#endif
#line 3 "C:/mytools2015/Dbase/TEST1/test.sqc"
#include <sqlapi.h>
sqlca sqlca;
#line 3 "C:/mytools2015/Dbase/TEST1/test.sqc"



#ifdef __NEVER__
  EXEC SQL BEGIN DECLARE SECTION;
#endif
#line 5 "C:/mytools2015/Dbase/TEST1/test.sqc"

  static char     dbname[20];
  static short    dbnameindi;
  static char     username[30];
  static char     password[30];
  static char     firstcolumn[30];
  static short    s;
  static unsigned short s1;
  static short    s2;
  static float    f;
  static double   d;
  static int      i;
  static unsigned int i1;
  static int      i2;
  static long     l;
  static unsigned long l1;
  static long     l2;
  static char     varch1[20];
  static char     longss[27];
  static varchar  myvarchar(512);
  static char     nullcolumn1[7];
  static double   dd;
  static int      ii;
  static int      month;
  static int      year;
  static Date     dddd1;
  static Date     dddd2;

#ifdef __NEVER__
  EXEC SQL END   DECLARE SECTION;
#endif
#line 32 "C:/mytools2015/Dbase/TEST1/test.sqc"


static void xchecksql(TCHAR *file,int line) {
  if(sqlca.sqlcode != SQL_OK) {
    _tprintf(_T("sql failure in %s line %d\n"),file,line);
    sqlca.dump();
    exit(-1);
  }
}

#define checkSql() xchecksql(_T(__FILE__),__LINE__)

/*
static void selectff() {

  exec sql
    select t || (select t from ff union select (substring(t,1,2) || t) from ff)
      into :firstcolumn
    from ff
    where
      k = (select k + 1 from ff intersect select k from ff)
    and :l1 is null
  end-exec

  checkSql();

}
*/

static void selectMyTable() {

#ifdef __NEVER__
  exec sql
    select firstcolumn
      into :firstcolumn
    from mytable
    where :l1 is null
  end-exec
#endif
#line 67 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  SqlApiVarList sqlapi_varl[2];
#line 68 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[0].sqltype = 17;
  sqlapi_varl[0].sqllen  = 4;
  sqlapi_varl[0].sqldata = (void*)&l1;
  sqlapi_varl[0].sqlind = 0L;
#line 68 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[1].sqltype = 27;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)firstcolumn;
  sqlapi_varl[1].sqlind = 0L;
#line 68 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_call(9,1,sqlapi_program_id,1,1,sqlapi_varl,sqlca);
#line 68 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 67 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();

}

static void insertMyTable() {

#ifdef __NEVER__
  exec sql
   insert into mytable (
      FIRSTCOLUMN
     ,S
     ,S1
     ,s2
     ,I
     ,I1
     ,I2
     ,L
     ,L1
     ,L2
     ,F
     ,D
     ,longss
     ,varch1
     )
   values (
     :firstcolumn
    ,:s
    ,:s1
    ,:s2
    ,:i
    ,:i1
    ,:i2
    ,:l
    ,:l1
    ,:l2
    ,:f
    ,:d
    ,:longss
    ,:myvarchar
    )
  end-exec
#endif
#line 107 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  SqlApiVarList sqlapi_varl[14];
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[0].sqltype = 27;
  sqlapi_varl[0].sqllen  = 30;
  sqlapi_varl[0].sqldata = (void*)firstcolumn;
  sqlapi_varl[0].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[1].sqltype = 7;
  sqlapi_varl[1].sqllen  = 2;
  sqlapi_varl[1].sqldata = (void*)&s;
  sqlapi_varl[1].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[2].sqltype = 9;
  sqlapi_varl[2].sqllen  = 2;
  sqlapi_varl[2].sqldata = (void*)&s1;
  sqlapi_varl[2].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[3].sqltype = 7;
  sqlapi_varl[3].sqllen  = 2;
  sqlapi_varl[3].sqldata = (void*)&s2;
  sqlapi_varl[3].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[4].sqltype = 11;
  sqlapi_varl[4].sqllen  = 4;
  sqlapi_varl[4].sqldata = (void*)&i;
  sqlapi_varl[4].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[5].sqltype = 13;
  sqlapi_varl[5].sqllen  = 4;
  sqlapi_varl[5].sqldata = (void*)&i1;
  sqlapi_varl[5].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[6].sqltype = 11;
  sqlapi_varl[6].sqllen  = 4;
  sqlapi_varl[6].sqldata = (void*)&i2;
  sqlapi_varl[6].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[7].sqltype = 15;
  sqlapi_varl[7].sqllen  = 4;
  sqlapi_varl[7].sqldata = (void*)&l;
  sqlapi_varl[7].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[8].sqltype = 17;
  sqlapi_varl[8].sqllen  = 4;
  sqlapi_varl[8].sqldata = (void*)&l1;
  sqlapi_varl[8].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[9].sqltype = 15;
  sqlapi_varl[9].sqllen  = 4;
  sqlapi_varl[9].sqldata = (void*)&l2;
  sqlapi_varl[9].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[10].sqltype = 23;
  sqlapi_varl[10].sqllen  = 4;
  sqlapi_varl[10].sqldata = (void*)&f;
  sqlapi_varl[10].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[11].sqltype = 25;
  sqlapi_varl[11].sqllen  = 8;
  sqlapi_varl[11].sqldata = (void*)&d;
  sqlapi_varl[11].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[12].sqltype = 27;
  sqlapi_varl[12].sqllen  = 27;
  sqlapi_varl[12].sqldata = (void*)longss;
  sqlapi_varl[12].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[13].sqltype = 31;
  sqlapi_varl[13].sqllen  = myvarchar.len();
  sqlapi_varl[13].sqldata = myvarchar.data();
  sqlapi_varl[13].sqlind = 0L;
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_call(9,2,sqlapi_program_id,14,0,sqlapi_varl,sqlca);
#line 108 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 107 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}


static void insertTabDate() {

#ifdef __NEVER__
  exec sql
	INSERT INTO mytable (
	   firstcolumn
	  ,c1
	  ,c2
	  ,s
	  ,i
	  ,i1
	  ,i2
	  ,l
	  ,l1
	  ,l2
	  ,f
	  ,d
	  ,varch1
	  ,varch2
	  ) values (
	  "1.8"
	  ,7
	  ,1
	  ,65
	  ,15 ** 2
	  ,58
	  ,59
	  ,34976
	  ,(12 + 5) % 17
	  ,234
	  ,2.45
	  ,2.567
	  ,"2"
	  ,substring("hallo" from 1 for 4 ) || "varch2" || "con"
	  )
	for stable access
  end-exec
#endif
#line 147 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,3,sqlapi_program_id,0,0,NULL,sqlca);
#line 160 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 159 "C:/mytools2015/Dbase/TEST1/test.sqc"


/*
  exec sql
   insert into tabdate (
      ch
     ,d
     )
   values (
     :ii - 120
    ,#28.01-03# - 2
    )
  end-exec
*/
  checkSql();
}

/*
  exec sql
    select a.firstcolumn into :myvarchar :s2 from lilletable a
  end-exec
*/

static void insertLilleTable() {

#ifdef __NEVER__
  EXEC SQL
    insert into lilletable (
      firstcolumn
     ,nullcolumn1
     ,dd
     ,ii
     ,d
    )
    values (
     :myvarchar
    ,"jesper"
    ,:dd
    ,:ii
    ,:d
    )
  END-EXEC
#endif
#line 186 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  SqlApiVarList sqlapi_varl[4];
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[0].sqltype = 31;
  sqlapi_varl[0].sqllen  = myvarchar.len();
  sqlapi_varl[0].sqldata = myvarchar.data();
  sqlapi_varl[0].sqlind = 0L;
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[1].sqltype = 25;
  sqlapi_varl[1].sqllen  = 8;
  sqlapi_varl[1].sqldata = (void*)&dd;
  sqlapi_varl[1].sqlind = 0L;
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[2].sqltype = 11;
  sqlapi_varl[2].sqllen  = 4;
  sqlapi_varl[2].sqldata = (void*)&ii;
  sqlapi_varl[2].sqlind = 0L;
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[3].sqltype = 25;
  sqlapi_varl[3].sqllen  = 8;
  sqlapi_varl[3].sqldata = (void*)&d;
  sqlapi_varl[3].sqlind = 0L;
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_call(9,4,sqlapi_program_id,4,0,sqlapi_varl,sqlca);
#line 187 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 186 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}

static void selectLilleTable() {

#ifdef __NEVER__
  EXEC SQL
    select
      " ",
      a.firstcolumn,
      b.firstcolumn,
      date(32,12,2004) + count(a.firstcolumn)
    into
      :firstcolumn,
      :firstcolumn,
      :firstcolumn,
      :dddd2 :s
    from lilletable a,
         lilletable b
    where a.firstcolumn <= "2" and currentdate < #29-2-00#
    group by a.firstcolumn, b.firstcolumn
    having count(distinct a.firstcolumn) = 2
    order by a.firstcolumn
  END-EXEC
#endif
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  SqlApiVarList sqlapi_varl[4];
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[0].sqltype = 27;
  sqlapi_varl[0].sqllen  = 30;
  sqlapi_varl[0].sqldata = (void*)firstcolumn;
  sqlapi_varl[0].sqlind = 0L;
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[1].sqltype = 27;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)firstcolumn;
  sqlapi_varl[1].sqlind = 0L;
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[2].sqltype = 27;
  sqlapi_varl[2].sqllen  = 30;
  sqlapi_varl[2].sqldata = (void*)firstcolumn;
  sqlapi_varl[2].sqlind = 0L;
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[3].sqltype = 0;
  sqlapi_varl[3].sqllen  = 4;
  sqlapi_varl[3].sqldata = (void*)&dddd2;
  sqlapi_varl[3].sqlind = &s;
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_call(9,5,sqlapi_program_id,0,4,sqlapi_varl,sqlca);
#line 209 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 208 "C:/mytools2015/Dbase/TEST1/test.sqc"

}

static void dropMyTable() {

#ifdef __NEVER__
  exec sql
    drop table mytable
  end-exec
#endif
#line 215 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,6,sqlapi_program_id,0,0,NULL,sqlca);
#line 216 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 215 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}

static char *randstr(char *str, int len) {
  for(int i = 0; i < len; i++)
    str[i] = 'a' + rand() % ('z'-'a'+1);
  str[i] = '\0';
  return str;
}

static void insertRowsIntoMyTable() {
  for(i = 0; i < 1000; i++) {
    randstr(firstcolumn,sizeof(firstcolumn)-1);
    s  = i;
    s1 = i;
    s2 = i;
    i1 = i;
    i2 = i;
    l  = i;
    l1 = i;
    l2 = i;
    f  = (float)i;
    d  = i;
    sprintf(varch1,"varch1-%d",i);
    sprintf(longss,"longs%-03d",i%11);
    printf("now inserting (i=%d)\n",i);
    char lstr[1000];
    sprintf(lstr,"%s","Jesper");
    myvarchar = lstr;
    insertMyTable();
  }
}

static void createMyTable() {

#ifdef __NEVER__
  exec sql
    create table mytable (
      firstcolumn char(30)  not null,
      c1  unsigned char not null default 2,
      c2  unsigned char not null default 255,
      cs1 unsigned char(1)  not null default "1",
      cs2 signed   char(1)  not null default "æ",
      s   short         not null,
      s1  unsigned short ,
      s2  signed   short not null default -327,
      i   int           not null,
      i1  unsigned int  not null,
      i2  signed   int  not null,
      l   long          not null,
      l1  unsigned long not null,
      l2  signed   long not null,
      f   float         not null,
      d   double        not null,
      longss            char(21) not null default "12345678901234567890",
      varch1            varchar(254) not null ,
      varch2            varchar(222) default "fisk",
      primary key (firstcolumn, longss, c1, c2, s2)
    )
  end-exec
#endif
#line 274 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,7,sqlapi_program_id,0,0,NULL,sqlca);
#line 275 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 274 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}

static void createLilleTable() {

#ifdef __NEVER__
  exec sql
    drop table lilletable
  end-exec
#endif
#line 282 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,8,sqlapi_program_id,0,0,NULL,sqlca);
#line 283 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 282 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();


#ifdef __NEVER__
  exec sql
    create table lilletable (
      firstcolumn varchar(240) not null,
      nullcolumn1 char(7),
      dd          double,
      ii          int,
      d           int not null,
      primary key (firstcolumn, d)
    )
  end-exec
#endif
#line 295 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,9,sqlapi_program_id,0,0,NULL,sqlca);
#line 296 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 295 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();


#ifdef __NEVER__
  exec sql
    create index ddd on lilletable (ii,dd,firstcolumn)
  end-exec
#endif
#line 301 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,10,sqlapi_program_id,0,0,NULL,sqlca);
#line 302 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 301 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();


#ifdef __NEVER__
  exec sql
    create index fff on lilletable (dd,ii,firstcolumn)
  end-exec
#endif
#line 307 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(9,11,sqlapi_program_id,0,0,NULL,sqlca);
#line 308 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 307 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}

static void insertRowsIntoLilleTable() {
  for(i = 0; i < 5; i++) {
    char tmp[100];
    randstr(tmp,sizeof(tmp)-1);
//    sprintf(tmp,"varchar100 = %d",i);
    myvarchar = varchar(100,tmp);
    dd = rand() % 100;
    ii = rand() % 3;
    d = i;
    printf("now inserting (i=%d)\n",i);
    insertLilleTable();
  }
}

static void insertRowsIntoTabdate() {
  for(i = 0; i < 200; i++) {
    ii = i;
    dddd2 -= 1;
    dd = i + 1;
    month = 12;
    year = 1994;
    insertTabDate();
  }
}

static void dbConnect() {

#ifdef __NEVER__
  EXEC SQL CONNECT TO :dbname user :username using :password;
#endif
#line 338 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  SqlApiVarList sqlapi_varl[3];
#line 339 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[0].sqltype = 29;
  sqlapi_varl[0].sqllen  = 20;
  sqlapi_varl[0].sqldata = (void*)dbname;
  sqlapi_varl[0].sqlind = 0L;
#line 339 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[1].sqltype = 29;
  sqlapi_varl[1].sqllen  = 30;
  sqlapi_varl[1].sqldata = (void*)username;
  sqlapi_varl[1].sqlind = 0L;
#line 339 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_varl[2].sqltype = 29;
  sqlapi_varl[2].sqllen  = 30;
  sqlapi_varl[2].sqldata = (void*)password;
  sqlapi_varl[2].sqlind = 0L;
#line 339 "C:/mytools2015/Dbase/TEST1/test.sqc"
  sqlapi_call(1,5,sqlapi_program_id,3,0,sqlapi_varl,sqlca);
#line 339 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 338 "C:/mytools2015/Dbase/TEST1/test.sqc"
#line 337 "C:/mytools2015/Dbase/TEST1/test.sqc"


  checkSql();
}

static void dbDisconnect() {

#ifdef __NEVER__
  EXEC SQL CONNECT RESET;
#endif
#line 344 "C:/mytools2015/Dbase/TEST1/test.sqc"
{
  sqlapi_call(1,3,sqlapi_program_id,0,0,NULL,sqlca);
#line 344 "C:/mytools2015/Dbase/TEST1/test.sqc"
}
#line 343 "C:/mytools2015/Dbase/TEST1/test.sqc"
#line 343 "C:/mytools2015/Dbase/TEST1/test.sqc"

}

typedef enum {
  CMD_DROP,
  CMD_CREATE,
  CMD_INSERT,
  CMD_NONE
} Command;

static void usage() {
  fprintf(stderr,"Usage:test [-dcin]\n");
  exit(-1);
}

int main(int argc, char **argv) {
  Command cmd = CMD_INSERT;
  char *cp;
  time_t tt;
  time(&tt);
  srand(tt);
  for(argv++;*argv && *(cp = *argv) == '-';argv++)
    for(cp++;*cp;cp++) {
      switch(*cp) {
      case 'd':
        cmd = CMD_DROP; continue;
      case 'c':
        cmd = CMD_CREATE; continue;
      case 'i':
        cmd = CMD_INSERT; continue;
      case 'n':
        cmd = CMD_NONE; continue;
      default:usage();
      }
      break;
    }

  strcpy(dbname   , "fisk");
  strcpy(username , "ADM" );
  strcpy(password , "ADM" );

  dbConnect();
  switch(cmd) {
    case CMD_DROP:
      dropMyTable();
      break;
    case CMD_CREATE:
      createLilleTable();
      createMyTable();
      break;
    case CMD_INSERT:
//      insertRowsIntoLilleTable();
    insertRowsIntoMyTable();
//      insertRowsIntoTabdate();
      break;
  }
  if(sqlca.sqlcode != SQL_OK)
    sqlca.dump();

  return 0;
}
