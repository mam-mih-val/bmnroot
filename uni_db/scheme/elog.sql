-- drop database bmn_elog;
create database bmn_elog;

create table person_
(
 person_id serial primary key,
 person_name varchar(30) unique not null,
 is_shift_leader boolean not null default false
);

create table beam_
(
 beam varchar(10) primary key
);

create table target_
(
 target varchar(10) primary key
);

create table type_
(
 type_id serial primary key,
 type_text varchar(30) unique not null
);

create table trigger_
(
 trigger_id serial primary key,
 trigger_info varchar(60) unique not null
);

create table log_record
(
 record_id serial primary key,
 record_date timestamp not null default now(),
 author int null references person_(person_id),
 record_type int not null references type_(type_id),
 run_number int null,
 shift_leader int null references person_(person_id),
 trigger_config int null references trigger_(trigger_id) on update cascade,
 daq_status varchar(70) null,
 sp_41 int null,
 field_comment varchar(70) null,
 beam varchar(10) null references beam_(beam),
 energy float null check (energy > 0),
 target varchar(10) null references target_(target),
 target_width float null,
 record_comment varchar(300)
);
