    select
      FIRSTCOLUMN
    from mytable a
    where
      a.i = 1
    and
      d >= 1

  union

      select
        FIRSTCOLUMN
      from mytable a
      where
        a.firstcolumn = "1"

    intersect

      select
        d
      from mytable a
      where
        a.i = 1
      and
        a.firstcolumn = "1"

    minus

      select
        dd
      from tabdate a
      where
        a.i = 1
      and
        a.firstcolumn = "1"
