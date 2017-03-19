SELECT
 a.firstcolumn

 FROM 
  lilletable a
 WHERE
  a.ii in (select d.ii
          from lilletable d 
          where d.dd = a.dd)
 and
  a.firstcolumn > "1"
