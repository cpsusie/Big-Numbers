select a.dd,max(dd) 
from lilletable a
group by a.dd
having a.dd = (select max(b.dd) from lilletable b
                where b.dd = 
                  (select min(c.dd) from lilletable c
                   where c.dd = max(a.dd)
                  )
              )
