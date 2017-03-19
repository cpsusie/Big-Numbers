SELECT
 b.firstcolumn
 FROM lilletable a,
      lilletable b
 WHERE
  b.dd = 1
and 
  a.ii = b.dd
and
  b.ii in (
    select s.d from lilletable s
    where s.dd = 2
  )
  and
   not exists ( 
     select *
       from lilletable c
      where c.firstcolumn >= b.firstcolumn
        and not exists (
          select *
           from mytable d
           where d.d = a.d
             and d.d between 7 and 11
        )
     union
       select * from lilletable
   )
order by
  b.firstcolumn
