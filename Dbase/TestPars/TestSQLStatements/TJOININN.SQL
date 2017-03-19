SELECT
   T1.service_provider_nr
INTO :a :ia,
     :b :ib,
     :c :ic
FROM INV_ITEM          T1
     INNER JOIN
       LINE_CDR_DETAIL T2
       ON (t1.service_provider_nr > t2.service_provider_nr)
    , LINE_CDR         T3
WHERE T1.service_provider_nr = ?sp
  AND T1.customer_nr         = ?cu
  AND T1.bill_seqnr          = ?bl
GROUP BY
  T1.service_provider_nr
ORDER BY
  T1.service_provider_nr ASC
BROWSE ACCESS
IN EXCLUSIVE MODE
