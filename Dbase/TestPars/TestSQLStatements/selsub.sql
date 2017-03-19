SELECT
  a.firstcolumn
FROM 
  lilletable a
 ,lilletable a1
WHERE
   a.firstcolumn = (
    select b.firstcolumn
    from lilletable b
    where b.dd = a.dd
      and b.ii = (
        select c.ii 
        from lilletable c
        where c.firstcolumn = b.firstcolumn
	and c.dd is null
      )
   )
and "a" in ( "b","c","a")
and 
  a.dd,a.ii = a1.dd,a1.ii
and
(
   a1.firstcolumn = "jesper"
// or
//   1 = 0
)