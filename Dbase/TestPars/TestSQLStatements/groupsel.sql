SELECT
  b.firstcolumn
 ,b.dd
 ,count(b.ii)
FROM 
  lilletable a
 ,lilletable b
WHERE
  a.firstcolumn = "1"
and
  b.ii = a.ii
group by
  1,2
having
  b.dd > 1
order by
   b.dd asc
,  b.ii desc