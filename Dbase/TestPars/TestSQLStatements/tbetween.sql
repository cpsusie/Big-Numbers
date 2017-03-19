SELECT
 a.firstcolumn,
 b.firstcolumn,
 c.firstcolumn
 FROM lilletable a,
      lilletable b,
      lilletable c
 WHERE
    c.dd,c.ii
  between
    b.dd,b.ii
  and
    a.dd,a.ii
 and
  a.ii,a.dd = 1,2
 and
  b.firstcolumn = a.firstcolumn
