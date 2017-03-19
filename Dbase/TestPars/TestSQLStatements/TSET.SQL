SELECT
  ta.i
 FROM INV_ITEM ta
 WHERE 
   ta.i not in  ( 
     select tb.a + 1
       from inv_item tb
      where tb.i = ta.i
     browse access
   )
  browse access

