INSERT INTO mytable (
   firstcolumn
  ,c1
  ,c2
  ,s 
  ,i 
  ,i1
  ,i2
  ,l 
  ,l1
  ,l2
  ,f 
  ,d 
  ,varch1
  ,varch2
  ) values (
  "1.8"
  ,7
  ,1
  ,65
  ,15 ** 2
  ,58
  ,59
  ,34976
  ,(12 + 5) % 17
  ,234
  ,2.45
  ,2.567
  ,"2"
  ,substring("hallo" from 1 for 4 ) || "varch2" || "con"
  )
for stable access
