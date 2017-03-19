SELECT
 a.firstcolumn,
 b.firstcolumn,
 c.firstcolumn
 FROM lilletable a,
      lilletable b,
      lilletable c
 WHERE
  c.ii = b.ii
 and
  a.ii,a.dd = 1,2
 and
  b.firstcolumn = a.firstcolumn
 and
  c.d = 3