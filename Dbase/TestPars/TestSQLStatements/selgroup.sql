SELECT
  substring(a.firstcolumn from 1 for 2)
 ,b.dd + b.d
 ,count(b.ii)
FROM 
  lilletable a
 ,lilletable b
WHERE
  a.firstcolumn = "1"
and
  b.ii = a.ii
group by
  1
 ,b.dd+b.d
 ,b.ii*2+1
having
  b.dd + b.d
 ,substring(a.firstcolumn from 1 for 2)
 >
 1
 ,"dsfklgj"
order by
  max(b.dd + b.d) asc
 ,b.ii*2+1 desc
 ,1