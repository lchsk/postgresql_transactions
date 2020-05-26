drop table if exists A;

create table A
(
    id int
);

drop table if exists B;

create table B
(
    id int,
    value int
);


drop table if exists C;

create table C
(
    id int,
    value int
);

drop table if exists E;
drop table if exists D;

create table D
(
    id bigint primary key,
    value int
);

create table E
(
    id bigint primary key,
    value int,
    d_id int references D(id)
);
