SELECT
  b.firstcolumn
 ,a.dd
 ,a.ii
 ,b.dd
 ,b.ii
 ,"fisk"
 ,"fusk"
FROM 
  lilletable a
 ,lilletable b
WHERE
  a.firstcolumn = "1"
and
  b.dd,b.ii = a.dd,a.ii
order by
  b.dd asc
 ,b.ii asc
 ,b.firstcolumn desc