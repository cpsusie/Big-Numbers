drop table lilletable;

run crelil.sql;

insert into lilletable (firstcolumn) values ("jesper");
insert into lilletable (firstcolumn) values ("jesper1");
insert into lilletable values ("t1","t1",1,1,1);
insert into lilletable values ("t2","t2",2,2,null);
insert into lilletable values ("t3","t3",3,3,null);
insert into lilletable values ("t4","t4",4,4,2);
insert into lilletable values ("t5","t5",5,5,1);
insert into lilletable values ("t6","t6",6,6,2);

create index ff on lilletable (nullcolumn1);
