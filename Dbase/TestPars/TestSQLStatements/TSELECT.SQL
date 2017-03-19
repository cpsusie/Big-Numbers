      SELECT
         T1.service_provider_nr                        ,
         T1.customer_nr                                ,
         T1.bill_seqnr                                 ,
         T1.contract_type_nr                           ,
         T1.contract_nr                                ,
         T1.sub_contr_nr                               ,
         T1.na_nr                                      ,
         T1.product_reference_code                     ,
         T1.bill_line_text_code                        ,
         T1.item_from_date                             ,
         T1.sales_channel                              ,
         T1.installation_line_nr                       ,
         T2.product_nr                                 ,
         T2.cdr_system_id                              ,
         T2.cdr_job_id                                 ,
         count(*)                                      ,
         min(T2.cdr_distribution_list_id)              ,
         min(T2.line_total_start_date)                 ,
         min(T3.a_number)                              ,
         min(T3.b_number)                              ,
         min(T3.rating_start_date * +100000000 + T3.rating_start_time ),
         sum(T3.rating_period)                         ,
         sum(T3.rerated_amount)                        ,
         sum(T4.rating_volume)
      INTO :a :ha,
           :b :hb,
           :c :hc

      FROM INV_ITEM                            T1,
           LINE_CDR_DETAIL                     T2,
           LINE_CDR                            T3,
           LINE_CDR_COST_DETAIL                T4
      WHERE T1.service_provider_nr             = ?sp
        AND count(*) > 1
        AND T1.customer_nr                     = ?cu
        AND T1.bill_seqnr                      = ?bl
        AND T1.transaction_type_88             = ?tr
        AND T1.line_total_start_date           > 0
        AND T1.discount_nr                     = 0
        AND T2.service_provider_nr             =
            T1.service_provider_nr
        AND T2.contract_nr                     =
            T1.contract_nr
        AND T2.sub_contr_nr                    =
            T1.sub_contr_nr
        AND T2.installation_line_nr            =
            T1.installation_line_nr
        AND T2.line_total_start_date           =
            T1.line_total_start_date
        AND T2.line_total_seqnr                =
            T1.line_total_seqnr
        AND T3.service_provider_nr             =
            T2.service_provideR_nr
        AND T3.contract_nr                     =
            T2.contract_nr
        AND T3.sub_contr_nr                    =
            T2.sub_contr_nr
        AND T3.installation_line_nr            =
            T2.installation_line_nr
        AND T3.line_total_start_date           =
            T2.line_total_start_date
        AND T3.line_total_seqnr                =
            T2.line_total_seqnr
        AND T3.a_number                        =
            T2.a_number
        AND T3.product_nr                      =
            T2.product_nr
        AND T3.b_number                 +  f     =
            T2.b_number
        AND T3.country_code                    =
            T2.country_code
        AND T3.rating_start_date               =
            T2.rating_start_date
        AND T3.rating_start_time               =
            T2.rating_start_time
        AND T3.switch_cdr_seqnr                =
            T2.switch_cdr_seqnr
        AND T3.cdr_seqnr                       =
            T2.cdr_seqnr

        AND T4.service_provider_nr             =
            T2.service_provideR_nr
        AND T4.contract_nr                     =
            T2.contract_nr
        AND T4.sub_contr_nr                    =
            T2.sub_contr_nr
        AND T4.installation_line_nr            =
            T2.installation_line_nr
        AND T4.line_total_start_date           =
            T2.line_total_start_date
        AND T4.line_total_seqnr                =
            T2.line_total_seqnr
        AND T4.a_number                        =
            T2.a_number
        AND T4.product_nr                      =
            T2.product_nr
        AND T4.b_number                        =
            T2.b_number
        AND T4.country_code                    =
            T2.country_code
        AND T4.rating_start_date               =
            T2.rating_start_date
        AND T4.rating_start_time               =
            T2.rating_start_time
        AND T4.switch_cdr_seqnr                =
            T2.switch_cdr_seqnr
        AND T4.cdr_seqnr                       =
            T2.cdr_seqnr

      GROUP BY
        T1.service_provider_nr                         ,
        T1.customer_nr                                 ,
        T1.bill_seqnr                                  ,
        T1.contract_type_nr                            ,
        T1.contract_nr                                 ,
        T1.sub_contr_nr                                ,
        T1.na_nr                                       ,
        T1.product_reference_code                      ,
        T1.bill_line_text_code                         ,
        T1.item_from_date                              ,
        T1.sales_channel                               ,
        T1.installation_line_nr                        ,
        T2.product_nr                                  ,
        T2.cdr_system_id                               ,
        T2.cdr_job_id
     HAVING T1.service_provider_nr             = ?sp
        AND T1.customer_nr                     = ?cu
        AND T1.bill_seqnr                      = ?bl
        AND T1.transaction_type_88             = ?tr
        AND T1.line_total_start_date           > 0
        AND T1.discount_nr                     = 0
        AND T2.service_provider_nr             =
            T1.service_provider_nr
        AND T2.contract_nr                     =
            T1.contract_nr
        AND T2.sub_contr_nr                    =
            T1.sub_contr_nr
        AND T2.installation_line_nr            =
            T1.installation_line_nr
        AND T2.line_total_start_date           =
            T1.line_total_start_date
        AND T2.line_total_seqnr                =
            T1.line_total_seqnr

        AND T3.service_provider_nr             =
            T2.service_provideR_nr
        AND T3.contract_nr                     =
            T2.contract_nr
        AND T3.sub_contr_nr                    =
            T2.sub_contr_nr
        AND T3.installation_line_nr            =
            T2.installation_line_nr
        AND T3.line_total_start_date           =
            T2.line_total_start_date
        AND T3.line_total_seqnr                =
            T2.line_total_seqnr
        AND T3.a_number                        =
            T2.a_number
        AND T3.product_nr                      =
            T2.product_nr
        AND T3.b_number                        =
            T2.b_number
        AND T3.country_code                    =
            T2.country_code
        AND T3.rating_start_date               =
            T2.rating_start_date
        AND T3.rating_start_time               =
            T2.rating_start_time
        AND T3.switch_cdr_seqnr                =
            T2.switch_cdr_seqnr
        AND T3.cdr_seqnr                       =
            T2.cdr_seqnr

        AND T4.service_provider_nr             =
            T2.service_provideR_nr
        AND T4.contract_nr                     =
            T2.contract_nr
        AND T4.sub_contr_nr                    =
            T2.sub_contr_nr
        AND T4.installation_line_nr            =
            T2.installation_line_nr
        AND T4.line_total_start_date           =
            T2.line_total_start_date
        AND T4.line_total_seqnr                =
            T2.line_total_seqnr
        AND T4.a_number                        =
            T2.a_number
        AND T4.product_nr                      =
            T2.product_nr
        AND T4.b_number                        =
            T2.b_number
        AND T4.country_code                    =
            T2.country_code
        AND T4.rating_start_date               =
            T2.rating_start_date
        AND T4.rating_start_time               =
            T2.rating_start_time
        AND T4.switch_cdr_seqnr                =
            T2.switch_cdr_seqnr
        AND T4.cdr_seqnr                       =
            T2.cdr_seqnr

      ORDER BY        
        T1.service_provider_nr                     ASC ,
        T1.customer_nr                             ASC ,
        T1.bill_seqnr                              ASC ,
        T1.contract_type_nr                        ASC ,
        T1.contract_nr                             ASC ,
        T1.sub_contr_nr                            ASC ,
        T1.na_nr                                   ASC

