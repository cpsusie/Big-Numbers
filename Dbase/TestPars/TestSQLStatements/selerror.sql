select A.firstcolumn,count(*)
from lilletable A
where firstcolumn > "1" 
and ii between 2234 and 2231
group by firstcolumn
order by firstcolumn
